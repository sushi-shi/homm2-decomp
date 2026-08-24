// Reviewed unused-local name matrix for fullMap::ChangeTilesetIndex.
// PoL: i32 dummy;
// Buka: i32 a;
//
// Verified /Od slot model:
//   idx=0, ptr=1, t=4, dummy=6, a=1 (hash buckets)
//   dummy layout: idx, ptr, t, dummy
//   a layout:     idx, a, ptr, t
// Although unused semantically, Buka `a` shifts the live ptr/t stack slots.
//
// Complete matrix:
//   build/full-map-change-tileset-unused-local-axes.json
//   build/source-variant-batch/full-map-change-tileset-unused-local/results.json
//   2 names * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// `dummy`: 25 exact states, max 100%; clean 98.095024%.
// `a`: no exact state, max 99.832580%; clean 97.932130%.
// Both families retained 35 blocks and 2/2 relocations; exact `dummy` states
// use the retail 746-byte size.
//
// Disposition: do not port Buka's compiler/version-specific bucket-1 pin.
