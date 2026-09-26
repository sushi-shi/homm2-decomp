// VC6 SP5 COMMAND source family g76-command-main; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g76-command-main/results.json; RVA 0x2b850
// Complete 2 arms; elapsed 2.088324s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'structured_control': 'keep', 'candidate': 'baseline'} / 702 / 9cbc6037ab587b54 / 056126be7995f482 / 46:46 / 46,0,0,0 / True
// 1 / {'structured_control': 'single_dispatch_pass', 'candidate': 'baseline'} / 705 / 1399d297ccf61422 / 1f7ded5a7b446c7a / 46:46 / 35,1,3,7 / False

// AXIS structured_control
// BEFORE (complete exact span):
// | MessageDispatchResult combatManager::Main(tag_message& message) {
// |     MessageDispatchResult result = MESSAGE_DISPATCH_CONSUME;
// |     army* currentArmy;
// |
// |     if (gbNoShowCombat == 0) {
// |         if (glTimers[0] < KBTickCount()) {
// |             PollSound();
// |             glTimers[0] = static_cast<i32>(
// |                 KBTickCount() + COMBAT_SOUND_POLL_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
// |             );
// |         }
// |         if (glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] < KBTickCount()
// |             && gbProcessingCombatAction == 0) {
// |             gbProcessingCombatAction = true;
// |             CycleCombatScreen();
// |             gbProcessingCombatAction = false;
// |         }
// |     }
// |
// |     CheckCastleAttack();
// |     if (CheckWin(&message) != 0)
// |         return MESSAGE_DISPATCH_FORWARD;
// |
// |     if (gbNoShowCombat == 0) {
// |         CombatRemotePacket* packet =
// |             reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
// |         if (packet != NULL && packet->type == REMOTE_MESSAGE_RELIABLE) {
// |             switch (packet->command) {
// |                 case REMOTE_COMMAND_ACTION:
// |                     giNextAction = packet->nextAction;
// |                     giNextActionExtra = packet->nextActionExtra;
// |                     giNextActionGridIndex = packet->nextActionGridIndex;
// |                     giNextActionGridIndex2 = packet->nextActionGridIndex2;
// |                     goto ProcessAction;
// |                 case REMOTE_COMMAND_MESSAGE:
// |                     PopNetBox(packet->text, packet->sender);
// |                     break;
// |             }
// |         }
// |
// |         if (gbThisNetHasControl == 0) {
// |             if (message.type == MESSAGE_KEY_DOWN) {
// |                 switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
// |                     case KEY_CLOSE_NETWORK_BOX:
// |                         PopNetBox(NULL, -1);
// |                         break;
// |                 }
// |             }
// |             return MESSAGE_DISPATCH_CONSUME;
// |         }
// |     }
// |
// |     currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |     if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
// |         currentArmy->GoBerserk();
// |         if (CheckWin(&message) != 0)
// |             return MESSAGE_DISPATCH_FORWARD;
// |     }
// |
// |     if (gbNoShowCombat == 0) {
// |         if (m_gridSelectionDisabled != 0) {
// |             while (message.type != MESSAGE_KEY_DOWN && message.type != MESSAGE_LEFT_BUTTON_DOWN
// |                    && message.type != MESSAGE_RIGHT_BUTTON_DOWN && message.type != MESSAGE_NONE) {
// |                 message = gpInputManager->GetEvent();
// |             }
// |             if (message.type != MESSAGE_NONE)
// |                 m_gridSelectionDisabled = false;
// |         }
// |         CheckChangeSelector();
// |     }
// |
// | ProcessAction:
// |     if (giNextAction == ACTION_NONE) {
// |         if (m_playerId[IDX(m_currentSide)] == -1
// |             || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
// |             || m_gridSelectionDisabled != 0) {
// |             CheckGetAIMove();
// |         } else {
// |             result = ProcessCombatMsg(message);
// |         }
// |     }
// |     if (giNextAction != ACTION_NONE)
// |         result = ProcessNextAction(message);
// |     return result;
// | }
// ALTERNATIVE single_dispatch_pass (unified source delta from BEFORE):
// | --- before
// | +++ single_dispatch_pass
// | @@ -21,55 +21,57 @@
// |      if (CheckWin(&message) != 0)
// |          return MESSAGE_DISPATCH_FORWARD;
// |
// | -    if (gbNoShowCombat == 0) {
// | -        CombatRemotePacket* packet =
// | -            reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
// | -        if (packet != NULL && packet->type == REMOTE_MESSAGE_RELIABLE) {
// | -            switch (packet->command) {
// | -                case REMOTE_COMMAND_ACTION:
// | -                    giNextAction = packet->nextAction;
// | -                    giNextActionExtra = packet->nextActionExtra;
// | -                    giNextActionGridIndex = packet->nextActionGridIndex;
// | -                    giNextActionGridIndex2 = packet->nextActionGridIndex2;
// | -                    goto ProcessAction;
// | -                case REMOTE_COMMAND_MESSAGE:
// | -                    PopNetBox(packet->text, packet->sender);
// | -                    break;
// | +    do {
// | +        if (gbNoShowCombat == 0) {
// | +            CombatRemotePacket* packet =
// | +                reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
// | +            if (packet != NULL && packet->type == REMOTE_MESSAGE_RELIABLE) {
// | +                switch (packet->command) {
// | +                    case REMOTE_COMMAND_ACTION:
// | +                        giNextAction = packet->nextAction;
// | +                        giNextActionExtra = packet->nextActionExtra;
// | +                        giNextActionGridIndex = packet->nextActionGridIndex;
// | +                        giNextActionGridIndex2 = packet->nextActionGridIndex2;
// | +                        continue;
// | +                    case REMOTE_COMMAND_MESSAGE:
// | +                        PopNetBox(packet->text, packet->sender);
// | +                        break;
// | +                }
// | +            }
// | +
// | +            if (gbThisNetHasControl == 0) {
// | +                if (message.type == MESSAGE_KEY_DOWN) {
// | +                    switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
// | +                        case KEY_CLOSE_NETWORK_BOX:
// | +                            PopNetBox(NULL, -1);
// | +                            break;
// | +                    }
// | +                }
// | +                return MESSAGE_DISPATCH_CONSUME;
// |              }
// |          }
// |
// | -        if (gbThisNetHasControl == 0) {
// | -            if (message.type == MESSAGE_KEY_DOWN) {
// | -                switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
// | -                    case KEY_CLOSE_NETWORK_BOX:
// | -                        PopNetBox(NULL, -1);
// | -                        break;
// | +        currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | +        if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
// | +            currentArmy->GoBerserk();
// | +            if (CheckWin(&message) != 0)
// | +                return MESSAGE_DISPATCH_FORWARD;
// | +        }
// | +
// | +        if (gbNoShowCombat == 0) {
// | +            if (m_gridSelectionDisabled != 0) {
// | +                while (message.type != MESSAGE_KEY_DOWN && message.type != MESSAGE_LEFT_BUTTON_DOWN
// | +                       && message.type != MESSAGE_RIGHT_BUTTON_DOWN && message.type != MESSAGE_NONE) {
// | +                    message = gpInputManager->GetEvent();
// |                  }
// | +                if (message.type != MESSAGE_NONE)
// | +                    m_gridSelectionDisabled = false;
// |              }
// | -            return MESSAGE_DISPATCH_CONSUME;
// | +            CheckChangeSelector();
// |          }
// | -    }
// |
// | -    currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | -    if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
// | -        currentArmy->GoBerserk();
// | -        if (CheckWin(&message) != 0)
// | -            return MESSAGE_DISPATCH_FORWARD;
// | -    }
// | +    } while (false);
// |
// | -    if (gbNoShowCombat == 0) {
// | -        if (m_gridSelectionDisabled != 0) {
// | -            while (message.type != MESSAGE_KEY_DOWN && message.type != MESSAGE_LEFT_BUTTON_DOWN
// | -                   && message.type != MESSAGE_RIGHT_BUTTON_DOWN && message.type != MESSAGE_NONE) {
// | -                message = gpInputManager->GetEvent();
// | -            }
// | -            if (message.type != MESSAGE_NONE)
// | -                m_gridSelectionDisabled = false;
// | -        }
// | -        CheckChangeSelector();
// | -    }
// | -
// | -ProcessAction:
// |      if (giNextAction == ACTION_NONE) {
// |          if (m_playerId[IDX(m_currentSide)] == -1
// |              || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
