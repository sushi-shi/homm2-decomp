// army::GetBestDirection (PoL RVA 0xbea3f; Buka RVA 0x7d9ec)
//
// Artifact:
//   build/source-variant-batch/get-best-direction-compares/results.json
// Coverage: complete 2 source arms x (clean + 50 forest/top states), 102/102.
// Seed: 1213156658. Generator untruncated; source restored.
//
// The five PoL source-first/equality-left comparisons and the authoritative
// Buka target-first/equality-right family emit the same target text in every
// paired state. Clean is 99.731960%; trials 3, 15, 29, 37, and 48 reach
// 99.995870%. Both arms retain 2303 bytes, exact 261/261 CFG, and 2/2 ordered
// relocations.
//
// PoL adopts the byte-neutral Buka comparison family. The remaining two Jcc
// opcode residual is independent of these spellings and stays parked with the
// prior `/Od` eight-local slot-order residual; no closure is claimed.
