// InitMenuHandler (SOURCE/KB, RVA 0x98d2d)
//
// Unchanged-source census:
//   build/init-menu-handler-cross-version-state-summary.json
//   build/tu-state-noise/init-menu-handler-cross-version
// All clean-plus-50 forest/top states emitted one identical 1688-byte object
// at 99.826760%, with ordered 89/89 relocation owners.
//
// Complete source/state product (204/204):
//   build/init-menu-handler-cross-version-axes.json
//   build/source-variant-batch/init-menu-handler-cross-version
// Axes:
//   1. PoL `hotspot <= mouse` vs Buka `mouse >= hotspot` for X and Y.
//      Byte-neutral in every state.
//   2. PoL `BaseWidgetCommand(MENU_CLOSE_COMMAND)` vs Buka's literal
//      `WIDGET_COMMAND_DIALOG_SELECT`.
//      Buka arm is 1686 bytes and 99.584690%; rejected.
//
// Buka's one-argument PlayAmbientMusic call belongs to its different sound
// backend API and was excluded from the shared-source product.
//
// Disposition: no source port. The menu logic and high-level CFG are shared,
// but PoL retains its helper-owned close command. The remaining switch/jump-
// table residual is compiler/build-state shape, not a gameplay difference.
