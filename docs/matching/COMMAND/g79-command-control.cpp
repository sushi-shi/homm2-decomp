// VC6 SP5 COMMAND source family g79-command-control; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g79-command-control/results.json; RVA 0x30ab0
// Complete 2 arms; elapsed 2.069953s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'structured_control': 'keep', 'candidate': 'baseline'} / 335 / 5eee721945a9bf22 / 6859b43906411448 / 18:18 / 18,0,0,0 / True
// 1 / {'structured_control': 'if_else_control', 'candidate': 'baseline'} / 328 / f8f56fbf1c6ab42d / 1a173c91ddf2be6c / 16:18 / 14,0,1,1 / False

// AXIS structured_control
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
// ALTERNATIVE if_else_control (unified source delta from BEFORE):
// | --- before
// | +++ if_else_control
// | @@ -12,15 +12,12 @@
// |              && (gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0
// |                  || m_playerId[IDX(COMBAT_DEFENDER_SIDE)] == 0))) {
// |          gbThisNetHasControl = true;
// | -        goto setup_view;
// | -    }
// | -    if (m_playerId[IDX(m_currentSide)] != -1 && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0
// | +    } else if (m_playerId[IDX(m_currentSide)] != -1 && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0
// |          && gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0)
// |          gbThisNetHasControl = false;
// |      else
// |          gbThisNetHasControl = true;
// |
// | -setup_view:
// |      m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
// |      SetupSmallView();
// |      ResetMouse();
