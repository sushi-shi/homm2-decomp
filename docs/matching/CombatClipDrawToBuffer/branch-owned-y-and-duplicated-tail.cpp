/*
icon::CombatClipDrawToBuffer (RVA 0x000c7b50, retail size 699)
Branch-owned Y and duplicated extent-tail structures, 2026-07-27.

Fresh evidence
--------------

The downstream draw dispatcher is already exact. The residual begins in the
extent prefix, where retail assigns entryOffset to ebx and limits to edi.
Retail also schedules the IconEntry::y load at the end of each orientation
predecessor, then shares the add-y/store-top/height tail. The retained source
previously loaded y only after the branch join. The complete function retains
39/39 blocks and exact flow, with 36 exact and three size-only blocks, and
37/37 relocation identities.

Branch-owned value matrix
-------------------------

Four structures were crossed with the clean state and 50 top-of-TU
declaration-forest states. All 204/204 cells compiled in 153.49 seconds
without truncation:

  shared direct Y:       96.938520%, size 691, 36 exact + 3 size-only
  branch-tail i16 Y:     96.569670%, size 694, 36 exact + 3 size-only
  branch-tail i32 Y:     83.795080%, size 710, 23 exact + 16 size-only
  branch-head i32 Y:     78.159836%, size 714, 22 exact + 17 size-only

The i16 arm is retained as a credible lower island because it approaches
retail size while preserving topology. It does not keep the Y load in the
predecessors: MSVC merges the assignments and emits a word move plus sign
extension in the shared block. The i32 arms create a stack slot and perturb
the exact dispatcher, contradicting the retail frame.

Artifacts:
  build/combatclip-branch-y-ownership-axes-20260727.json
  build/combatclip-branch-y-ownership-clean-manifest-20260727.json
  build/match-variants/combatclip-branch-y-ownership-clean-20260727/results.json
  build/combatclip-branch-y-ownership-top-states-manifest-20260727.json
  build/match-variants/combatclip-branch-y-ownership-top-states-20260727/results.json

Duplicated tail matrix
----------------------

Three high-level ownership forms were crossed with the same clean plus 50 top
states. All 153/153 cells compiled in 115.02 seconds without truncation:

  shared top and bottom:       96.938520%, size 691
  duplicated top, shared end:  87.454920%, size 685
  duplicated top and bottom:   97.000000%, size 690

Duplicating both top and bottom inside the orientation arms is a credible
source-level structure: MSVC tail-merges the identical computations back to
the retail-compatible 39-block CFG, while entering a new higher scheduling
island. It preserves all 37 relocations and raises the clean hash-scoped MAX
from 96.9385 to 97.0000. This arm is retained in source. The official
`homm2 status update` path recorded its new source hash and maximum.

Artifacts:
  build/combatclip-duplicated-top-tail-axes-20260727.json
  build/combatclip-duplicated-top-tail-clean-manifest-20260727.json
  build/match-variants/combatclip-duplicated-top-tail-clean-20260727/results.json
  build/combatclip-duplicated-top-tail-top-states-manifest-20260727.json
  build/match-variants/combatclip-duplicated-top-tail-top-states-20260727/results.json

Full extent helper
------------------

A separate inline helper owning the whole coordinate calculation was crossed
with the direct body over 102/102 clean-plus-50 cells in 76.89 seconds. Every
paired cell was byte-identical at the same state. The helper therefore has no
emitted ownership boundary and is rejected.

Artifacts:
  build/combatclip-full-extent-helper-axes-20260727.json
  build/combatclip-full-extent-helper-top-states-manifest-20260727.json
  build/match-variants/combatclip-full-extent-helper-top-states-20260727/results.json
*/

#if 0
// Credible lower island: branch-owned serialized-width Y.
i16 entryY;
if (mirror != ICON_DRAW_NORMAL) {
    // right/left calculations
    entryY = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y;
} else {
    // left/right calculations
    entryY = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y;
}
limits->top = entryY + y;

// Retained higher island: both common tail assignments appear in each arm.
if (mirror != ICON_DRAW_NORMAL) {
    // right/left calculations
    limits->top = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y + y;
    limits->bottom =
        reinterpret_cast<IconEntry*>(m_data + entryOffset)->h + limits->top - 1;
} else {
    // left/right calculations
    limits->top = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y + y;
    limits->bottom =
        reinterpret_cast<IconEntry*>(m_data + entryOffset)->h + limits->top - 1;
}
#endif
