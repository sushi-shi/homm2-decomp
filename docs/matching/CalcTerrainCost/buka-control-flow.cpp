// CalcTerrainCost @ PoL RVA 0x000a4ba0
//
// PoL source before:
//   baseCost/roadCost locals, terrainCost/pathfindingCost labels, three gotos
// Buka exact / retained PoL source:
//   no locals or labels; nested mobility guard and straight fall-through tail
//
// Complete clean structural product:
//   build/source-variant-batch/calc-terrain-cost-structure/results.json
//   2/2 cells; both source families are byte- and relocation-identical and
//   exact at 117 bytes, 12/12 blocks, and ordered 5/5 relocations.
//
// Retained-source full-build audit repeats 100.000000% with the exact graph
// and relocation stream.
//
// Disposition: invariant matching decomp; retain Buka's local-free semantic
// control flow in PoL. No gameplay or shipped-byte change.
