// VC6 SP5 source family s69-newgame-scenario-initializers; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/s69-newgame-scenario-initializers/results.json; RVA 0x7869f
// Complete 4 arms; elapsed 3.097598s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 3126 / cf2bfb85df320da1 / bf67ff05a044140f / 85:85 / 85,0,0,0 / True
// 1 / {'family': 'remove_01', 'candidate': 'baseline'} / 3119 / 728e369112961e4c / 133926f3e4d86ba9 / 85:85 / 84,1,0,0 / True
// 2 / {'family': 'remove_10', 'candidate': 'baseline'} / 3119 / 49c318c11d7bea6c / 133926f3e4d86ba9 / 85:85 / 84,1,0,0 / True
// 3 / {'family': 'remove_11', 'candidate': 'baseline'} / 3112 / bb70e5af5515da17 / 875b9523dd493a47 / 85:85 / 84,1,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0xc36)
// | void game::ShowScenInfo(void) {
// |     i32 columnGap;
// |     i32 availWidth;
// |     i32 firstColumnX;
// |     i32 playerStep;
// |     i32 playerCounter;
// |     i32 mapSize;
// |     b32 locked;
// |     char* name;
// |     i32 yExtra;
// |     i32 raceNameWidth;
// |     tag_message msg;
// |     i32 multiplayerYOffset;
// |     widget* nameWidget;
// |     widget* iconControl;
// |     heroWindow* window;
// |
// |     gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
// |     window = new heroWindow(SCENARIO_WINDOW_X, SCENARIO_WINDOW_Y, "sceninfo.bin");
// |     if (window == NULL)
// |         MemError();
// |     SetWinText(window, GAME_SCENARIO_WINDOW_TEXT_ID);
// |
// |     msg.type = MESSAGE_WIDGET;
// |     msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |     msg.payload.widget.id = NEW_GAME_SCENARIO_NAME;
// |     msg.payload.widget.data.text = m_mapHeader.name;
// |     window->BroadcastMessage(msg);
// |
// |     msg.payload.widget.id = GAME_SCENARIO_DIFFICULTY;
// |     msg.payload.widget.data.text = cDifficulty[IDX(m_mapHeader.difficulty)];
// |     window->BroadcastMessage(msg);
// |     msg.payload.widget.id = GAME_SCENARIO_SELECTED_DIFFICULTY;
// |     msg.payload.widget.data.text = cDifficulty[IDX(m_difficulty)];
// |     window->BroadcastMessage(msg);
// |
// |     sprintf(gText, "%d", CalcDifficultyRating());
// |     strcat(gText, "%");
// |     msg.payload.widget.id = GAME_SCENARIO_RATING;
// |     msg.payload.widget.data.text = gText;
// |     window->BroadcastMessage(msg);
// |
// |     mapSize = MAP_SIZE_SMALL_INDEX;
// |     if (m_mapHeader.width == MAP_DIMENSION_MEDIUM)
// |         mapSize = MAP_SIZE_MEDIUM_INDEX;
// |     else if (m_mapHeader.width == MAP_DIMENSION_LARGE)
// |         mapSize = MAP_SIZE_LARGE_INDEX;
// |     else if (m_mapHeader.width == MAP_DIMENSION_XLARGE)
// |         mapSize = MAP_SIZE_XLARGE_INDEX;
// |     msg.payload.widget.id = GAME_SCENARIO_MAP_SIZE;
// |     msg.payload.widget.data.text = cMapSize[mapSize];
// |     window->BroadcastMessage(msg);
// |
// |     msg.payload.widget.id = GAME_SCENARIO_DESCRIPTION;
// |     msg.payload.widget.data.text = m_mapHeader.description;
// |     window->BroadcastMessage(msg);
// |     GetVictoryConditionText(gText);
// |     msg.payload.widget.id = GAME_SCENARIO_VICTORY;
// |     msg.payload.widget.data.text = gText;
// |     window->BroadcastMessage(msg);
// |     GetLossConditionText(gText);
// |     msg.payload.widget.id = GAME_SCENARIO_LOSS;
// |     msg.payload.widget.data.text = gText;
// |     window->BroadcastMessage(msg);
// |
// |     iconControl = NULL;
// |     nameWidget = NULL;
// |     availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |     columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |     firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
// |     playerStep = columnGap + PLAYER_COLUMN_WIDTH;
// |     multiplayerYOffset = 0;
// |
// |     for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
// |         if (giNumHumanPlayers > 1) {
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_HUMAN_X_OFFSET
// |                 ),
// |                 static_cast<i16>(multiplayerYOffset + SCENARIO_PLAYER_HUMAN_Y),
// |                 PLAYER_HUMAN_WIDTH,
// |                 PLAYER_HUMAN_HEIGHT,
// |                 "ngextra.icn",
// |                 PLAYER_HUMAN_FRAME,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             window->AddWidget(iconControl, -1);
// |
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_HANDICAP_X_OFFSET
// |                 ),
// |                 static_cast<i16>(multiplayerYOffset + SCENARIO_PLAYER_HANDICAP_Y),
// |                 PLAYER_HANDICAP_WIDTH,
// |                 PLAYER_HANDICAP_HEIGHT,
// |                 "ngextra.icn",
// |                 0,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             window->AddWidget(iconControl, -1);
// |         }
// |
// |         iconControl = new iconWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// |             ),
// |             SCENARIO_PLAYER_RACE_Y,
// |             PLAYER_RACE_WIDTH,
// |             static_cast<i16>(
// |                 giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// |                                       : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |             ),
// |             "ngextra.icn",
// |             static_cast<i16>(
// |                 giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// |                                       : GAME_RACE_WIDGET_SINGLE_FRAME
// |             ),
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |             WIDGET_KIND_ICON_DIRECT,
// |             PLAYER_WIDGET_FILL_COLOR
// |         );
// |         if (iconControl == NULL)
// |             MemError();
// |         window->AddWidget(iconControl, -1);
// |
// |         iconControl = new iconWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// |             ),
// |             SCENARIO_PLAYER_COLOR_Y,
// |             PLAYER_COLOR_WIDTH,
// |             PLAYER_COLOR_HEIGHT,
// |             "ngextra.icn",
// |             static_cast<i16>(
// |                 giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// |                                       : GAME_COLOR_WIDGET_SINGLE_FRAME
// |             ),
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// |             WIDGET_KIND_ICON_DIRECT,
// |             PLAYER_WIDGET_FILL_COLOR
// |         );
// |         if (iconControl == NULL)
// |             MemError();
// |         window->AddWidget(iconControl, -1);
// |
// |         if (giNumHumanPlayers > 1) {
// |             name = static_cast<char*>(
// |                 H2_ALLOC(PLAYER_LABEL_CAPACITY)
// |             );
// |             sprintf(name, " ");
// |             nameWidget = new textWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
// |                 ),
// |                 SCENARIO_PLAYER_NAME_Y,
// |                 PLAYER_NAME_WIDTH,
// |                 PLAYER_NAME_HEIGHT,
// |                 name,
// |                 "smalfont.fnt",
// |                 FONT_DRAW_DEFAULT,
// |                 static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
// |                 WIDGET_KIND_TEXT,
// |                 FONT_ALIGN_CENTER
// |             );
// |             if (nameWidget == NULL)
// |                 MemError();
// |             window->AddWidget(nameWidget, -1);
// |         }
// |
// |         yExtra = 0;
// |         iconControl = new iconWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_RACE_ICON_X_OFFSET
// |             ),
// |             static_cast<i16>(
// |                 yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_ICON_Y
// |             ),
// |             PLAYER_RACE_ICON_WIDTH,
// |             PLAYER_RACE_ICON_HEIGHT,
// |             "ngextra.icn",
// |             PLAYER_RACE_ICON_FRAME,
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// |             WIDGET_KIND_ICON_DIRECT,
// |             PLAYER_WIDGET_FILL_COLOR
// |         );
// |         if (iconControl == NULL)
// |             MemError();
// |         window->AddWidget(iconControl, -1);
// |
// |         name = static_cast<char*>(
// |             H2_ALLOC(PLAYER_LABEL_CAPACITY)
// |         );
// |         sprintf(name, "A");
// |         raceNameWidth = m_mapHeader.playerCount < PLAYER_RACE_NAME_NARROW_THRESHOLD
// |                             ? PLAYER_RACE_NAME_WIDE_WIDTH
// |                         : m_mapHeader.playerCount < MAP_HEADER_PLAYER_COUNT
// |                             ? PLAYER_RACE_NAME_NARROW_WIDTH
// |                             : 0;
// |         nameWidget = new textWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_RACE_NAME_X_OFFSET
// |                 - raceNameWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
// |             ),
// |             static_cast<i16>(
// |                 yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_NAME_Y
// |             ),
// |             static_cast<i16>(raceNameWidth + PLAYER_RACE_NAME_BASE_WIDTH),
// |             PLAYER_RACE_NAME_HEIGHT,
// |             name,
// |             "smalfont.fnt",
// |             FONT_DRAW_DEFAULT,
// |             static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
// |             WIDGET_KIND_TEXT,
// |             FONT_ALIGN_CENTER
// |         );
// |         if (nameWidget == NULL)
// |             MemError();
// |         window->AddWidget(nameWidget, -1);
// |
// |         iconControl = new iconWidget(
// |             static_cast<i16>(
// |                 firstColumnX + playerStep * playerCounter + PLAYER_RACE_CYCLE_X_OFFSET
// |             ),
// |             static_cast<i16>(
// |                 yExtra + multiplayerYOffset + SCENARIO_PLAYER_RACE_CYCLE_Y
// |             ),
// |             PLAYER_RACE_CYCLE_WIDTH,
// |             PLAYER_RACE_CYCLE_HEIGHT,
// |             "ngextra.icn",
// |             PLAYER_RACE_CYCLE_FRAME,
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// |             WIDGET_KIND_ICON_DIRECT,
// |             PLAYER_WIDGET_FILL_COLOR
// |         );
// |         if (iconControl == NULL)
// |             MemError();
// |         window->AddWidget(iconControl, -1);
// |     }
// |
// |     for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
// |         if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER) {
// |             sprintf(
// |                 gText,
// |                 ""
// |             );
// |         } else if (strlen(cPlayerNames[m_setupPlayerNetworkId[playerCounter]]) > 0) {
// |             sprintf(gText, cPlayerNames[m_setupPlayerNetworkId[playerCounter]]);
// |         } else {
// |             sprintf(gText, "\xc8\xe3\xf0\xee\xea %d", m_setupPlayerNetworkId[playerCounter] + 1);
// |         }
// |         msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |         msg.payload.widget.id =
// |             NEW_GAME_PLAYER_NAME_FIRST + playerCounter;
// |         msg.payload.widget.data.text = gText;
// |         window->BroadcastMessage(msg);
// |
// |         msg.payload.widget.command = playerCounter != m_selectedSetupPlayer
// |                                                          ? NEW_GAME_WIDGET_DISABLE
// |                                                          : NEW_GAME_WIDGET_ENABLE;
// |         msg.payload.widget.id =
// |             NEW_GAME_PLAYER_SELECT_FIRST + playerCounter;
// |         msg.payload.widget.data.value = GAME_WIDGET_REFRESH_FRAME;
// |         window->BroadcastMessage(msg);
// |
// |         if (m_setupPlayerType[playerCounter] != GAME_PLAYER_DEFAULT
// |             || (giNumHumanPlayers > 1
// |                 && m_setupPlayerNetworkId[playerCounter] != GAME_COMPUTER_PLAYER))
// |             locked = false;
// |         else
// |             locked = true;
// |         msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |         msg.payload.widget.id =
// |             NEW_GAME_COLOR_FIRST + playerCounter;
// |         if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
// |             msg.payload.widget.data.value =
// |                 (locked ? GAME_COMPUTER_COLOR_LOCKED_FRAME
// |                                    : GAME_COMPUTER_COLOR_UNLOCKED_FRAME)
// |                 + m_setupPlayerColor[playerCounter];
// |         else
// |             msg.payload.widget.data.value =
// |                 (locked ? GAME_HUMAN_COLOR_LOCKED_FRAME
// |                                    : GAME_HUMAN_COLOR_UNLOCKED_FRAME)
// |                 + m_setupPlayerColor[playerCounter];
// |         if (giNumHumanPlayers > 1)
// |             msg.payload.widget.data.value +=
// |                 GAME_MULTIPLAYER_COLOR_FRAME_OFFSET;
// |         window->BroadcastMessage(msg);
// |
// |         msg.payload.widget.command =
// |             locked ? NEW_GAME_WIDGET_DISABLE : NEW_GAME_WIDGET_ENABLE;
// |         msg.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |         window->BroadcastMessage(msg);
// |
// |         msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |         msg.payload.widget.id =
// |             NEW_GAME_HANDICAP_FIRST + playerCounter;
// |         if (m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER)
// |             msg.payload.widget.data.value = NEW_GAME_RACE_NAME_FIRST;
// |         else
// |             msg.payload.widget.data.value = IDX(m_playerHandicap[playerCounter]);
// |         window->BroadcastMessage(msg);
// |         msg.payload.widget.command =
// |             m_setupPlayerNetworkId[playerCounter] == GAME_COMPUTER_PLAYER ? NEW_GAME_WIDGET_DISABLE
// |                                                                           : NEW_GAME_WIDGET_ENABLE;
// |         msg.payload.widget.data.value = GAME_WIDGET_INACTIVE_FRAME;
// |         window->BroadcastMessage(msg);
// |
// |         msg.payload.widget.command = NEW_GAME_WIDGET_SET_FRAME;
// |         msg.payload.widget.id =
// |             NEW_GAME_RACE_CYCLE_FIRST + playerCounter;
// |         msg.payload.widget.data.value =
// |             (locked ? GAME_FIXED_RACE_FRAME_BASE : GAME_RANDOM_RACE_FRAME_BASE)
// |             + IDX(m_setupPlayerRace[playerCounter]);
// |         window->BroadcastMessage(msg);
// |
// |         sprintf(gText, gAlignmentNames[IDX(m_setupPlayerRace[playerCounter])]);
// |         msg.payload.widget.command = NEW_GAME_WIDGET_SET_TEXT;
// |         msg.payload.widget.id =
// |             NEW_GAME_RACE_NAME_FIRST + playerCounter;
// |         msg.payload.widget.data.text = gText;
// |         window->BroadcastMessage(msg);
// |     }
// |
// |     gpWindowManager->DoDialog(window, EventWindowHandler, 0);
// |     delete window;
// | }
// |
// ALTERNATIVE remove_01 (unified delta from BEFORE):
// | --- before
// | +++ remove_01
// | @@ -65,7 +65,6 @@
// |      window->BroadcastMessage(msg);
// |
// |      iconControl = NULL;
// | -    nameWidget = NULL;
// |      availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |      columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |      firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
// ALTERNATIVE remove_10 (unified delta from BEFORE):
// | --- before
// | +++ remove_10
// | @@ -64,7 +64,6 @@
// |      msg.payload.widget.data.text = gText;
// |      window->BroadcastMessage(msg);
// |
// | -    iconControl = NULL;
// |      nameWidget = NULL;
// |      availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |      columnGap = availWidth / (m_mapHeader.playerCount + 1);
// ALTERNATIVE remove_11 (unified delta from BEFORE):
// | --- before
// | +++ remove_11
// | @@ -64,8 +64,6 @@
// |      msg.payload.widget.data.text = gText;
// |      window->BroadcastMessage(msg);
// |
// | -    iconControl = NULL;
// | -    nameWidget = NULL;
// |      availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |      columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |      firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
