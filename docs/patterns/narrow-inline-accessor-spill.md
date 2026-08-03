# A byte-wide inline accessor spills its result; a direct field read does not

**Trigger.** `/Ob1` expands an in-class accessor that returns a narrow type (`i8`) by
storing the return value into a frame slot of its own before the caller widens it. A
direct read of the same field goes straight to `movsx`. So an extra
`mov r8,[..] / mov [ebp-N],r8 / movsx r32,[ebp-N]` triple in *our* object - and a frame
one slot larger than retail's - means retail read the member, not the accessor.

Measured on `advManager::AdvPanel` (RVA 0x14173). `playerData::CurrentHero()` is
`i8 CurrentHero(void) { return m_currentHero; }`, `m_currentHero` at offset 2.

## The divergence

```
retail                                     ours
------------------------------------------ ------------------------------------------
8b 0d ..   mov   ecx, gpCurPlayer          8b 0d ..   mov   ecx, gpCurPlayer
0f be 51 02  movsx edx, byte [ecx+2]       8a 51 02   mov   dl, byte [ecx+2]
                                           88 55 c0   mov   [ebp-0x40], dl
                                           0f be 45 c0  movsx eax, byte [ebp-0x40]
83 fa ff   cmp   edx, -1                   83 f8 ff   cmp   eax, -1
75 49      jne   ...                       75 49      jne   ...
```

Prologue: retail `83 ec 3c  sub esp,0x3c`, ours `83 ec 40  sub esp,0x40`. The extra
4-byte slot at `ebp-0x40` is the accessor's return temp; it pushed the `this` spill,
the `new` temp and the switch temp each one slot deeper (`-0x40/-0x44/-0x48` retail vs
`-0x44/-0x48/-0x4c` ours) and moved the jump table five bytes along
(`ff 24 95 94 02 00 00` vs `ff 24 85 99 02 00 00`).

## What made it match

```cpp
        if (gpCurPlayer->CurrentHero() == INVALID_HERO) {   // ours
```
```cpp
        if (gpCurPlayer->m_currentHero == INVALID_HERO) {   // retail
```

96.70% -> EXACT with that single token. Note this is *site-local*: other call sites in
the same TU (`advManager::CheckCastSpell`, 100% exact) do carry the accessor's temp, so
the retail source genuinely mixes the two spellings. Read the frame, not the habit.
