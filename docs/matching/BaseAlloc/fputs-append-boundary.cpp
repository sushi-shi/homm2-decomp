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
*/
