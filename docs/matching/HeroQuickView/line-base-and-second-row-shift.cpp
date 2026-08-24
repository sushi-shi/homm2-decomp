// advManager::HeroQuickView (SOURCE/ADVMGR, RVA 0x6235b)
//
// Matrix 1: build/hqv-axes.json -> build/source-variant-batch/hqv.
// line_base static_read (WINNER, integrated): the three H2_ALLOC_AT line
// arguments read s_quickViewLineBase (movsx + add 0x9b/0xe3/0x10e); the tree
// folded QUICK_VIEW_LINE_BASE literals. 99.11 -> 99.79 clean, 101/101 blocks,
// zero topology deltas; banked 99.87 by island replay.
//
// Matrix 2: build/hero-quick-view-buka-y-axes.json ->
// build/source-variant-batch/hero-quick-view-buka-y (102/102 complete).
// Buka's exact `+ (BASELINE - entry->h) + SECOND_ROW_SHIFT` parent is the
// winner and is retained. Clean VC4.2 rises 99.715164% -> 99.839710%; forest/
// top seed 1213156658 trial 22 reaches 99.992600%, retail size 3378, exact
// 101/101 blocks and ordered 109/109 relocations. This recovers retail's
// separate `add 0x1e; add 0x6` instructions.
//
// Matrix 3: build/hero-quick-view-buka-placement-axes.json ->
// build/source-variant-batch/hero-quick-view-buka-placement (184/184 complete).
// Both Buka top-row `base + stride * index` spellings and the reversed
// second-row bound sum are byte-neutral in every requested VC4.2 state. Trial
// 22's remaining 20 raw bytes are three commutative operand-load islands; the
// instruction/CFG frontier and owner-relative relocation stream are exact.
//
// Disposition: retain only the Buka second-row Y grouping. The function logic
// is invariant; the residual and the non-retained placement spellings are
// compiler-state differences, not 2.0 gameplay changes.
