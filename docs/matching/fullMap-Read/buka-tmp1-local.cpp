// Reviewed identifier-name matrix for fullMap::Read's oldMapCell pointer.
// PoL: oldMapCell* tmp;
// Buka: oldMapCell* tmp1;
//
// Frame/slot evidence:
//   homm2 od-frames --rva 0xb7da
// reports equal 48-byte frames and saved registers, but local-reference-order.
// The verified /Od slot model gives:
//   tmp  -> bucket 4  -> -0x8 in [nb, tmp, x, y, tmp2]
//   tmp1 -> bucket 11 -> -0x10 in [nb, x, y, tmp1, tmp2]
//
// Complete paired state sweeps (seed 1213156658, clean + 50 forest/top):
//   build/source-variant-batch/full-map-read-tmp-name/results.json
//   build/source-variant-batch/full-map-read-tmp1/results.json
//   102/102 compiled; no truncation; source restored.
//
// `tmp` reached exact in seven states. `tmp1` reached no exact state; its best
// was 99.936650% (clean 97.176470%). Both remained 661 bytes, 22 blocks, and
// 16/16 relocations, so the residual is the predicted name-sensitive slot use.
//
// Disposition: do not port Buka's compiler/version-specific `tmp1` spelling.
