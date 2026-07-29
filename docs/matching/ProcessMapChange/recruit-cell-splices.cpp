// advManager::ProcessMapChange (SOURCE/CURSOR, RVA 0x10152) — 2026-07-29
// -> EXACT 100 (was 99.59)
//
// Matrix: build/pmc-axes.json -> build/source-variant-batch/pmc
// (4 arms x {clean + 8 forest states}, complete).
//
// The MAP_CHANGE_RECRUIT_HERO case binds four map cells. The two POINTER
// BINDINGS (recruitCell_i, heroCell_i) splice
// `gpGame->m_worldMap.cells + width * change.y + change.x` textually (no
// /Ob1 continuation stub); the two READ-THROUGH sites (m_locationType,
// m_occupiedTown) keep the fullMap::GetCell inline — the accessor
// read/write asymmetry from the canonicalization catalog, decided per SITE.
// splice_CD closed the function at the clean state: zero differing rows,
// 98/98 relocs, retail size 1871.
