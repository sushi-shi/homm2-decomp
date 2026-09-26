// VC6 SP5 COMMAND source family s62-right-click; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-right-click/results.json; RVA 0x2d964
// Complete 8 arms; elapsed 6.597177s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 491 / 106638e40b5eeaf3 / 67942683d66d7364 / 35:35 / 35,0,0,0 / True
// 1 / {'local_role_subset': 'army_index', 'candidate': 'baseline'} / 471 / ceb7fc53fe4e7370 / 48600a62a56f1a6c / 35:35 / 34,1,0,0 / True
// 2 / {'local_role_subset': 'row', 'candidate': 'baseline'} / 477 / fd044923159b08da / 8bc9dce99d458ddb / 35:35 / 34,1,0,0 / True
// 3 / {'local_role_subset': 'row_army_index', 'candidate': 'baseline'} / 457 / 8225ecf098ae2fc6 / e3c6b14316550c4c / 35:35 / 33,2,0,0 / True
// 4 / {'local_role_subset': 'column', 'candidate': 'baseline'} / 477 / 1bda3eda8762fedf / 8bc9dce99d458ddb / 35:35 / 34,1,0,0 / True
// 5 / {'local_role_subset': 'column_army_index', 'candidate': 'baseline'} / 457 / 1c283cdb6e192264 / e3c6b14316550c4c / 35:35 / 33,2,0,0 / True
// 6 / {'local_role_subset': 'column_row', 'candidate': 'baseline'} / 463 / 89f8c2b6ab690297 / 27cea001be301756 / 35:35 / 34,1,0,0 / True
// 7 / {'local_role_subset': 'column_row_army_index', 'candidate': 'baseline'} / 443 / ef481b60b05b0be6 / 84a84ec4195ac5e0 / 35:35 / 33,2,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | i32 combatManager::RightClick(i32 hexIndex) {
// |     i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |     i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |     if (hexIndex == INVALID_HEX)
// |         return 0;
// |
// |     switch (hexIndex) {
// |         case COMBAT_BALLISTA_HEX:
// |             if (m_inCastleCombat != 0)
// |                 ViewBallista(1);
// |             return 0;
// |         case COMBAT_GRID_RIGHT_HERO_HEX:
// |             if (m_heroes[1] != NULL) {
// |                 ViewGeneral(COMBAT_DEFENDER_SIDE, 0, 1);
// |                 ResetMouse();
// |             }
// |             return 0;
// |         case COMBAT_GRID_LEFT_SPECIAL_HEX:
// |             if (m_heroes[0] != NULL) {
// |                 ViewGeneral(COMBAT_ATTACKER_SIDE, 0, 1);
// |                 ResetMouse();
// |             }
// |             return 0;
// |         default:
// |             if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1)
// |                 return 0;
// |
// |             CombatSide side = m_hexCells[hexIndex].m_occupantSide;
// |             i32 H2_UNUSED(armyIdx) = m_hexCells[hexIndex].m_occupantIndex;
// |             if (m_hexCells[hexIndex].m_blocked != 0
// |                 && (gpCombatManager->m_inCastleCombat == 0
// |                     || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// |                     || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// |                         && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// |                             || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// |                                        .m_occupantSide
// |                                    != COMBAT_SIDE_NONE
// |                             || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// |                                        .m_deadOccupantCount
// |                                    != 0)))) {
// |                 return 0;
// |             } else {
// |                 if (side != COMBAT_SIDE_NONE) {
// |                     switch (side) {
// |                         case COMBAT_ATTACKER_SIDE:
// |                         case COMBAT_DEFENDER_SIDE:
// |                             gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |                             ViewArmy(
// |                                 &m_armies[IDX(side)][m_hexCells[m_selectedHex].m_occupantIndex],
// |                                 1
// |                             );
// |                             ResetMouse();
// |                             return 0;
// |                     }
// |                 } else {
// |                     return 0;
// |                 }
// |             }
// |             break;
// |     }
// |     return 0;
// | }
// ALTERNATIVE army_index (unified source delta from BEFORE):
// | --- before
// | +++ army_index
// | @@ -26,7 +26,6 @@
// |                  return 0;
// |
// |              CombatSide side = m_hexCells[hexIndex].m_occupantSide;
// | -            i32 H2_UNUSED(armyIdx) = m_hexCells[hexIndex].m_occupantIndex;
// |              if (m_hexCells[hexIndex].m_blocked != 0
// |                  && (gpCombatManager->m_inCastleCombat == 0
// |                      || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// ALTERNATIVE row (unified source delta from BEFORE):
// | --- before
// | +++ row
// | @@ -1,6 +1,5 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// |      i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -    i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// |
// ALTERNATIVE row_army_index (unified source delta from BEFORE):
// | --- before
// | +++ row_army_index
// | @@ -1,6 +1,5 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// |      i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -    i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// |
// | @@ -26,7 +25,6 @@
// |                  return 0;
// |
// |              CombatSide side = m_hexCells[hexIndex].m_occupantSide;
// | -            i32 H2_UNUSED(armyIdx) = m_hexCells[hexIndex].m_occupantIndex;
// |              if (m_hexCells[hexIndex].m_blocked != 0
// |                  && (gpCombatManager->m_inCastleCombat == 0
// |                      || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// ALTERNATIVE column (unified source delta from BEFORE):
// | --- before
// | +++ column
// | @@ -1,5 +1,4 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// | -    i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |      i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// ALTERNATIVE column_army_index (unified source delta from BEFORE):
// | --- before
// | +++ column_army_index
// | @@ -1,5 +1,4 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// | -    i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |      i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// | @@ -26,7 +25,6 @@
// |                  return 0;
// |
// |              CombatSide side = m_hexCells[hexIndex].m_occupantSide;
// | -            i32 H2_UNUSED(armyIdx) = m_hexCells[hexIndex].m_occupantIndex;
// |              if (m_hexCells[hexIndex].m_blocked != 0
// |                  && (gpCombatManager->m_inCastleCombat == 0
// |                      || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// ALTERNATIVE column_row (unified source delta from BEFORE):
// | --- before
// | +++ column_row
// | @@ -1,6 +1,4 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// | -    i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -    i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// |
// ALTERNATIVE column_row_army_index (unified source delta from BEFORE):
// | --- before
// | +++ column_row_army_index
// | @@ -1,6 +1,4 @@
// |  i32 combatManager::RightClick(i32 hexIndex) {
// | -    i32 H2_UNUSED(col) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// | -    i32 H2_UNUSED(row) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |      if (hexIndex == INVALID_HEX)
// |          return 0;
// |
// | @@ -26,7 +24,6 @@
// |                  return 0;
// |
// |              CombatSide side = m_hexCells[hexIndex].m_occupantSide;
// | -            i32 H2_UNUSED(armyIdx) = m_hexCells[hexIndex].m_occupantIndex;
// |              if (m_hexCells[hexIndex].m_blocked != 0
// |                  && (gpCombatManager->m_inCastleCombat == 0
// |                      || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
