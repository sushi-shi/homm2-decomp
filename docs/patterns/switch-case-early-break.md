# switch-case-early-break

**Symptom.** Inside one `switch` case, an `if` arm ends with a **short** `jmp`
to a join a few bytes ahead, where retail ends it with a **near** `jmp` to the
end of the whole `switch`. The two encodings differ in length (2 vs 5 bytes),
so every branch displacement after the case shifts and the function reads a few
bytes short.

    ours (if / else)                          retail (if { ...; break; } ...)
    ----------------------------------------  ----------------------------------------
    e8 00 00 00 00  call NormalDialog         e8 00 00 00 00  call NormalDialog
    eb 3b           jmp  <join after else>    e9 63 07 00 00  jmp  <end of switch>
    8b 55 f0        mov  edx, [ebp-0x10]      8b 55 f0        mov  edx, [ebp-0x10]
    ...             <else arm>                ...             <rest of the case>

(`townManager::Main` 0xa6842, `TOWN_WIDGET_BUILDING_CASTLE_UPGRADE`.)

## The rule

`if (c) { A } else { B } break;` gives the then-arm a jump to the join that
precedes the case's own `break` jump — a SHORT jump when the else-arm is small.
`if (c) { A break; } B break;` gives the then-arm the case's `break` target
directly, which is the far end of the switch — always a NEAR jump in a large
dispatcher.

So: **a case arm that jumps all the way to the switch end is an early `break`
inside the `if`, not an `if/else`.** The reverse reading holds too — a short
jump to a local join means retail really did write `else`.

The same shape decides where a `goto` label sits relative to a guard. In
`townManager::Main` eight `goto showBuildingInformation;` sites all targeted a
byte 6 further along in retail than in ours: the label is INSIDE the
`if (quickView) {` arm of the shared case, past the re-test, not in front of it:

    retail  jmp <label>            label = 0x115f  (`push $1`, first stmt of the arm)
    ours    jmp <label>            label = 0x1159  (`cmp [quickView],0`, the guard)

**Closes.** `townManager::Main` 95.51% -> EXACT (three sites: the dwelling
case, the castle-upgrade case, and the shared building-information label).
