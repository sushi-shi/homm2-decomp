// VC6 SP5 source product s73-events-monster-values; RVA 0x48730.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.031137s; no compile failure or timeout; source restored.
// build/source-variant-batch/s73-events-monster-values/results.json and build/s73-events-monster-values-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'independent-values': 'keep', 'candidate': 'baseline'} / 980 / adbe4cfa4b829b03 / e9b6a002f53a44b5 / 41:41 / 41,0,0,0,0 / True
// 1 / {'independent-values': 'recover', 'candidate': 'baseline'} / 980 / 50ee9d77cf22d9cb / e9b6a002f53a44b5 / 41:41 / 41,0,0,0,0 / True

// AXIS independent-values
// BEFORE (complete exact span):
// | VA(0x00448730, 0x3d4)
// | void advManager::ComputerMonsterInteract(mapCell* cell, hero* eventHero, i32* handled) {
// |     CreatureType monsterType;
// |     i32 replacementSlot;
// |     i32 creatureCount[MONSTER_COMBAT_VALUE_COUNT];
// |     i32 bought;
// |     float armyRatio;
// |     u32 forceJoin;
// |     i32 purchaseWorth;
// |     i32 joiningCost;
// |     i32 joiningCount;
// |
// |     monsterType = static_cast<CreatureType>(cell->m_objectIndex);
// |     creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
// |     forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
// |     armyRatio = static_cast<double>(
// |                     gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
// |                 )
// |                 / static_cast<double>(
// |                     creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// |                     * gMonsterDatabase[IDX(monsterType)].fightValue
// |                 );
// |
// |     if (eventHero->m_army.CanJoin(monsterType)
// |         && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && armyRatio > MONSTER_STRENGTH_JOIN
// |         && monsterType != CREATURE_GHOST && monsterType != CREATURE_EARTH_ELEMENTAL
// |         && monsterType != CREATURE_AIR_ELEMENTAL && monsterType != CREATURE_FIRE_ELEMENTAL
// |         && monsterType != CREATURE_WATER_ELEMENTAL) {
// |         if (forceJoin) {
// |             gpPhilAI->EvaluateOneTimeCreaturePurchase(
// |                 monsterType,
// |                 creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// |                 1,
// |                 bought,
// |                 purchaseWorth,
// |                 replacementSlot
// |             );
// |             if (bought > 0) {
// |                 gpGame->GiveArmy(
// |                     &eventHero->m_army,
// |                     monsterType,
// |                     creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// |                     replacementSlot
// |                 );
// |                 *handled = 1;
// |             } else {
// |                 *handled = 1;
// |             }
// |         } else {
// |             if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                 != HERO_SKILL_LEVEL_NONE) {
// |                 if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                     == HERO_SKILL_LEVEL_EXPERT)
// |                     joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT];
// |                 else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                          == HERO_SKILL_LEVEL_ADVANCED)
// |                     joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// |                                    / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
// |                 else
// |                     joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// |                                    / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
// |                 if (!joiningCount)
// |                     joiningCount = 1;
// |
// |                 joiningCost = static_cast<i32>(
// |                     gMonsterDatabase[IDX(monsterType)].cost
// |                     * creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// |                     * 0.75 /* MONSTER_AI_JOIN_COST_FRACTION */
// |                 );
// |                 if (joiningCost
// |                     > gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]) {
// |                     if (armyRatio > MONSTER_STRENGTH_FLEE)
// |                         goto computerMonstersFlee;
// |                     else
// |                         goto fightComputerMonsters;
// |                 }
// |                 gpPhilAI->EvaluateOneTimeCreaturePurchase(
// |                     monsterType,
// |                     creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// |                     1,
// |                     bought,
// |                     purchaseWorth,
// |                     replacementSlot
// |                 );
// |                 if (bought > 0) {
// |                     gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -= joiningCost;
// |                     gpGame->GiveArmy(
// |                         &eventHero->m_army,
// |                         monsterType,
// |                         joiningCount,
// |                         replacementSlot
// |                     );
// |                     *handled = 1;
// |                 } else
// |                     goto fightComputerMonsters;
// |             }
// |         }
// |     }
// |
// |     if (armyRatio > MONSTER_STRENGTH_FLEE) {
// |     computerMonstersFlee:
// |         gpAdvManager->GiveExperience(
// |             eventHero,
// |             creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// |                 * gMonsterDatabase[IDX(monsterType)].hitPoints,
// |             1
// |         );
// |         eventHero->CheckLevel();
// |         if (eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
// |             && eventHero->m_army.CanJoin(CREATURE_SKELETON)) {
// |             gpGame->GiveArmy(
// |                 &eventHero->m_army,
// |                 CREATURE_SKELETON,
// |                 static_cast<i32>(
// |                     static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
// |                     * 0.1 /* MONSTER_NECROMANCY_FRACTION */
// |                     * eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
// |                 ),
// |                 -1
// |             );
// |         }
// |         *handled = 1;
// |         return;
// |     } else {
// |     fightComputerMonsters:
// |         creatureCount[MONSTER_COMBAT_RESULT] =
// |             gpPhilAI->CombatMonsterEvent(eventHero, monsterType, creatureCount, cell);
// |         if (creatureCount[MONSTER_COMBAT_RESULT] != 0) {
// |             *handled = 1;
// |         } else {
// |             cell->m_objectMetadata =
// |                 (cell->m_objectMetadata & MONSTER_FLAGS_MASK)
// |                 + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & MONSTER_COUNT_MASK);
// |         }
// |     }
// | }
// |
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -2,7 +2,8 @@
// |  void advManager::ComputerMonsterInteract(mapCell* cell, hero* eventHero, i32* handled) {
// |      CreatureType monsterType;
// |      i32 replacementSlot;
// | -    i32 creatureCount[MONSTER_COMBAT_VALUE_COUNT];
// | +    i32 creatureCount;
// | +    i32 combatResult;
// |      i32 bought;
// |      float armyRatio;
// |      u32 forceJoin;
// | @@ -11,13 +12,13 @@
// |      i32 joiningCount;
// |
// |      monsterType = static_cast<CreatureType>(cell->m_objectIndex);
// | -    creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
// | +    creatureCount = cell->m_objectMetadata & MONSTER_COUNT_MASK;
// |      forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
// |      armyRatio = static_cast<double>(
// |                      gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
// |                  )
// |                  / static_cast<double>(
// | -                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// | +                    creatureCount
// |                      * gMonsterDatabase[IDX(monsterType)].fightValue
// |                  );
// |
// | @@ -29,7 +30,7 @@
// |          if (forceJoin) {
// |              gpPhilAI->EvaluateOneTimeCreaturePurchase(
// |                  monsterType,
// | -                creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// | +                creatureCount,
// |                  1,
// |                  bought,
// |                  purchaseWorth,
// | @@ -39,7 +40,7 @@
// |                  gpGame->GiveArmy(
// |                      &eventHero->m_army,
// |                      monsterType,
// | -                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// | +                    creatureCount,
// |                      replacementSlot
// |                  );
// |                  *handled = 1;
// | @@ -51,20 +52,20 @@
// |                  != HERO_SKILL_LEVEL_NONE) {
// |                  if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                      == HERO_SKILL_LEVEL_EXPERT)
// | -                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT];
// | +                    joiningCount = creatureCount;
// |                  else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                           == HERO_SKILL_LEVEL_ADVANCED)
// | -                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// | +                    joiningCount = creatureCount
// |                                     / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
// |                  else
// | -                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// | +                    joiningCount = creatureCount
// |                                     / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
// |                  if (!joiningCount)
// |                      joiningCount = 1;
// |
// |                  joiningCost = static_cast<i32>(
// |                      gMonsterDatabase[IDX(monsterType)].cost
// | -                    * creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// | +                    * creatureCount
// |                      * 0.75 /* MONSTER_AI_JOIN_COST_FRACTION */
// |                  );
// |                  if (joiningCost
// | @@ -76,7 +77,7 @@
// |                  }
// |                  gpPhilAI->EvaluateOneTimeCreaturePurchase(
// |                      monsterType,
// | -                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
// | +                    creatureCount,
// |                      1,
// |                      bought,
// |                      purchaseWorth,
// | @@ -101,7 +102,7 @@
// |      computerMonstersFlee:
// |          gpAdvManager->GiveExperience(
// |              eventHero,
// | -            creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
// | +            creatureCount
// |                  * gMonsterDatabase[IDX(monsterType)].hitPoints,
// |              1
// |          );
// | @@ -112,7 +113,7 @@
// |                  &eventHero->m_army,
// |                  CREATURE_SKELETON,
// |                  static_cast<i32>(
// | -                    static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
// | +                    static_cast<double>(creatureCount)
// |                      * 0.1 /* MONSTER_NECROMANCY_FRACTION */
// |                      * eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
// |                  ),
// | @@ -123,14 +124,14 @@
// |          return;
// |      } else {
// |      fightComputerMonsters:
// | -        creatureCount[MONSTER_COMBAT_RESULT] =
// | -            gpPhilAI->CombatMonsterEvent(eventHero, monsterType, creatureCount, cell);
// | -        if (creatureCount[MONSTER_COMBAT_RESULT] != 0) {
// | +        combatResult =
// | +            gpPhilAI->CombatMonsterEvent(eventHero, monsterType, &creatureCount, cell);
// | +        if (combatResult != 0) {
// |              *handled = 1;
// |          } else {
// |              cell->m_objectMetadata =
// |                  (cell->m_objectMetadata & MONSTER_FLAGS_MASK)
// | -                + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & MONSTER_COUNT_MASK);
// | +                + (creatureCount & MONSTER_COUNT_MASK);
// |          }
// |      }
// |  }
