# Semantic naming checkpoint

This branch applies all 80 scoped naming corrections from the
[review](https://github.com/sushi-shi/homm2-decomp/pull/65), including their
header declarations and callers. Function names and executable behavior are
unchanged. The mixed-role `EvaluateMonsterEvent::attackerLoss` remains held.

The native Debug build and all 24 existing CTest checks pass. The renamed
source/header files preserve their string and character literal payloads.
XML/save keys and Lua API names retain their compatibility spellings; mod
consumers use the new C++ member names. This is compile/contract evidence,
not exhaustive gameplay testing.

The proposal below is an unmerged PR against `ironfist`. The README branch
chain is propagated through separate review branches, not direct integration
commits.

| # | Owner | Previous name | Current name |
| --- | --- | --- | --- |
| 1 | `town` | `m_garrison` | `m_dwellingAvailable` |
| 2 | `town` | `m_buildState` | `m_mageGuildLevel` |
| 3 | `TownConstant` | `TOWN_GARRISON_SLOT_COUNT` | `TOWN_DWELLING_STOCK_SLOT_COUNT` |
| 4 | `philAI::CreaturesToBuy` | `nGarrison` | `availableCount` |
| 5 | `army` | `m_roundCounter` | `m_mirrorImageRoundsRemaining` |
| 6 | `army` | `m_animationCycle` | `m_shootingAnimationActive` |
| 7 | `combatManager` | `m_heroDeathPending` | `m_heroLossReactionPending` |
| 8 | `combatManager` | `m_heroAlternateDeathPending` | `m_heroOpponentLossReactionPending` |
| 9 | `combatManager` | `m_heroDeathAnimationPlayed` | `m_heroLossReactionPlayed` |
| 10 | `combatManager` | `m_heroAlternateDeathAnimationPlayed` | `m_heroOpponentLossReactionPlayed` |
| 11 | `CombatCycleConstant` | `HERO_ANIMATION_DEATH_FIRST` | `HERO_ANIMATION_LOSS_REACTION` |
| 12 | `CombatCycleConstant` | `HERO_ANIMATION_DEATH_SECOND` | `HERO_ANIMATION_OPPONENT_LOSS_REACTION` |
| 13 | `playerData` | `m_aiDifficulty` | `m_aiPersonality` |
| 14 | `playerData` | `m_cheatValue` | `m_bonusPuzzlePieces` |
| 15 | `playerAIData` | `m_upgradeValueWeight` | `m_fightValueResourceWeight` |
| 16 | `SamplePlaybackData` | `loopCount` | `looping` |
| 17 | `game` | `m_viewArmyResult` | `m_dialogAnimationCounter` |
| 18 | `hexcell` | `m_pathReachable` | `m_movementOrAttackReachable` |
| 19 | `armyGroup::IsHomogeneous` | `countRaces` | `alignmentMode` |
| 20 | `armyGroup::IsHomogeneous` | `numCreatureTypes` | `creatureTypeRuns` |
| 21 | `MapMonsterMetadata` | `MAP_MONSTER_GUARD_FLAG` | `MAP_MONSTER_FORCE_JOIN` |
| 22 | `monster generation` | `MONSTER_GUARD_ROLL_MIN` | `MONSTER_FORCE_JOIN_ROLL_MIN` |
| 23 | `monster generation` | `MONSTER_GUARD_ROLL_MAX` | `MONSTER_FORCE_JOIN_ROLL_MAX` |
| 24 | `monster generation` | `MONSTER_GUARD_CUTOFF` | `MONSTER_FORCE_JOIN_CUTOFF` |
| 25 | `CalcTerrainCost` | `useRoad` | `sourceHasRoad` |
| 26 | `CalcTerrainCost` | `usePathfinding` | `destinationHasRoad` |
| 27 | `SEARCH scratch storage` | `s_currentWater` | `s_currentHasRoad` |
| 28 | `SEARCH scratch storage` | `s_targetWater` | `s_targetHasRoad` |
| 29 | `philAI::EvaluateMonsterEvent` | `purchaseCost` | `purchaseCount` |
| 30 | `philAI::EvaluateMonsterEvent` | `unusedPurchaseValue` | `replacementSlot` |
| 31 | `philAI::EvaluateMonsterEvent` | `willJoin` | `forceJoin` |
| 32 | `philAI::FightValueOfStack` | `useHero` | `useAdjustedFightValue` |
| 33 | `philAI::FightValueOfStack` | `useEnemyMods` | `applySiegeAttackerModifiers` |
| 34 | `armyGroup::DamageGroup` | `damagePercent` | `casualtyFraction` |
| 35 | `philAI::DamageGroup` | `damage` | `casualtyFraction` |
| 36 | `advManager::PlayerMonsterInteract` | `handled` | `removeMonsterObject` |
| 37 | `advManager::ComputerMonsterInteract` | `handled` | `removeMonsterObject` |
| 38 | `advManager::CheckAdjacentMon` | `killed` | `removeMonster` |
| 39 | `philAI::ProbableOutcomeOfBattle` | `attackerLoss` | `attackerLossValue` |
| 40 | `philAI::ProbableOutcomeOfBattle` | `defenderLoss` | `defenderLossValue` |
| 41 | `philAI::ProbableOutcomeOfBattle` | `attackerRemaining` | `expectedAttackerLossValue` |
| 42 | `philAI::ProbableOutcomeOfBattle` | `defenderRemaining` | `expectedDefenderLossValue` |
| 43 | `philAI::QuickCombat` | `attackerDead` | `attackerLossValue` |
| 44 | `philAI::QuickCombat` | `defenderDead` | `defenderLossValue` |
| 45 | `philAI::QuickCombat` | `attackerRemaining` | `expectedAttackerLossValue` |
| 46 | `philAI::QuickCombat` | `remainB` | `expectedDefenderLossValue` |
| 47 | `philAI::ChooseEvaluateBattle` | `attackerLoss` | `attackerLossValue` |
| 48 | `philAI::ChooseEvaluateBattle` | `defenderLoss` | `defenderLossValue` |
| 49 | `philAI::ChooseEvaluateBattle` | `attackerRemaining` | `expectedAttackerLossValue` |
| 50 | `philAI::ChooseEvaluateBattle` | `defenderRemaining` | `expectedDefenderLossValue` |
| 51 | `philAI::EvaluateMineEvent` | `attackerLoss` | `attackerLossValue` |
| 52 | `philAI::EvaluateMineEvent` | `defenderLoss` | `defenderLossValue` |
| 53 | `philAI::EvaluateMineEvent` | `attackerRemaining` | `expectedAttackerLossValue` |
| 54 | `philAI::EvaluateMineEvent` | `defenderRemaining` | `expectedDefenderLossValue` |
| 55 | `philAI::EvaluateMonsterEvent` | `defenderLoss` | `defenderLossValue` |
| 56 | `philAI::EvaluateMonsterEvent` | `attackerRemaining` | `expectedAttackerLossValue` |
| 57 | `philAI::EvaluateMonsterEvent` | `defenderRemaining` | `expectedDefenderLossValue` |
| 58 | `philAI::EvaluateHeroEvent` | `attackerLost` | `attackerLossValue` |
| 59 | `philAI::EvaluateHeroEvent` | `defenderLost` | `defenderLossValue` |
| 60 | `philAI::EvaluateHeroEvent` | `aliveA` | `expectedAttackerLossValue` |
| 61 | `philAI::EvaluateHeroEvent` | `aliveB` | `expectedDefenderLossValue` |
| 62 | `philAI::ChooseToFightForArtifact` | `lostA` | `attackerLossValue` |
| 63 | `philAI::ChooseToFightForArtifact` | `lostB` | `defenderLossValue` |
| 64 | `philAI::ChooseToFightForArtifact` | `remainA` | `expectedAttackerLossValue` |
| 65 | `philAI::ChooseToFightForArtifact` | `remainB` | `expectedDefenderLossValue` |
| 66 | `philAI::EvaluateArtifactEvent` | `lostA` | `attackerLossValue` |
| 67 | `philAI::EvaluateArtifactEvent` | `lostB` | `defenderLossValue` |
| 68 | `philAI::EvaluateArtifactEvent` | `remainA` | `expectedAttackerLossValue` |
| 69 | `philAI::EvaluateArtifactEvent` | `remainB` | `expectedDefenderLossValue` |
| 70 | `philAI::EvaluateTownEvent` | `lostA` | `attackerLossValue` |
| 71 | `philAI::EvaluateTownEvent` | `lostB` | `defenderLossValue` |
| 72 | `philAI::EvaluateTownEvent` | `remainA` | `expectedAttackerLossValue` |
| 73 | `philAI::EvaluateTownEvent` | `remainB` | `expectedDefenderLossValue` |
| 74 | `philAI::QuickCombat` | `attackerDamage` | `attackerCasualtyFraction` |
| 75 | `philAI::QuickCombat` | `defenderDamage` | `defenderCasualtyFraction` |
| 76 | `philAI::FightEvent` | `attackerLoss` | `attackerCasualtyFraction` |
| 77 | `philAI::FightEvent` | `defenderLoss` | `defenderCasualtyFraction` |
| 78 | `philAI::TownEvent` | `attackerLoss` | `attackerCasualtyFraction` |
| 79 | `philAI::TownEvent` | `defenderLoss` | `defenderCasualtyFraction` |
| 80 | `philAI::CombatMonsterEvent` | `defenderLoss` | `defenderCasualtyFraction` |
