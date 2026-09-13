// VC6 SP5 COMMAND source family r21-command-widgets; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r21-command-widgets/results.json; RVA 0x2ec2c
// Complete 2 arms; elapsed 2.239823s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'semantic_counter_scope': 'keep', 'candidate': 'baseline'} / 2517 / 9f666fa2cee14f27 / 2a4dd1899c1ee621 / 57:57 / 57,0,0,0 / True
// 1 / {'semantic_counter_scope': 'separate_counter', 'candidate': 'baseline'} / 2517 / 587cc44f716c5aa5 / 81a55fa503740847 / 57:57 / 57,0,0,0 / True

// AXIS semantic_counter_scope
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
// ALTERNATIVE separate_counter (unified source delta from BEFORE):
// | --- before
// | +++ separate_counter
// | @@ -20,9 +20,9 @@
// |      char* text_1;
// |      icon* monsterIcons;
// |
// | -    for (side_4 = 0; side_4 < WIN_LOSE_WIDGET_COUNT; ++side_4) {
// | -        m_winLoseBottomWidgets[side_4] = NULL;
// | -        m_winLoseBottomTextWidgets[side_4] = NULL;
// | +    for (i32 widgetIndex = 0; widgetIndex < WIN_LOSE_WIDGET_COUNT; ++widgetIndex) {
// | +        m_winLoseBottomWidgets[widgetIndex] = NULL;
// | +        m_winLoseBottomTextWidgets[widgetIndex] = NULL;
// |      }
// |      for (side_4 = 0; side_4 < COMBAT_SIDE_COUNT; ++side_4) {
// |          casualtyQuantity_13[side_4] = 0;
