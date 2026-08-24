// army::CheckLuck @ PoL RVA 0x0004f93e
//
// The old cross-version ledger described an intermediate Buka reconstruction
// with a named `creatureName` temporary. That claim is stale: current exact
// Buka source and current exact PoL source both use two inline ternaries.
//
// Current authoritative difference:
//   PoL:  quantity > 1  ? plural : singular
//   Buka: quantity <= 1 ? singular : plural
//
// Complete clean product:
//   build/source-variant-batch/check-luck-name-polarity/results.json
//   2 bad-luck arms * 2 good-luck arms = 4/4 cells. Every arm is exact at
//   100.000000%, 642 bytes, 25/25 blocks, and ordered 37/37 relocations.
//
// Disposition: invariant matching decomp. PoL adopts both current Buka
// singular-first ternaries. The obsolete local-hoist claim is rejected by
// the completed exact Buka source. No gameplay or shipped-byte change.
