// VC6 SP5 COMMAND source family s62-control-store; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-control-store/results.json; RVA 0x30ab0
// Complete 2 arms; elapsed 2.007266s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'duplicate_store': 'keep', 'candidate': 'baseline'} / 335 / 5eee721945a9bf22 / 6859b43906411448 / 18:18 / 18,0,0,0 / True
// 1 / {'duplicate_store': 'single_assignment', 'candidate': 'baseline'} / 323 / fed29792e818ad2d / 1db8ad78afdab5a7 / 18:18 / 17,1,0,0 / True

// AXIS duplicate_store
// BEFORE (complete exact span):
// | void combatManager::GetControl(void) {
// |     m_selectedHex = INVALID_HEX;
// |     m_previousCommand = COMBAT_INVALID_COMMAND;
// |     m_previousCommand = COMBAT_INVALID_COMMAND;
// |     if (gpCombatManager->m_active == 1)
// |         gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |     CheckChangeSelector();
// |     if (gbRemoteOn == 0 || m_playerId[IDX(COMBAT_ATTACKER_SIDE)] < 0
// |         || m_playerId[IDX(COMBAT_DEFENDER_SIDE)] < 0
// |         || gbHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] == 0
// |         || (gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] == 0
// |             && (gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0
// |                 || m_playerId[IDX(COMBAT_DEFENDER_SIDE)] == 0))) {
// |         gbThisNetHasControl = true;
// |         goto setup_view;
// |     }
// |     if (m_playerId[IDX(m_currentSide)] != -1 && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0
// |         && gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0)
// |         gbThisNetHasControl = false;
// |     else
// |         gbThisNetHasControl = true;
// |
// | setup_view:
// |     m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
// |     SetupSmallView();
// |     ResetMouse();
// | }
// ALTERNATIVE single_assignment (unified source delta from BEFORE):
// | --- before
// | +++ single_assignment
// | @@ -1,6 +1,5 @@
// |  void combatManager::GetControl(void) {
// |      m_selectedHex = INVALID_HEX;
// | -    m_previousCommand = COMBAT_INVALID_COMMAND;
// |      m_previousCommand = COMBAT_INVALID_COMMAND;
// |      if (gpCombatManager->m_active == 1)
// |          gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
