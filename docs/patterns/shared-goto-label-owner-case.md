# shared-goto-label-owner-case

**Symptom.** A block of code that several `switch` cases share (a common tail)
sits in the wrong case: ours emits it inline in case A and every other case
reaches it with `jmp`; retail emits it inline in case C and cases A and B
`jmp` to it. Instruction-for-instruction identical, but the whole block moves.

    ours                                       retail
    ---------------------------------------    ---------------------------------------
    MINE:      ...EventWindow                  MINE:      ...EventWindow
    1639       mov  edx, [giCurPlayer]         1673       e9 f7 00 00 00  jmp 0x176f
               ...ClaimMine...                 ALCHEMIST: ...EventWindow
    1662       jmp  <switch end>               16f2       eb 7b           jmp 0x176f
    ALCHEMIST: ...EventWindow                  SAWMILL:   ...EventWindow
    1756       e9 de fe ff ff  jmp 0x1639      176f       mov eax, [giCurPlayer]
    SAWMILL:   ...EventWindow                             ...ClaimMine...
    17xx       jmp 0x1639                      1797       jmp  <switch end>

**The rule.** A `goto` label is not free-floating: `/Od` emits statements in
source order, so **the case that reaches the shared tail by FALLING INTO it —
the one with no `jmp` of its own — is the case that physically contains the
label.** Every other case shows an explicit `jmp` (short or near depending on
distance).

Read it off the target address: if the shared block's address is *inside* a
later case's span, move the label there and give the earlier cases a `goto`.

The same rule places a label inside an `if` body: at
`DoEvent`'s `MAP_OBJECT_TROLL_BRIDGE` the `recruitTroll:` label is *inside*
`if (dialogResult == MONSTER_DIALOG_YES) { ... }` in the last arm, so the
guarded arm's `goto recruitTroll` jumps past the re-test.

**Closes.** `advManager::DoEvent`:
* `claimMine:` moved MINE -> SAWMILL (MINE and ALCHEMIST_LAB gained `goto`);
* `findTeleportDestination:` moved WHIRLPOOL -> STONE_LITHS;
* `giveArtifact:` moved out of the guarded-artifact arm into
  `case ARTIFACT_EVENT_MODE_PICKUP`;
* `recruitTroll:` / `recruitLich:` / `recruitDragon:` moved inside the
  `dialogResult == YES` guard of their last arm.

and in `advManager::DoAIEvent`, `creaturePurchase:` gained a 17th
`goto creaturePurchase;` (the last dwelling case ends with an explicit `goto`
even though its label follows immediately — the stub island has one entry per
`goto`, so counting the parked `jmp` stubs counts the gotos).
