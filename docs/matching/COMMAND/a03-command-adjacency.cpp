// VC6 SP5 COMMAND source family a03-command-adjacency; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/a03-command-adjacency/results.json; RVA 0x2bc31
// Complete 3 arms; elapsed 2.987273s; input restored.
// Disposition: retain all removals (row_index for A03); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'actual_row_owner': 'keep', 'candidate': 'baseline'} / 1880 / e6b25349d7e78466 / 643c03d47bd4ccb4 / 130:130 / 130,0,0,0 / True
// 1 / {'actual_row_owner': 'row_index', 'candidate': 'baseline'} / 1880 / e6b25349d7e78466 / 643c03d47bd4ccb4 / 130:130 / 130,0,0,0 / True
// 2 / {'actual_row_owner': 'row_pointer', 'candidate': 'baseline'} / 1880 / e6b25349d7e78466 / 643c03d47bd4ccb4 / 130:130 / 130,0,0,0 / True

// AXIS actual_row_owner
// BEFORE (complete exact span):
// | *(&m_adjacency[0][0] + targetHex * COMBAT_DIRECTION_ADJACENT_COUNT + direction_28)
// ALTERNATIVE row_index (unified source delta from BEFORE):
// | --- before
// | +++ row_index
// | @@ -1 +1 @@
// | -*(&m_adjacency[0][0] + targetHex * COMBAT_DIRECTION_ADJACENT_COUNT + direction_28)
// | +m_adjacency[targetHex][direction_28]
// ALTERNATIVE row_pointer (unified source delta from BEFORE):
// | --- before
// | +++ row_pointer
// | @@ -1 +1 @@
// | -*(&m_adjacency[0][0] + targetHex * COMBAT_DIRECTION_ADJACENT_COUNT + direction_28)
// | +*(m_adjacency[targetHex] + direction_28)
