// S50, parent c65e989f: complete structural owner family, 6/6 in 6.519s.
// Effective source alternatives (all other statements unchanged):
//
// saved owner A: b32 savedShow3[3];  reads/writes savedShow3[0]
// saved owner B: b32 savedShow3[1];  reads/writes savedShow3[0]
// saved owner C: b32 savedShow3;     reads/writes savedShow3
//
// unused owner A: i32 H2_UNUSED(unusedVars1)[7];
// unused owner B: declaration removed (there are no uses).
//
// All 3x2 combinations enumerated in build/s50-doai-axes.json and tested by
// build/s50-doai-manifest.json; results: build/s50-doai-batch/results.json.
// A/A 2879 bytes; A/B 2855; B/A=C/A 2867; B/B=C/B 2855.
// Retail size 2879. All 141-block graphs align, but smaller forms fail size.
// No source retained; no fake padding added. Private float relocation identity
// prevents the generic checker from declaring the baseline closed.
// After restoration/full build, all 89 PHILAI functions, allocated sections
// and ordered native relocations/addends match build/ai-review-before.obj.
