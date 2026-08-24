// Reviewed shared-header structural variant for fullMap::Cell.
// Buka: cell = cells + x + y * width;
// PoL:  cell = cells + width * y + x;
//
// Full canonical rebuild with the Buka body:
//   - GetNewCellExtraOverlay exact: 467 bytes, 22 blocks, 2/2 relocs.
//   - GetNewCellExtraObject exact: 467 bytes, 22 blocks, 2/2 relocs.
//   - Repository totals unchanged: 1171 live exact, 1341 exact-max.
//
// State evidence:
//   build/cell-body-overlay-buka-summary.json
//   build/cell-body-object-buka-summary.json
// Both clean baselines and forest/top trial 1 closed exactly. Their text and
// raw/normalized relocation hashes equal the corresponding pre-change clean
// Row+Extra artifacts in get-new-cell-extra-{overlay,object}/results.json.
//
// Disposition: port the byte/relocation-invariant Buka Cell expression.
