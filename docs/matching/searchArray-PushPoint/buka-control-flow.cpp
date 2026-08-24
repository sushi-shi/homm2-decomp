// searchArray::PushPoint @ PoL RVA 0x000a4c20
//
// PoL source before:
//   one bounds/queue wrapper, low-before-high initialization, one compound
//   visited test, low>=high search stop, and giCurTempMobility<cost
// Buka exact / retained PoL source:
//   bounds and capacity early returns, high-before-low initialization, two
//   visited early returns, high<=low search stop, and cost>giCurTempMobility
//
// Clean structural product:
//   build/source-variant-batch/push-point-structure/results.json
//   PoL nested family:       96.145164%, 615 bytes, 38/38 relocations
//   Buka early-return family: 99.623660%, 615 bytes, 38/38 relocations,
//                             exact 25/25 graph
//
// Complete structural/state product:
//   build/source-variant-batch/push-point-structure-state/results.json
//   2 arms * (clean + 50 forest/top states) = 102/102 cells
//   Buka source at trial 42 closes exactly: 100.000000%, 615 bytes,
//   25/25 blocks, and complete ordered 38/38 relocations.
//
// Audited retained-source replay:
//   build/push-point-selected-replay.json
//   build/tu-state-noise/push-point-selected
//   forest/top trial 42 repeats exact closure; MAX 100 recorded for unchanged
//   source hash 9dc93a99c250. The generated probe is not retained.
//
// Disposition: matching decomp; retain the complete Buka control-flow family
// in PoL. The runtime conditions are equivalent; no gameplay change.
