// VC6 SP5 COMMAND source family s62-victory; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-victory/results.json; RVA 0x2f601
// Complete 2 arms; elapsed 2.181291s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 2839 / 4f4d2a3363b8ba73 / 6c4a6115d48fc8ae / 117:117 / 117,0,0,0 / True
// 1 / {'local_role_subset': 'cost', 'candidate': 'baseline'} / 2839 / 3a1ef345a6c51d4d / 6a21abc6ca683e4e / 117:117 / 117,0,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::DoVictory(H2_ENUM_PARAM(CombatResult, i32) winningSide) {
// |     army* pTroop;
// |     i32 numFades;
// |     i32 bodies;
// |     i32 levelsGained = 0;
// |     i32 livingCount;
// |     i32 lastLivingArmy;
// |     i32 troopIndex;
// |     i32 loop;
// |     i32 waitTimer;
// |     tag_message message;
// |     CombatSide combatSide;
// |     i32 H2_UNUSED(cost);
// |     i32 necroEligible;
// |     i32 emptySlots;
// |     char experienceText[VICTORY_EXPERIENCE_TEXT_SIZE];
// |
// |     if (m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL
// |         && m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_isCaptain != 0)
// |         m_heroes[IDX(COMBAT_DEFENDER_SIDE)] = NULL;
// |     gbShowingLoseWindow = false;
// |     gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_FIRST;
// |     giWinCmbtFrame = 0;
// |     giSkeletonsCreated = 0;
// |     iMaxTransferArtifacts = 0;
// |     iCurTransferArtifact = -1;
// |     bSkeletonsShown = false;
// |     bodies = 0;
// |     necroEligible = 0;
// |
// |     for (combatSide = COMBAT_ATTACKER_SIDE; IDX(combatSide) < COMBAT_SIDE_COUNT; ++combatSide) {
// |         livingCount = 0;
// |         lastLivingArmy = -1;
// |         for (troopIndex = 0; troopIndex < gpCombatManager->m_armyCount[IDX(combatSide)];
// |              ++troopIndex) {
// |             pTroop = &m_armies[IDX(combatSide)][troopIndex];
// |             if (pTroop->m_quantity > 0) {
// |                 lastLivingArmy = troopIndex;
// |                 if (pTroop->m_temporaryResurrectionQuantity > 0)
// |                     pTroop->m_quantity -= pTroop->m_temporaryResurrectionQuantity;
// |                 if (pTroop->m_quantity < 0)
// |                     pTroop->m_quantity = 0;
// |                 livingCount += pTroop->m_quantity;
// |             }
// |             if (CombatResultForSide(combatSide) == winningSide && pTroop->m_quantity > 0
// |                 && HAS(pTroop->m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE) == 0
// |                 && pTroop->m_monsterType != CREATURE_EARTH_ELEMENTAL
// |                 && pTroop->m_monsterType != CREATURE_AIR_ELEMENTAL
// |                 && pTroop->m_monsterType != CREATURE_FIRE_ELEMENTAL
// |                 && pTroop->m_monsterType != CREATURE_WATER_ELEMENTAL
// |                 && pTroop->m_monsterType != CREATURE_SKELETON) {
// |                 ++necroEligible;
// |             }
// |             if (OppositeCombatResult(CombatResultForSide(combatSide)) == winningSide) {
// |                 bodies += pTroop->m_initialQuantity - pTroop->m_quantity;
// |             }
// |         }
// |         if (livingCount == 0 && lastLivingArmy != -1)
// |             m_armies[IDX(combatSide)][lastLivingArmy].m_quantity = 1;
// |     }
// |
// |     if (winningSide != COMBAT_RESULT_DRAW && necroEligible < VICTORY_NECROMANCY_STACK_LIMIT
// |         && m_heroes[IDX(winningSide)] != NULL
// |         && m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
// |         giSkeletonsCreated = static_cast<i32>(
// |             bodies
// |             * (m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY)
// |                * COMBAT_NECROMANCY_LEVEL_FACTOR)
// |         );
// |         if (giSkeletonsCreated <= 0 && bodies != 0)
// |             giSkeletonsCreated = 1;
// |     }
// |
// |     m_nonVisualCombat = true;
// |     FreeArmies();
// |     CombatMessage("", 1, 1, 0);
// |     gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
// |     numFades = VICTORY_FADE_STEPS;
// |     if (m_terrainType == TERRAIN_WASTELAND)
// |         numFades = VICTORY_WASTELAND_FADE_STEPS;
// |     waitTimer = KBTickCount();
// |     for (loop = 0; loop < numFades; ++loop) {
// |         PollSound();
// |         DelayTil(&waitTimer);
// |         waitTimer = KBTickCount() + VICTORY_FADE_DELAY;
// |         DimBitmapArea(
// |             gpWindowManager->m_screen,
// |             0,
// |             0,
// |             COMBAT_SCREEN_WIDTH,
// |             COMBAT_SCREEN_HEIGHT,
// |             VICTORY_DIM_STEP
// |         );
// |         PollSound();
// |         gpWindowManager->UpdateScreenRegion(0, 0, SCREEN_MAX_X, SCREEN_MAX_Y);
// |         PollSound();
// |     }
// |
// |     switch (winningSide) {
// |         case COMBAT_RESULT_DRAW:
// |             gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
// |             DoLoseWindow();
// |             break;
// |         case COMBAT_RESULT_ATTACKER:
// |         case COMBAT_RESULT_DEFENDER:
// |             if (m_heroes[IDX(winningSide)] != NULL) {
// |                 if (m_eagleEyeSpell[IDX(winningSide)] != SPELL_NONE) {
// |                     m_heroes[IDX(winningSide)]->m_spells[IDX(m_eagleEyeSpell[IDX(winningSide)])] =
// |                         1;
// |                 }
// |                 m_experienceValue[IDX(OppositeCombatResult(winningSide))] =
// |                     ExperienceValueOfStack(
// |                         CombatSideForResult(OppositeCombatResult(winningSide))
// |                     );
// |                 if (gbRetreatWin != 0)
// |                     m_experienceValue[IDX(OppositeCombatResult(winningSide))] -=
// |                         COMBAT_HERO_EXPERIENCE_VALUE;
// |                 if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL
// |                     && winningSide == COMBAT_RESULT_ATTACKER)
// |                     m_experienceValue[IDX(OppositeCombatResult(winningSide))] +=
// |                         COMBAT_HERO_EXPERIENCE_VALUE;
// |                 levelsGained = gpAdvManager->GiveExperience(
// |                     m_heroes[IDX(winningSide)],
// |                     m_experienceValue[IDX(OppositeCombatResult(winningSide))],
// |                     gbThisNetHumanPlayer[IDX(m_heroes[IDX(winningSide)]->m_owner)] == 0
// |                 );
// |
// |                 if (gbRetreatWin == 0) {
// |                     emptySlots = 0;
// |                     if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
// |                         && m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL) {
// |                         for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
// |                             if (m_heroes[IDX(winningSide)]->m_artifacts[loop]
// |                                 == ARTIFACT_NONE) {
// |                                 ++emptySlots;
// |                             }
// |                         }
// |                         for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
// |                             if (m_heroes[IDX(OppositeCombatResult(winningSide))]
// |                                         ->m_artifacts[loop]
// |                                     >= ARTIFACT_ARCANE_NECKLACE
// |                                 && m_heroes[IDX(OppositeCombatResult(winningSide))]
// |                                            ->m_artifacts[loop]
// |                                        != ARTIFACT_MAGIC_BOOK
// |                                 && iMaxTransferArtifacts < emptySlots) {
// |                                 iTransferArtifacts[iMaxTransferArtifacts] =
// |                                     m_heroes[IDX(OppositeCombatResult(winningSide))]
// |                                         ->m_artifacts[loop];
// |                                 iTransferArtifactsInfo[iMaxTransferArtifacts] =
// |                                     m_heroes[IDX(OppositeCombatResult(winningSide))]
// |                                         ->m_artifactExtra[loop];
// |                                 ++iMaxTransferArtifacts;
// |                             }
// |                         }
// |                     }
// |                 }
// |             }
// |
// |             if (!(giCurPlayer == -1 || gbThisNetHumanPlayer[giCurPlayer] == 0
// |                   || m_playerId[IDX(winningSide)] != giCurPlayer)
// |                 || !(
// |                     giCurPlayer == -1 || m_playerId[IDX(winningSide)] == -1
// |                     || gbThisNetHumanPlayer[giCurPlayer] != 0
// |                     || gbThisNetHumanPlayer[m_playerId[IDX(winningSide)]] == 0
// |                 )
// |                 || !(
// |                     m_playerId[IDX(winningSide)] == -1
// |                     || gbThisNetHumanPlayer[m_playerId[IDX(winningSide)]] == 0
// |                 )) {
// |                 gpSoundManager->SwitchAmbientMusic(VICTORY_MUSIC);
// |                 m_winLoseWindow =
// |                     new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
// |                 if (m_winLoseWindow == NULL)
// |                     MemError();
// |
// |                 if (m_heroes[IDX(winningSide)] != NULL) {
// |                     if (gbCombatSurrender != 0) {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_SURRENDERED)]);
// |                     } else if (gbRetreatWin != 0) {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_FLED)]);
// |                     } else {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_VICTORY)]);
// |                     }
// |                     if (levelsGained > 0 && winningSide == COMBAT_RESULT_DEFENDER
// |                         && giNumHumanPlayers > 1) {
// |                         sprintf(
// |                             experienceText,
// |                             cBattleResults[IDX(RESULT_TEXT_NETWORK_EXPERIENCE)],
// |                             m_heroes[IDX(winningSide)]->m_name,
// |                             m_experienceValue[IDX(OppositeCombatResult(winningSide))],
// |                             levelsGained
// |                         );
// |                     } else {
// |                         sprintf(
// |                             experienceText,
// |                             cBattleResults[IDX(RESULT_TEXT_EXPERIENCE)],
// |                             m_heroes[IDX(winningSide)]->m_name,
// |                             m_experienceValue[IDX(OppositeCombatResult(winningSide))]
// |                         );
// |                     }
// |                     strcat(gText, experienceText);
// |                     m_heroes[IDX(winningSide)]->ApplyBattleWinTemps();
// |                 } else {
// |                     if (gbCombatSurrender != 0) {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_SURRENDERED)]);
// |                     } else if (gbRetreatWin != 0) {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_FLED)]);
// |                     } else {
// |                         sprintf(gText, cBattleResults[IDX(RESULT_TEXT_VICTORY)]);
// |                     }
// |                 }
// |                 message.type = MESSAGE_WIDGET;
// |                 message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
// |                 message.payload.widget.id = WIN_LOSE_TEXT_ID;
// |                 message.payload.widget.data.text = gText;
// |                 m_winLoseWindow->BroadcastMessage(message);
// |                 ShowDeadArmies(m_winLoseWindow);
// |                 if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
// |                     giDialogTimeout = KBTickCount() + WIN_LOSE_DIALOG_TIMEOUT;
// |                 gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
// |                 giDialogTimeout = 0;
// |                 delete m_winLoseWindow;
// |                 if (m_heroes[IDX(OppositeCombatResult(winningSide))] != NULL)
// |                     m_heroes[IDX(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
// |             } else {
// |                 if (m_heroes[IDX(winningSide)] != NULL)
// |                     m_heroes[IDX(winningSide)]->ApplyBattleWinTemps();
// |                 if (m_heroes[IDX(OppositeCombatResult(winningSide))] != NULL)
// |                     m_heroes[IDX(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
// |                 gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
// |                 DoLoseWindow();
// |             }
// |             break;
// |     }
// |     gMapX = gpAdvManager->m_mapOriginX;
// |     gMapY = gpAdvManager->m_mapOriginY;
// | }
// ALTERNATIVE cost (unified source delta from BEFORE):
// | --- before
// | +++ cost
// | @@ -10,7 +10,6 @@
// |      i32 waitTimer;
// |      tag_message message;
// |      CombatSide combatSide;
// | -    i32 H2_UNUSED(cost);
// |      i32 necroEligible;
// |      i32 emptySlots;
// |      char experienceText[VICTORY_EXPERIENCE_TEXT_SIZE];
