// VC6 SP5 source family s69-newgame-window-initializers; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/s69-newgame-window-initializers/results.json; RVA 0x766a7
// Complete 4 arms; elapsed 3.152263s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1940 / a82616cb79137c3d / 5bcb53de2af7ca8e / 62:62 / 62,0,0,0 / True
// 1 / {'family': 'remove_01', 'candidate': 'baseline'} / 1933 / f1b51e387d2f9af7 / 45f1a19bab69a6f7 / 62:62 / 61,1,0,0 / True
// 2 / {'family': 'remove_10', 'candidate': 'baseline'} / 1933 / 9fb3a7d11a28ad19 / 45f1a19bab69a6f7 / 62:62 / 61,1,0,0 / True
// 3 / {'family': 'remove_11', 'candidate': 'baseline'} / 1926 / 9fb8f1569c7ef62c / 26cf4d6d2e4e5bd8 / 62:62 / 61,1,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x794)
// |     void game::InitNewGameWindow(void) {
// |         i32 columnGap;
// |         i32 availWidth;
// |         i32 firstColumnX;
// |         i32 playerStep;
// |         i32 playerCounter;
// |         char* name;
// |         i32 yExtra;
// |         i32 raceNameWidth;
// |         i32 multiplayerYOffset;
// |         widget* nameWidget;
// |         widget* iconControl;
// |
// |         iconControl = NULL;
// |         nameWidget = NULL;
// |         availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |         columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |         firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
// |         playerStep = columnGap + PLAYER_COLUMN_WIDTH;
// |         multiplayerYOffset = 0;
// |
// |         if (giNumHumanPlayers > 1 && iMPBaseType != MULTIPLAYER_BASE_HOT_SEAT)
// |             multiplayerYOffset = PLAYER_MULTIPLAYER_Y_OFFSET;
// |
// |         for (playerCounter = 0; playerCounter < m_mapHeader.playerCount; ++playerCounter) {
// |             if (giNumHumanPlayers > 1) {
// |                 iconControl = new iconWidget(
// |                     static_cast<i16>(
// |                         firstColumnX + playerStep * playerCounter
// |                         + PLAYER_HUMAN_X_OFFSET
// |                     ),
// |                     static_cast<i16>(multiplayerYOffset + PLAYER_HUMAN_Y),
// |                     PLAYER_HUMAN_WIDTH,
// |                     PLAYER_HUMAN_HEIGHT,
// |                     "ngextra.icn",
// |                     PLAYER_HUMAN_FRAME,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(playerCounter + NEW_GAME_PLAYER_HUMAN_FIRST),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     PLAYER_WIDGET_FILL_COLOR
// |                 );
// |                 if (iconControl == NULL)
// |                     MemError();
// |                 m_newGameWindow->AddWidget(iconControl, -1);
// |
// |                 iconControl = new iconWidget(
// |                     static_cast<i16>(
// |                         firstColumnX + playerStep * playerCounter
// |                         + PLAYER_HANDICAP_X_OFFSET
// |                     ),
// |                     static_cast<i16>(multiplayerYOffset + PLAYER_HANDICAP_Y),
// |                     PLAYER_HANDICAP_WIDTH,
// |                     PLAYER_HANDICAP_HEIGHT,
// |                     "ngextra.icn",
// |                     0,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(playerCounter + NEW_GAME_HANDICAP_FIRST),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     PLAYER_WIDGET_FILL_COLOR
// |                 );
// |                 if (iconControl == NULL)
// |                     MemError();
// |                 m_newGameWindow->AddWidget(iconControl, -1);
// |             }
// |
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_RACE_X_OFFSET
// |                 ),
// |                 PLAYER_RACE_Y,
// |                 PLAYER_RACE_WIDTH,
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_HEIGHT
// |                                           : GAME_RACE_WIDGET_SINGLE_HEIGHT
// |                 ),
// |                 "ngextra.icn",
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_RACE_WIDGET_MULTIPLAYER_FRAME
// |                                           : GAME_RACE_WIDGET_SINGLE_FRAME
// |                 ),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(iconControl, -1);
// |
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_SELECT_X_OFFSET
// |                 ),
// |                 PLAYER_SELECT_Y,
// |                 PLAYER_SELECT_WIDTH,
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_PLAYER_WIDGET_MULTIPLAYER_HEIGHT
// |                                           : GAME_PLAYER_WIDGET_SINGLE_HEIGHT
// |                 ),
// |                 "ngextra.icn",
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_PLAYER_WIDGET_MULTIPLAYER_FRAME
// |                                           : GAME_PLAYER_WIDGET_SINGLE_FRAME
// |                 ),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_PLAYER_SELECT_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(iconControl, -1);
// |
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter + PLAYER_COLOR_X_OFFSET
// |                 ),
// |                 PLAYER_COLOR_Y,
// |                 PLAYER_COLOR_WIDTH,
// |                 PLAYER_COLOR_HEIGHT,
// |                 "ngextra.icn",
// |                 static_cast<i16>(
// |                     giNumHumanPlayers > 1 ? GAME_COLOR_WIDGET_MULTIPLAYER_FRAME
// |                                           : GAME_COLOR_WIDGET_SINGLE_FRAME
// |                 ),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_COLOR_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(iconControl, -1);
// |
// |             if (giNumHumanPlayers > 1) {
// |                 name = static_cast<char*>(H2_ALLOC(PLAYER_LABEL_CAPACITY));
// |                 sprintf(name, " ");
// |                 nameWidget = new textWidget(
// |                     static_cast<i16>(
// |                         firstColumnX + playerStep * playerCounter + PLAYER_NAME_X_OFFSET
// |                     ),
// |                     PLAYER_NAME_Y,
// |                     PLAYER_NAME_WIDTH,
// |                     PLAYER_NAME_HEIGHT,
// |                     name,
// |                     "smalfont.fnt",
// |                     FONT_DRAW_DEFAULT,
// |                     static_cast<i16>(playerCounter + NEW_GAME_PLAYER_NAME_FIRST),
// |                     WIDGET_KIND_TEXT,
// |                     FONT_ALIGN_CENTER
// |                 );
// |                 if (nameWidget == NULL)
// |                     MemError();
// |                 m_newGameWindow->AddWidget(nameWidget, -1);
// |             }
// |
// |             yExtra = 0;
// |             if (giNumHumanPlayers == 1)
// |                 yExtra = PLAYER_SINGLE_Y_OFFSET;
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter
// |                     + PLAYER_RACE_ICON_X_OFFSET
// |                 ),
// |                 static_cast<i16>(
// |                     yExtra + multiplayerYOffset + PLAYER_RACE_ICON_Y
// |                 ),
// |                 PLAYER_RACE_ICON_WIDTH,
// |                 PLAYER_RACE_ICON_HEIGHT,
// |                 "ngextra.icn",
// |                 PLAYER_RACE_ICON_FRAME,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_ICON_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(iconControl, -1);
// |
// |             name = static_cast<char*>(
// |                 H2_ALLOC(PLAYER_LABEL_CAPACITY)
// |             );
// |             sprintf(name, "A");
// |             raceNameWidth = m_mapHeader.playerCount < PLAYER_RACE_NAME_NARROW_THRESHOLD
// |                                 ? PLAYER_RACE_NAME_WIDE_WIDTH
// |                             : m_mapHeader.playerCount < PLAYER_RACE_NAME_HIDDEN_THRESHOLD
// |                                 ? PLAYER_RACE_NAME_NARROW_WIDTH
// |                                 : 0;
// |             nameWidget = new textWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter
// |                     + PLAYER_RACE_NAME_X_OFFSET - raceNameWidth / PLAYER_RACE_NAME_CENTER_DIVISOR
// |                 ),
// |                 static_cast<i16>(
// |                     yExtra + multiplayerYOffset + PLAYER_RACE_NAME_Y
// |                 ),
// |                 static_cast<i16>(raceNameWidth + PLAYER_RACE_NAME_BASE_WIDTH),
// |                 PLAYER_RACE_NAME_HEIGHT,
// |                 name,
// |                 "smalfont.fnt",
// |                 FONT_DRAW_DEFAULT,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_NAME_FIRST),
// |                 WIDGET_KIND_TEXT,
// |                 FONT_ALIGN_CENTER
// |             );
// |             if (nameWidget == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(nameWidget, -1);
// |
// |             iconControl = new iconWidget(
// |                 static_cast<i16>(
// |                     firstColumnX + playerStep * playerCounter
// |                     + PLAYER_RACE_CYCLE_X_OFFSET
// |                 ),
// |                 static_cast<i16>(
// |                     yExtra + multiplayerYOffset + PLAYER_RACE_CYCLE_Y
// |                 ),
// |                 PLAYER_RACE_CYCLE_WIDTH,
// |                 PLAYER_RACE_CYCLE_HEIGHT,
// |                 "ngextra.icn",
// |                 PLAYER_RACE_CYCLE_FRAME,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(playerCounter + NEW_GAME_RACE_CYCLE_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 PLAYER_WIDGET_FILL_COLOR
// |             );
// |             if (iconControl == NULL)
// |                 MemError();
// |             m_newGameWindow->AddWidget(iconControl, -1);
// |         }
// |     }
// |
// ALTERNATIVE remove_01 (unified delta from BEFORE):
// | --- before
// | +++ remove_01
// | @@ -13,7 +13,6 @@
// |          widget* iconControl;
// |
// |          iconControl = NULL;
// | -        nameWidget = NULL;
// |          availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |          columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |          firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
// ALTERNATIVE remove_10 (unified delta from BEFORE):
// | --- before
// | +++ remove_10
// | @@ -12,7 +12,6 @@
// |          widget* nameWidget;
// |          widget* iconControl;
// |
// | -        iconControl = NULL;
// |          nameWidget = NULL;
// |          availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |          columnGap = availWidth / (m_mapHeader.playerCount + 1);
// ALTERNATIVE remove_11 (unified delta from BEFORE):
// | --- before
// | +++ remove_11
// | @@ -12,8 +12,6 @@
// |          widget* nameWidget;
// |          widget* iconControl;
// |
// | -        iconControl = NULL;
// | -        nameWidget = NULL;
// |          availWidth = PLAYER_AREA_WIDTH - m_mapHeader.playerCount * PLAYER_COLUMN_WIDTH;
// |          columnGap = availWidth / (m_mapHeader.playerCount + 1);
// |          firstColumnX = columnGap + PLAYER_FIRST_X_OFFSET;
