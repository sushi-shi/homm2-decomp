# goto-label-jump-stub

**Symptom.** A loop exit that our `break;` lowers as ONE direct jump reaches
retail's join through a two-hop stub parked after the last statement, with a
short jump hopping over the stub into the epilogue:

    ours (break;)                        retail
    ------------------------------------ ---------------------------------------
    ; loop body exit                     ; loop body exit
    e9 48 01 00 00  jmp <join>           e9 59 02 00 00  jmp <stub>
    ...                                  ...
    ; last statement, then epilogue      ; last statement, then
    5b              pop  ebx             eb 05           jmp <epilogue>
    8b e5           mov  esp, ebp        e9 e7 fe ff ff  jmp <join>   ; <stub>
    5d              pop  ebp             5b              pop  ebx
    c2 0c 00        ret  0xc             8b e5           mov  esp, ebp
                                         5d              pop  ebp
                                         c2 0c 00        ret  0xc

(CreateDiffFile 0x5f47a. `<join>` is the first statement after the
`if (...) {...} else { ... while (1) {...} }`, i.e. +0x433.)

**Cause.** VC6 /Od gives a source `goto` its own label id. When the label
happens to sit exactly where an existing block already starts, the two labels
are not coalesced: the goto's label is emitted at the END of the function as a
`jmp` to the real one, and the fall-through into the epilogue jumps over it. A
`break` reuses the loop's own exit label and jumps direct.

**Close.** Spell the loop exit as a goto to a label placed on the join:

                    length = 0;
                    goto Finish;
                }
                ...
        }
    Finish:
        sprintf(gText, "%s%s", ".\\DATA\\", diffName);

Measured: `break` 1356/1359 bytes, `goto Finish` 1363 = retail size, EXACT.
Wrapping the tail in `for(;;)`, `while(1)`, `do{}while(0)` or appending a dead
`goto` after `return;` all produce byte-identical output to the plain `break`
(VC6 drops the unreachable back edge), so the stub can only come from a live
`goto`.

## Reading it in reverse: stubs on OUR side mean our `goto` is retail's `break`

The rule is one stub per live `goto` statement, parked in source order right
before the epilogue. Counting them therefore counts the source's gotos.
`DataEntryWindowHandler` (0xc0a50) had five `goto normalEvent;` exits and one
structural fall-through; retail has the opposite:

```
ours (5x goto normalEvent)                 retail
------------------------------------       ------------------------------------
; each exit site                           ; each exit site
e9 79 01 00 00  jmp <stub_i>               0f 85 5d 01 00 00  jne <normalEvent>
...                                        e9 43 01 00 00     jmp <normalEvent>
36a6: mov ecx,[ebp-8]                      365b: mov ecx,[ebp-8]
      call EventWindowHandler                    call EventWindowHandler
36ae: eb 0c  jmp <epilogue>                3663: eb 05  jmp <epilogue>
36b0: eb f4  jmp 36a6   ; stub_1           3665: e9 f9 fe ff ff jmp <gotText>
36b2: eb f2  jmp 36a6   ; stub_2                 ; the ONE goto in the function
36b4: eb f0  jmp 36a6   ; stub_3
36b6: eb ee  jmp 36a6   ; stub_4
36b8: eb ec  jmp 36a6   ; stub_5
36ba: eb ea  jmp 36a6   ; stub_6
36bc: mov esp, ebp ...                     366a: mov esp, ebp ...
```

Retail reaches `EventWindowHandler` with *direct* `jne`/`jmp`s and keeps a
single stub for a jump the other way. So the exits are not gotos: they are the
end of an `if` whose body is everything up to that call, plus `break`s out of
the switches nested inside it, and the one stub is a real
`goto` into a `case` label. What closed it:

```cpp
    if (bDataEntryTime == ENTRY_PHASE_POINTER_SENT) {
        ++bDataEntryTime;
        goto gotText;                       /* the only goto -> the only stub */
    }
    if (message.type == MESSAGE_WIDGET) {   /* jne straight to the tail call  */
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) { case ENTRY_CANCEL_BUTTON: ... }
                break;                      /* direct jmp to the tail call    */
            case WIDGET_COMMAND_SELECT:
                switch (message.payload.widget.id) {
                    case ENTRY_TEXT_WIDGET:
                    gotText:
                        ...
                        if (strlen(...) == 0)
                            break;          /* direct jmp to the tail call    */
                }
        }
    }
    return EventWindowHandler(message);
```

An implicit switch default (no `default:` clause) targets the end of that
switch, so when the innermost switch is the last statement of the last case of
the outer switch, and that switch is the last statement of the `if`, all four
"give up" edges collapse onto one address — which is why retail needs no stubs.

Related: [if-else-two-jmp-backedge](if-else-two-jmp-backedge.md) is the same
jmp-to-jmp shape produced by if/else nesting rather than by a label id.

## The stubs are parked in REVERSE source order — use that to LOCATE a missing goto

Three functions of SOURCE/CURSOR + SOURCE/FINDPATH measured the ordering: the
stub block immediately before the epilogue lists one `jmp` per live `goto`, and
the FIRST `goto` in source order gets the HIGHEST stub address.

`searchArray::FindCombatPath` (0x4a906), four gotos:

```
source order                                    stub (address ascending)
1  if (!ValidHex(..)) goto restoreMoatFailure;   1201: jmp 11a6   <- 4th source goto
2  if (m_pathLength == 0) goto ..Failure;        11ff: jmp 11a6   <- 3rd
3  else if (bestHex != targetHex) goto ..;       11fd: jmp 11ad   <- ... reversed:
4  result = m_pathLength; goto restoreMoat;      1203: jmp 11a6
```

read bottom-up: `11fd`->restoreMoat is goto #4, `11ff`/`1201`->failure are #3/#2,
`1203` is #1.

That makes the block a direct census. `advManager::MoveHero` (0x340ca) had ten
stubs on our side and TWELVE in retail; the two extra sat at source positions 9
and 10, i.e. between the `!ValidMove` guard (#8) and the two adjacent-monster
gotos (#11/#12) — which pinned them to the two 5-byte `jmp`s at the end of the
cursor-cell `switch` and at the end of its enclosing `if`:

```
retail                                     ours
1a49: mov [ebp-0x28], 0   ; eventCell=NULL  1a3f: mov [ebp-0x28], 0
1a50: e9 .. jmp <stub#9>  ; goto            (falls straight through)
1a55: e9 .. jmp <stub#10> ; goto            1a2b: ja  <movementDone>   (direct)
1a5a: movementDone:                         1a46: movementDone:
```

Adding the two `goto movementDone;` statements (one closing the `switch`'s
enclosing block, one after the `if`) reproduced them.  A structural
fall-through into a label compiles to a DIRECT `jcc`/`jmp` at the label; only a
written `goto` costs a stub, so a stub deficit is always a missing `goto`
statement and a stub surplus is a `goto` retail spells as fall-through or
`break`.

`searchArray::TestPossibleDirections` (0x4a136) is the extreme case: thirteen
stubs, i.e. thirteen `gSearchTerrain = TERRAIN_INVALID; goto storeDirection;`
sites written out longhand rather than one shared `invalidDirection:` block.

## The stub's TARGET names the label — even when that address is inside a nested arm

Counting stubs gives the number of `goto`s; reading each stub's destination
gives the LABEL, and the label address then has to be spelled somewhere the
structure already reaches. It does not have to be at function scope.

`CycleColors` (0xb6b80, BASE/WINMGR) has exactly one stub:

```
3f4: eb 05              jmp <epilogue>     ; hop over the stub
3f6: e9 7b fe ff ff     jmp 0x276          ; the ONE stub
3fb: 8b e5 5d c3        <epilogue>
```

so the body contains exactly one `goto`, reached from

```
87: 85 c9              testl %ecx, %ecx
89: 75 05              jne  0x8e           ; short jcc over ...
8b: e9 68 03 00 00     jmp  0x3f6          ; ... the goto
```

The other three long jumps in the function (`e9` to 0x3a1, at 0x14f, 0x2b9 and
0x332) are DIRECT, so they are structural arm ends, not gotos — which forces
0x3a1 to be the end of an `if/else` and 0x276 to be a point INSIDE the else:

```cpp
    if (gbEveryOtherCycle == 0)
        goto cycleType;                     /* the one stub */
    if (giCycleType == WINDOW_COLOR_CYCLE_WORLD_VIEW) {
        ...                                 /* arm end: direct jmp 0x3a1 */
    } else {
        <five palette rotations>
    cycleType:                              /* 0x276, inside the else block */
        if (giCycleType == ..._DEFAULT) { ... }        /* direct jmp 0x3a1 */
        else if (giCycleType == ..._COMBAT) { ... }    /* direct jmp 0x3a1 */
        else if (giCycleType == ..._COMBAT_ALTERNATE) { ... }
    }
    /* 0x3a1 */
```

A `goto` that enters a block is legal C++ as long as it skips no
initialisation, and nothing else reproduces one stub plus three direct arm-end
jumps. `CycleColors` 80.72% -> EXACT.
