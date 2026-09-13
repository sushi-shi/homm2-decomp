// VC6 SP5 COMMAND source family s62-small-view; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-small-view/results.json; RVA 0x32338
// Complete 4 arms; elapsed 3.457656s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 317 / 42b86bed6f8173b0 / b437679743b535a7 / 17:17 / 17,0,0,0 / True
// 1 / {'local_role_subset': 'word2', 'candidate': 'baseline'} / 317 / 2d4303a83c2ec23c / b437679743b535a7 / 17:17 / 17,0,0,0 / True
// 2 / {'local_role_subset': 'word1', 'candidate': 'baseline'} / 317 / 2d4303a83c2ec23c / b437679743b535a7 / 17:17 / 17,0,0,0 / True
// 3 / {'local_role_subset': 'word1_word2', 'candidate': 'baseline'} / 315 / 346d3bf7131308da / 4877e20b03840f98 / 17:17 / 17,0,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::SetupSmallView(void) {
// |     i32 H2_UNUSED(unusedSmallViewWord1);
// |     i32 H2_UNUSED(unusedSmallViewWord2);
// |
// |     if (m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
// |         && m_smallViewLastX[IDX(COMBAT_DEFENDER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
// |         DrawSmallView(IDX(COMBAT_DEFENDER_SIDE), 1);
// |     }
// |
// |     if (gbThisNetHasControl != 0 && m_playerId[IDX(m_currentSide)] != PLAYER_NONE
// |         && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0) {
// |         if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != m_currentSide
// |             || m_smallViewArmyIndex[IDX(COMBAT_ATTACKER_SIDE)] != m_currentArmyIndex) {
// |             if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
// |                 && m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != m_currentSide
// |                 && m_smallViewLastX[IDX(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
// |                 m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
// |                 DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
// |             }
// |             m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_currentSide;
// |             m_smallViewArmyIndex[IDX(COMBAT_ATTACKER_SIDE)] = m_currentArmyIndex;
// |             DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
// |         }
// |     } else {
// |         m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
// |         if (m_smallViewLastX[IDX(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE)
// |             DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
// |     }
// | }
// ALTERNATIVE word2 (unified source delta from BEFORE):
// | --- before
// | +++ word2
// | @@ -1,6 +1,5 @@
// |  void combatManager::SetupSmallView(void) {
// |      i32 H2_UNUSED(unusedSmallViewWord1);
// | -    i32 H2_UNUSED(unusedSmallViewWord2);
// |
// |      if (m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
// |          && m_smallViewLastX[IDX(COMBAT_DEFENDER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
// ALTERNATIVE word1 (unified source delta from BEFORE):
// | --- before
// | +++ word1
// | @@ -1,5 +1,4 @@
// |  void combatManager::SetupSmallView(void) {
// | -    i32 H2_UNUSED(unusedSmallViewWord1);
// |      i32 H2_UNUSED(unusedSmallViewWord2);
// |
// |      if (m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
// ALTERNATIVE word1_word2 (unified source delta from BEFORE):
// | --- before
// | +++ word1_word2
// | @@ -1,6 +1,4 @@
// |  void combatManager::SetupSmallView(void) {
// | -    i32 H2_UNUSED(unusedSmallViewWord1);
// | -    i32 H2_UNUSED(unusedSmallViewWord2);
// |
// |      if (m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
// |          && m_smallViewLastX[IDX(COMBAT_DEFENDER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
