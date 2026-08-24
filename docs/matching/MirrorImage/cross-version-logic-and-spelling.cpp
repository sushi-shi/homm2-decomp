/*
combatManager::MirrorImage cross-version audit, 2026-08-24.

Targets:
  PoL 2.0: RVA 0x00028389, 0x5c8 bytes, 46 blocks.
  Buka:    RVA 0x0009f7c0, 0x64a bytes.

Logic classification: DO NOT BACKPORT. Buka's six
(facing, direction, sourcePart, distance) exclusion guards are absent from
the PoL retail CFG. PoL walks the ray immediately after selecting
searchDirection; its 46-block target contains none of the six continue arms.

Source matrix:
  build/mirror-image-cross-version-manifest.json
  build/source-variant-batch/mirror-image-cross-version/results.json

The complete 8/8 product independently tested the ray-loop relational mirror,
deadline0/deadline1, and both limit-counter pre/post-increment spellings.
deadline1 is the only slot-correct arm and improves the clean object from
99.830505% to 99.849880%. Pre/post increment and both relational spellings
are byte-identical under VC4.2. Retain Buka's post-increment spelling and the
shared deadline1 name.

Relational/state matrix:
  build/mirror-image-relational-state-manifest.json
  build/source-variant-batch/mirror-image-relational-state/results.json

All 102 requested cells completed: two relational spellings x clean plus 50
forest/top states. Both spellings remained byte-identical in every state.
The unchanged-source census is in build/mirror-image-state-summary.json and
build/tu-state-noise/mirror-image-cross-version/ (50/50 trials, eight unique
states, no exact island).

Retained result: 1480/1480 bytes, exact 46-block graph/topology, complete
38/38 relocation ownership, three raw byte differences. The residual is one
equivalent ray-loop compare lowering (mov/cmp/jcc); source form and compiler
state axes above do not move it.
*/
