// combatManager::ChainLightning (SOURCE/SPELLS, RVA 0x2685a)
//
// Baseline: live 100.000000%, 865 bytes, exact 23/23 CFG and ordered 37/37
// relocations. Buka is independently exact at RVA 0x9df96.
//
// Cross-version source product (16/16, complete):
//   build/chain-lightning-cross-version-axes.json
//   build/source-variant-batch/chain-lightning-cross-version
// Dimensions: force-angle ternary/if-else, direct/equal-arm bolt-angle args,
// and both timing-product orders. All combinations have identical masked bytes,
// 865-byte size, 23-block graph and 37 relocation entries under VC4.2.
//
// The angle arm has independent PoL evidence: firstBolt is stored as 1 and 0
// but was otherwise unread. Retaining equal-arm ternaries with PoL's existing
// angle constants preserves direct normalized-build exactness and 37/37 ordered
// relocations. Adding Buka's two separate equal-valued enum declarations changes
// VC4.2 TU state and regresses the live object to 99.98%, so they stay Buka-only.
// That clean-source rebuild also raised current-hash MAX for DoEvent to
// 96.7877%, SeedCombatPosition to 99.3987%, and DetermineEffectOfSpell to
// 98.6294%; the root-generated monotonic ratchets are retained.
//
// Disposition: retain only the source-level firstBolt use. No behavior changes;
// force-angle CFG and timing-product spellings stay in their retail-specific forms.
