# compiler-temp-vs-named-local

**Symptom.** Every named local of a function sits in the hash-ordered block at
the top of the frame; compiler temps live BELOW it (below the `this` spill for
a `__thiscall` member). A residual whose only defect is one slot at the wrong
end of the frame — plus the `this` spill and every temp shifted by that slot's
size — means our source named a value that retail left as a temp (or the
reverse).

**Case 1 — call results folded into one expression** (PerMonth 0x59b19):

    ours (two named locals)                retail (two temps)
    -------------------------------------- --------------------------------------
    mov  ecx, [ebp-0x28]   ; this @ -0x28  mov  ecx, [ebp-0x20]   ; this @ -0x20
    call GetRandomNumTroops                call GetRandomNumTroops
    mov  [ebp-0x4],  eax   ; firstCount    mov  [ebp-0x24], eax   ; temp
    ...                                    ...
    mov  [ebp-0x20], eax   ; secondCount   mov  [ebp-0x28], eax   ; temp
    mov  ecx, [ebp-0x4]                    mov  ecx, [ebp-0x24]
    add  ecx, [ebp-0x20]                   add  ecx, [ebp-0x28]

Retail's frame is 9 named locals wide; ours was 11 wide with `this` pushed two
slots deeper. Close: drop the locals and write the one expression —

    spot->m_objectMetadata =
        GetRandomNumTroops(static_cast<CreatureType>(giMonthTypeExtra))
        + GetRandomNumTroops(static_cast<CreatureType>(giMonthTypeExtra));

**Case 2 — an inner-block local vs a ternary temp** (NewMap 0x4f755). The
8-byte double for a handicap factor:

    ours  (double resourceScale;)          retail
    -------------------------------------- --------------------------------------
    c7 45 a8 33333333  mov [ebp-0x58], ..  c7 45 5c 33333333  mov [ebp-0xa4], ..
    c7 45 ac 3feb3333  mov [ebp-0x54], ..  c7 45 60 3feb3333  mov [ebp-0xa0], ..
    ...                                    ...
    dc 4d a8  fmul qword ptr [ebp-0x58]    dc 4d 5c  fmul qword ptr [ebp-0xa4]

An inner-block declaration still slots straight after the OUTER named group
(-0x58, just under the last outer local at -0x50), so it lands at the top of
the frame; a ternary's temp is allocated last and lands at the very bottom
(-0xa4), after `this`. Close: inline the ternary —

    (m_players + player)->m_resources[nTown] = static_cast<i32>(
        (m_players + player)->m_resources[nTown]
        * (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE
               ? GAME_HANDICAP_MODERATE_RESOURCE_FACTOR
               : GAME_HANDICAP_SEVERE_RESOURCE_FACTOR)
    );

**Case 3 — an inline accessor's byte return vs a direct member read**
(NewMap, `playerData::TownId(i32)`):

    ours  (m_players[p].TownId(0))         retail (m_players[p].m_townIds[0])
    -------------------------------------- --------------------------------------
    8a 84 0a e3040000  mov  al,[edx+ecx+0x4e3]   0f be 84 0a e3040000 movsx eax,byte ptr [edx+ecx+0x4e3]
    88 85 68ffffff     mov  [ebp-0x98],al        6b c0 64             imul eax,eax,0x64
    0f be 8d 68ffffff  movsx ecx,[ebp-0x98]
    6b c9 64           imul ecx,ecx,0x64

An out-of-frame byte return of an inlined accessor is materialised through a
1-byte temp before the sign extension; the direct member read sign-extends in
place. Close: index the member array directly.

**Case 4 - a nested `?:` chain never shares one slot** (`townManager::SetupWell`
0xaacfd, the CP1251 first-letter upcase). Retail writes ONE byte slot from four
arms and reads it once:

    retail                                    ours (4-way nested ternary, char arms)
    ----------------------------------------  ----------------------------------------
    88 8d 5c ff ff ff  mov [ebp-0xa4], cl     88 8d 5b ff ff ff  mov [ebp-0xa5], cl
    eb 26              jmp  <end>             eb 2e              jmp  <end>
    ...  (three more arms, same slot) ...     ...  (three more arms, THREE slots) ...
    c6 85 5c ff ff ff a8  mov [ebp-0xa4],0xa8 c6 85 59 ff ff ff a8  mov [ebp-0xa7],0xa8
    8a 8d 5c ff ff ff  mov cl, [ebp-0xa4]     8a 8d 59 ff ff ff  mov cl, [ebp-0xa7]
    88 0d <gText>      mov [gText], cl        88 8d 5a ff ff ff  mov [ebp-0xa6], cl
                                              8a 95 5a ff ff ff  mov dl, [ebp-0xa6]
                                              88 95 5b ff ff ff  mov [ebp-0xa5], dl
                                              8a 85 5b ff ff ff  mov al, [ebp-0xa5]
                                              88 05 <gText>      mov [gText], al

VC6 gives every `?:` NODE its own temp and copies inward-to-outward, so an
N-level chain costs N slots and N-1 copies no matter how the arms are typed
(`char`, `i8`, `u8` and int arms were all measured). One slot with four writes
and one read is therefore a NAMED local, not a conditional expression. Close:

```cpp
char upperFirst;
if (static_cast<u8>(gText[0]) >= 'a' && static_cast<u8>(gText[0]) <= 'z')
    upperFirst = static_cast<u8>(gText[0]) - ' ';
else if (...)
    ...
gText[0] = upperFirst;
```

`SetupWell` 90.32% -> EXACT.

**Diagnosis rule.** Diff with `(%ebp)` displacements VISIBLE. If the temps and
the `this` spill are shifted by a constant while the instruction stream is
identical, count the named slots: the side with more named slots has a source
local the other side does not.
