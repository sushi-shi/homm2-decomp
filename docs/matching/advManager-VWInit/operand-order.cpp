// Function: advManager::VWInit
// Retail RVA/size: 0x000ae2bb / 0x0233
//
// Reviewed source-shape matrix:
//   build/vwinit-operand-order-axes.json
//   build/vwinit-operand-order-manifest.json
//   build/source-variant-batch/vwinit-operand-order/results.json
//
// COMPLETE: 4/4 clean-state variants, no truncation, source restored.
// The two independent operand-order axes produced:
//
//   MAP_WIDTH first, scale first:           99.834710%
//   MAP_WIDTH first, viewable-cells first:  99.917360%
//   viewable-cells first, scale first:       99.917360%
//   viewable-cells first for both:          100.000000%
//
// Every arm retained size 563 and 79 relocations. The accepted arm has the
// exact 17-block CFG and complete ordered relocation identity at 79/79 sites.
// Disposition: retain the unique exact source spelling below; generated source
// and compiler-state declarations were not retained.

// Accepted expressions:
//
//     if (iVWViewableCells != MAP_WIDTH) {
//
//     iVWXPixelOffset =
//         (WORLD_WINDOW_X - IDX(iVWViewableCells) * IDX(giViewWorldScale)) >> 1;
