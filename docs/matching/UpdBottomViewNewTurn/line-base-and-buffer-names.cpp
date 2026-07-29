// UpdBottomViewNewTurn (SOURCE/ADVMGR, RVA 0x613b0) — 2026-07-29 matrix -> EXACT
//
// Matrix: build/ubvnt-axes.json -> build/source-variant-batch/ubvnt
// (6 arms x {clean + 8 forest states}, complete).
//
// Axis line_base: the two H2_ALLOC_AT line arguments spelled with the
// NEW_TURN_LINE_BASE enum folded to the literal 0x11f9 (was in tree) — but
// retail reads the /Gi s_newTurnLineBase static (movsx + add 0x29/0x43),
// 12 bytes fatter. static_read WINS (see gi-line-var-lowering).
//
// Axis buffer_names (od_slots): retail walks the day buffer (-0xc) before the
// week buffer (-0x10); weekText(6)/dayText(10) violates that, week(9)/day(4)
// and weekStr(8)/dayString(3) both satisfy it. week/day kept.
//
// Winner is 100.0 at the clean state; the raw-side ordered-reloc identity
// difference is the function-local-static naming attribution that reviewed
// target normalization maps (0 differing rows normalized, 45/45 relocs,
// retail size 870). homm2 build recorded MAX 100.
