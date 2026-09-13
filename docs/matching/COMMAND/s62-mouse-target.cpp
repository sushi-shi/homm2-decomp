// VC6 SP5 COMMAND source family s62-mouse-target; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-mouse-target/results.json; RVA 0x2c389
// Complete 2 arms; elapsed 2.215192s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 1397 / 57b4bbd0fa485000 / ff4fa93d94b936c4 / 93:93 / 93,0,0,0 / True
// 1 / {'local_role_subset': 'target', 'candidate': 'baseline'} / 1358 / fd6fc081802fe7ee / f820f1886c9cc3cb / 93:93 / 92,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::CheckSetMouseDirection(i32 mouseX, i32 mouseY, i32 targetHex) {
// |     if (m_gridSelectionDisabled != 0)
// |         return;
// |     if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
// |         return;
// |
// |     i32 relativeX_7 =
// |         mouseX - (targetHex % COMBAT_GRID_ROW_LENGTH - 1) * COMBAT_HEX_HORIZONTAL_STEP;
// |     relativeX_7 -= COMBAT_HEX_GRID_LEFT_ORIGIN;
// |     if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
// |         relativeX_7 -= COMBAT_HEX_ROW_STAGGER;
// |     i32 relativeY_9 = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN
// |                       - targetHex / COMBAT_GRID_ROW_LENGTH * COMBAT_HEX_VERTICAL_STEP;
// |     relativeY_9 -= DIRECTION_MOUSE_TOP_OFFSET;
// |     relativeX_7 -= COMBAT_HEX_ROW_STAGGER;
// |     relativeY_9 -= DIRECTION_MOUSE_CENTER_OFFSET;
// |
// |     i32 sector_6 = 0;
// |     if (relativeX_7 < 0) {
// |         if (relativeY_9 < 0)
// |             sector_6 += DIRECTION_SECTOR_THREE_QUARTERS;
// |         else
// |             sector_6 += DIRECTION_SECTOR_HALF;
// |     } else {
// |         if (relativeY_9 < 0)
// |             sector_6 += DIRECTION_SECTOR_ORIGIN;
// |         else
// |             sector_6 += DIRECTION_SECTOR_QUARTER;
// |     }
// |
// |     relativeX_7 = abs(relativeX_7);
// |     relativeY_9 = abs(relativeY_9);
// |     /* The parenthesised divisor cast is load-bearing: without it VC6 folds the
// |        divisor into a single `fidiv`, while retail keeps both operands on the x87
// |        stack (`fild`, `fild`, `fdivp`). */
// |     float ratio_3 = static_cast<float>(relativeX_7) / (static_cast<float>(relativeY_9));
// |     if (sector_6 == 0 || sector_6 == DIRECTION_SECTOR_HALF) {
// |         if (ratio_3 > COMBAT_DIRECTION_SLOPE_STEEPEST)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
// |         else if (ratio_3 > COMBAT_DIRECTION_SLOPE_STEEP)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_STEEP;
// |         else if (ratio_3 > COMBAT_DIRECTION_SLOPE_DIAGONAL)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
// |         else if (ratio_3 > COMBAT_DIRECTION_SLOPE_SHALLOW)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
// |         else if (ratio_3 > COMBAT_DIRECTION_SLOPE_SHALLOWEST)
// |             sector_6++;
// |     } else {
// |         if (ratio_3 < COMBAT_DIRECTION_SLOPE_SHALLOWEST)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
// |         else if (ratio_3 < COMBAT_DIRECTION_SLOPE_SHALLOW)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_STEEP;
// |         else if (ratio_3 < COMBAT_DIRECTION_SLOPE_DIAGONAL)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
// |         else if (ratio_3 < COMBAT_DIRECTION_SLOPE_STEEP)
// |             sector_6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
// |         else if (ratio_3 < COMBAT_DIRECTION_SLOPE_STEEPEST)
// |             sector_6++;
// |     }
// |
// |     if (m_directionMap[sector_6] == m_mouseDirection)
// |         return;
// |
// |     m_mouseDirection = m_directionMap[sector_6];
// |     CombatHexDirection direction_5 =
// |         OppositeDirection(static_cast<CombatHexDirection>(m_directionMap[sector_6]));
// |     CombatHexDirection directionCopy_1 = direction_5;
// |     CombatHexDirection alternateDirection = COMBAT_DIRECTION_INVALID;
// |     army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |     army* H2_UNUSED(targetArmy_13) =
// |         &m_armies[IDX(currentArmy->m_targetSide)][currentArmy->m_targetIndex];
// |
// |     if (direction_5 == COMBAT_DIRECTION_WIDE_WEST || direction_5 == COMBAT_DIRECTION_WIDE_EAST) {
// |         if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
// |             if (currentArmy->m_facing == ARMY_FACING_RIGHT
// |                 && direction_5 == COMBAT_DIRECTION_WIDE_WEST) {
// |                 direction_5 = COMBAT_DIRECTION_NORTHWEST;
// |                 alternateDirection = COMBAT_DIRECTION_NORTHEAST;
// |             } else if (currentArmy->m_facing == ARMY_FACING_RIGHT
// |                        && direction_5 == COMBAT_DIRECTION_WIDE_EAST) {
// |                 direction_5 = COMBAT_DIRECTION_SOUTHWEST;
// |                 alternateDirection = COMBAT_DIRECTION_SOUTHEAST;
// |             } else if (currentArmy->m_facing == ARMY_FACING_LEFT
// |                        && direction_5 == COMBAT_DIRECTION_WIDE_WEST) {
// |                 direction_5 = COMBAT_DIRECTION_NORTHEAST;
// |                 alternateDirection = COMBAT_DIRECTION_NORTHWEST;
// |             } else {
// |                 direction_5 = COMBAT_DIRECTION_SOUTHEAST;
// |                 alternateDirection = COMBAT_DIRECTION_SOUTHWEST;
// |             }
// |         } else {
// |             if (m_hexCells[targetHex - 1].m_occupantSide == currentArmy->m_targetSide
// |                 && m_hexCells[targetHex - 1].m_occupantIndex == currentArmy->m_targetIndex) {
// |                 targetHex--;
// |             }
// |             if (direction_5 == COMBAT_DIRECTION_WIDE_WEST)
// |                 direction_5 = COMBAT_DIRECTION_NORTHEAST;
// |             else
// |                 direction_5 = COMBAT_DIRECTION_SOUTHEAST;
// |         }
// |     } else {
// |         if (currentArmy->m_facing == ARMY_FACING_RIGHT
// |             && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
// |             if (direction_5 == COMBAT_DIRECTION_NORTHWEST || direction_5 == COMBAT_DIRECTION_WEST
// |                 || direction_5 == COMBAT_DIRECTION_SOUTHWEST)
// |                 targetHex--;
// |         } else if (currentArmy->m_facing == ARMY_FACING_LEFT
// |                    && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
// |                    && (direction_5 == COMBAT_DIRECTION_NORTHEAST
// |                        || direction_5 == COMBAT_DIRECTION_EAST
// |                        || direction_5 == COMBAT_DIRECTION_SOUTHEAST)) {
// |             targetHex++;
// |         }
// |     }
// |
// |     m_directionTargetHex = m_adjacency[targetHex][IDX(direction_5)];
// |     i32 rearHex_5 = IGNORED_HEX;
// |     if (currentArmy->m_facing == ARMY_FACING_LEFT
// |         && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
// |         rearHex_5 = m_directionTargetHex - 1;
// |     }
// |     if (currentArmy->m_facing == ARMY_FACING_RIGHT
// |         && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
// |         rearHex_5 = m_directionTargetHex + 1;
// |     }
// |     if (ValidHexToStandOn(m_directionTargetHex) == 0 || ValidHexToStandOn(rearHex_5) == 0) {
// |         if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
// |             && (directionCopy_1 == COMBAT_DIRECTION_WIDE_WEST
// |                 || directionCopy_1 == COMBAT_DIRECTION_WIDE_EAST)) {
// |             if (currentArmy->m_facing == ARMY_FACING_RIGHT)
// |                 m_directionTargetHex++;
// |             else
// |                 m_directionTargetHex--;
// |         } else {
// |             if (alternateDirection != COMBAT_DIRECTION_INVALID)
// |                 m_directionTargetHex = m_adjacency[targetHex][IDX(alternateDirection)];
// |         }
// |     }
// |     gpMouseManager->SetPointer(m_mouseDirection + POINTER_ATTACK_OFFSET);
// | }
// ALTERNATIVE target (unified source delta from BEFORE):
// | --- before
// | +++ target
// | @@ -67,8 +67,6 @@
// |      CombatHexDirection directionCopy_1 = direction_5;
// |      CombatHexDirection alternateDirection = COMBAT_DIRECTION_INVALID;
// |      army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | -    army* H2_UNUSED(targetArmy_13) =
// | -        &m_armies[IDX(currentArmy->m_targetSide)][currentArmy->m_targetIndex];
// |
// |      if (direction_5 == COMBAT_DIRECTION_WIDE_WEST || direction_5 == COMBAT_DIRECTION_WIDE_EAST) {
// |          if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
