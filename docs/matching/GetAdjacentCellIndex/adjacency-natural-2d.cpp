// army::GetAdjacentCellIndex + GetAdjacentCellIndexNoArmy (SOURCE/PATH,
// RVAs 0xbe48a / 0xbe52f) — 2026-07-29 matrices -> both MAX 100
//
// Matrices: build/gaci-axes.json, build/gacina-axes.json ->
// build/source-variant-batch/gaci{,na} (3 arms x {clean + 8 forest states}).
//
// The tree carried reversed-subscript spellings
// (IDX(direction)[m_adjacency[0] + hex * COUNT] and
// IDX(direction)[m_adjacency[hex]]); the natural 2D access
// gpCombatManager->m_adjacency[sourceHex][IDX(direction)] reached audited
// exact under forest states for both functions (for the NoArmy sibling every
// spelling was byte-identical, so the clean 2D form was kept). Banked
// 93.10 -> 100.00 and 91.10 -> 100.00 by official replays
// (build/tu-state-noise/path-a, path-b).
