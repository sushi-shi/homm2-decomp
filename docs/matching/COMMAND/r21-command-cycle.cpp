// VC6 SP5 COMMAND source family r21-command-cycle; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r21-command-cycle/results.json; RVA 0x315fa
// Complete 2 arms; elapsed 2.064022s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'semantic_counter_scope': 'keep', 'candidate': 'baseline'} / 2352 / 76e1a8f3b42919dd / f11b94a8bd923c04 / 108:108 / 108,0,0,0 / True
// 1 / {'semantic_counter_scope': 'separate_counter', 'candidate': 'baseline'} / 2352 / 4a81bb45fa5b13e0 / f11b94a8bd923c04 / 108:108 / 108,0,0,0 / True

// AXIS semantic_counter_scope
// BEFORE (complete exact span):
// | void combatManager::CycleCombatScreen(void) {
// |     i32 nextHeroAnimation_0[COMBAT_SIDE_COUNT];
// |     army* currentArmy_2;
// |     i32 cyclingCount_1;
// |     i32 index_0;
// |     i32 animationIndex_27;
// |     u8 cycleArmy[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |     CombatSide side_7;
// |     float accumulatedChance;
// |     float roll_3;
// |
// |     CheckUpdateCombatMessages();
// |     currentArmy_2 = NULL;
// |     cyclingCount_1 = 0;
// |     gpCombatManager->ResetLimitCreature();
// |     for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// |         if (m_heroOverlayIcons[index_0] == NULL)
// |             continue;
// |
// |         if (m_heroes[index_0] != NULL)
// |             m_heroOverlayFrame[index_0] =
// |                 (m_heroOverlayFrame[index_0] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
// |         ++m_drawHeroOverlay[index_0];
// |     }
// |
// |     memset(cycleArmy, 0, sizeof(cycleArmy));
// |     for (side_7 = COMBAT_ATTACKER_SIDE; IDX(side_7) < COMBAT_SIDE_COUNT; ++side_7) {
// |         for (index_0 = 0; index_0 < gpCombatManager->m_armyCount[IDX(side_7)]; ++index_0) {
// |             currentArmy_2 = gpCombatManager->m_armies[IDX(side_7)] + index_0;
// |             if (HAS(currentArmy_2->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                     == 0
// |                 && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
// |                 && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
// |                 && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
// |                 && ((currentArmy_2->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
// |                      && currentArmy_2->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST)
// |                     || (currentArmy_2->m_animationSequence == ARMY_ANIMATION_STAND
// |                         && currentArmy_2->m_lastAnimationTime
// |                                    + currentArmy_2->m_frameInfo.standStillDelay
// |                                < KBTickCount()))) {
// |                 ++cyclingCount_1;
// |                 ++cycleArmy[IDX(side_7)][index_0];
// |                 ++m_limitCreatureCount[IDX(side_7)][index_0];
// |             }
// |         }
// |     }
// |
// |     for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// |         nextHeroAnimation_0[index_0] = -1;
// |         if (m_heroIcons[index_0] == NULL)
// |             continue;
// |
// |         if (m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_FIRST
// |             || m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_SECOND
// |             || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_FIRST
// |             || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_SECOND
// |             || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_LAST) {
// |             m_drawHero[index_0] = true;
// |         } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// |                    && m_heroDeathAnimationPlayed[index_0] == 0
// |                    && m_heroDeathPending[index_0] != 0) {
// |             if (m_playerId[IDX(m_currentSide)] == -1
// |                 || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
// |                 m_heroAlternateDeathPending[index_0] = 0;
// |                 m_heroDeathPending[index_0] = 0;
// |             } else {
// |                 m_heroAlternateDeathPending[index_0] = 0;
// |                 m_heroDeathPending[index_0] = 0;
// |                 m_heroDeathAnimationPlayed[index_0] = 1;
// |                 if (sCmbtHero[m_heroSpriteIndex[index_0]]
// |                         .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
// |                     > 0) {
// |                     nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_FIRST;
// |                     m_drawHero[index_0] = true;
// |                 }
// |             }
// |         } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// |                    && m_heroAlternateDeathAnimationPlayed[index_0] == 0
// |                    && m_heroAlternateDeathPending[index_0] != 0) {
// |             if (m_playerId[IDX(m_currentSide)] == -1
// |                 || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
// |                 m_heroAlternateDeathPending[index_0] = 0;
// |                 m_heroDeathPending[index_0] = 0;
// |             } else {
// |                 m_heroAlternateDeathPending[index_0] = 0;
// |                 m_heroDeathPending[index_0] = 0;
// |                 m_heroAlternateDeathAnimationPlayed[index_0] = 1;
// |                 if (sCmbtHero[m_heroSpriteIndex[index_0]]
// |                         .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
// |                     > 0) {
// |                     nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_SECOND;
// |                     m_drawHero[index_0] = true;
// |                 }
// |             }
// |         } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// |                    && m_heroCycleTimer[index_0] + HERO_IDLE_DELAY < KBTickCount()) {
// |             if (sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount > 1) {
// |                 nextHeroAnimation_0[index_0] =
// |                     Random(0, sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount - 1)
// |                     + HERO_ANIMATION_IDLE_FIRST;
// |             } else {
// |                 nextHeroAnimation_0[index_0] = HERO_ANIMATION_IDLE_FIRST;
// |             }
// |             m_drawHero[index_0] = true;
// |         }    }
// |     if (m_heroIcons[IDX(COMBAT_ATTACKER_SIDE)] != NULL)
// |         m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] = true;
// |     if (m_heroIcons[IDX(COMBAT_DEFENDER_SIDE)] != NULL)
// |         m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] = true;
// |
// |     if (cyclingCount_1 == 0 && m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] == 0
// |         && m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] == 0
// |         && m_drawHeroOverlay[IDX(COMBAT_ATTACKER_SIDE)] == 0
// |         && m_drawHeroOverlay[IDX(COMBAT_DEFENDER_SIDE)] == 0)
// |         goto setCycleTimer;
// |     gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
// |     for (side_7 = COMBAT_ATTACKER_SIDE; IDX(side_7) < COMBAT_SIDE_COUNT; ++side_7) {
// |         for (index_0 = 0; index_0 < gpCombatManager->m_armyCount[IDX(side_7)]; ++index_0) {
// |             currentArmy_2 = gpCombatManager->m_armies[IDX(side_7)] + index_0;
// |             if (cycleArmy[IDX(side_7)][index_0] != 0) {
// |                 if (currentArmy_2->m_animationSequence == ARMY_ANIMATION_STAND) {
// |                     roll_3 = static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
// |                            / COMBAT_IDLE_ROLL_DIVISOR;
// |                     accumulatedChance = 0.0f;
// |                     currentArmy_2->m_standingAnimation =
// |                         currentArmy_2->m_frameInfo.standingAnimationCount - 1;
// |                     for (animationIndex_27 = 0;
// |                          animationIndex_27 < currentArmy_2->m_frameInfo.standingAnimationCount - 1;
// |                          ++animationIndex_27) {
// |                         accumulatedChance +=
// |                             currentArmy_2->m_frameInfo.standingAnimationChances[animationIndex_27];
// |                         if (roll_3 < accumulatedChance) {
// |                             currentArmy_2->m_standingAnimation = animationIndex_27;
// |                             animationIndex_27 = STANDING_ANIMATION_SEARCH_DONE;
// |                         }
// |                     }
// |                     currentArmy_2->m_animationSequence = ArmyAnimationSequence(
// |                         currentArmy_2->m_standingAnimation
// |                         + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)
// |                     );
// |                     currentArmy_2->m_animationFrame = 0;
// |                 } else {
// |                     ++currentArmy_2->m_animationFrame;
// |                     if (currentArmy_2->m_frameInfo.standStillDelay == 0
// |                         && currentArmy_2->m_frameInfo.standingAnimationCount == 1
// |                         && Random(0, IDLE_ROLL_MAX) < IDLE_REPEAT_CHANCE) {
// |                         --currentArmy_2->m_animationFrame;
// |                     }
// |                     if (currentArmy_2->m_animationFrame
// |                         >= currentArmy_2->m_frameInfo.animationFrameCount
// |                                [currentArmy_2->m_standingAnimation
// |                                 + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)]) {
// |                         currentArmy_2->m_animationSequence = ARMY_ANIMATION_STAND;
// |                         currentArmy_2->m_animationFrame = 0;
// |                         currentArmy_2->m_lastAnimationTime = KBTickCount();
// |                         if (currentArmy_2->m_frameInfo.standStillDelay > 0) {
// |                             currentArmy_2->m_lastAnimationTime = static_cast<i32>(
// |                                 currentArmy_2->m_lastAnimationTime
// |                                 + (Random(0, currentArmy_2->m_frameInfo.standStillDelay)
// |                                        * COMBAT_STAND_DELAY_RANDOM_FACTOR
// |                                    - currentArmy_2->m_frameInfo.standStillDelay
// |                                          * COMBAT_STAND_DELAY_BASE_FACTOR)
// |                             );
// |                         }
// |                     }
// |                 }
// |             }
// |         }
// |     }
// |     for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// |         if (m_drawHero[index_0] != 0) {
// |             if (nextHeroAnimation_0[index_0] != -1) {
// |                 m_heroAnimationState[index_0] = nextHeroAnimation_0[index_0];
// |                 m_heroAnimationFrame[index_0] = 0;
// |             } else {
// |                 ++m_heroAnimationFrame[index_0];
// |                 if (m_heroAnimationFrame[index_0]
// |                     >= sCmbtHero[m_heroSpriteIndex[index_0]]
// |                            .animationFrameCount[m_heroAnimationState[index_0]]) {
// |                     m_heroAnimationState[index_0] = HERO_ANIMATION_STAND;
// |                     m_heroAnimationFrame[index_0] = 0;
// |                     m_heroCycleTimer[index_0] = KBTickCount();
// |                 }
// |             }
// |         }
// |     }
// |     DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
// | setCycleTimer:
// |     glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = static_cast<i32>(
// |         KBTickCount() + COMBAT_CYCLE_TIMER_FACTOR * gfCombatSpeedMod[gConfig.combatSpeed]
// |     );
// | }
// ALTERNATIVE separate_counter (unified source delta from BEFORE):
// | --- before
// | +++ separate_counter
// | @@ -3,6 +3,7 @@
// |      army* currentArmy_2;
// |      i32 cyclingCount_1;
// |      i32 index_0;
// | +    i32 heroSideIndex;
// |      i32 animationIndex_27;
// |      u8 cycleArmy[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
// |      CombatSide side_7;
// | @@ -13,14 +14,14 @@
// |      currentArmy_2 = NULL;
// |      cyclingCount_1 = 0;
// |      gpCombatManager->ResetLimitCreature();
// | -    for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// | -        if (m_heroOverlayIcons[index_0] == NULL)
// | +    for (heroSideIndex = IDX(COMBAT_ATTACKER_SIDE); heroSideIndex < COMBAT_SIDE_COUNT; ++heroSideIndex) {
// | +        if (m_heroOverlayIcons[heroSideIndex] == NULL)
// |              continue;
// |
// | -        if (m_heroes[index_0] != NULL)
// | -            m_heroOverlayFrame[index_0] =
// | -                (m_heroOverlayFrame[index_0] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
// | -        ++m_drawHeroOverlay[index_0];
// | +        if (m_heroes[heroSideIndex] != NULL)
// | +            m_heroOverlayFrame[heroSideIndex] =
// | +                (m_heroOverlayFrame[heroSideIndex] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
// | +        ++m_drawHeroOverlay[heroSideIndex];
// |      }
// |
// |      memset(cycleArmy, 0, sizeof(cycleArmy));
// | @@ -45,63 +46,63 @@
// |          }
// |      }
// |
// | -    for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// | -        nextHeroAnimation_0[index_0] = -1;
// | -        if (m_heroIcons[index_0] == NULL)
// | +    for (heroSideIndex = IDX(COMBAT_ATTACKER_SIDE); heroSideIndex < COMBAT_SIDE_COUNT; ++heroSideIndex) {
// | +        nextHeroAnimation_0[heroSideIndex] = -1;
// | +        if (m_heroIcons[heroSideIndex] == NULL)
// |              continue;
// |
// | -        if (m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_FIRST
// | -            || m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_SECOND
// | -            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_FIRST
// | -            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_SECOND
// | -            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_LAST) {
// | -            m_drawHero[index_0] = true;
// | -        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// | -                   && m_heroDeathAnimationPlayed[index_0] == 0
// | -                   && m_heroDeathPending[index_0] != 0) {
// | +        if (m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_DEATH_FIRST
// | +            || m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_DEATH_SECOND
// | +            || m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_IDLE_FIRST
// | +            || m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_IDLE_SECOND
// | +            || m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_IDLE_LAST) {
// | +            m_drawHero[heroSideIndex] = true;
// | +        } else if (m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_STAND
// | +                   && m_heroDeathAnimationPlayed[heroSideIndex] == 0
// | +                   && m_heroDeathPending[heroSideIndex] != 0) {
// |              if (m_playerId[IDX(m_currentSide)] == -1
// |                  || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
// | -                m_heroAlternateDeathPending[index_0] = 0;
// | -                m_heroDeathPending[index_0] = 0;
// | +                m_heroAlternateDeathPending[heroSideIndex] = 0;
// | +                m_heroDeathPending[heroSideIndex] = 0;
// |              } else {
// | -                m_heroAlternateDeathPending[index_0] = 0;
// | -                m_heroDeathPending[index_0] = 0;
// | -                m_heroDeathAnimationPlayed[index_0] = 1;
// | -                if (sCmbtHero[m_heroSpriteIndex[index_0]]
// | +                m_heroAlternateDeathPending[heroSideIndex] = 0;
// | +                m_heroDeathPending[heroSideIndex] = 0;
// | +                m_heroDeathAnimationPlayed[heroSideIndex] = 1;
// | +                if (sCmbtHero[m_heroSpriteIndex[heroSideIndex]]
// |                          .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
// |                      > 0) {
// | -                    nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_FIRST;
// | -                    m_drawHero[index_0] = true;
// | +                    nextHeroAnimation_0[heroSideIndex] = HERO_ANIMATION_DEATH_FIRST;
// | +                    m_drawHero[heroSideIndex] = true;
// |                  }
// |              }
// | -        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// | -                   && m_heroAlternateDeathAnimationPlayed[index_0] == 0
// | -                   && m_heroAlternateDeathPending[index_0] != 0) {
// | +        } else if (m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_STAND
// | +                   && m_heroAlternateDeathAnimationPlayed[heroSideIndex] == 0
// | +                   && m_heroAlternateDeathPending[heroSideIndex] != 0) {
// |              if (m_playerId[IDX(m_currentSide)] == -1
// |                  || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
// | -                m_heroAlternateDeathPending[index_0] = 0;
// | -                m_heroDeathPending[index_0] = 0;
// | +                m_heroAlternateDeathPending[heroSideIndex] = 0;
// | +                m_heroDeathPending[heroSideIndex] = 0;
// |              } else {
// | -                m_heroAlternateDeathPending[index_0] = 0;
// | -                m_heroDeathPending[index_0] = 0;
// | -                m_heroAlternateDeathAnimationPlayed[index_0] = 1;
// | -                if (sCmbtHero[m_heroSpriteIndex[index_0]]
// | +                m_heroAlternateDeathPending[heroSideIndex] = 0;
// | +                m_heroDeathPending[heroSideIndex] = 0;
// | +                m_heroAlternateDeathAnimationPlayed[heroSideIndex] = 1;
// | +                if (sCmbtHero[m_heroSpriteIndex[heroSideIndex]]
// |                          .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
// |                      > 0) {
// | -                    nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_SECOND;
// | -                    m_drawHero[index_0] = true;
// | +                    nextHeroAnimation_0[heroSideIndex] = HERO_ANIMATION_DEATH_SECOND;
// | +                    m_drawHero[heroSideIndex] = true;
// |                  }
// |              }
// | -        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
// | -                   && m_heroCycleTimer[index_0] + HERO_IDLE_DELAY < KBTickCount()) {
// | -            if (sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount > 1) {
// | -                nextHeroAnimation_0[index_0] =
// | -                    Random(0, sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount - 1)
// | +        } else if (m_heroAnimationState[heroSideIndex] == HERO_ANIMATION_STAND
// | +                   && m_heroCycleTimer[heroSideIndex] + HERO_IDLE_DELAY < KBTickCount()) {
// | +            if (sCmbtHero[m_heroSpriteIndex[heroSideIndex]].idleAnimationCount > 1) {
// | +                nextHeroAnimation_0[heroSideIndex] =
// | +                    Random(0, sCmbtHero[m_heroSpriteIndex[heroSideIndex]].idleAnimationCount - 1)
// |                      + HERO_ANIMATION_IDLE_FIRST;
// |              } else {
// | -                nextHeroAnimation_0[index_0] = HERO_ANIMATION_IDLE_FIRST;
// | +                nextHeroAnimation_0[heroSideIndex] = HERO_ANIMATION_IDLE_FIRST;
// |              }
// | -            m_drawHero[index_0] = true;
// | +            m_drawHero[heroSideIndex] = true;
// |          }    }
// |      if (m_heroIcons[IDX(COMBAT_ATTACKER_SIDE)] != NULL)
// |          m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] = true;
// | @@ -167,19 +168,19 @@
// |              }
// |          }
// |      }
// | -    for (index_0 = IDX(COMBAT_ATTACKER_SIDE); index_0 < COMBAT_SIDE_COUNT; ++index_0) {
// | -        if (m_drawHero[index_0] != 0) {
// | -            if (nextHeroAnimation_0[index_0] != -1) {
// | -                m_heroAnimationState[index_0] = nextHeroAnimation_0[index_0];
// | -                m_heroAnimationFrame[index_0] = 0;
// | +    for (heroSideIndex = IDX(COMBAT_ATTACKER_SIDE); heroSideIndex < COMBAT_SIDE_COUNT; ++heroSideIndex) {
// | +        if (m_drawHero[heroSideIndex] != 0) {
// | +            if (nextHeroAnimation_0[heroSideIndex] != -1) {
// | +                m_heroAnimationState[heroSideIndex] = nextHeroAnimation_0[heroSideIndex];
// | +                m_heroAnimationFrame[heroSideIndex] = 0;
// |              } else {
// | -                ++m_heroAnimationFrame[index_0];
// | -                if (m_heroAnimationFrame[index_0]
// | -                    >= sCmbtHero[m_heroSpriteIndex[index_0]]
// | -                           .animationFrameCount[m_heroAnimationState[index_0]]) {
// | -                    m_heroAnimationState[index_0] = HERO_ANIMATION_STAND;
// | -                    m_heroAnimationFrame[index_0] = 0;
// | -                    m_heroCycleTimer[index_0] = KBTickCount();
// | +                ++m_heroAnimationFrame[heroSideIndex];
// | +                if (m_heroAnimationFrame[heroSideIndex]
// | +                    >= sCmbtHero[m_heroSpriteIndex[heroSideIndex]]
// | +                           .animationFrameCount[m_heroAnimationState[heroSideIndex]]) {
// | +                    m_heroAnimationState[heroSideIndex] = HERO_ANIMATION_STAND;
// | +                    m_heroAnimationFrame[heroSideIndex] = 0;
// | +                    m_heroCycleTimer[heroSideIndex] = KBTickCount();
// |                  }
// |              }
// |          }
