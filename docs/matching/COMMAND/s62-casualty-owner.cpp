// VC6 SP5 COMMAND source family s62-casualty-owner; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-casualty-owner/results.json; RVA 0x2ec2c
// Complete 13 arms; elapsed 10.372837s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'bounded_owner_cleanup': 'keep', 'candidate': 'baseline'} / 2517 / 9f666fa2cee14f27 / 2a4dd1899c1ee621 / 57:57 / 57,0,0,0 / True
// 1 / {'bounded_owner_cleanup': 'single_0', 'candidate': 'baseline'} / 2508 / cad5c1e14620864b / 4662f90a9818bc5d / 57:57 / 56,1,0,0 / True
// 2 / {'bounded_owner_cleanup': 'single_1', 'candidate': 'baseline'} / 2508 / d0780eec8df8c855 / 4662f90a9818bc5d / 57:57 / 56,1,0,0 / True
// 3 / {'bounded_owner_cleanup': 'single_2', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 4 / {'bounded_owner_cleanup': 'single_3', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 5 / {'bounded_owner_cleanup': 'single_4', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 6 / {'bounded_owner_cleanup': 'single_5', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 7 / {'bounded_owner_cleanup': 'single_6', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 8 / {'bounded_owner_cleanup': 'single_7', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 9 / {'bounded_owner_cleanup': 'single_8', 'candidate': 'baseline'} / 2517 / 71779d78b6593389 / 45d75415505b548f / 57:57 / 57,0,0,0 / True
// 10 / {'bounded_owner_cleanup': 'geometry', 'candidate': 'baseline'} / 2499 / 36c53e0133e17ba4 / c6c890d7c79da371 / 57:57 / 56,1,0,0 / True
// 11 / {'bounded_owner_cleanup': 'words', 'candidate': 'baseline'} / 2517 / 77d20684e7c5a365 / a26dff101fad6b4d / 57:57 / 57,0,0,0 / True
// 12 / {'bounded_owner_cleanup': 'all', 'candidate': 'baseline'} / 2499 / f6fbd4d985d5730c / c7803cc7cdae164f / 57:57 / 56,1,0,0 / True

// AXIS bounded_owner_cleanup
// BEFORE (complete exact span):
// | void combatManager::ShowDeadArmies(class heroWindow* window) {
// |     i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |     i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |     i32 spacing_6;
// |     i32 y_7;
// |     i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |     i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |     i32 side_4;
// |     i32 H2_UNUSED(unusedCasualtyWord116);
// |     i32 H2_UNUSED(unusedCasualtyWord92);
// |     i32 H2_UNUSED(unusedCasualtyWord86);
// |     i32 H2_UNUSED(unusedCasualtyWord52);
// |     i32 H2_UNUSED(unusedCasualtyWord46);
// |     i32 H2_UNUSED(unusedCasualtyWord13);
// |     i32 H2_UNUSED(unusedCasualtyWord0);
// |     i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// |     i32 armyIndex;
// |     i32 displayedCount;
// |     i32 startX_0;
// |     char* text_1;
// |     icon* monsterIcons;
// |
// |     for (side_4 = 0; side_4 < WIN_LOSE_WIDGET_COUNT; ++side_4) {
// |         m_winLoseBottomWidgets[side_4] = NULL;
// |         m_winLoseBottomTextWidgets[side_4] = NULL;
// |     }
// |     for (side_4 = 0; side_4 < COMBAT_SIDE_COUNT; ++side_4) {
// |         casualtyQuantity_13[side_4] = 0;
// |         for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; ++armyIndex) {
// |             if (m_armies[side_4][armyIndex].m_monsterType != CREATURE_NONE
// |                 && m_armies[side_4][armyIndex].m_initialQuantity
// |                        > m_armies[side_4][armyIndex].m_quantity) {
// |                 casualtyType_1[side_4][casualtyQuantity_13[side_4]] =
// |                     IDX(m_armies[side_4][armyIndex].m_monsterType);
// |                 casualtyCount[side_4][casualtyQuantity_13[side_4]] =
// |                     m_armies[side_4][armyIndex].m_initialQuantity
// |                     - m_armies[side_4][armyIndex].m_quantity;
// |                 ++casualtyQuantity_13[side_4];
// |             }
// |         }
// |     }
// |
// |     text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
// |     sprintf(text_1, "\xcf\xee\xf2\xe5\xf0\xe8");
// |     m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] = new textWidget(
// |         CASUALTY_TITLE_X,
// |         CASUALTY_TITLE_Y,
// |         CASUALTY_WINDOW_WIDTH,
// |         CASUALTY_TEXT_HEIGHT,
// |         text_1,
// |         "smalfont.fnt",
// |         FONT_DRAW_DEFAULT,
// |         CASUALTY_TEXT_WIDGET_ID,
// |         WIDGET_KIND_TEXT,
// |         FONT_ALIGN_CENTER
// |     );
// |     if (m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] == NULL)
// |         MemError();
// |     window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET], -1);
// |
// |     for (side_4 = 0; side_4 < COMBAT_SIDE_COUNT; ++side_4) {
// |         y_7 = side_4 ? CASUALTY_DEFENDER_Y : CASUALTY_ATTACKER_Y;
// |         text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
// |         sprintf(
// |             text_1,
// |             side_4 == IDX(COMBAT_ATTACKER_SIDE)
// |                 ? "\xcd\xe0\xef\xe0\xe4\xe0\xe2\xf8\xe8\xe5:" /* "Нападавшие:" */
// |                 : "\xce\xe1\xee\xf0\xee\xed\xff\xe2\xf8\xe8\xe5\xf1\xff:" /* "Оборонявшиеся:" */
// |         );
// |         m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4] = new textWidget(
// |             CASUALTY_TITLE_X,
// |             y_7 + CASUALTY_SIDE_LABEL_Y_OFFSET,
// |             CASUALTY_WINDOW_WIDTH,
// |             CASUALTY_TEXT_HEIGHT,
// |             text_1,
// |             "smalfont.fnt",
// |             FONT_DRAW_DEFAULT,
// |             CASUALTY_TEXT_WIDGET_ID,
// |             WIDGET_KIND_TEXT,
// |             FONT_ALIGN_CENTER
// |         );
// |         if (m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4] == NULL)
// |             MemError();
// |         window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4], -1);
// |
// |         if (casualtyQuantity_13[side_4] <= 0) {
// |             text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_NONE_CAPACITY));
// |             sprintf(text_1, "\xcd\xe5\xf2" /* "Нет" */);
// |             m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE] = new textWidget(
// |                 CASUALTY_TITLE_X,
// |                 y_7 + CASUALTY_NONE_Y_OFFSET,
// |                 CASUALTY_WINDOW_WIDTH,
// |                 CASUALTY_TEXT_HEIGHT,
// |                 text_1,
// |                 "smalfont.fnt",
// |                 FONT_DRAW_DEFAULT,
// |                 side_4 * CASUALTY_WIDGET_ID_STRIDE + CASUALTY_TEXT_WIDGET_ID_FIRST,
// |                 WIDGET_KIND_TEXT,
// |                 FONT_ALIGN_CENTER
// |             );
// |             if (m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE] == NULL)
// |                 MemError();
// |             window->AddWidget(m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE], -1);
// |         }
// |
// |         monsterIcons = gpResourceManager->GetIcon("mons32.icn");
// |         displayedCount = casualtyQuantity_13[side_4] < CASUALTY_DISPLAY_LIMIT
// |                                 ? casualtyQuantity_13[side_4]
// |                                 : CASUALTY_DISPLAY_LIMIT;
// |         spacing_6 = CASUALTY_ICON_SPACING;
// |         startX_0 = (CASUALTY_WINDOW_WIDTH - spacing_6 * displayedCount) / CASUALTY_CENTER_DIVISOR
// |                    + CASUALTY_CENTER_X_OFFSET;
// |         for (armyIndex = 0; armyIndex < displayedCount; ++armyIndex) {
// |             m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
// |                 new iconWidget(
// |                     startX_0 + spacing_6 * armyIndex + CASUALTY_ICON_CELL_WIDTH / 2
// |                         - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->x
// |                         + (CASUALTY_ICON_CELL_WIDTH
// |                            - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->w)
// |                               / CASUALTY_CENTER_DIVISOR
// |                         + 1,
// |                     y_7 + CASUALTY_ICON_TOP_OFFSET
// |                         - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->y
// |                         + (CASUALTY_ICON_CELL_WIDTH
// |                            - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->h),
// |                     CASUALTY_ICON_WIDTH,
// |                     CASUALTY_ICON_HEIGHT,
// |                     "mons32.icn",
// |                     casualtyType_1[side_4][armyIndex],
// |                     ICON_DRAW_NORMAL,
// |                     side_4 * CASUALTY_WIDGET_ID_STRIDE + armyIndex
// |                         + CASUALTY_ICON_WIDGET_ID_FIRST,
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     1
// |                 );
// |             if (m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] == NULL)
// |                 MemError();
// |
// |             text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_QUANTITY_CAPACITY));
// |             sprintf(text_1, "%d", casualtyCount[side_4][armyIndex]);
// |             m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
// |                 new textWidget(
// |                     startX_0 + spacing_6 * armyIndex + CASUALTY_QUANTITY_X_OFFSET,
// |                     y_7 + CASUALTY_QUANTITY_Y_OFFSET,
// |                     CASUALTY_QUANTITY_WIDTH,
// |                     CASUALTY_QUANTITY_HEIGHT,
// |                     text_1,
// |                     "smalfont.fnt",
// |                     FONT_DRAW_DEFAULT,
// |                     side_4 * CASUALTY_WIDGET_ID_STRIDE + armyIndex
// |                         + CASUALTY_TEXT_WIDGET_ID_FIRST,
// |                     WIDGET_KIND_TEXT,
// |                     FONT_ALIGN_CENTER
// |                 );
// |             if (m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex]
// |                 == NULL)
// |                 MemError();
// |             window->AddWidget(
// |                 m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
// |                 -1
// |             );
// |             window->AddWidget(
// |                 m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
// |                 -1
// |             );
// |         }
// |         gpResourceManager->Dispose(monsterIcons);
// |     }
// | }
// ALTERNATIVE single_0 (unified source delta from BEFORE):
// | --- before
// | +++ single_0
// | @@ -1,5 +1,4 @@
// |  void combatManager::ShowDeadArmies(class heroWindow* window) {
// | -    i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |      i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 spacing_6;
// |      i32 y_7;
// ALTERNATIVE single_1 (unified source delta from BEFORE):
// | --- before
// | +++ single_1
// | @@ -3,7 +3,6 @@
// |      i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 spacing_6;
// |      i32 y_7;
// | -    i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// |      i32 H2_UNUSED(unusedCasualtyWord116);
// ALTERNATIVE single_2 (unified source delta from BEFORE):
// | --- before
// | +++ single_2
// | @@ -6,7 +6,6 @@
// |      i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// | -    i32 H2_UNUSED(unusedCasualtyWord116);
// |      i32 H2_UNUSED(unusedCasualtyWord92);
// |      i32 H2_UNUSED(unusedCasualtyWord86);
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// ALTERNATIVE single_3 (unified source delta from BEFORE):
// | --- before
// | +++ single_3
// | @@ -7,7 +7,6 @@
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// |      i32 H2_UNUSED(unusedCasualtyWord116);
// | -    i32 H2_UNUSED(unusedCasualtyWord92);
// |      i32 H2_UNUSED(unusedCasualtyWord86);
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// |      i32 H2_UNUSED(unusedCasualtyWord46);
// ALTERNATIVE single_4 (unified source delta from BEFORE):
// | --- before
// | +++ single_4
// | @@ -8,7 +8,6 @@
// |      i32 side_4;
// |      i32 H2_UNUSED(unusedCasualtyWord116);
// |      i32 H2_UNUSED(unusedCasualtyWord92);
// | -    i32 H2_UNUSED(unusedCasualtyWord86);
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// |      i32 H2_UNUSED(unusedCasualtyWord46);
// |      i32 H2_UNUSED(unusedCasualtyWord13);
// ALTERNATIVE single_5 (unified source delta from BEFORE):
// | --- before
// | +++ single_5
// | @@ -9,7 +9,6 @@
// |      i32 H2_UNUSED(unusedCasualtyWord116);
// |      i32 H2_UNUSED(unusedCasualtyWord92);
// |      i32 H2_UNUSED(unusedCasualtyWord86);
// | -    i32 H2_UNUSED(unusedCasualtyWord52);
// |      i32 H2_UNUSED(unusedCasualtyWord46);
// |      i32 H2_UNUSED(unusedCasualtyWord13);
// |      i32 H2_UNUSED(unusedCasualtyWord0);
// ALTERNATIVE single_6 (unified source delta from BEFORE):
// | --- before
// | +++ single_6
// | @@ -10,7 +10,6 @@
// |      i32 H2_UNUSED(unusedCasualtyWord92);
// |      i32 H2_UNUSED(unusedCasualtyWord86);
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// | -    i32 H2_UNUSED(unusedCasualtyWord46);
// |      i32 H2_UNUSED(unusedCasualtyWord13);
// |      i32 H2_UNUSED(unusedCasualtyWord0);
// |      i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// ALTERNATIVE single_7 (unified source delta from BEFORE):
// | --- before
// | +++ single_7
// | @@ -11,7 +11,6 @@
// |      i32 H2_UNUSED(unusedCasualtyWord86);
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// |      i32 H2_UNUSED(unusedCasualtyWord46);
// | -    i32 H2_UNUSED(unusedCasualtyWord13);
// |      i32 H2_UNUSED(unusedCasualtyWord0);
// |      i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// |      i32 armyIndex;
// ALTERNATIVE single_8 (unified source delta from BEFORE):
// | --- before
// | +++ single_8
// | @@ -12,7 +12,6 @@
// |      i32 H2_UNUSED(unusedCasualtyWord52);
// |      i32 H2_UNUSED(unusedCasualtyWord46);
// |      i32 H2_UNUSED(unusedCasualtyWord13);
// | -    i32 H2_UNUSED(unusedCasualtyWord0);
// |      i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// |      i32 armyIndex;
// |      i32 displayedCount;
// ALTERNATIVE geometry (unified source delta from BEFORE):
// | --- before
// | +++ geometry
// | @@ -1,9 +1,7 @@
// |  void combatManager::ShowDeadArmies(class heroWindow* window) {
// | -    i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |      i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 spacing_6;
// |      i32 y_7;
// | -    i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// |      i32 H2_UNUSED(unusedCasualtyWord116);
// ALTERNATIVE words (unified source delta from BEFORE):
// | --- before
// | +++ words
// | @@ -6,13 +6,6 @@
// |      i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// | -    i32 H2_UNUSED(unusedCasualtyWord116);
// | -    i32 H2_UNUSED(unusedCasualtyWord92);
// | -    i32 H2_UNUSED(unusedCasualtyWord86);
// | -    i32 H2_UNUSED(unusedCasualtyWord52);
// | -    i32 H2_UNUSED(unusedCasualtyWord46);
// | -    i32 H2_UNUSED(unusedCasualtyWord13);
// | -    i32 H2_UNUSED(unusedCasualtyWord0);
// |      i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// |      i32 armyIndex;
// |      i32 displayedCount;
// ALTERNATIVE all (unified source delta from BEFORE):
// | --- before
// | +++ all
// | @@ -1,18 +1,9 @@
// |  void combatManager::ShowDeadArmies(class heroWindow* window) {
// | -    i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |      i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 spacing_6;
// |      i32 y_7;
// | -    i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      i32 side_4;
// | -    i32 H2_UNUSED(unusedCasualtyWord116);
// | -    i32 H2_UNUSED(unusedCasualtyWord92);
// | -    i32 H2_UNUSED(unusedCasualtyWord86);
// | -    i32 H2_UNUSED(unusedCasualtyWord52);
// | -    i32 H2_UNUSED(unusedCasualtyWord46);
// | -    i32 H2_UNUSED(unusedCasualtyWord13);
// | -    i32 H2_UNUSED(unusedCasualtyWord0);
// |      i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
// |      i32 armyIndex;
// |      i32 displayedCount;
