// VC6 SP5 source family s69-newgame-update; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/s69-newgame-update/results.json; RVA 0x76e3b
// Complete 2 arms; elapsed 1.861029s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1295 / 431eaddd7ff55a6d / dee21ed9ddd159bb / 35:35 / 35,0,0,0 / True
// 1 / {'family': 'remove_1', 'candidate': 'baseline'} / 1295 / cd455c5363ea88c0 / dee21ed9ddd159bb / 35:35 / 35,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x50f)
// |     void game::UpdateNewGameWindow(void) {
// |         b32 playerLockedValue;
// |         tag_message message;
// |         i32 playerIndex;
// |         i32 H2_UNUSED(unusedPlayer17);
// |
// |         strcpy(gText, m_mapHeader.name);
// |         message.type = MESSAGE_WIDGET;
// |         message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |         message.payload.widget.id = NEW_GAME_SCENARIO_NAME;
// |         message.payload.widget.data.text = gText;
// |         m_newGameWindow->BroadcastMessage(message);
// |
// |         message.payload.widget.command = NEW_GAME_WIDGET_DISABLE;
// |         message.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
// |         for (playerIndex = 0; playerIndex < IDX(DIFFICULTY_COUNT); ++playerIndex) {
// |             message.payload.widget.id = NEW_GAME_DIFFICULTY_FIRST + playerIndex;
// |             m_newGameWindow->BroadcastMessage(message);
// |         }
// |         message.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
// |         message.payload.widget.id = NEW_GAME_DIFFICULTY_FIRST + IDX(m_difficulty);
// |         m_newGameWindow->BroadcastMessage(message);
// |
// |         if (giNumHumanPlayers > 1) {
// |             for (playerIndex = 0; playerIndex < GAME_CHAT_LINE_COUNT; ++playerIndex) {
// |                 sprintf(gText, cTextReceivedBuffer[playerIndex]);
// |                 message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |                 message.payload.widget.id = NEW_GAME_CHAT_FIRST + playerIndex;
// |                 message.payload.widget.data.text = gText;
// |                 m_newGameWindow->BroadcastMessage(message);
// |             }
// |         }
// |
// |         for (playerIndex = 0; playerIndex < m_mapHeader.playerCount; ++playerIndex) {
// |             if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER) {
// |                 sprintf(
// |                     gText,
// |                     ""
// |                 );
// |             } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerIndex]]) > 0) {
// |                 sprintf(gText, cPlayerNames[m_setupPlayerNetworkId[playerIndex]]);
// |             } else {
// |                 sprintf(gText, "\xc8\xe3\xf0\xee\xea %d", m_setupPlayerNetworkId[playerIndex] + 1);
// |             }
// |             message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |             message.payload.widget.id = NEW_GAME_PLAYER_NAME_FIRST + playerIndex;
// |             message.payload.widget.data.text = gText;
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             message.payload.widget.command = playerIndex == m_selectedSetupPlayer
// |                                                  ? NEW_GAME_WIDGET_ENABLE
// |                                                  : NEW_GAME_WIDGET_DISABLE;
// |             message.payload.widget.id = NEW_GAME_PLAYER_SELECT_FIRST + playerIndex;
// |             message.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             if (m_setupPlayerType[playerIndex] != GAME_PLAYER_DEFAULT
// |                 || (giNumHumanPlayers > 1
// |                     && m_setupPlayerNetworkId[playerIndex] != GAME_COMPUTER_PLAYER))
// |                 playerLockedValue = false;
// |             else
// |                 playerLockedValue = true;
// |             message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |             message.payload.widget.id = NEW_GAME_COLOR_FIRST + playerIndex;
// |             if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER)
// |                 message.payload.widget.data.value =
// |                     (playerLockedValue ? GAME_COMPUTER_COLOR_LOCKED_FRAME
// |                                        : GAME_COMPUTER_COLOR_UNLOCKED_FRAME)
// |                     + m_setupPlayerColor[playerIndex];
// |             else
// |                 message.payload.widget.data.value =
// |                     (playerLockedValue ? GAME_HUMAN_COLOR_LOCKED_FRAME
// |                                        : GAME_HUMAN_COLOR_UNLOCKED_FRAME)
// |                     + m_setupPlayerColor[playerIndex];
// |             if (giNumHumanPlayers > 1)
// |                 message.payload.widget.data.value += GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             message.payload.widget.command =
// |                 playerLockedValue ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
// |             message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |             message.payload.widget.id = NEW_GAME_HANDICAP_FIRST + playerIndex;
// |             if (m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER)
// |                 message.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
// |             else
// |                 message.payload.widget.data.value = IDX(m_playerHandicap[playerIndex]);
// |             m_newGameWindow->BroadcastMessage(message);
// |             message.payload.widget.command =
// |                 m_setupPlayerNetworkId[playerIndex] == GAME_COMPUTER_PLAYER
// |                     ? NEW_GAME_WIDGET_DISABLE
// |                     : NEW_GAME_WIDGET_ENABLE;
// |             message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             if (m_mapHeader.playerRace[m_setupPlayerColor[playerIndex]] == FACTION_RANDOM)
// |                 playerLockedValue = false;
// |             else
// |                 playerLockedValue = true;
// |             message.payload.widget.command = NEW_GAME_WIDGET_ENABLE;
// |             message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |             m_newGameWindow->BroadcastMessage(message);
// |             message.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |             message.payload.widget.id = NEW_GAME_RACE_CYCLE_FIRST + playerIndex;
// |             message.payload.widget.data.value =
// |                 (playerLockedValue ? GAME_FIXED_RACE_FRAME_BASE : GAME_RANDOM_RACE_FRAME_BASE)
// |                 + IDX(m_setupPlayerRace[playerIndex]);
// |             m_newGameWindow->BroadcastMessage(message);
// |
// |             sprintf(gText, gAlignmentNames[IDX(m_setupPlayerRace[playerIndex])]);
// |             message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |             message.payload.widget.id = NEW_GAME_RACE_NAME_FIRST + playerIndex;
// |             message.payload.widget.data.text = gText;
// |             m_newGameWindow->BroadcastMessage(message);
// |             message.payload.widget.command =
// |                 playerLockedValue ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
// |             message.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |             m_newGameWindow->BroadcastMessage(message);
// |         }
// |
// |         gpGame->m_difficultyRating = static_cast<i16>(CalcDifficultyRating());
// |         message.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |         message.payload.widget.id = NEW_GAME_RATING;
// |         sprintf(gText, "%s %d%%", "\xd0\xe5\xe9\xf2\xe8\xed\xe3", gpGame->m_difficultyRating);
// |         message.payload.widget.data.text = gText;
// |         m_newGameWindow->BroadcastMessage(message);
// |         DrawNGKPDisplayString(0);
// |     }
// |
// ALTERNATIVE remove_1 (unified delta from BEFORE):
// | --- before
// | +++ remove_1
// | @@ -3,7 +3,6 @@
// |          b32 playerLockedValue;
// |          tag_message message;
// |          i32 playerIndex;
// | -        i32 H2_UNUSED(unusedPlayer17);
// |
// |          strcpy(gText, m_mapHeader.name);
// |          message.type = MESSAGE_WIDGET;
