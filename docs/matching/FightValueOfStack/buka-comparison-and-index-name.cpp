// philAI::FightValueOfStack, PoL RVA 0x3fed2.
//
// Current PoL loop condition:
//   for (scoreIndex15 = 0; spellCount > scoreIndex15; scoreIndex15++)
// Buka exact loop condition:
//   for (t = 0; t < spellCount; t++)
//
// Complete comparison product:
//   build/fight-value-of-stack-relational-manifest.json
//   build/source-variant-batch/fight-value-of-stack-relational/results.json
//   2 source arms x (clean + 50 forest/top states) = 102/102 cells.
//   Both arms have the same maximum: 99.913890%, 3064 bytes, exact 147/147
//   block topology, and the complete ordered 72/72 relocation stream.
//
// Complete Buka index-name descendant:
//   build/fight-value-of-stack-index-name-manifest.json
//   build/source-variant-batch/fight-value-of-stack-index-name/results.json
//   1 source arm x (clean + 50 forest/top states) = 51/51 cells.
//   Renaming scoreIndex15 to Buka's t moves the VC4.2 local from bucket 2 to
//   bucket 4 and lowers the best island to 99.843056%.
//
// Unchanged-source census:
//   build/fight-value-of-stack-state-summary.json
//   build/tu-state-noise/fight-value-of-stack-cross-version/
//   Seed 1213156658, trial 6: 99.913890%, retail size, exact CFG, ordered
//   relocations. The residual is three raw bytes in the inner loop comparison:
//   candidate loads spellCount and emits jge; retail loads the index and emits
//   jle. The frame and every referenced stack offset agree.
//
// DISPOSITION: retain the PoL source. Buka's comparison is VC4.2-byte-neutral
// and its local name is a measured regression. No gameplay logic differs; the
// remaining island is compiler name/TU-state behavior, not a transferable edit.
