// Exact-Buka local-name comparison for army::ValidRange.
//
// PoL: adjacentHex / directionResult
// Buka: adj / directionResult1
// VC4.2 buckets: adjacentHex=8, directionResult=13, adj=0,
// directionResult1=0.
//
// Complete independent matrix:
//   build/path-valid-range-name-axes.json
//   build/source-variant-batch/path-valid-range-name/results.json
//   4 name families * (clean + 50 forest/top states) = 204/204.
//
// adjacentHex + directionResult: 41/51 exact.
// adj + directionResult:         41/51 exact.
// Either directionResult1 family: 0/102 exact; best 99.796240%.
// adjacentHex/adj paired text and both relocation hashes in all 102 shared
// states. Every arm retained 1098 bytes, 59 blocks, 20/20 relocations.
//
// Disposition: port Buka adj; keep PoL directionResult.

#if 0
i32 adj;
CombatHexDirection directionResult;
#endif
