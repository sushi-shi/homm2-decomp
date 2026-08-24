// dpnet_init (PoL RVA 0x1eeaf; Buka RVA 0x36866)
//
// Artifacts:
//   build/source-variant-batch/dpnet-init-clean/results.json
//   build/source-variant-batch/dpnet-init-buka/results.json
//   build/dpnet-init-final-replay-summary.json
//   build/dpnet-init-final-record-summary.json
// Coverage: complete 8 source arms x (clean + 50 forest/top states), 408/408.
// Seed: 1213156658. Generator untruncated; source restored.
//
// `rc` pairs with `result` in every state and is exact in the clean PoL-name,
// count-first shape. `dpEnumerate` never closes (best 99.932990%). Buka's
// index-first guest comparison is 99.072170%/784 bytes in the clean state but
// reaches the retail 785-byte text under forest trial 2, with either result
// name. Keep PoL `enumerateFunction`; adopt Buka `rc` and comparison.
//
// Trial 2 replay is an audited exact closure: zero retail-byte delta, exact
// 29/29 CFG, complete ordered relocation gates (74/74 raw records; canonical
// helper 71/71). Retained MAX 100.0000 for hash 65d2af2d0962.
