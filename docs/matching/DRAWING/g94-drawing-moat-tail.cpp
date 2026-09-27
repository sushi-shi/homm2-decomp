// VC6 SP5 source family g94-drawing-moat-tail; parent aab81af5.
// Source record only: each alternative is relative to its own BEFORE, not cumulative.
// No cross-family or TU-state census is claimed.
// build/source-variant-batch/g94-drawing-moat-tail/results.json; RVA 0x395b8
// Complete 2 arms; elapsed 1.764616s; source restored.
// Disposition: retain original; every tested nonoriginal arm changes function text.
// Raw/normalized relocation hashes can reflect private label counter spelling;
// final proof compares actual ordered targets/sites and the complete claimed byte span.
// UpdateMouseGrid has an unchanged EH/FS site-model gap, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 5406 / 430b7ec7cb606625 / aa92848f4a996d5c / 206:206 / 205,1,0,0 / True
// 1 / {'source_family': 'structured_moat', 'candidate': 'baseline'} / 5447 / 3456cf1f0b0f60d6 / 9e41896e989d9324 / 199:206 / 142,8,24,25 / False

// AXIS source_family
// BEFORE (complete exact span):
// | void combatManager::DrawFrame(
// |     i32 updateScreen,
// |     i32 computeExtent,
// |     i32 redrawExtent,
// |     i32 extentOnly,
// |     i32 delay,
// |     i32 drawBackground,
// |     i32 waitForTimer
// | ) {
// |     i32 col;
// |     CombatDrawLayer row;
// |     i32 H2_UNUSED(unusedDrawState1);
// |     H2_ENUM_STORAGE_STEPPED(ArmyDrawState, i32) state;
// |     b32 extentChanged1;
// |     i32 H2_UNUSED(unusedDrawState26);
// |     i32 H2_UNUSED(unused7);
// |     i32 H2_UNUSED(unusedDrawState8);
// |     i32 H2_UNUSED(unusedDrawState);
// |     i32 H2_UNUSED(gridWasShowing1);
// |     i32 endColumn;
// |     b32 skipSpecialOccupants8;
// |     i32 columnStep1;
// |     i32 startColumn;
// |     i32 armyIndex7;
// |
// |     if (m_nonVisualCombat != 0)
// |         return;
// |     if (gbNoShowCombat != 0)
// |         return;
// |     if (m_combatWindowOpen == 0)
// |         return;
// |
// |     PollSound();
// |     gpMouseManager->m_cursorReady = false;
// |
// |     if (computeExtent != 0) {
// |         extentChanged1 = false;
// |         for (state = 0; IDX(state) < COMBAT_SIDE_COUNT; state++) {
// |             for (armyIndex7 = 0; armyIndex7 < COMBAT_ARMY_SLOT_COUNT_DRAWING; armyIndex7++) {
// |                 if (m_limitCreatureCount[IDX(state)][armyIndex7] > 0) {
// |                     extentChanged1 = true;
// |                     gbComputeExtent = true;
// |                     gbSaveBiggestExtent = true;
// |                     gbReturnAfterComputeExtent = true;
// |                     m_armies[IDX(state)][armyIndex7].DrawToBuffer(
// |                         m_hexCells[m_armies[IDX(state)][armyIndex7].m_hex].m_x,
// |                         m_hexCells[m_armies[IDX(state)][armyIndex7].m_hex].m_y,
// |                         0
// |                     );
// |                     gbReturnAfterComputeExtent = false;
// |                     gbComputeExtent = false;
// |                     gbSaveBiggestExtent = false;
// |                 }
// |             }
// |         }
// |
// |         for (state = 0; IDX(state) < COMBAT_SIDE_COUNT; state++) {
// |             if (m_drawHero[IDX(state)] != 0) {
// |                 extentChanged1 = true;
// |                 gbComputeExtent = true;
// |                 gbSaveBiggestExtent = true;
// |                 gbReturnAfterComputeExtent = true;
// |                 m_heroIcons[IDX(state)]->CombatClipDrawToBuffer(
// |                     IDX(state) == 0 ? COMBAT_HERO_LEFT_X : COMBAT_HERO_RIGHT_X,
// |                     IDX(state) == 0 ? COMBAT_HERO_LEFT_Y : COMBAT_HERO_RIGHT_Y,
// |                     sCmbtHero[m_heroSpriteIndex[IDX(state)]]
// |                         .animationFrames[m_heroAnimationState[IDX(state)]][m_heroAnimationFrame[IDX(state)]],
// |                     &m_heroLimits[IDX(state)],
// |                     ICON_DRAW_FLIPPED,
// |                     0,
// |                     NULL,
// |                     NULL
// |                 );
// |                 gbReturnAfterComputeExtent = false;
// |                 gbComputeExtent = false;
// |                 gbSaveBiggestExtent = false;
// |             }
// |
// |             if (m_drawHeroOverlay[IDX(state)] != 0) {
// |                 extentChanged1 = true;
// |                 gbComputeExtent = true;
// |                 gbSaveBiggestExtent = true;
// |                 gbReturnAfterComputeExtent = true;
// |                 m_heroOverlayIcons[IDX(state)]->CombatClipDrawToBuffer(
// |                     IDX(state) == 0 ? COMBAT_HERO_LEFT_X
// |                                : ((m_heroes[IDX(state)]->m_isCaptain ? -1 : 0)
// |                                   & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
// |                                      + COMBAT_HERO_RIGHT_X,
// |                     IDX(state) == 0 ? COMBAT_HERO_LEFT_Y
// |                                : ((m_heroes[IDX(state)]->m_isCaptain ? -1 : 0)
// |                                   & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
// |                                      + COMBAT_HERO_RIGHT_Y,
// |                     m_heroOverlayFrame[IDX(state)],
// |                     &m_heroOverlayLimits[IDX(state)],
// |                     ICON_DRAW_FLIPPED,
// |                     0,
// |                     NULL,
// |                     NULL
// |                 );
// |                 gbReturnAfterComputeExtent = false;
// |                 gbComputeExtent = false;
// |                 gbSaveBiggestExtent = false;
// |             }
// |         }
// |
// |         if (extentOnly != 0)
// |             goto finish;
// |         if (extentChanged1 == 0)
// |             goto finish;
// |
// |         giMinExtentX--;
// |         giMinExtentY--;
// |         giMaxExtentX++;
// |         giMaxExtentY++;
// |         if (giMinExtentX < 0)
// |             giMinExtentX = 0;
// |         if (giMinExtentY < 0)
// |             giMinExtentY = 0;
// |         if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
// |             giMaxExtentX = COMBAT_MAX_EXTENT_X;
// |         if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
// |             giMaxExtentY = COMBAT_MAX_EXTENT_Y;
// |     }
// |
// |     if (drawBackground != 0) {
// |         if (m_backgroundDrawn != 0) {
// |             if (computeExtent != 0 || redrawExtent != 0 || gbLimitToExtent != 0) {
// |                 m_backgroundBuffer->CopyTo(
// |                     gpWindowManager->m_screen,
// |                     giMinExtentX,
// |                     giMinExtentY,
// |                     giMinExtentX,
// |                     giMinExtentY,
// |                     giMaxExtentX - giMinExtentX + 1,
// |                     giMaxExtentY - giMinExtentY + 1
// |                 );
// |             } else {
// |                 m_backgroundBuffer->CopyTo(
// |                     gpWindowManager->m_screen,
// |                     0,
// |                     0,
// |                     0,
// |                     0,
// |                     COMBAT_SCREEN_WIDTH,
// |                     COMBAT_AREA_HEIGHT
// |                 );
// |             }
// |         } else {
// |             DrawBackground();
// |         }
// |     }
// |
// |     if (computeExtent != 0) {
// |         gbLimitToExtent = true;
// |         gbComputeExtent = true;
// |     }
// |
// |     for (row = DRAW_FIRST_LAYER; row < DRAW_LAYER_COUNT; row++) {
// |         if (row == DRAW_RIGHT_HERO_LAYER && m_heroes[1] != NULL) {
// |             m_heroIcons[1]->CombatClipDrawToBuffer(
// |                 ((m_heroes[1]->m_isCaptain ? -1 : 0)
// |                  & (COMBAT_HERO_RIGHT_ALT_X - COMBAT_HERO_RIGHT_X))
// |                     + COMBAT_HERO_RIGHT_X,
// |                 ((m_heroes[1]->m_isCaptain ? -1 : 0)
// |                  & (COMBAT_HERO_RIGHT_ALT_Y - COMBAT_HERO_RIGHT_Y))
// |                     + COMBAT_HERO_RIGHT_Y,
// |                 sCmbtHero[m_heroSpriteIndex[IDX(COMBAT_DEFENDER_SIDE)]]
// |                     .animationFrames[m_heroAnimationState[IDX(COMBAT_DEFENDER_SIDE)]]
// |                                     [m_heroAnimationFrame[IDX(COMBAT_DEFENDER_SIDE)]],
// |                 &m_heroLimits[1],
// |                 ICON_DRAW_FLIPPED,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |             m_heroOverlayIcons[1]->CombatClipDrawToBuffer(
// |                 COMBAT_HERO_RIGHT_X,
// |                 COMBAT_HERO_RIGHT_Y,
// |                 m_heroOverlayFrame[1],
// |                 &m_heroOverlayLimits[1],
// |                 ICON_DRAW_FLIPPED,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |
// |         if (row == DRAW_LEFT_HERO_LAYER && m_heroes[0] != NULL) {
// |             m_heroIcons[0]->CombatClipDrawToBuffer(
// |                 COMBAT_HERO_LEFT_X,
// |                 COMBAT_HERO_LEFT_Y,
// |                 sCmbtHero[m_heroSpriteIndex[IDX(COMBAT_ATTACKER_SIDE)]]
// |                     .animationFrames[m_heroAnimationState[IDX(COMBAT_ATTACKER_SIDE)]]
// |                                     [m_heroAnimationFrame[IDX(COMBAT_ATTACKER_SIDE)]],
// |                 &m_heroLimits[0],
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |             m_heroOverlayIcons[0]->CombatClipDrawToBuffer(
// |                 COMBAT_HERO_LEFT_X,
// |                 COMBAT_HERO_LEFT_Y,
// |                 m_heroOverlayFrame[0],
// |                 &m_heroOverlayLimits[0],
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |
// |         startColumn = COMBAT_GRID_FIRST_COLUMN;
// |         endColumn = COMBAT_GRID_COLUMN_END;
// |         columnStep1 = 1;
// |         if (m_inCastleCombat != 0 && row >= DRAW_CASTLE_REVERSE_ROW) {
// |             startColumn = COMBAT_GRID_REVERSE_FIRST_COLUMN;
// |             endColumn = COMBAT_GRID_REVERSE_COLUMN_END;
// |             columnStep1 = -1;
// |         }
// |
// |         if (m_inCastleCombat != 0 && row == DRAW_CATAPULT_LAYER) {
// |             m_combatIcons[IDX(COMBAT_ICON_CATAPULT)]->CombatClipDrawToBuffer(
// |                 COMBAT_CATAPULT_X,
// |                 COMBAT_CATAPULT_Y,
// |                 m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)],
// |                 &m_catapultLimits,
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |         if (m_inCastleCombat != 0 && row == DRAW_WALL_TOP_LAYER
// |             && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
// |             m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
// |                 0,
// |                 0,
// |                 IDX(m_drawbridgeState) + DRAWBRIDGE_TOWER_FRAME_BASE,
// |                 &m_upperWallLimits,
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |         if (m_inCastleCombat != 0 && row == DRAW_WALL_MIDDLE_LAYER
// |             && m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED) {
// |             m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
// |                 0,
// |                 0,
// |                 COMBAT_CASTLE_TOP_LAYER_FRAME,
// |                 &m_middleWallLimits,
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |         if (m_inCastleCombat != 0 && row == DRAW_GATE_LAYER) {
// |             m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
// |                 0,
// |                 0,
// |                 m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)]
// |                             == COMBAT_WALL_STATE_KEEP_STANDING
// |                     ? COMBAT_CASTLE_GATE_CLOSED_FRAME
// |                     : COMBAT_CASTLE_GATE_FRAME,
// |                 &m_gateLimits,
// |                 ICON_DRAW_NORMAL,
// |                 0,
// |                 NULL,
// |                 NULL
// |             );
// |         }
// |
// |         skipSpecialOccupants8 = false;
// |         if (m_inCastleCombat != 0
// |             && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
// |                    != COMBAT_WALL_STATE_DESTROYED
// |             && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
// |                    != COMBAT_WALL_STATE_SECTION_DESTROYED) {
// |             skipSpecialOccupants8 = true;
// |             for (state = ARMY_DRAW_BEHIND; state < ARMY_DRAW_PHASE_COUNT; state++) {
// |                 m_hexCells[IDX(COMBAT_CASTLE_SPECIAL_HEX_FIRST)].DrawOccupant(state, 0);
// |                 m_hexCells[IDX(COMBAT_CASTLE_SPECIAL_HEX_SECOND)].DrawOccupant(state, 0);
// |             }
// |         }
// |
// |         for (col = startColumn; col != endColumn; col += columnStep1)
// |             m_hexCells[IDX(row) * COMBAT_GRID_ROW_LENGTH + col].DrawLowerDeadOccupants();
// |
// |         for (col = startColumn; col != endColumn; col += columnStep1)
// |             m_hexCells[IDX(row) * COMBAT_GRID_ROW_LENGTH + col].DrawUpperDeadOccupant();
// |
// |         for (state = ARMY_DRAW_BEHIND; state < ARMY_DRAW_PHASE_COUNT; state++) {
// |             if (state == ARMY_DRAW_NORMAL) {
// |                 for (col = startColumn; col != endColumn; col += columnStep1) {
// |                     if (m_hexCells[IDX(row) * COMBAT_GRID_ROW_LENGTH + col].m_obstacleIndex
// |                         != -1) {
// |                         m_hexCells[IDX(row) * COMBAT_GRID_ROW_LENGTH + col].DrawObstacle();
// |                     }
// |                 }
// |             }
// |
// |             for (col = startColumn; col != endColumn; col += columnStep1) {
// |                 u16 wallCoordinates1[IDX(FACTION_COUNT)][WALL_COORDINATE_COUNT] = {
// |                     {443, 153, 399, 237, 399, 321, 443, 405},
// |                     {443, 153, 399, 237, 399, 321, 443, 405},
// |                     {443, 153, 399, 237, 399, 321, 443, 405},
// |                     {443, 153, 399, 237, 399, 321, 443, 405},
// |                     {443, 153, 399, 237, 399, 321, 443, 405},
// |                     {443, 153, 399, 237, 399, 321, 443, 405}
// |                 };
// |                 u8 wallFrameOffsets1[WALL_FRAME_OFFSET_COUNT] = {0, 4, 8, 23, 27, 35, 31};
// |                 i32 wallX7;
// |                 i32 wallFrame1;
// |                 i32 wallY;
// |                 H2_ENUM_STORAGE(CombatCastleHex, i32) castleHex;
// |
// |                 if (m_inCastleCombat != 0 && state == ARMY_DRAW_BEHIND) {
// |                     castleHex = IDX(row) * COMBAT_GRID_ROW_LENGTH + col;
// |                     wallFrame1 = 0;
// |                     wallX7 = 0;
// |                     wallY = 0;
// |                     switch (castleHex) {
// |                         case COMBAT_CASTLE_HEX_TOP_TOWER:
// |                             wallFrame1 =
// |                                 wallFrameOffsets1[IDX(
// |                                     m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
// |                                 )]
// |                                 + TOP_TOWER_BASE_FRAME;
// |                             break;
// |                         case COMBAT_CASTLE_HEX_SECOND_TOWER:
// |                             wallFrame1 =
// |                                 wallFrameOffsets1[IDX(
// |                                     m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)]
// |                                 )]
// |                                 + SECOND_TOWER_BASE_FRAME;
// |                             break;
// |                         case COMBAT_CASTLE_HEX_THIRD_TOWER:
// |                             wallFrame1 =
// |                                 wallFrameOffsets1[IDX(
// |                                     m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)]
// |                                 )]
// |                                 + THIRD_TOWER_BASE_FRAME;
// |                             break;
// |                         case COMBAT_CASTLE_HEX_BOTTOM_TOWER:
// |                             wallFrame1 =
// |                                 wallFrameOffsets1[IDX(
// |                                     m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
// |                                 )]
// |                                 + BOTTOM_TOWER_BASE_FRAME;
// |                             break;
// |                         case COMBAT_CASTLE_HEX_TOP_WALL:
// |                             wallFrame1 = IDX(m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)])
// |                                          + COMBAT_CASTLE_WALL_BASE_FRAME;
// |                             wallX7 =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_TOP_X];
// |                             wallY =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_TOP_Y];
// |                             break;
// |                         case COMBAT_CASTLE_HEX_SECOND_WALL:
// |                             wallFrame1 = IDX(m_wallStates[IDX(COMBAT_WALL_SLOT_SECOND_TOWER)])
// |                                          + COMBAT_CASTLE_WALL_BASE_FRAME;
// |                             wallX7 =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_SECOND_X];
// |                             wallY =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_SECOND_Y];
// |                             break;
// |                         case COMBAT_CASTLE_HEX_THIRD_WALL:
// |                             wallFrame1 = IDX(m_wallStates[IDX(COMBAT_WALL_SLOT_THIRD_TOWER)])
// |                                          + COMBAT_CASTLE_WALL_BASE_FRAME;
// |                             wallX7 =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_THIRD_X];
// |                             wallY =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_THIRD_Y];
// |                             break;
// |                         case COMBAT_CASTLE_HEX_BOTTOM_WALL:
// |                             wallFrame1 = IDX(m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)])
// |                                          + COMBAT_CASTLE_WALL_BASE_FRAME;
// |                             wallX7 =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_BOTTOM_X];
// |                             wallY =
// |                                 wallCoordinates1[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
// |                                                 [WALL_BOTTOM_Y];
// |                             break;
// |                         case COMBAT_CASTLE_HEX_GATE:
// |                             if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN)
// |                                 wallFrame1 = GATE_VISIBLE_FRAME;
// |                             break;
// |                     }
// |                     if (wallFrame1 != 0) {
// |                         m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
// |                             wallX7,
// |                             wallY,
// |                             wallFrame1,
// |                             &m_hexCells[IDX(castleHex)].m_limits[0],
// |                             ICON_DRAW_NORMAL,
// |                             0,
// |                             NULL,
// |                             NULL
// |                         );
// |                     }
// |                 }
// |
// |                 if (skipSpecialOccupants8 == 0
// |                     || (IDX(row) * COMBAT_GRID_ROW_LENGTH + col
// |                             != IDX(COMBAT_CASTLE_SPECIAL_HEX_FIRST)
// |                         && IDX(row) * COMBAT_GRID_ROW_LENGTH + col
// |                                != IDX(COMBAT_CASTLE_SPECIAL_HEX_SECOND))) {
// |                     m_hexCells[IDX(row) * COMBAT_GRID_ROW_LENGTH + col].DrawOccupant(
// |                         state,
// |                         0
// |                     );
// |                 }
// |             }
// |         }
// |
// |         if (m_inCastleCombat == 0
// |             || (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_MOAT)) == 0)
// |             goto endRow;
// |         if (row == DRAW_CASTLE_GATE_ROW && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN)
// |             goto endRow;
// |
// |         if (giWalkingTo == moatCell[IDX(row)] || giWalkingTo2 == moatCell[IDX(row)]
// |             || giWalkingFrom == moatCell[IDX(row)] || giWalkingFrom2 == moatCell[IDX(row)]) {
// |             if (abs(giWalkingTo - giWalkingFrom) <= 1)
// |                 goto drawMoat;
// |             if (IDX(row)
// |                 != (giWalkingTo / COMBAT_GRID_ROW_LENGTH > giWalkingFrom / COMBAT_GRID_ROW_LENGTH
// |                         ? giWalkingTo / COMBAT_GRID_ROW_LENGTH
// |                         : giWalkingFrom / COMBAT_GRID_ROW_LENGTH))
// |                 goto endRow;
// |
// |             if (gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN
// |                 && (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// |                     || giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)) {
// |                 if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// |                     && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1)
// |                     goto endRow;
// |                 if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1
// |                     && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)
// |                     goto endRow;
// |
// |                 if (giWalkingTo < giWalkingFrom) {
// |                     if (moatCell[IDX(row)] == giWalkingFrom
// |                         || moatCell[IDX(row)] == giWalkingFrom2)
// |                         goto drawMoat;
// |                     goto endRow;
// |                 } else {
// |                     if (moatCell[IDX(row)] == giWalkingTo || moatCell[IDX(row)] == giWalkingTo2)
// |                         goto drawMoat;
// |                     goto endRow;
// |                 }
// |             } else {
// |                 i32 drawbridgeTop;
// |                 i32 drawbridgeBottom;
// |
// |                 drawbridgeTop = m_hexCells[giWalkingFrom].m_y - DRAWBRIDGE_TOP_CLIP_OFFSET
// |                               + giWalkingYMod;
// |                 drawbridgeBottom =
// |                     m_hexCells[giWalkingFrom > giWalkingTo ? giWalkingFrom : giWalkingTo].m_y
// |                     + DRAWBRIDGE_BOTTOM_CLIP_OFFSET;
// |                 IconToBitmap(
// |                     m_combatIcons[IDX(COMBAT_ICON_DRAWBRIDGE)],
// |                     gpWindowManager->m_screen,
// |                     0,
// |                     0,
// |                     0,
// |                     ICON_DRAW_CLIP,
// |                     0,
// |                     drawbridgeTop,
// |                     COMBAT_SCREEN_WIDTH,
// |                     drawbridgeBottom - drawbridgeTop + 1,
// |                     0
// |                 );
// |                 goto endRow;
// |             }
// |         } else {
// |             if (m_hexCells[moatCell[IDX(row)]].m_occupantSide == COMBAT_SIDE_NONE)
// |                 goto endRow;
// |         drawMoat:
// |             m_combatIcons[IDX(COMBAT_ICON_MOAT)]
// |                 ->CombatClipDrawToBuffer(
// |                     0, 0, IDX(row), &m_moatLimits[IDX(row)], ICON_DRAW_NORMAL, 0, NULL, NULL
// |                 );
// |             m_hexCells[moatCell[IDX(row)] - 1].DrawOccupant(ARMY_DRAW_ALL, 1);
// |             m_hexCells[moatCell[IDX(row)]].DrawOccupant(ARMY_DRAW_ALL, 1);
// |             m_hexCells[moatCell[IDX(row)] + 1].DrawOccupant(ARMY_DRAW_ALL, 1);
// |         }
// |     endRow:
// |         ;
// |         gridWasShowing1 = 0;
// |     }
// |
// |     if (gConfig.combatArmyInfoLevel > 0 && m_smallViewSide[0] != COMBAT_SIDE_NONE) {
// |         DrawSmallView(0, 0);
// |         DrawSmallView(1, 0);
// |     }
// |
// |     gpMouseManager->m_cursorReady = true;
// |     PollSound();
// |     if (computeExtent != 0 || redrawExtent != 0) {
// |         redrawExtent = 0;
// |         gbLimitToExtent = false;
// |         gbComputeExtent = false;
// |         if (waitForTimer != 0)
// |             DelayTil(glTimers);
// |         glTimers[0] =
// |             static_cast<i32>(KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
// |         gbFullCombatScreenDrawn = false;
// |         if (updateScreen == 1) {
// |             if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
// |                 giMaxExtentY = COMBAT_MAX_EXTENT_Y;
// |             gbEnlargeScreenBlit = false;
// |             gpWindowManager->UpdateScreenRegion(
// |                 giMinExtentX,
// |                 giMinExtentY,
// |                 giMaxExtentX - giMinExtentX + 1,
// |                 giMaxExtentY - giMinExtentY + 1
// |             );
// |             gbEnlargeScreenBlit = true;
// |         }
// |     } else if (updateScreen == 1) {
// |         if (waitForTimer != 0)
// |             DelayTil(glTimers);
// |         gbFullCombatScreenDrawn = true;
// |         glTimers[0] =
// |             static_cast<i32>(KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * delay);
// |         UpdateCombatArea();
// |     }
// |
// | finish:
// |     gpMouseManager->m_cursorReady = true;
// |     PollSound();
// | }
// ALTERNATIVE structured_moat (unified delta from BEFORE):
// | --- before
// | +++ structured_moat
// | @@ -423,43 +423,36 @@
// |              }
// |          }
// |
// | -        if (m_inCastleCombat == 0
// | -            || (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_MOAT)) == 0)
// | -            goto endRow;
// | -        if (row == DRAW_CASTLE_GATE_ROW && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN)
// | -            goto endRow;
// | -
// | -        if (giWalkingTo == moatCell[IDX(row)] || giWalkingTo2 == moatCell[IDX(row)]
// | -            || giWalkingFrom == moatCell[IDX(row)] || giWalkingFrom2 == moatCell[IDX(row)]) {
// | -            if (abs(giWalkingTo - giWalkingFrom) <= 1)
// | -                goto drawMoat;
// | -            if (IDX(row)
// | -                != (giWalkingTo / COMBAT_GRID_ROW_LENGTH > giWalkingFrom / COMBAT_GRID_ROW_LENGTH
// | -                        ? giWalkingTo / COMBAT_GRID_ROW_LENGTH
// | -                        : giWalkingFrom / COMBAT_GRID_ROW_LENGTH))
// | -                goto endRow;
// | -
// | -            if (gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN
// | -                && (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// | -                    || giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)) {
// | -                if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// | -                    && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1)
// | -                    goto endRow;
// | -                if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1
// | -                    && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)
// | -                    goto endRow;
// | -
// | -                if (giWalkingTo < giWalkingFrom) {
// | -                    if (moatCell[IDX(row)] == giWalkingFrom
// | -                        || moatCell[IDX(row)] == giWalkingFrom2)
// | -                        goto drawMoat;
// | -                    goto endRow;
// | +        do {
// | +            b32 drawMoatLayer = false;
// | +            if (m_inCastleCombat == 0
// | +                || (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_MOAT)) == 0)
// | +                break;
// | +            if (row == DRAW_CASTLE_GATE_ROW && m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN)
// | +                break;
// | +            if (giWalkingTo == moatCell[IDX(row)] || giWalkingTo2 == moatCell[IDX(row)]
// | +                || giWalkingFrom == moatCell[IDX(row)] || giWalkingFrom2 == moatCell[IDX(row)]) {
// | +                if (abs(giWalkingTo - giWalkingFrom) <= 1) {
// | +                    drawMoatLayer = true;
// |                  } else {
// | -                    if (moatCell[IDX(row)] == giWalkingTo || moatCell[IDX(row)] == giWalkingTo2)
// | -                        goto drawMoat;
// | -                    goto endRow;
// | -                }
// | -            } else {
// | +                    if (IDX(row)
// | +                        != (giWalkingTo / COMBAT_GRID_ROW_LENGTH > giWalkingFrom / COMBAT_GRID_ROW_LENGTH
// | +                            ? giWalkingTo / COMBAT_GRID_ROW_LENGTH : giWalkingFrom / COMBAT_GRID_ROW_LENGTH))
// | +                        break;
// | +                    if (gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN
// | +                        && (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// | +                            || giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)) {
// | +                        if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW
// | +                            && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1)
// | +                            break;
// | +                        if (giWalkingTo / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW - 1
// | +                            && giWalkingFrom / COMBAT_GRID_ROW_LENGTH == COMBAT_CASTLE_GATE_ROW)
// | +                            break;
// | +                        if (giWalkingTo < giWalkingFrom)
// | +                            drawMoatLayer = moatCell[IDX(row)] == giWalkingFrom || moatCell[IDX(row)] == giWalkingFrom2;
// | +                        else
// | +                            drawMoatLayer = moatCell[IDX(row)] == giWalkingTo || moatCell[IDX(row)] == giWalkingTo2;
// | +                    } else {
// |                  i32 drawbridgeTop;
// |                  i32 drawbridgeBottom;
// |
// | @@ -481,12 +474,12 @@
// |                      drawbridgeBottom - drawbridgeTop + 1,
// |                      0
// |                  );
// | -                goto endRow;
// | -            }
// | -        } else {
// | -            if (m_hexCells[moatCell[IDX(row)]].m_occupantSide == COMBAT_SIDE_NONE)
// | -                goto endRow;
// | -        drawMoat:
// | +                    }
// | +                }
// | +            } else {
// | +                drawMoatLayer = m_hexCells[moatCell[IDX(row)]].m_occupantSide != COMBAT_SIDE_NONE;
// | +            }
// | +            if (drawMoatLayer) {
// |              m_combatIcons[IDX(COMBAT_ICON_MOAT)]
// |                  ->CombatClipDrawToBuffer(
// |                      0, 0, IDX(row), &m_moatLimits[IDX(row)], ICON_DRAW_NORMAL, 0, NULL, NULL
// | @@ -494,9 +487,8 @@
// |              m_hexCells[moatCell[IDX(row)] - 1].DrawOccupant(ARMY_DRAW_ALL, 1);
// |              m_hexCells[moatCell[IDX(row)]].DrawOccupant(ARMY_DRAW_ALL, 1);
// |              m_hexCells[moatCell[IDX(row)] + 1].DrawOccupant(ARMY_DRAW_ALL, 1);
// | -        }
// | -    endRow:
// | -        ;
// | +            }
// | +        } while (0);
// |          gridWasShowing1 = 0;
// |      }
// |
