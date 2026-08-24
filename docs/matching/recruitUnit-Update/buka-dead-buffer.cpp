// recruitUnit::Update (PoL RVA 0x8b7ce; Buka RVA 0x8c855)
//
// Axis: retain/remove the unused `char text[NAME_SIZE]` declaration.
// Artifact:
//   build/source-variant-batch/recruit-update-dead-buffer/results.json
// Coverage: complete 2 source arms x (clean + 50 forest/top states), 102/102.
// Seed: 1213156658. Source restored.
//
// PoL-present clean: 100.000000%, 290 bytes, exact 4/4 CFG, 18 sites.
// Buka-absent clean: 99.670586%, 290-byte boundary, exact 4/4 CFG, 18 sites.
// The absent arm changes `sub esp, 0x34` to `sub esp, 0x20` and shifts every
// message/this slot by NAME_SIZE. No absent-buffer state closes; its two
// observed orbits score 99.670586% and 99.623530%. Keep the dead buffer in
// PoL: it is required 2.0 frame evidence, not portable Buka cleanup.
