// VC6 SP5 source product c79-events-ratio-player; RVA 0x47fff.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 3 arms, 3.988702s; no compile failure or timeout; source restored.
// build/source-variant-batch/c79-events-ratio-player/results.json and build/c79-events-ratio-player-axes.json
// Disposition: apply trial2. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'floating-division': 'both', 'candidate': 'baseline'} / 1841 / c9a16a228b4496b7 / 6f7877899664a4f3 / 89:89 / 89,0,0,0,0 / True
// 1 / {'floating-division': 'implicit-0', 'candidate': 'baseline'} / 1841 / c9a16a228b4496b7 / 6f7877899664a4f3 / 89:89 / 89,0,0,0,0 / True
// 2 / {'floating-division': 'implicit-1', 'candidate': 'baseline'} / 1841 / c9a16a228b4496b7 / 6f7877899664a4f3 / 89:89 / 89,0,0,0,0 / True

// AXIS floating-division
// BEFORE (complete exact span):
// | VA(0x00447fff, 0x731)
// | void advManager::PlayerMonsterInteract(
// |     mapCell* cell,
// |     mapCell* combatCell,
// |     hero* eventHero,
// |     i32* handled,
// |     i32 x,
// |     i32 y,
// |     i32 unused,
// |     i32 combatX,
// |     i32 combatY
// | ) {
// |     CreatureType monsterType;
// |     CombatResult result;
// |     i32 creatureCount;
// |     float armyRatio;
// |     i32 forceJoin;
// |     i32 joiningCost;
// |     char monsterText[MONSTER_OFFER_BUFFER_SIZE];
// |     i32 numJoining;
// |
// |     unused = 0;
// |     gpMouseManager->ShowColorPointer();
// |     monsterType = static_cast<CreatureType>(cell->m_objectIndex);
// |     forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
// |     creatureCount = cell->m_objectMetadata & MONSTER_COUNT_MASK;
// |     armyRatio = static_cast<double>(
// |                     gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
// |                 )
// |                 / static_cast<double>(
// |                     creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue
// |                 );
// |
// |     if (gbInCampaign
// |         && ((gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
// |              && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF))
// |             || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_OGRE_ALLIANCE)]
// |                 && (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD))
// |             || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DRAGON_ALLIANCE)]
// |                 && (monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON
// |                     || monsterType == CREATURE_BLACK_DRAGON)))) {
// |         if (!eventHero->m_army.CanJoin(monsterType)) {
// |             if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
// |                 NormalDialog(
// |                     "\xc3\xed\xee\xec\xfb \xef\xf0\xe8\xe2\xe5\xf2\xf1\xf2\xe2\xf3\xfe "
// |                         "\xe2\xe0\xf1: \"\xc4\xf0\xf3\xe3 \xd0\xee\xeb\xe0\xed\xe4\xe0 "
// |                         "- \xed\xe0\xf8 \xe4\xf0\xf3\xe3. \xc2\xfb \xec\xee\xe6\xe5\xf2\xe5 "
// |                         "\xef\xf0\xee\xe9\xf2\xe8.\"",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
// |                 NormalDialog(
// |                     "\xce\xe3\xf0\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xe2\xe0\xf1: "
// |                         "\"\xd1\xee\xfe\xe7\xed\xe8\xea\xe8 \xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xe0 "
// |                         "\xec\xee\xe3\xf3\xf2 \xef\xf0\xee\xe9\xf2\xe8.\"",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             else
// |                 NormalDialog(
// |                     "\xc4\xf0\xe0\xea\xee\xed\xfb, \xef\xee\xf1\xec\xee\xf2\xf0\xe5\xe2 "
// |                         "\xed\xe0 \xe2\xe0\xf1, \xef\xf0\xee\xec\xee\xeb\xe2\xe8\xeb\xe8: "
// |                         "\"\xcd\xe0\xf8 \xe0\xeb\xfc\xff\xed\xf1 \xf1 \xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xee\xec "
// |                         "\xe2\xfb\xed\xf3\xe6\xe4\xe0\xe5\xf2 \xed\xe0\xf1 \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
// |                         "\xea \xe2\xe0\xec.  \xca \xed\xe5\xf1\xf7\xe0\xf1\xf2\xfc\xfe, "
// |                         "\xf3 \xe2\xe0\xf1 \xed\xe5\xf2 \xe4\xeb\xff \xed\xe0\xf1 \xec\xe5\xf1\xf2\xe0. "
// |                         "\xca\xe0\xea\xe0\xff \xe6\xe0\xeb\xee\xf1\xf2\xfc!\"  \xc8 "
// |                         "\xe1\xfb\xf1\xf2\xf0\xee \xf3\xeb\xe5\xf2\xe5\xeb\xe8.",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             *handled = 1;
// |         } else {
// |             if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
// |                 NormalDialog(
// |                     "\xc3\xed\xee\xec\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xf1\xe2\xee\xe8\xf5 "
// |                         "\xf1\xee\xfe\xe7\xed\xe8\xea\xee\xe2 \xe8 \xf0\xe0\xe4\xfb "
// |                         "\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff \xea "
// |                         "\xed\xe8\xec.",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
// |                 NormalDialog(
// |                     "\xce\xe3\xf0\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xed\xe0\xf1, "
// |                         "\xea\xe0\xea \xe2\xf0\xe0\xe3\xee\xe2 \xe3\xed\xee\xec\xee\xe2 "
// |                         "\xe8 \xf0\xe0\xe4\xfb \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
// |                         "\xea \xe2\xe0\xec. ",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             else
// |                 NormalDialog(
// |                     "\xc3\xee\xf0\xe4\xe5\xeb\xe8\xe2\xfb\xe5 \xe4\xf0\xe0\xea\xee\xed\xfb "
// |                         "\xe2\xfb\xed\xf3\xe6\xe4\xe5\xed\xfb \xe1\xfb\xeb\xe8 \xef\xf0\xe8\xe7\xed\xe0\xf2\xfc "
// |                         "\xe2 \xe2\xe0\xf1 \xf1\xe2\xee\xe8\xf5 '\xf1\xee\xfe\xe7\xed\xe8\xea\xee\xe2' "
// |                         "\xe8 \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
// |                         "\xea \xe2\xe0\xec.",
// |                     NORMAL_DIALOG_INFO,
// |                     -1,
// |                     -1,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1,
// |                     0
// |                 );
// |             eventHero->m_army.Add(monsterType, creatureCount, -1);
// |             *handled = 1;
// |         }
// |         return;
// |     }
// |
// |     if (gbInCampaign && gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)]
// |         && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
// |         NormalDialog(
// |             "\"\xc2\xf0\xe0\xe3\xe8 \xe3\xed\xee\xec\xee\xe2! \xc1\xe5\xe3\xe8\xf2\xe5, "
// |                 "\xe5\xf1\xeb\xe8 \xe6\xe8\xe7\xed\xfc \xe4\xee\xf0\xee\xe3\xe0.\"",
// |             NORMAL_DIALOG_INFO,
// |             -1,
// |             -1,
// |             -1,
// |             0,
// |             -1,
// |             0,
// |             -1,
// |             0
// |         );
// |         *handled = 1;
// |         return;
// |     }
// |
// |     if (xIsPlayingExpansionCampaign && xCampaign.HasAward(AWARD_ELVEN_ALLIANCE)
// |         && (monsterType == CREATURE_ELF || monsterType == CREATURE_GRAND_ELF)) {
// |         *handled = 1;
// |         if (eventHero->m_army.CanJoin(monsterType)) {
// |             NormalDialog(
// |                 "\xc5\xe4\xe2\xe0 \xe2\xfb \xef\xee\xe4\xee\xf8\xeb\xe8 \xea "
// |                     "\xee\xf2\xf0\xff\xe4\xf3 \xfd\xeb\xfc\xf4\xee\xe2, \xea\xe0\xea "
// |                     "\xe8\xf5 \xe2\xee\xe6\xe0\xea \xef\xf0\xe8\xe7\xe2\xe0\xeb "
// |                     "\xf1\xe2\xee\xe9 \xee\xf2\xf0\xff\xe4 \xea \xee\xf0\xf3\xe6\xe8\xfe. "
// |                     "\xce\xed \xf1\xea\xe0\xe7\xe0\xeb \xe8\xec: \"\xca\xf2\xee "
// |                     "\xe8\xe7 \xe2\xe0\xf1 \xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee "
// |                     "\xf1\xec\xe5\xeb, \xf7\xf2\xee\xe1\xfb \xe2\xf1\xf2\xf3\xef\xe8\xf2\xfc "
// |                     "\xe2 \xe0\xf0\xec\xe8\xfe \xed\xe0\xf8\xe5\xe3\xee \xec\xf3\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xee\xe3\xee "
// |                     "\xf1\xee\xfe\xe7\xed\xe8\xea\xe0?\" \xc2\xe5\xf1\xfc \xee\xf2\xf0\xff\xe4 "
// |                     "\xe5\xe4\xe8\xed\xee\xe4\xf3\xf8\xed\xee \xe2\xf1\xf2\xf3\xef\xe8\xeb "
// |                     "\xe2 \xe2\xe0\xf8\xf3 \xe0\xf0\xec\xe8\xfe.",
// |                 NORMAL_DIALOG_INFO,
// |                 -1,
// |                 -1,
// |                 -1,
// |                 0,
// |                 -1,
// |                 0,
// |                 -1,
// |                 0
// |             );
// |             eventHero->m_army.Add(monsterType, creatureCount, -1);
// |         } else {
// |             NormalDialog(
// |                 "\xdd\xeb\xfc\xf4\xfb \xed\xe0\xf1\xf2\xee\xf0\xee\xe6\xe8\xeb\xe8\xf1\xfc "
// |                     "\xef\xf0\xe8 \xe2\xe0\xf8\xe5\xec \xef\xf0\xe8\xe1\xeb\xe8\xe6\xe5\xed\xe8\xe8. "
// |                     "\xc8\xf5 \xe2\xee\xe6\xe0\xea \xee\xe1\xf0\xe0\xf2\xe8\xeb\xf1\xff "
// |                     "\xea \xe2\xe0\xec \xf1\xee \xf1\xeb\xee\xe2\xe0\xec\xe8: \"\xcc\xfb "
// |                     "\xed\xe5 \xec\xee\xe6\xe5\xec \xef\xf0\xe8\xed\xff\xf2\xfc "
// |                     "\xf3\xf7\xe0\xf1\xf2\xe8\xe5 \xe2 \xe2\xe0\xf8\xe8\xf5 \xe4\xe5\xeb\xe0\xf5, "
// |                     "\xf1\xee\xfe\xe7\xed\xe8\xea! \xcf\xf0\xee\xf5\xee\xe4\xe8, "
// |                     "\xed\xe0\xf8\xe0 \xef\xee\xe1\xe5\xe4\xe0 \xe1\xf3\xe4\xe5\xf2 "
// |                     "\xe2\xe0\xf8\xe5\xe9.\"",
// |                 NORMAL_DIALOG_INFO,
// |                 -1,
// |                 -1,
// |                 -1,
// |                 0,
// |                 -1,
// |                 0,
// |                 -1,
// |                 0
// |             );
// |         }
// |         return;
// |     }
// |
// |     if (eventHero->m_army.CanJoin(monsterType)
// |         && armyRatio
// |             > 2.0 /* MONSTER_STRENGTH_JOIN */
// |         && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
// |         && monsterType != CREATURE_EARTH_ELEMENTAL && monsterType != CREATURE_AIR_ELEMENTAL
// |         && monsterType != CREATURE_FIRE_ELEMENTAL && monsterType != CREATURE_WATER_ELEMENTAL) {
// |         if (forceJoin) {
// |             sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[IDX(monsterType)]);
// |             EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
// |             if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
// |                 eventHero->m_army.Add(monsterType, creatureCount, -1);
// |                 *handled = 1;
// |                 return;
// |             } else {
// |                 EventWindow(
// |                     EVENT_TEXT_MONSTER_REFUSAL,
// |                     NORMAL_DIALOG_INFO,
// |                     "",
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1
// |                 );
// |                 goto fightMonsters;
// |             }
// |         } else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                    != HERO_SKILL_LEVEL_NONE) {
// |             if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                 == HERO_SKILL_LEVEL_EXPERT)
// |                 numJoining = creatureCount;
// |             else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
// |                      == HERO_SKILL_LEVEL_ADVANCED)
// |                 numJoining = creatureCount / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
// |             else
// |                 numJoining = creatureCount / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
// |             if (!numJoining)
// |                 numJoining = 1;
// |
// |             joiningCost = gMonsterDatabase[IDX(monsterType)].cost * creatureCount;
// |             if (joiningCost > gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]) {
// |                 if (armyRatio
// |                     > 5.0 /* MONSTER_STRENGTH_FLEE */)
// |                     goto monstersFlee;
// |                 else
// |                     goto fightMonsters;
// |             }
// |
// |             if (creatureCount == 1) {
// |                 sprintf(
// |                     gText,
// |                     "%s \xed\xe0\xf5\xee\xe4\xe8\xf2\xf1\xff \xef\xee\xe4 \xe2\xef\xe5\xf7\xe0\xf2\xeb\xe5\xed\xe8\xe5\xec "
// |                         "\xe2\xe0\xf8\xe5\xe9 \xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xf7\xed\xee\xf1\xf2\xe8, "
// |                         "\xe8 \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 \xf1\xe2\xee\xe8 "
// |                         "\xf3\xf1\xeb\xf3\xe3\xe8 \xe2\xe0\xf8\xe5\xec\xf3 \xe2\xee\xe9\xf1\xea\xf3 "
// |                         "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb?",
// |                     gArmyNames[IDX(monsterType)],
// |                     joiningCost
// |                 );
// |             } else {
// |                 sprintf(
// |                     gText,
// |                     "\xc2\xee\xe8\xed\xfb \xef\xee\xea\xee\xf0\xe5\xed\xfb \xe2\xe0\xf8\xe8\xec "
// |                         "\xf8\xe0\xf0\xec\xee\xec \xe8 \xe2\xfb\xe4\xe2\xe8\xed\xf3\xeb\xe8 "
// |                         "\xef\xf0\xe5\xe4\xeb\xee\xe6\xe5\xed\xe8\xe5:\n\n"
// |                 );
// |                 if (numJoining == creatureCount)
// |                     sprintf(
// |                         monsterText,
// |                         "\xce\xf2\xf0\xff\xe4 \xe8\xe7 %d %s \xe2\xf1\xf2\xf3\xef\xff\xf2 "
// |                             "\xe2 \xf0\xff\xe4\xfb \xe2\xe0\xf8\xe5\xe9 \xe0\xf0\xec\xe8\xe8 "
// |                             "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb "
// |                             "\xef\xf0\xe8\xed\xff\xf2\xfc \xe8\xf5?",
// |                         creatureCount,
// |                         gArmyNamesPlural[IDX(monsterType)],
// |                         joiningCost
// |                     );
// |                 else
// |                     sprintf(
// |                         monsterText,
// |                         "%d \xe8\xe7 %d %s \xe2\xf1\xf2\xf3\xef\xff\xf2 \xe2 \xf0\xff\xe4\xfb "
// |                             "\xe2\xe0\xf8\xe5\xe9 \xe0\xf0\xec\xe8\xe8, \xe0 \xee\xf1\xf2\xe0\xe2\xf8\xe8\xe5\xf1\xff "
// |                             "\xee\xf1\xf2\xe0\xe2\xff\xf2 \xe2\xe0\xf1 \xe2 \xef\xee\xea\xee\xe5 "
// |                             "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb?",
// |                         numJoining,
// |                         creatureCount,
// |                         gArmyNamesPlural[IDX(monsterType)],
// |                         joiningCost
// |                     );
// |                 strcat(gText, monsterText);
// |             }
// |
// |             NormalDialog(
// |                 gText,
// |                 NORMAL_DIALOG_CONFIRM,
// |                 -1,
// |                 -1,
// |                 IDX(RES_GOLD),
// |                 joiningCost,
// |                 -1,
// |                 0,
// |                 -1,
// |                 0
// |             );
// |             if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
// |                 eventHero->m_army.Add(monsterType, numJoining, -1);
// |                 *handled = 1;
// |                 gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -= joiningCost;
// |                 return;
// |             } else {
// |                 EventWindow(
// |                     EVENT_TEXT_MONSTER_REFUSAL,
// |                     NORMAL_DIALOG_INFO,
// |                     "",
// |                     -1,
// |                     0,
// |                     -1,
// |                     0,
// |                     -1
// |                 );
// |                 goto fightMonsters;
// |             }
// |         }
// |     }
// |
// |     if (armyRatio > MONSTER_STRENGTH_FLEE) {
// |     monstersFlee:
// |         sprintf(
// |             gText,
// |             "\xc3\xf0\xf3\xef\xef\xe0 %s, \xf1\xf2\xf0\xe0\xf8\xe0\xf1\xfc "
// |                 "\xec\xee\xf9\xe8 \xe2\xe0\xf8\xe5\xe3\xee \xe2\xee\xe8\xed\xf1\xf2\xe2\xe0, "
// |                 "\xe1\xf0\xee\xf1\xe8\xeb\xe0\xf1\xfc \xe2\xf0\xe0\xf1\xf1\xfb\xef\xed\xf3\xfe. "
// |                 "\xd1\xee\xe8\xe7\xe2\xee\xeb\xe8\xf2\xe5 \xe8\xe7\xeb\xee\xe2\xe8\xf2\xfc "
// |                 "\xe8\xf5 \xe8 \xe7\xe0\xf1\xf2\xe0\xe2\xe8\xf2\xfc \xe4\xf0\xe0\xf2\xfc\xf1\xff?",
// |             gArmyNamesPlural[IDX(monsterType)]
// |         );
// |         EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
// |         if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
// |             goto fightMonsters;
// |         *handled = 1;
// |         return;
// |     }
// |
// | fightMonsters:
// |     result = CombatMonsterEvent(
// |         eventHero,
// |         monsterType,
// |         creatureCount,
// |         combatCell,
// |         x,
// |         y,
// |         unused,
// |         combatX,
// |         combatY,
// |         CREATURE_NONE,
// |         0,
// |         0,
// |         CREATURE_NONE,
// |         0,
// |         0
// |     );
// |     if (result == COMBAT_RESULT_ATTACKER || result == COMBAT_RESULT_DRAW)
// |         *handled = 1;
// | }
// |
// OPTION both
// OPTION implicit-0
// | --- before
// | +++ implicit-0
// | @@ -24,9 +24,7 @@
// |      monsterType = static_cast<CreatureType>(cell->m_objectIndex);
// |      forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
// |      creatureCount = cell->m_objectMetadata & MONSTER_COUNT_MASK;
// | -    armyRatio = static_cast<double>(
// | -                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
// | -                )
// | +    armyRatio = (gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0))
// |                  / static_cast<double>(
// |                      creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue
// |                  );
// OPTION implicit-1
// | --- before
// | +++ implicit-1
// | @@ -27,9 +27,7 @@
// |      armyRatio = static_cast<double>(
// |                      gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
// |                  )
// | -                / static_cast<double>(
// | -                    creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue
// | -                );
// | +                / (creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue);
// |
// |      if (gbInCampaign
// |          && ((gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
