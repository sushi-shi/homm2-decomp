// army::ValidFlight (SOURCE/FLY, RVA 0xa5b95) — 2026-07-29 -> EXACT 100
//
// Matrices: build/vflight-axes.json, build/vflight2-axes.json ->
// build/source-variant-batch/vflight{,2}.
//
// 1. Four returning branches wrap their fall-through in explicit `else`
//    (the if/else-both-return two-jmp class): the CanFit guard and all three
//    search-loop exits. 97.80 -> 99.20, retail size restored.
// 2. direction4/adjacentHex1 both hash to bucket 5 — retail walks adjacentHex1
//    first, fixed purely by declaring direction4 earlier.
// 3. The CanFit "exact target" flag is retail's arithmetic `1 - pathMode`
//    (mov eax,1; sub), not `pathMode == ARMY_PATH_ANY_TARGET_HEX` (cmp/sbb/neg)
//    — and it differs semantically for COMBAT_AI_PATH_TO_TARGET (-1), where
//    retail passes 2. Audited exact at the clean state, 18/18 relocs.
//
// Sibling FlyTo (98.73) needs a full frame solve: retail frame 0xc0 vs our
// 0xb4 — three extra dead dword locals plus a slot scramble. Queued.
//
// RandomizeMine (SOURCE/GAME, 0x7f5f1): de-inlined the four RandomMineType
// sites to static_cast<MineType>(Random(IDX(a), IDX(b))) — retail has no
// continuation stubs there (98.7 -> 99.51 banked). Remaining rows: one
// byte-RMW/movsx idiom at the m_objType |= 1 site (m_objTypeBits arm proved
// byte-neutral; at-state or a different lvalue) plus jump-table decode noise.
