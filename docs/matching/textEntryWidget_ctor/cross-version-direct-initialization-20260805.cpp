/*
 * Cross-version exact closure for the parameterized textEntryWidget ctor,
 * BASE/Textntry RVA 0x000d87b0.
 *
 * The exact donor directly publishes m_icon, m_iconFrame, and rectangle
 * fields. The prior source introduced loadedIcon and rectX caches and delayed
 * m_iconFrame until after unrelated flag stores. Retail proves the donor
 * publication order.
 *
 * Complete matrix: current cached and mainline direct structures crossed with
 * clean plus 50 forest/top states = 102/102 cells:
 *
 *   build/text-entry-ctor-order-manifest.json
 *   build/source-variant-batch/text-entry-ctor-order/results.json
 *
 * The direct donor arm is clean-state exact: 308/308 bytes, exact 3/3 CFG,
 * complete ordered 6/6 relocations. Source hash df7b44566fe0; live 100.0000%
 * retained.
 */

// The full retained implementation is in src/BASE/Textntry.cpp. Its initial
// semantic phase is the direct field-publication sequence from the donor.
