// Reviewed comparison-order matrix for UpdateSystemOptionsMenu's menu guard.
// PoL: hmnuAdv != hmnuApp
// Buka: hmnuApp != hmnuAdv
//
// Canonical pre-change PoL object: exact at 784 bytes, 50 blocks, and 38/38
// owner-visible relocations.
//
// Complete matrix:
//   build/update-system-options-menu-guard-axes.json
//   build/source-variant-batch/update-system-options-menu-guard/results.json
//   2 orders * (clean + 50 forest/top states) = 102/102 compiled;
//   no truncation; source restored.
//
// The permuter occupied a separate 99.747120% KB state orbit, but every one of
// its 51 order pairs had identical text and raw/normalized relocation hashes.
// Thus operand order does not own that orbit or any relocation-site change.
// The post-port canonical rebuild remains exact at 784 bytes, 50 blocks, and
// 38/38 owner-visible relocations.
//
// Disposition: port the byte/relocation-invariant Buka operand order.
