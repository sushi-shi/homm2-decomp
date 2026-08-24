// searchArray::FindCombatPath @ PoL RVA 0x000a53f0
//
// Baseline PoL source:
//   inline QuickDistance/Clear bodies, nested ValidHex body, reused return
//   local, moatHex/offset/currentHex/moatCost locals, queue != 0, raw storage
//   node access
//   95.219880%, 1022/1026 bytes, exact 54/54 graph, 23/23 relocations
//
// Full Buka structural product:
//   build/source-variant-batch/find-combat-path-structure/results.json
//   PoL parent:  95.219880%, 1022 bytes, exact 54/54 graph
//   Buka parent: 83.436745%,  936 bytes, 51/54 blocks, edge/pred deltas
//                50/21, 21 flow mismatches, three missing blocks
//
// Retail PoL has no QuickDistance or Clear callee relocation. Together with
// the contradictory full-Buka graph, this rejects Buka's real helper calls,
// un-nested failure guard, separate result local, and restore-label family as
// VC6/compiler-specific for this target.
//
// Complete call-free donor product:
//   build/source-variant-batch/find-combat-path-internal/results.json
//   64/64 clean cells across direct moat reads, repeated facing ternaries,
//   queue > 0, direct node.x use, inlined moat ternary, and GetNode access.
//   Each of the four local eliminations alone reaches 95.807230%, 1021 bytes;
//   queue comparison and GetNode are clean-byte-neutral. The complete six-
//   donor Buka arm returns to the baseline clean island.
//
// Complete reduced family/state product:
//   build/source-variant-batch/find-combat-path-reduced-state/results.json
//   8 reviewed arms * (clean + 50 forest/top states) = 408/408 cells
//   retained complete six-donor arm reaches 99.759030% at forest trial 8,
//   exact retail size 1026, exact 54/54 graph, and complete ordered 23/23
//   relocations. Only three instruction bytes remain.
//
// Unchanged-source clue artifacts:
//   build/find-combat-path-selected-state-summary.json
//   build/tu-state-noise/find-combat-path-selected
//   50/50 forest/top states, 19 byte/relocation states; trial 8 repeats the
//   99.759030% retail-size, exact-CFG, exact-relocation island. No closure.
//
// Disposition: mixed compiler boundary. Retain Buka's direct moat reads,
// repeated facing ternaries, positive queue condition, direct node.x use,
// inlined moat-cost ternary, and GetNode path reconstruction. Keep PoL's
// inlined helpers and outer failure/result CFG. No gameplay change.
