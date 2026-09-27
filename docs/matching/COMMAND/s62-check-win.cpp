// VC6 SP5 COMMAND source family s62-check-win; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-check-win/results.json; RVA 0x2d369
// Complete 2 arms; elapsed 2.222327s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 517 / adf8e455ce98eca6 / 168bfd2686c953b1 / 26:26 / 26,0,0,0 / True
// 1 / {'local_role_subset': 'word', 'candidate': 'baseline'} / 517 / b25fb19429be98a0 / 168bfd2686c953b1 / 26:26 / 26,0,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | i32 combatManager::CheckWin(struct tag_message* message) {
// |     b32 combatEnded = false;
// |     if (IsWinner(m_currentSide) != 0) {
// |         combatEnded = true;
// |         if (IsWinner(OppositeCombatSide(m_currentSide)) != 0)
// |             m_combatResult = COMBAT_RESULT_DRAW;
// |         else
// |             m_combatResult = CombatResultForSide(m_currentSide);
// |     } else if (IsWinner(OppositeCombatSide(m_currentSide)) != 0) {
// |         combatEnded = true;
// |         m_combatResult = CombatResultForSide(OppositeCombatSide(m_currentSide));
// |     } else if (m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0) {
// |         combatEnded = true;
// |         gbRetreatWin = true;
// |         m_combatResult = m_sideRetreated[0] != 0 ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
// |     }
// |
// |     if (combatEnded != 0 && m_combatResult != COMBAT_RESULT_DRAW) {
// |         b32 armyAlive = false;
// |         i32 H2_UNUSED(unusedWinWord37);
// |         i32 armyIndex;
// |         for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
// |             if (m_armies[IDX(m_combatResult)][armyIndex].m_monsterType != CREATURE_NONE
// |                 && m_armies[IDX(m_combatResult)][armyIndex].m_quantity > 0
// |                 && HAS(m_armies[IDX(m_combatResult)][armyIndex].m_monster.flags.all,
// |                        MONSTER_FLAGS_SUMMONED)
// |                        == 0) {
// |                 armyAlive = true;
// |             }
// |         }
// |         if (armyAlive == 0)
// |             m_combatResult = COMBAT_RESULT_DRAW;
// |     }
// |
// |     if (combatEnded != 0) {
// |         DoVictory(m_combatResult);
// |         if (gbNoShowCombat == 0) {
// |             message->type = COMBAT_WIN_MESSAGE;
// |             message->payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
// |         }
// |     }
// |     return combatEnded;
// | }
// ALTERNATIVE word (unified source delta from BEFORE):
// | --- before
// | +++ word
// | @@ -17,7 +17,6 @@
// |
// |      if (combatEnded != 0 && m_combatResult != COMBAT_RESULT_DRAW) {
// |          b32 armyAlive = false;
// | -        i32 H2_UNUSED(unusedWinWord37);
// |          i32 armyIndex;
// |          for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
// |              if (m_armies[IDX(m_combatResult)][armyIndex].m_monsterType != CREATURE_NONE
