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

## Second tell: the accessor hoists ahead of the argument pushes

When the accessor call is an *argument*, the extra temp is not the only difference:
`/Ob1` expands the inline body as a statement before the call's right-to-left push
sequence starts, so every later argument's `push` moves after it. A direct member
read stays inside the argument evaluation and keeps retail's order.

```cpp
struct P { char pad[0x49c]; i8 color; };
class G { public: P p[4]; i8 GetColor(int i) { return p[i].color; } };
G* gp;  void Sink(int,int,int,int);

void a_accessor(int who) { Sink(1, gp->GetColor(who), -1, 5000); }   // ours
void a_member(int who)   { Sink(1, gp->p[who].color, -1, 5000); }    // retail
```

```
a_accessor  (ours)                            a_member  (retail)
--------------------------------------------- ---------------------------------------------
83 ec 08     subl $0x8, %esp                  51           pushl %ecx        ; frame 4, not 8
89 4d f8     movl %ecx, -0x8(%ebp)            89 4d fc     movl %ecx, -0x4(%ebp)
8b 45 f8     movl -0x8(%ebp), %eax            68 88 13 00 00  pushl $0x1388  ; args first
69 c0 9d 04 00 00  imull $0x49d, %eax, %eax   6a ff        pushl $-0x1
8b 0d ..     movl 0x0, %ecx                   8b 45 fc     movl -0x4(%ebp), %eax
8a 94 01 9c 04 00 00  movb 0x49c(%ecx,%eax), %dl   69 c0 9d 04 00 00  imull $0x49d, %eax, %eax
88 55 fc     movb %dl, -0x4(%ebp)   ; the temp 8b 0d ..     movl 0x0, %ecx
68 88 13 00 00  pushl $0x1388  ; args AFTER   0f be 94 01 9c 04 00 00  movsbl 0x49c(%ecx,%eax), %edx
6a ff        pushl $-0x1                      b9 01 00 00 00  movl $0x1, %ecx
0f be 55 fc  movsbl -0x4(%ebp), %edx          e8 ..        calll Sink
b9 01 00 00 00  movl $0x1, %ecx
e8 ..        calll Sink
```

Measured on `CheckEndGame` (RVA 0x69899, three sites), `ReceiveHostReportsPlayerExit`
(0x6edbf) and `ReceiveRemotePlayerExit` (0x6f115): all five call sites needed

```cpp
    gpGame->m_players[static_cast<i8>(player)].m_color,   // retail
    gpGame->GetPlayerColor(static_cast<i8>(player)),      // ours
```

so in this image `game::GetPlayerColor` is a reconstruction convenience, not what
those call sites were written as.

## Corollary (measured, still OPEN): the accessor's RETURN WIDTH also decides the widening

Where the accessor result feeds a **`short`** parameter, the direct member read is not
enough: `char -> short` lowers to a 16-bit `movsx` (`66 0f be`), while retail widens to
32 bits and pushes.

```
retail                                                ours (direct member read)
----------------------------------------------------- -----------------------------------------------------
0f be 8c 10 9c 04 00 00  movsbl 0x49c(%eax,%edx), %ecx 66 0f be 8c 10 9c 04 00 00  movsbw 0x49c(%eax,%edx), %cx
51                       pushl  %ecx                   51                          pushl  %ecx
```

Probe (same flags), `struct G { i8 tab[8][0x11b]; ... }; void SinkS(short,short);`:

```cpp
signed char GetC(int i) { return tab[i][0x49c]; }   // b1: movb -> temp; movsbw temp   (spill + 16-bit)
int         GetI(int i) { return tab[i][0x49c]; }   // b2: movsbl 0x49c(%ecx,%eax),%edx (NO temp, 32-bit)
short       GetS(int i) { return tab[i][0x49c]; }   // b3: movsbw 0x49c(%ecx,%eax),%dx  (no temp, 16-bit)
SinkS(1, gp->tab[i][0x49c]);                        //     movsbw ...                   (no temp, 16-bit)
SinkS(1, (int)gp->tab[i][0x49c]);                   //     movsbw ...  (the cast folds)
```

Only **b2** - an inline accessor declared to return `int` - reproduces retail's
`movsbl` with no temp slot. So `game::GetPlayerColor` is `i32 GetPlayerColor(i32)`
in retail, not `i8`; an `i8` return would force call sites either to spill a byte
temp (accessor form) or to emit `movsbw` (member form).

**CLOSED** at `advManager::UpdBottomViewEnemyTurn` (RVA 0xad72, 99.98% -> EXACT).
`GetPlayerColor` already returns `i32`; the one-byte residual was the *call site*
still reading the member directly:

```cpp
    gpGame->GetPlayerColor(static_cast<char>(giCurPlayer)),                    // retail
    static_cast<i32>(gpGame->m_players[static_cast<char>(giCurPlayer)].m_color) // ours
```

The `i32`-returning accessor expands in place (no hoist, no temp) because its body
needs no frame slot, so the ten `iconWidget` argument pushes keep retail's
right-to-left order. Which of the two spellings a site wants is decided by the
widening width: a `short` parameter wants the accessor (`movsbl`), an `i32`
parameter or an assignment to an `i32` lvalue is byte-identical either way - the
same function's `BroadcastMessage` arm keeps the member read.
