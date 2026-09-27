// VC6 SP5 Campaign source family r23-campaign-init-roles; parent d7472bf9.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r23-campaign-init-roles/results.json; RVA 0x2387e
// Complete 4 arms; elapsed 2.733045s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2653 / 48b0bea1b939c5c0 / e32ae726f081cfa7 / 120:118 / 116,0,0,2 / False
// 1 / {'source_family': 'army_slot', 'candidate': 'baseline'} / 2653 / a8c021080f5beb54 / a8898acda89b8319 / 120:118 / 116,0,0,2 / False
// 2 / {'source_family': 'hero_record', 'candidate': 'baseline'} / 2653 / ae0916a696854419 / a8898acda89b8319 / 120:118 / 116,0,0,2 / False
// 3 / {'source_family': 'hero_record_army_slot', 'candidate': 'baseline'} / 2653 / add355c35f43fa1c / c06304b1ab4ae5bc / 120:118 / 116,0,0,2 / False

// AXIS source_family
// BEFORE (complete exact span):
// | void game::InitCampaignMap(void) {
// |     playerData* campaignPlayerCurrent9;
// |     i32 bestHeroPriorityLocal;
// |     i32 swappedHero;
// |     i32 heroPositionValue;
// |     i32 scanPositionId;
// |     b32 savedNewGameSetup;
// |     i32 playerSlotSlot4;
// |     SCampaignChoice* choiceBest1;
// |     i32 heroPriorityBest3;
// |     i32 bestHeroPositionCandidate;
// |     i32 selectedChoicePosition0;
// |     i32 H2_UNUSED(mapHeaderResultCampaign3);
// |     CampaignBonusHeroPosition bonusHeroIndexPosition;
// |
// |     selectedChoicePosition0 = m_campaignChoice[IDX(iCurViewSide)][iCurViewMap];
// |     if (m_campaignType != m_campaignStartingSide && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// |         choiceBest1 =
// |             &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][selectedChoicePosition0];
// |     } else {
// |         choiceBest1 =
// |             &campaignChoices[IDX(m_campaignType)][m_campaignScenario][selectedChoicePosition0];
// |     }
// |
// |     gpGame->m_campaignScenarioWon = 0;
// |     memset(m_setupPlayerColor, 0, CAMPAIGN_SETUP_RESET_SIZE);
// |     if (m_campaignScenario + 1 == CAMPAIGN_SWITCHING_SCENARIO + 1
// |         && m_campaignStartingSide != m_campaignType) {
// |         sprintf(
// |             m_mapFilename,
// |             "CAMP%c%02dB.H2C",
// |             m_campaignType == CAMPAIGN_ROLAND ? 'E' : 'G',
// |             m_campaignScenario + 1
// |         );
// |     } else {
// |         sprintf(
// |             m_mapFilename,
// |             "CAMP%c%02d.H2C",
// |             m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E',
// |             m_campaignScenario + 1
// |         );
// |     }
// |     m_newGameInitialized = false;
// |     if (m_campaignScenario == 0)
// |         m_campaignScore = 0;
// |     strcpy(gMapName, m_mapFilename);
// |     mapHeaderResultCampaign3 = GetMapHeader(m_mapFilename, &m_mapHeader);
// |     LoadGame("origdata.bin", 1, 0);
// |     InitNewGame(NULL);
// |
// |     if (choiceBest1->type == CAMPAIGN_CHOICE_ALIGNMENT) {
// |         playerSlotSlot4 = 0;
// |         if (m_campaignType == CAMPAIGN_ARCHIBALD) {
// |             if (m_mapHeader.playerEnabled[0])
// |                 ++playerSlotSlot4;
// |             if (m_mapHeader.playerEnabled[1])
// |                 ++playerSlotSlot4;
// |         }
// |         m_setupPlayerRace[playerSlotSlot4] = choiceBest1->faction;
// |     }
// |
// |     if (m_campaignScenario + 1 <= CAMPAIGN_EASY_SCENARIO_LIMIT)
// |         gpGame->m_difficulty = DIFFICULTY_EASY;
// |     else if (m_campaignScenario + 1 <= CAMPAIGN_NORMAL_SCENARIO_LIMIT)
// |         gpGame->m_difficulty = DIFFICULTY_NORMAL;
// |     else
// |         gpGame->m_difficulty = DIFFICULTY_HARD;
// |     m_playerCount = m_mapHeader.playerCount;
// |     NewMap(gMapName);
// |
// |     bestHeroPositionCandidate = 0;
// |     campaignPlayerCurrent9 = &gpGame->m_players[0];
// |     for (heroPositionValue = 0; heroPositionValue < campaignPlayerCurrent9->m_heroCount;
// |          ++heroPositionValue) {
// |         bestHeroPriorityLocal = -1;
// |         for (scanPositionId = heroPositionValue;
// |              scanPositionId < campaignPlayerCurrent9->m_heroCount;
// |              ++scanPositionId) {
// |             if (gpGame->m_heroRecs[campaignPlayerCurrent9->m_heroIds[scanPositionId]].m_portrait
// |                     == CAMPAIGN_HERO_ROLAND
// |                 || gpGame->m_heroRecs[campaignPlayerCurrent9->m_heroIds[scanPositionId]].m_portrait
// |                        == CAMPAIGN_HERO_ARCHIBALD) {
// |                 heroPriorityBest3 = CAMPAIGN_HERO_PRIORITY_HIGH;
// |             } else if (gpGame->m_heroRecs[campaignPlayerCurrent9->m_heroIds[scanPositionId]]
// |                                .m_portrait
// |                            == CAMPAIGN_HERO_CORLAGON
// |                        || gpGame->m_heroRecs[campaignPlayerCurrent9->m_heroIds[scanPositionId]]
// |                                   .m_portrait
// |                               == CAMPAIGN_HERO_HALTON) {
// |                 heroPriorityBest3 = CAMPAIGN_HERO_PRIORITY_NORMAL;
// |             } else {
// |                 heroPriorityBest3 = 0;
// |             }
// |             if (heroPriorityBest3 > bestHeroPriorityLocal) {
// |                 bestHeroPriorityLocal = heroPriorityBest3;
// |                 bestHeroPositionCandidate = scanPositionId;
// |             }
// |         }
// |         if (bestHeroPriorityLocal != -1) {
// |             swappedHero = campaignPlayerCurrent9->m_heroIds[heroPositionValue];
// |             campaignPlayerCurrent9->m_heroIds[heroPositionValue] =
// |                 campaignPlayerCurrent9->m_heroIds[bestHeroPositionCandidate];
// |             campaignPlayerCurrent9->m_heroIds[bestHeroPositionCandidate] =
// |                 static_cast<i8>(swappedHero);
// |         }
// |     }
// |     if (campaignPlayerCurrent9->m_heroCount)
// |         campaignPlayerCurrent9->m_currentHero = campaignPlayerCurrent9->m_heroIds[0];
// |
// |     switch (choiceBest1->type) {
// |         case CAMPAIGN_CHOICE_RESOURCE:
// |             m_players[0].m_resources[IDX(choiceBest1->resource)] += choiceBest1->amount;
// |             break;
// |         case CAMPAIGN_CHOICE_ARTIFACT:
// |             if (m_players[0].m_heroCount > 0)
// |                 GiveArtifact(
// |                     gpGame->GetHero(m_players[0].m_heroIds[0]),
// |                     choiceBest1->artifact,
// |                     false,
// |                     -1
// |                 );
// |             break;
// |         case CAMPAIGN_CHOICE_SPELL:
// |             if (m_players[0].m_heroCount > 0) {
// |                 bonusHeroIndexPosition = CAMPAIGN_BONUS_HERO_FIRST;
// |                 if (m_campaignType == CAMPAIGN_ROLAND
// |                     && m_campaignScenario + 1 == SCENARIO_SIX
// |                     && m_players[0].m_heroCount > 1)
// |                     bonusHeroIndexPosition = CAMPAIGN_BONUS_HERO_SECOND;
// |                 gpGame->GetHero(m_players[0].m_heroIds[bonusHeroIndexPosition])
// |                     ->m_spells[IDX(choiceBest1->spell)] = 1;
// |             }
// |             break;
// |         case CAMPAIGN_CHOICE_SECONDARY_SKILL:
// |             if (m_players[0].m_heroCount > 0)
// |                 gpGame->GetHero(m_players[0].m_heroIds[0])
// |                     ->SetSS(
// |                         static_cast<HeroSecondarySkill>(choiceBest1->value),
// |                         static_cast<HeroSkillLevel>(choiceBest1->amount)
// |                     );
// |             break;
// |         case CAMPAIGN_CHOICE_CREATURES:
// |             if (m_players[0].m_heroCount > 0)
// |                 gpGame->GetHero(m_players[0].m_heroIds[0])
// |                     ->m_army.Add(choiceBest1->creature, choiceBest1->amount, -1);
// |             break;
// |         case CAMPAIGN_CHOICE_PUZZLE_PIECES:
// |             m_players[0].m_cheatValue = static_cast<i8>(choiceBest1->value);
// |             break;
// |         case CAMPAIGN_CHOICE_EXPERIENCE: {
// |             savedNewGameSetup = gbInNewGameSetup;
// |             gbInNewGameSetup = true;
// |             if (m_players[0].m_heroCount > 0) {
// |                 gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += choiceBest1->value;
// |                 gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
// |             }
// |             gbInNewGameSetup = savedNewGameSetup;
// |             break;
// |         }
// |         case CAMPAIGN_CHOICE_NONE:
// |             break;
// |     }
// |
// |     if ((m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)]
// |          || (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)]
// |              && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1))
// |         && m_players[0].m_heroCount > 0) {
// |         GiveArtifact(gpGame->GetHero(m_players[0].m_heroIds[0]), ARTIFACT_ULTIMATE_CROWN, false, -1);
// |     }
// |     gbRetreatWin = true;
// |
// |     if (m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)]) {
// |         for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_HERO_COUNT; ++heroPositionValue) {
// |             if (gpGame->m_heroRecs[heroPositionValue].m_portrait == CAMPAIGN_HERO_CORLAGON)
// |                 gpGame->m_heroRecs[heroPositionValue].Deallocate(0);
// |         }
// |     }
// |
// |     if (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]) {
// |         hero* armyHero = gpGame->GetHero(m_players[CAMPAIGN_CARRYOVER_PLAYER].m_heroIds[0]);
// |         for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// |              ++heroPositionValue) {
// |             if (armyHero->m_army.m_creatureCounts[heroPositionValue] >= 1)
// |                 armyHero->m_army.m_creatureCounts[heroPositionValue] *=
// |                     CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
// |         }
// |     }
// |
// |     if (m_campaignScenario + 1 == SCENARIO_SEVEN
// |         && m_campaignType == CAMPAIGN_ARCHIBALD) {
// |         b32 savedNewGame = gbInNewGameSetup;
// |         hero* armyHero;
// |         gbInNewGameSetup = true;
// |         armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// |         for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// |              ++heroPositionValue) {
// |             armyHero->m_army.m_creatureTypes[heroPositionValue] = CREATURE_NONE;
// |             armyHero->m_army.m_creatureCounts[heroPositionValue] = 0;
// |         }
// |         switch (armyHero->m_cursorType) {
// |             case FACTION_WARLOCK:
// |                 armyHero->m_army.Add(CREATURE_CENTAUR, WARLOCK_CENTAUR_COUNT, -1);
// |                 armyHero->m_army.Add(CREATURE_GARGOYLE, WARLOCK_GARGOYLE_COUNT, -1);
// |                 armyHero->m_army.Add(CREATURE_GRIFFIN, WARLOCK_GRIFFIN_COUNT, -1);
// |                 break;
// |             case FACTION_BARBARIAN:
// |                 armyHero->m_army
// |                     .Add(CREATURE_ORC_CHIEF, BARBARIAN_ORC_CHIEF_COUNT, -1);
// |                 armyHero->m_army.Add(CREATURE_OGRE, BARBARIAN_OGRE_COUNT, -1);
// |                 armyHero->m_army.Add(CREATURE_GOBLIN, BARBARIAN_GOBLIN_COUNT, -1);
// |                 break;
// |             case FACTION_NECROMANCER:
// |                 armyHero->m_army
// |                     .Add(CREATURE_SKELETON, NECROMANCER_SKELETON_COUNT, -1);
// |                 armyHero->m_army
// |                     .Add(CREATURE_ROYAL_MUMMY, NECROMANCER_MUMMY_COUNT, -1);
// |                 armyHero->m_army
// |                     .Add(CREATURE_VAMPIRE_LORD, NECROMANCER_VAMPIRE_COUNT, -1);
// |                 break;
// |         }
// |         gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += CAMPAIGN_EXPERIENCE_BONUS;
// |         gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
// |         gbInNewGameSetup = savedNewGame;
// |     }
// |
// |     if ((m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)]
// |          && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)
// |         || m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)]) {
// |         hero* armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// |         for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// |              ++heroPositionValue) {
// |             armyHero->m_army.m_creatureTypes[heroPositionValue] =
// |                 m_campaignCarryoverCreatureTypes[heroPositionValue];
// |             armyHero->m_army.m_creatureCounts[heroPositionValue] =
// |                 m_campaignCarryoverCreatureCounts[heroPositionValue]
// |                 * (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
// |                        ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
// |                        : 1);
// |         }
// |     }
// |
// |     if (m_campaignType == CAMPAIGN_ARCHIBALD
// |         && m_campaignScenario + 1 == SCENARIO_SIX) {
// |         gpGame->m_mapHeader.victoryCondition = MAP_VICTORY_DEFEAT_SIDE;
// |         gpGame->m_mapHeader.victoryConditionValue = CAMPAIGN_SWITCH_VICTORY_VALUE;
// |         gpGame->m_mapHeader.allowNormalVictory = 1;
// |     }
// |     if (m_campaignType == CAMPAIGN_ROLAND
// |         && m_campaignScenario + 1 == SCENARIO_NINE) {
// |         gpGame->m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
// |         gpGame->m_mapHeader.lossConditionValue = 0;
// |     }
// |     if (m_campaignType == CAMPAIGN_ROLAND
// |         && m_campaignScenario + 1 == SCENARIO_SEVEN)
// |         gpGame->m_mapHeader.lossConditionValue = CAMPAIGN_ROLAND_TIME_LIMIT;
// | }
// ALTERNATIVE army_slot (unified source delta from BEFORE):
// | --- before
// | +++ army_slot
// | @@ -4,6 +4,7 @@
// |      i32 swappedHero;
// |      i32 heroPositionValue;
// |      i32 scanPositionId;
// | +    i32 armySlot;
// |      b32 savedNewGameSetup;
// |      i32 playerSlotSlot4;
// |      SCampaignChoice* choiceBest1;
// | @@ -178,10 +179,10 @@
// |
// |      if (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]) {
// |          hero* armyHero = gpGame->GetHero(m_players[CAMPAIGN_CARRYOVER_PLAYER].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            if (armyHero->m_army.m_creatureCounts[heroPositionValue] >= 1)
// | -                armyHero->m_army.m_creatureCounts[heroPositionValue] *=
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            if (armyHero->m_army.m_creatureCounts[armySlot] >= 1)
// | +                armyHero->m_army.m_creatureCounts[armySlot] *=
// |                      CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
// |          }
// |      }
// | @@ -192,10 +193,10 @@
// |          hero* armyHero;
// |          gbInNewGameSetup = true;
// |          armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            armyHero->m_army.m_creatureTypes[heroPositionValue] = CREATURE_NONE;
// | -            armyHero->m_army.m_creatureCounts[heroPositionValue] = 0;
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            armyHero->m_army.m_creatureTypes[armySlot] = CREATURE_NONE;
// | +            armyHero->m_army.m_creatureCounts[armySlot] = 0;
// |          }
// |          switch (armyHero->m_cursorType) {
// |              case FACTION_WARLOCK:
// | @@ -227,12 +228,12 @@
// |           && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)
// |          || m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)]) {
// |          hero* armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            armyHero->m_army.m_creatureTypes[heroPositionValue] =
// | -                m_campaignCarryoverCreatureTypes[heroPositionValue];
// | -            armyHero->m_army.m_creatureCounts[heroPositionValue] =
// | -                m_campaignCarryoverCreatureCounts[heroPositionValue]
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            armyHero->m_army.m_creatureTypes[armySlot] =
// | +                m_campaignCarryoverCreatureTypes[armySlot];
// | +            armyHero->m_army.m_creatureCounts[armySlot] =
// | +                m_campaignCarryoverCreatureCounts[armySlot]
// |                  * (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
// |                         ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
// |                         : 1);
// ALTERNATIVE hero_record (unified source delta from BEFORE):
// | --- before
// | +++ hero_record
// | @@ -3,6 +3,7 @@
// |      i32 bestHeroPriorityLocal;
// |      i32 swappedHero;
// |      i32 heroPositionValue;
// | +    i32 heroRecordIndex;
// |      i32 scanPositionId;
// |      b32 savedNewGameSetup;
// |      i32 playerSlotSlot4;
// | @@ -170,9 +171,9 @@
// |      gbRetreatWin = true;
// |
// |      if (m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)]) {
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_HERO_COUNT; ++heroPositionValue) {
// | -            if (gpGame->m_heroRecs[heroPositionValue].m_portrait == CAMPAIGN_HERO_CORLAGON)
// | -                gpGame->m_heroRecs[heroPositionValue].Deallocate(0);
// | +        for (heroRecordIndex = 0; heroRecordIndex < CAMPAIGN_HERO_COUNT; ++heroRecordIndex) {
// | +            if (gpGame->m_heroRecs[heroRecordIndex].m_portrait == CAMPAIGN_HERO_CORLAGON)
// | +                gpGame->m_heroRecs[heroRecordIndex].Deallocate(0);
// |          }
// |      }
// |
// ALTERNATIVE hero_record_army_slot (unified source delta from BEFORE):
// | --- before
// | +++ hero_record_army_slot
// | @@ -3,7 +3,9 @@
// |      i32 bestHeroPriorityLocal;
// |      i32 swappedHero;
// |      i32 heroPositionValue;
// | +    i32 heroRecordIndex;
// |      i32 scanPositionId;
// | +    i32 armySlot;
// |      b32 savedNewGameSetup;
// |      i32 playerSlotSlot4;
// |      SCampaignChoice* choiceBest1;
// | @@ -170,18 +172,18 @@
// |      gbRetreatWin = true;
// |
// |      if (m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)]) {
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_HERO_COUNT; ++heroPositionValue) {
// | -            if (gpGame->m_heroRecs[heroPositionValue].m_portrait == CAMPAIGN_HERO_CORLAGON)
// | -                gpGame->m_heroRecs[heroPositionValue].Deallocate(0);
// | +        for (heroRecordIndex = 0; heroRecordIndex < CAMPAIGN_HERO_COUNT; ++heroRecordIndex) {
// | +            if (gpGame->m_heroRecs[heroRecordIndex].m_portrait == CAMPAIGN_HERO_CORLAGON)
// | +                gpGame->m_heroRecs[heroRecordIndex].Deallocate(0);
// |          }
// |      }
// |
// |      if (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]) {
// |          hero* armyHero = gpGame->GetHero(m_players[CAMPAIGN_CARRYOVER_PLAYER].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            if (armyHero->m_army.m_creatureCounts[heroPositionValue] >= 1)
// | -                armyHero->m_army.m_creatureCounts[heroPositionValue] *=
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            if (armyHero->m_army.m_creatureCounts[armySlot] >= 1)
// | +                armyHero->m_army.m_creatureCounts[armySlot] *=
// |                      CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
// |          }
// |      }
// | @@ -192,10 +194,10 @@
// |          hero* armyHero;
// |          gbInNewGameSetup = true;
// |          armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            armyHero->m_army.m_creatureTypes[heroPositionValue] = CREATURE_NONE;
// | -            armyHero->m_army.m_creatureCounts[heroPositionValue] = 0;
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            armyHero->m_army.m_creatureTypes[armySlot] = CREATURE_NONE;
// | +            armyHero->m_army.m_creatureCounts[armySlot] = 0;
// |          }
// |          switch (armyHero->m_cursorType) {
// |              case FACTION_WARLOCK:
// | @@ -227,12 +229,12 @@
// |           && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)
// |          || m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)]) {
// |          hero* armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
// | -        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
// | -             ++heroPositionValue) {
// | -            armyHero->m_army.m_creatureTypes[heroPositionValue] =
// | -                m_campaignCarryoverCreatureTypes[heroPositionValue];
// | -            armyHero->m_army.m_creatureCounts[heroPositionValue] =
// | -                m_campaignCarryoverCreatureCounts[heroPositionValue]
// | +        for (armySlot = 0; armySlot < CAMPAIGN_ARMY_SLOT_COUNT;
// | +             ++armySlot) {
// | +            armyHero->m_army.m_creatureTypes[armySlot] =
// | +                m_campaignCarryoverCreatureTypes[armySlot];
// | +            armyHero->m_army.m_creatureCounts[armySlot] =
// | +                m_campaignCarryoverCreatureCounts[armySlot]
// |                  * (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
// |                         ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
// |                         : 1);
