// Exact-Buka local-name comparison for army::FindPath.
//
// PoL: pathResult / savedSpeed
// Buka: pathResult2 / savedSpeed2
// VC4.2 buckets: pathResult=5, savedSpeed=10, pathResult2=15, savedSpeed2=8.
//
// Complete independent matrix:
//   build/path-find-path-name-axes.json
//   build/source-variant-batch/path-find-path-name/results.json
//   4 name families * (clean + 50 forest/top states) = 204/204.
//
// pathResult + savedSpeed:  51/51 exact.
// pathResult + savedSpeed2: 51/51 exact.
// pathResult2 with either speed name: 0/102 exact; best 99.926315%.
// savedSpeed/savedSpeed2 paired text and both relocation hashes in all 102
// shared states. Every arm retained 330 bytes, 20 blocks, 9/9 relocations.
//
// Disposition: port Buka savedSpeed2; keep PoL pathResult.

#if 0
i32 pathResult;
i32 savedSpeed2;
#endif
