// Reviewed independent local-name matrix for spotBlock.
// Current PoL: spotPos / updatedDelta
// Buka:       index / updatedDelta2
// Upstream:   pos / newdelta
//
// Verified MSVC 4.2 hash buckets:
//   spotPos=3, index=12, pos=14
//   updatedDelta=15, updatedDelta2=6, newdelta=1, delta=10
// The three coherent families therefore predict different `/Od` slot orders.
//
// Complete matrix:
//   build/spot-block-local-names-axes.json
//   build/source-variant-batch/spot-block-local-names/results.json
//   3 position names * 3 updated-delta names * (clean + 50 forest/top states)
//   = 459/459 compiled; no truncation; source restored.
//
// No tested state closed exactly. Current spotPos/updatedDelta retained the
// historical exact MAX and had the best live island: 99.621216%, 449 bytes.
// Full Buka index/updatedDelta2: max 99.477270%.
// Full upstream pos/newdelta: max 99.477270%.
// The best single-name alternatives also stayed lower: index with current
// updatedDelta 99.545456%; current spotPos with updatedDelta2 99.507576%.
// All families retained the 33-block structural orbit and 17/17 relocations.
//
// Disposition: do not port Buka or upstream names; keep PoL's slot pins.
