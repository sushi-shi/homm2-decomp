// ViewSpellsHandler (SOURCE/GAME, RVA 0x79e3b)
//
// Unchanged-source census:
//   build/view-spells-handler-cross-version-state-summary.json
//   build/tu-state-noise/view-spells-handler-cross-version
// All clean-plus-50 forest/top probes emit one 1683-byte PoL object at
// 99.784110%, with 101/101 relocation entries.
//
// Hover comparison product (102/102):
//   build/view-spells-handler-hover-axes.json
//   build/source-variant-batch/view-spells-handler-hover
// PoL `msg.id == lastHoverId` and Buka `lastHoverId == msg.id` are byte-neutral
// under VC4.2 in every state; no source change retained.
//
// Previous-page behavior product (102/102):
//   build/view-spells-handler-refresh-axes.json
//   build/source-variant-batch/view-spells-handler-refresh
// PoL exits immediately when the page is already at zero. Buka additionally
// calls UpdateSpellWidgets() and MoveWindow(0, 0). The Buka arm is 1714 bytes,
// has 105 relocations, and falls to 99.070270%; PoL remains 1683 bytes,
// 101 relocations, and 99.784110%.
//
// Disposition: no source port. The extra refresh is a proven later behavior
// change; the remaining PoL residual is compiler/jump-table ownership.
