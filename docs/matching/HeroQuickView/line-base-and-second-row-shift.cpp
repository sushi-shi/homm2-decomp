// advManager::HeroQuickView (SOURCE/ADVMGR, RVA 0x6235b) — 2026-07-29 matrices
//
// Matrix 1: build/hqv-axes.json -> build/source-variant-batch/hqv.
// line_base static_read (WINNER, integrated): the three H2_ALLOC_AT line
// arguments read s_quickViewLineBase (movsx + add 0x9b/0xe3/0x10e); the tree
// folded QUICK_VIEW_LINE_BASE literals. 99.11 -> 99.79 clean, 101/101 blocks,
// zero topology deltas; banked 99.87 by island replay.
//
// Matrix 2: build/hqv2-axes.json -> build/source-variant-batch/hqv2, OPEN.
// The last residual is one size_only block: retail emits the second-row icon
// Y shift as `add ebx,0x1e; add ebx,0x6` (unfolded 30 + 6) while our
// `+ ARMY_QUICK_ICON_BASELINE + ARMY_QUICK_SECOND_ROW_ICON_SHIFT` folds to
// `add ebx,0x24`. Measured rejections: static_cast<i32> barrier, IDX()
// wrapper, term order swap — all still fold (or score no better).
// UNTRIED: a volatile-free lvalue barrier (e.g. the shift kept in an i16
// local like the /Gi line-base statics), or the +6 originating from a
// different semantic term entirely; needs retail-context reasoning.
