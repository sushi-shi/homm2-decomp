// VC6 SP5 COMMAND source family s62-reset-round; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-reset-round/results.json; RVA 0x2d1a5
// Complete 2 arms; elapsed 2.203061s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 452 / 8324a4b5c296349c / 4146427e38d75e3d / 17:17 / 17,0,0,0 / True
// 1 / {'local_role_subset': 'word', 'candidate': 'baseline'} / 452 / 0853afaa61d82a34 / 4146427e38d75e3d / 17:17 / 17,0,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::ResetRound(void) {
// |     i32 H2_UNUSED(unusedResetRoundWord6);
// |
// |     m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
// |     m_heroAlternateDeathAnimationPlayed[0] = m_heroAlternateDeathAnimationPlayed[1] = 0;
// |     m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
// |     m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
// |     m_catapultAttacksRemaining[0] = m_catapultAttackCount[0];
// |     m_catapultAttacksRemaining[1] = m_catapultAttackCount[1];
// |     m_keepAttacksRemaining[0] = 1;
// |     m_keepAttacksRemaining[1] = 1;
// |     m_heroCastSpell[0] = m_heroCastSpell[1] = false;
// |
// |     memset(gpCombatManager->m_removedArmies, 0, sizeof(gpCombatManager->m_removedArmies));
// |     gpCombatManager->m_removedArmyPresent = 0;
// |
// |     CombatSide side;
// |     i32 armyIndex;
// |     for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
// |         for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
// |             army* currentArmy = m_armies[IDX(side)] + armyIndex;
// |             if (currentArmy->m_quantity > 0) {
// |                 currentArmy->m_monster.flags.abilityFlags &= MONSTER_FLAGS_ROUND_PERSISTENT_MASK;
// |                 if (currentArmy->m_monsterType == CREATURE_TROLL
// |                     || currentArmy->m_monsterType == CREATURE_WAR_TROLL)
// |                     currentArmy->m_hitPointsLost = 0;
// |                 currentArmy->DecrementSpellRounds();
// |                 if (currentArmy->m_roundCounter == 0)
// |                     currentArmy->ProcessDeath(1);
// |             }
// |         }
// |     }
// |
// |     if (gpCombatManager->m_removedArmyPresent != 0)
// |         gpCombatManager->MakeCreaturesVanish();
// |     m_currentSpeed = ROUND_INITIAL_SPEED;
// | }
// ALTERNATIVE word (unified source delta from BEFORE):
// | --- before
// | +++ word
// | @@ -1,5 +1,4 @@
// |  void combatManager::ResetRound(void) {
// | -    i32 H2_UNUSED(unusedResetRoundWord6);
// |
// |      m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
// |      m_heroAlternateDeathAnimationPlayed[0] = m_heroAlternateDeathAnimationPlayed[1] = 0;
