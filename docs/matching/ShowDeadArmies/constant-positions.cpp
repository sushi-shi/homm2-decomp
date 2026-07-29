// combatManager::ShowDeadArmies (SOURCE/COMMAND, RVA 0x2e2bf) — 2026-07-29
// MAX 99.0052 -> 99.9793
//
// Matrices: build/sda-axes.json, build/sda2-axes.json, build/sda3-axes.json
// -> build/source-variant-batch/sda{,2,3}.
//
// 1. center_cast plain_int (integrated): the icon-centering term has NO i16
//    truncation; the static_cast<i16> emitted a retail-absent movsx eax,ax.
// 2. Constant POSITIONS (integrated): retail's icon X is
//    `armyIndex*spacing + startX + 16 - x + (32-w)/2 + 1` and icon Y is
//    `(y + 19 - y' - h') + 32`. Our folded 17 and 51 tail constants were the
//    same values summed: MSVC 4.2 folds only ADJACENT constant terms, so the
//    retail bytes pin WHERE each constant sits in the expression. This is the
//    general mechanism behind HeroQuickView's open `add 0x1e; add 0x6` pair —
//    the 6 belongs at a different position in that expression, not beside 30.
//    Named as CASUALTY_ICON_CELL_WIDTH / 2, + 1 (the TownQuickView nudge
//    idiom), CASUALTY_ICON_TOP_OFFSET (19), CASUALTY_ICON_CELL_WIDTH.
// 3. loss_check mirror (quantity vs initialQuantity): byte-neutral both
//    spellings — at-state; kept the retail-true `m_quantity < m_initialQuantity`.
//
// Residual: that single collection-loop cmp mirror; island-only.
