// widget constructors
// PoL RVAs 0xdde00 / 0xdde60; Buka RVAs 0xd4010 / 0xd40a0.
//
// Artifacts:
//   build/source-variant-batch/widget-parameter-store-order/results.json
//   build/source-variant-batch/widget-default-store-order/results.json
// Seed 1213156658, forest/top. Source restored.
//
// Each matrix is complete: 2 source arms x (clean + 50 states), 102/102.
// Parameter ctor: both arms exact at 90 bytes, 1/1 CFG, one relocation.
// Default ctor: both arms exact at 63 bytes, 1/1 CFG, one relocation.
// Every paired state gives both arms identical evidence. PoL adopts Buka's
// m_id-before-flags parameter order and links-before-id default order.
