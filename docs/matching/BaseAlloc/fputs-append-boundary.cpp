/*
BaseAlloc, retail RVA 0x000c3d70, size 0x20f.

This file records rejected newline/fputs ownership structures. It is evidence,
not compiled game source.

The live function has exact 18/18 CFG flow, 17 exact block bodies, one
size-only block, and complete relocation identity/count. In the final log
block, retail pushes the FILE* argument before scanning logText, then loads the
newline word and stores it through the scan end pointer. The reconstructed
source appends in a separate statement before calling fputs.

Five source structures were tested:

  - separate append followed by fputs;
  - inline char*-returning append helper;
  - the same helper with an explicit end local;
  - inline void append used through a comma expression in the fputs argument;
  - strcat directly in the fputs argument.

Every structure was crossed independently with the clean state and 50 forest
states at both top and target-adjacent insertion:

  build/match-variants/basealloc-fputs-append-boundary-20260727/
  build/match-variants/basealloc-fputs-append-boundary-target-20260727/

Both 255-candidate matrices completed without truncation and restored source.
All three inline forms were byte-identical to one another in every useful
state: 95.459460%, size 523, 30/30 raw relocations, exact 18-block flow, and
17 exact plus one size-only block. They move size closer to retail than the
514-byte open form, but do not improve bytes or close the final block.
The strcat argument expands to a contradictory 86.777020%, size-547 orbit.

Neither insertion position exposed the retained 95.7297% historical island,
and no source shape improved that MAX. No helper or generated state was
retained.

Cross-version initialization follow-up, 2026-08-05:

The exact Gold/Buka body calls `InitMemEntry()` instead of spelling the
initialization block open. To subtract later-only compiler behavior, a
four-arm matrix paired the open block or later call with the existing helper's
ordinary or explicitly inline definition. The paired inline-definition/call
arm was byte-identical to the open PoL block in the clean state and every one
of 50 forest/top states. The full 204/204 state product completed; no arm beat
95.459460% in the current census and no exact closure appeared. The ordinary
out-of-line call was structurally wrong at 78.939186%, size 434, and 25/30
relocations.

Artifacts:

  build/basealloc-cross-version-init-helper-axes.json
  build/basealloc-cross-version-init-helper-clean-manifest.json
  build/source-variant-batch/basealloc-cross-version-init-helper-clean/results.json
  build/basealloc-cross-version-init-helper-states-manifest.json
  build/source-variant-batch/basealloc-cross-version-init-helper-states/results.json

Disposition: retain the open PoL initialization. The later helper boundary is
codegen-neutral only when explicitly inlined and does not improve the durable
95.7297% MAX.
*/
