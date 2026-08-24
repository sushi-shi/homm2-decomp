// advManager::ControlPanel (SOURCE/ADVMGR, RVA 0x6ab9a)
//
// Unchanged-source census:
//   build/control-panel-cross-version-state-summary.json
//   build/tu-state-noise/control-panel-cross-version
// All clean-plus-50 forest/top states emitted one identical 484-byte object at
// 99.808914%, with the retail 20-block CFG and ordered 30/30 relocations.
//
// Complete four-arm source/state product (204/204):
//   build/control-panel-cross-version-axes.json
//   build/source-variant-batch/control-panel-cross-version
// Structural arms: current, Buka outer-scope `tag_message`, Buka `panel` name,
// and both Buka choices. Hoisting the message alone wins every state; the
// `panel` rename is neutral on the old scope and cancels the scope gain on the
// correct outer scope.
//
// Retained result: function-scope `tag_message message`, PoL `controlPanel`
// window name. Live VC4.2 improves 99.808914% -> 99.8217%, remains 484 bytes,
// has the exact 20/20 CFG and ordered 30/30 relocations. The remaining masked-
// operand residual has identical instructions and symbolic branch targets.
//
// Disposition: matching-only invariant scope port; no gameplay logic changes.
