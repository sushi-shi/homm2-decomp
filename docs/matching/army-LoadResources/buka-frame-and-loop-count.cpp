// army::LoadResources (PoL RVA 0x4aec8; Buka RVA 0x18b7d)
//
// Axes: eight PoL dead i32 declarations vs Buka `i` only; loopCount 1 vs 0.
// Artifact: build/source-variant-batch/army-load-resources-buka/results.json
// Coverage: complete 4/4 clean-source Cartesian product; source restored.
//
// PoL dead frame + value 1: exact, 1190 bytes, 42/42 CFG, 87/87 relocs.
// PoL dead frame + value 0: 99.996340%.
// Buka i-only frame + value 1: 99.783880%.
// Buka i-only frame + value 0: 99.780220%.
//
// Disposition: retain PoL's compiler-specific dead frame and byte-pinned
// loopCount 1. Buka's loopCount 0 is version logic; do not port either item.
