// VC6 SP5 COMMAND source family c68-surrender-portrait; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c68-surrender-portrait/results.json; RVA 0x3049b
// Complete 2 arms; elapsed 2.173562s; input restored.
// Disposition: retain all removals (row_index for A03); combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'byte_to_vararg': 'keep', 'candidate': 'baseline'} / 957 / 77258fa625f3d2e8 / 69148334ec93a97c / 24:24 / 24,0,0,0 / True
// 1 / {'byte_to_vararg': 'remove', 'candidate': 'baseline'} / 957 / 77258fa625f3d2e8 / 53373581e6ac68b5 / 24:24 / 24,0,0,0 / True

// AXIS byte_to_vararg
// BEFORE (complete exact span):
// | static_cast<i32>(m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_portrait)
// ALTERNATIVE remove (unified source delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +1 @@
// | -static_cast<i32>(m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_portrait)
// | +m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_portrait
