// VC6 SP5 source family r26-drawing-small-roles; parent aab81af5.
// Source record only: each alternative is relative to its own BEFORE, not cumulative.
// No cross-family or TU-state census is claimed.
// build/source-variant-batch/r26-drawing-small-roles/results.json; RVA 0x3aad6
// Complete 2 arms; elapsed 1.704669s; source restored.
// Disposition: retain original; every tested nonoriginal arm changes function text.
// Raw/normalized relocation hashes can reflect private label counter spelling;
// final proof compares actual ordered targets/sites and the complete claimed byte span.
// UpdateMouseGrid has an unchanged EH/FS site-model gap, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2857 / 588287b772bbe9a4 / 74ff48d9bcdae7f1 / 100:96 / 96,0,0,0 / False
// 1 / {'source_family': 'modifier_roles', 'candidate': 'baseline'} / 3013 / 4531fd8a1e9eadce / 2494a1b776033ab3 / 100:96 / 96,0,0,0 / False

// AXIS source_family
// BEFORE (complete exact span):
// | void combatManager::DrawSmallView(i32 viewIndex, i32 updateScreen) {
// |     u8 spellPositions4[COMBAT_SMALL_VIEW_MAX_SPELLS][COMBAT_SMALL_VIEW_MAX_SPELLS]
// |                      [SPELL_POSITION_AXIS_COUNT] = {
// |         {{COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
// |         {{COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
// |         {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_SECOND},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
// |         {{COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
// |         {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_SECOND, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FOURTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_UNUSED_POSITION, COMBAT_SMALL_VIEW_UNUSED_POSITION}},
// |         {{COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_FIRST},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FIRST, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_SPELL_X_THIRD, COMBAT_SMALL_VIEW_SPELL_Y_THIRD},
// |          {COMBAT_SMALL_VIEW_SPELL_X_FIFTH, COMBAT_SMALL_VIEW_SPELL_Y_THIRD}}
// |     };
// |     b32 oldLimit;
// |     i32 H2_UNUSED(unusedSpell1);
// |     army* smallArmy;
// |     IconDrawResult drawResult1;
// |     i32 H2_UNUSED(unusedSlot);
// |     i32 viewX1;
// |     i32 iconX9;
// |     i32 spellSlot1;
// |     i32 spellIcon;
// |     i32 viewY;
// |     i32 iconY9;
// |     i32 visibleSpellCount;
// |
// |     if (gbNoShowCombat != 0)
// |         return;
// |     if (m_combatWindowOpen == 0)
// |         return;
// |     if (gConfig.combatArmyInfoLevel == 0)
// |         return;
// |     if (gbInDrawSmallView != 0)
// |         return;
// |
// |     gbInDrawSmallView = true;
// |     if (m_smallViewSide[viewIndex] == COMBAT_SIDE_NONE
// |         || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_LEFT_X
// |             && m_smallViewSide[viewIndex] == COMBAT_DEFENDER_SIDE)
// |         || (m_smallViewLastX[viewIndex] == COMBAT_SMALL_VIEW_RIGHT_X
// |             && m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)) {
// |         if (m_smallViewLastX[viewIndex] >= 0) {
// |             gbLimitToExtent = true;
// |             giMinExtentX = m_smallViewLastX[viewIndex];
// |             giMinExtentY = m_smallViewLastY[viewIndex];
// |             giMaxExtentX = m_smallViewLastX[viewIndex] + m_smallViewWidth[viewIndex] - 1;
// |             giMaxExtentY = m_smallViewLastY[viewIndex] + m_smallViewHeight[viewIndex] - 1;
// |             DrawFrame(0, 0, 0, 0, 0, 1, 1);
// |             gpWindowManager->UpdateScreenRegion(
// |                 giMinExtentX,
// |                 giMinExtentY,
// |                 giMaxExtentX - giMinExtentX + 1,
// |                 giMaxExtentY - giMinExtentY + 1
// |             );
// |             gbLimitToExtent = false;
// |             m_smallViewLastX[viewIndex] = -1;
// |         }
// |         if (m_smallViewSide[viewIndex] == COMBAT_SIDE_NONE) {
// |             gbInDrawSmallView = false;
// |             return;
// |         }
// |     }
// |
// |     if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
// |         if (viewIndex == 0) {
// |             if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
// |                 viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
// |             else
// |                 viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
// |             viewY = COMBAT_SMALL_VIEW_FULL_Y;
// |         } else {
// |             if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
// |                 viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
// |             else
// |                 viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
// |             if (m_smallViewSide[1] == m_smallViewSide[0])
// |                 viewY = COMBAT_SMALL_VIEW_FULL_RIGHT_Y;
// |             else
// |                 viewY = COMBAT_SMALL_VIEW_FULL_Y;
// |         }
// |         m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
// |         m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_FULL_HEIGHT;
// |     } else {
// |         if (viewIndex == 0) {
// |             if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
// |                 viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
// |             else
// |                 viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
// |             viewY = COMBAT_SMALL_VIEW_COMPACT_Y;
// |         } else {
// |             if (m_smallViewSide[viewIndex] == COMBAT_ATTACKER_SIDE)
// |                 viewX1 = COMBAT_SMALL_VIEW_LEFT_X;
// |             else
// |                 viewX1 = COMBAT_SMALL_VIEW_RIGHT_X;
// |             if (m_smallViewSide[1] == m_smallViewSide[0])
// |                 viewY = COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y;
// |             else
// |                 viewY = COMBAT_SMALL_VIEW_COMPACT_Y;
// |         }
// |         m_smallViewWidth[viewIndex] = COMBAT_SMALL_VIEW_WIDTH;
// |         m_smallViewHeight[viewIndex] = COMBAT_SMALL_VIEW_COMPACT_HEIGHT;
// |     }
// |
// |     m_smallViewLastX[viewIndex] = viewX1;
// |     m_smallViewLastY[viewIndex] = viewY;
// |     oldLimit = gbLimitToExtent;
// |     if (updateScreen != 0)
// |         gbLimitToExtent = false;
// |
// |     smallArmy = &m_armies[IDX(m_smallViewSide[viewIndex])][m_smallViewArmyIndex[viewIndex]];
// |     drawResult1 = m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_BACKGROUND)]->CombatClipDrawToBuffer(
// |         viewX1,
// |         viewY,
// |         gConfig.combatArmyInfoLevel != COMBAT_SMALL_VIEW_FULL_INFO,
// |         &m_smallViewLimits,
// |         ICON_DRAW_NORMAL,
// |         0,
// |         NULL,
// |         NULL
// |     );
// |     viewX1 += COMBAT_SMALL_VIEW_INSET_X;
// |     if (drawResult1 != ICON_DRAW_SKIPPED) {
// |         if (smallArmy->m_quantity > 1)
// |             sprintf(
// |                 gText,
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_QUANTITY_PLURAL)],
// |                 smallArmy->m_quantity
// |             );
// |         else
// |             sprintf(
// |                 gText,
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_QUANTITY_SINGULAR)],
// |                 smallArmy->m_quantity
// |             );
// |         smallFont->DrawBoundedString(
// |             gText,
// |             viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |             viewY + COMBAT_SMALL_VIEW_QUANTITY_Y,
// |             COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |             COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |             FONT_DRAW_DEFAULT,
// |             FONT_ALIGN_CENTER
// |         );
// |
// |         if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO) {
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_ATTACK)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_ATTACK),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_DEFENSE)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_DEFENSE),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_HIT_POINTS)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_HIT_POINTS),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_DAMAGE)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_DAMAGE),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_MORALE)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             smallFont->DrawBoundedString(
// |                 cMiniViewText[IDX(SMALL_VIEW_TEXT_LUCK)],
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_LEFT
// |             );
// |             if (HAS(smallArmy->m_monster.flags.all, COMBAT_ARMY_FLAG_SHOOTER))
// |                 smallFont->DrawBoundedString(
// |                     cMiniViewText[IDX(SMALL_VIEW_TEXT_SHOTS)],
// |                     viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                     viewY + COMBAT_SMALL_VIEW_SHOTS_Y,
// |                     COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                     COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                     FONT_DRAW_DEFAULT,
// |                     FONT_ALIGN_LEFT
// |                 );
// |
// |             sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.attack));
// |             smallFont->DrawBoundedString(
// |                 gText,
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_ATTACK),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_RIGHT
// |             );
// |             sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.defense));
// |             smallFont->DrawBoundedString(
// |                 gText,
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_DEFENSE),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_RIGHT
// |             );
// |             sprintf(gText, "%d", static_cast<u32>(smallArmy->m_monster.hitPoints));
// |             smallFont->DrawBoundedString(
// |                 gText,
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_HIT_POINTS),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_RIGHT
// |             );
// |             sprintf(
// |                 gText,
// |                 "%d-%d",
// |                 static_cast<i32>(smallArmy->m_monster.damageMin),
// |                 static_cast<i32>(smallArmy->m_monster.damageMax)
// |             );
// |             smallFont->DrawBoundedString(
// |                 gText,
// |                 viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                 viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                     + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_DAMAGE),
// |                 COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                 COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                 FONT_DRAW_DEFAULT,
// |                 FONT_ALIGN_RIGHT
// |             );
// |
// |             spellSlot1 = 0;
// |             if (smallArmy->m_morale > 0) {
// |                 for (spellSlot1 = 0; spellSlot1 < smallArmy->m_morale; spellSlot1++)
// |                     m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                         viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// |                             - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                         viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                             + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                         IDX(SMALL_VIEW_GOOD_MORALE_FRAME),
// |                         ICON_DRAW_NORMAL
// |                     );
// |             } else if (smallArmy->m_morale < 0) {
// |                 for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_morale; spellSlot1++)
// |                     m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                         viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// |                             - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                         viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                             + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                         IDX(SMALL_VIEW_BAD_MORALE_FRAME),
// |                         ICON_DRAW_NORMAL
// |                     );
// |             } else {
// |                 m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                     viewX1 + COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X,
// |                     viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                         + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                     IDX(SMALL_VIEW_NEUTRAL_MORALE_FRAME),
// |                     ICON_DRAW_NORMAL
// |                 );
// |             }
// |
// |             spellSlot1 = 0;
// |             if (smallArmy->m_luck > 0) {
// |                 for (spellSlot1 = 0; spellSlot1 < smallArmy->m_luck; spellSlot1++)
// |                     m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                         viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// |                             - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                         viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                             + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                         IDX(SMALL_VIEW_GOOD_LUCK_FRAME),
// |                         ICON_DRAW_NORMAL
// |                     );
// |             } else if (smallArmy->m_luck < 0) {
// |                 for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_luck; spellSlot1++)
// |                     m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                         viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// |                             - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                         viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                             + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                         IDX(SMALL_VIEW_BAD_LUCK_FRAME),
// |                         ICON_DRAW_NORMAL
// |                     );
// |             } else {
// |                 m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                     viewX1 + COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X,
// |                     viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                         + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                     IDX(SMALL_VIEW_NEUTRAL_LUCK_FRAME),
// |                     ICON_DRAW_NORMAL
// |                 );
// |             }
// |
// |             if (HAS(smallArmy->m_monster.flags.all, COMBAT_ARMY_FLAG_SHOOTER)) {
// |                 sprintf(gText, "%d", static_cast<i32>(smallArmy->m_monster.shots));
// |                 smallFont->DrawBoundedString(
// |                     gText,
// |                     viewX1 + COMBAT_SMALL_VIEW_TEXT_X,
// |                     viewY + COMBAT_SMALL_VIEW_SHOTS_Y,
// |                     COMBAT_SMALL_VIEW_TEXT_WIDTH,
// |                     COMBAT_SMALL_VIEW_TEXT_HEIGHT,
// |                     FONT_DRAW_DEFAULT,
// |                     FONT_ALIGN_RIGHT
// |                 );
// |             }
// |         }
// |
// |         visibleSpellCount = smallArmy->m_spellCount < COMBAT_SMALL_VIEW_MAX_SPELLS
// |                                  ? smallArmy->m_spellCount
// |                                  : COMBAT_SMALL_VIEW_MAX_SPELLS;
// |         spellIcon = -1;
// |         for (spellSlot1 = 0; spellSlot1 < visibleSpellCount; spellSlot1++) {
// |             spellIcon++;
// |             while (smallArmy->m_spellInfluence[spellIcon] == 0)
// |                 spellIcon++;
// |
// |             iconX9 = viewX1 + COMBAT_SMALL_VIEW_INSET_X;
// |             if (gConfig.combatArmyInfoLevel == COMBAT_SMALL_VIEW_FULL_INFO)
// |                 iconY9 = viewY + COMBAT_SMALL_VIEW_FULL_SPELL_Y;
// |             else
// |                 iconY9 = viewY + COMBAT_SMALL_VIEW_COMPACT_SPELL_Y;
// |             iconX9 += spellPositions4[visibleSpellCount - 1][spellSlot1][0];
// |             iconY9 += spellPositions4[visibleSpellCount - 1][spellSlot1][1];
// |             iconX9 +=
// |                 (COMBAT_SMALL_VIEW_ICON_SIZE
// |                  - GetIconEntry(m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)], spellIcon)->w)
// |                 >> 1;
// |             iconY9 +=
// |                 (COMBAT_SMALL_VIEW_ICON_SIZE
// |                  - GetIconEntry(m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)], spellIcon)->h)
// |                 >> 1;
// |             m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)]
// |                 ->DrawToBuffer(iconX9, iconY9, spellIcon, ICON_DRAW_NORMAL);
// |         }
// |     }
// |
// |     gbLimitToExtent = oldLimit;
// |     viewX1 -= COMBAT_SMALL_VIEW_INSET_X;
// |     if (updateScreen != 0)
// |         gpWindowManager->UpdateScreenRegion(
// |             viewX1,
// |             viewY,
// |             m_smallViewWidth[viewIndex],
// |             m_smallViewHeight[viewIndex]
// |         );
// |     gbInDrawSmallView = false;
// | }
// ALTERNATIVE modifier_roles (unified delta from BEFORE):
// | --- before
// | +++ modifier_roles
// | @@ -46,6 +46,8 @@
// |      i32 viewX1;
// |      i32 iconX9;
// |      i32 spellSlot1;
// | +    i32 moraleIcon;
// | +    i32 luckIcon;
// |      i32 spellIcon;
// |      i32 viewY;
// |      i32 iconY9;
// | @@ -291,22 +293,22 @@
// |                  FONT_ALIGN_RIGHT
// |              );
// |
// | -            spellSlot1 = 0;
// | +            moraleIcon = 0;
// |              if (smallArmy->m_morale > 0) {
// | -                for (spellSlot1 = 0; spellSlot1 < smallArmy->m_morale; spellSlot1++)
// | +                for (moraleIcon = 0; moraleIcon < smallArmy->m_morale; moraleIcon++)
// |                      m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                          viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// | -                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// | +                            - moraleIcon * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                          viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                              + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                          IDX(SMALL_VIEW_GOOD_MORALE_FRAME),
// |                          ICON_DRAW_NORMAL
// |                      );
// |              } else if (smallArmy->m_morale < 0) {
// | -                for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_morale; spellSlot1++)
// | +                for (moraleIcon = 0; moraleIcon < -smallArmy->m_morale; moraleIcon++)
// |                      m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                          viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// | -                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// | +                            - moraleIcon * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                          viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                              + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_MORALE),
// |                          IDX(SMALL_VIEW_BAD_MORALE_FRAME),
// | @@ -322,22 +324,22 @@
// |                  );
// |              }
// |
// | -            spellSlot1 = 0;
// | +            luckIcon = 0;
// |              if (smallArmy->m_luck > 0) {
// | -                for (spellSlot1 = 0; spellSlot1 < smallArmy->m_luck; spellSlot1++)
// | +                for (luckIcon = 0; luckIcon < smallArmy->m_luck; luckIcon++)
// |                      m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                          viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// | -                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// | +                            - luckIcon * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                          viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                              + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                          IDX(SMALL_VIEW_GOOD_LUCK_FRAME),
// |                          ICON_DRAW_NORMAL
// |                      );
// |              } else if (smallArmy->m_luck < 0) {
// | -                for (spellSlot1 = 0; spellSlot1 < -smallArmy->m_luck; spellSlot1++)
// | +                for (luckIcon = 0; luckIcon < -smallArmy->m_luck; luckIcon++)
// |                      m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)]->DrawToBuffer(
// |                          viewX1 + COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X
// | -                            - spellSlot1 * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// | +                            - luckIcon * COMBAT_SMALL_VIEW_MODIFIER_STEP,
// |                          viewY + COMBAT_SMALL_VIEW_FIRST_STAT_Y
// |                              + COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT * IDX(STAT_ROW_LUCK),
// |                          IDX(SMALL_VIEW_BAD_LUCK_FRAME),
