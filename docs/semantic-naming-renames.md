# Proposed renames for review

No game identifier in this list has been changed. Each row is one scoped
proposal, including separately named caller locals; approval can be selective.
The function/class spelling in the Owner column is retained. Source/header
declarations and all references belonging to the same symbol must follow an
accepted rename. Matching-only alias names require their own frame review.
The links point to the current decomp source, not historical original names.

The [audit](semantic-naming-audit.md) supplies formulas, public-symbol evidence,
caller traps, cross-branch checks, and compatibility limits. Some proposals
correct a contradicted role; others make a narrower existing contract explicit.

This review contains **80 explicit proposals**.

| # | Kind | Owner | Old | Proposed new | Source anchor | Reason |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | member | `town` | `m_garrison` | `m_dwellingAvailable` | [town.h:90](../include/SOURCE/town.h#L90) | Dwelling stock / guild level, not defending army / general building progress. |
| 2 | member | `town` | `m_buildState` | `m_mageGuildLevel` | [town.h:72](../include/SOURCE/town.h#L72) | Dwelling stock / guild level, not defending army / general building progress. |
| 3 | constant | `TownConstant` | `TOWN_GARRISON_SLOT_COUNT` | `TOWN_DWELLING_STOCK_SLOT_COUNT` | [town.h:49](../include/SOURCE/town.h#L49) | Twelve serialized dwelling-stock entries. |
| 4 | local | `philAI::CreaturesToBuy` | `nGarrison` | `availableCount` | [PHILAI.cpp:3132](../src/SOURCE/PHILAI.cpp#L3132) | Reads the recruitment stock. |
| 5 | member | `army` | `m_roundCounter` | `m_mirrorImageRoundsRemaining` | [army.h:52](../include/SOURCE/army.h#L52) | Mirror lifetime / shooting continuation flag. |
| 6 | member | `army` | `m_animationCycle` | `m_shootingAnimationActive` | [army.h:32](../include/SOURCE/army.h#L32) | Mirror lifetime / shooting continuation flag. |
| 7 | member | `combatManager` | `m_heroDeathPending` | `m_heroLossReactionPending` | [combatManager.h:501](../include/SOURCE/combatManager.h#L501) | Reaction to troop loss or friendly fire; does not mean hero death. |
| 8 | member | `combatManager` | `m_heroAlternateDeathPending` | `m_heroOpponentLossReactionPending` | [combatManager.h:502](../include/SOURCE/combatManager.h#L502) | Reaction to troop loss or friendly fire; does not mean hero death. |
| 9 | member | `combatManager` | `m_heroDeathAnimationPlayed` | `m_heroLossReactionPlayed` | [combatManager.h:503](../include/SOURCE/combatManager.h#L503) | Reaction to troop loss or friendly fire; does not mean hero death. |
| 10 | member | `combatManager` | `m_heroAlternateDeathAnimationPlayed` | `m_heroOpponentLossReactionPlayed` | [combatManager.h:504](../include/SOURCE/combatManager.h#L504) | Reaction to troop loss or friendly fire; does not mean hero death. |
| 11 | constant | `CombatCycleConstant` | `HERO_ANIMATION_DEATH_FIRST` | `HERO_ANIMATION_LOSS_REACTION` | [COMMAND.cpp:248](../src/SOURCE/COMMAND.cpp#L248) | Sprite sequence selected for the corresponding reaction. |
| 12 | constant | `CombatCycleConstant` | `HERO_ANIMATION_DEATH_SECOND` | `HERO_ANIMATION_OPPONENT_LOSS_REACTION` | [COMMAND.cpp:249](../src/SOURCE/COMMAND.cpp#L249) | Sprite sequence selected for the corresponding reaction. |
| 13 | member | `playerData` | `m_aiDifficulty` | `m_aiPersonality` | [playerData.h:59](../include/SOURCE/playerData.h#L59) | Personality / bonus pieces; neither is restricted to difficulty / cheating. |
| 14 | member | `playerData` | `m_cheatValue` | `m_bonusPuzzlePieces` | [playerData.h:60](../include/SOURCE/playerData.h#L60) | Personality / bonus pieces; neither is restricted to difficulty / cheating. |
| 15 | member | `playerAIData` | `m_upgradeValueWeight` | `m_fightValueResourceWeight` | [playerData.h:44](../include/SOURCE/playerData.h#L44) | Weight converts fight value into resource value beyond upgrades. |
| 16 | member | `SamplePlaybackData` | `loopCount` | `looping` | [sampleData.h:28](../include/BASE/sampleData.h#L28) | Boolean repeated playback, not a finite repeat count. |
| 17 | member | `game` | `m_viewArmyResult` | `m_dialogAnimationCounter` | [game.h:148](../include/SOURCE/game.h#L148) | Tavern frame counter; initialized by army dialog too. |
| 18 | member | `hexcell` | `m_pathReachable` | `m_movementOrAttackReachable` | [hexcell.h:31](../include/SOURCE/hexcell.h#L31) | Includes ranged attack targets with no walking path. |
| 19 | argument | `armyGroup::IsHomogeneous` | `countRaces` | `alignmentMode` | [ARMYGRP.cpp:148](../src/SOURCE/ARMYGRP.cpp#L148) | Sentinel -1 enables tally / counter counts adjacent type runs. |
| 20 | local | `armyGroup::IsHomogeneous` | `numCreatureTypes` | `creatureTypeRuns` | [ARMYGRP.cpp:149](../src/SOURCE/ARMYGRP.cpp#L149) | Sentinel -1 enables tally / counter counts adjacent type runs. |
| 21 | constant | `MapMonsterMetadata` | `MAP_MONSTER_GUARD_FLAG` | `MAP_MONSTER_FORCE_JOIN` | [GAME.h:78](../include/SOURCE/GAME.h#L78) | 0x1000 is the free-join disposition bit. |
| 22 | constant | `monster generation` | `MONSTER_GUARD_ROLL_MIN` | `MONSTER_FORCE_JOIN_ROLL_MIN` | [GAME.cpp:460](../src/SOURCE/GAME.cpp#L460) | Random roll selects the free-join bit. |
| 23 | constant | `monster generation` | `MONSTER_GUARD_ROLL_MAX` | `MONSTER_FORCE_JOIN_ROLL_MAX` | [GAME.cpp:461](../src/SOURCE/GAME.cpp#L461) | Random roll selects the free-join bit. |
| 24 | constant | `monster generation` | `MONSTER_GUARD_CUTOFF` | `MONSTER_FORCE_JOIN_CUTOFF` | [GAME.cpp:462](../src/SOURCE/GAME.cpp#L462) | Random roll selects the free-join bit. |
| 25 | argument | `CalcTerrainCost` | `useRoad` | `sourceHasRoad` | [FINDPATH.cpp:95](../src/SOURCE/FINDPATH.cpp#L95) | Caller passes source / destination road flag; skill level is separate. |
| 26 | argument | `CalcTerrainCost` | `usePathfinding` | `destinationHasRoad` | [FINDPATH.cpp:96](../src/SOURCE/FINDPATH.cpp#L96) | Caller passes source / destination road flag; skill level is separate. |
| 27 | static | `SEARCH scratch storage` | `s_currentWater` | `s_currentHasRoad` | [SEARCH.cpp:16](../src/SOURCE/SEARCH.cpp#L16) | Assigned from m_isRoad, not terrain water. |
| 28 | static | `SEARCH scratch storage` | `s_targetWater` | `s_targetHasRoad` | [SEARCH.cpp:29](../src/SOURCE/SEARCH.cpp#L29) | Assigned from m_isRoad, not terrain water. |
| 29 | local | `philAI::EvaluateMonsterEvent` | `purchaseCost` | `purchaseCount` | [PHILAI.cpp:7128](../src/SOURCE/PHILAI.cpp#L7128) | Purchase-count output / replacement-slot output / free-join bit. |
| 30 | local | `philAI::EvaluateMonsterEvent` | `unusedPurchaseValue` | `replacementSlot` | [PHILAI.cpp:7124](../src/SOURCE/PHILAI.cpp#L7124) | Purchase-count output / replacement-slot output / free-join bit. |
| 31 | local | `philAI::EvaluateMonsterEvent` | `willJoin` | `forceJoin` | [PHILAI.cpp:7130](../src/SOURCE/PHILAI.cpp#L7130) | Purchase-count output / replacement-slot output / free-join bit. |
| 32 | argument | `philAI::FightValueOfStack` | `useHero` | `useAdjustedFightValue` | [PHILAI.cpp:3989](../src/SOURCE/PHILAI.cpp#L3989) | Includes nonlinear stack weighting / attacker siege adjustments. |
| 33 | argument | `philAI::FightValueOfStack` | `useEnemyMods` | `applySiegeAttackerModifiers` | [PHILAI.cpp:3992](../src/SOURCE/PHILAI.cpp#L3992) | Includes nonlinear stack weighting / attacker siege adjustments. |
| 34 | argument | `armyGroup::DamageGroup` | `damagePercent` | `casualtyFraction` | [ARMYGRP.cpp:257](../src/SOURCE/ARMYGRP.cpp#L257) | 0..1 fraction converted into kill-roll threshold. |
| 35 | argument | `philAI::DamageGroup` | `damage` | `casualtyFraction` | [PHILAI.cpp:5729](../src/SOURCE/PHILAI.cpp#L5729) | Forwards the same 0..1 fraction. |
| 36 | argument | `advManager::PlayerMonsterInteract` | `handled` | `removeMonsterObject` | [EVENTS.cpp:7216](../src/SOURCE/EVENTS.cpp#L7216) | Output makes callers erase the monster; joining/fleeing also set it. |
| 37 | argument | `advManager::ComputerMonsterInteract` | `handled` | `removeMonsterObject` | [EVENTS.cpp:7464](../src/SOURCE/EVENTS.cpp#L7464) | Output makes callers erase the monster; joining/fleeing also set it. |
| 38 | local | `advManager::CheckAdjacentMon` | `killed` | `removeMonster` | [CURSOR.cpp:909](../src/SOURCE/CURSOR.cpp#L909) | Passed as remove-object output and gates EraseObj. |
| 39 | argument | `philAI::ProbableOutcomeOfBattle` | `attackerLoss` | `attackerLossValue` | [PHILAI.cpp:2371](../src/SOURCE/PHILAI.cpp#L2371) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 40 | argument | `philAI::ProbableOutcomeOfBattle` | `defenderLoss` | `defenderLossValue` | [PHILAI.cpp:2372](../src/SOURCE/PHILAI.cpp#L2372) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 41 | argument | `philAI::ProbableOutcomeOfBattle` | `attackerRemaining` | `expectedAttackerLossValue` | [PHILAI.cpp:2373](../src/SOURCE/PHILAI.cpp#L2373) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 42 | argument | `philAI::ProbableOutcomeOfBattle` | `defenderRemaining` | `expectedDefenderLossValue` | [PHILAI.cpp:2374](../src/SOURCE/PHILAI.cpp#L2374) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 43 | local | `philAI::QuickCombat` | `attackerDead` | `attackerLossValue` | [PHILAI.cpp:4346](../src/SOURCE/PHILAI.cpp#L4346) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 44 | local | `philAI::QuickCombat` | `defenderDead` | `defenderLossValue` | [PHILAI.cpp:4352](../src/SOURCE/PHILAI.cpp#L4352) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 45 | local | `philAI::QuickCombat` | `attackerRemaining` | `expectedAttackerLossValue` | [PHILAI.cpp:4353](../src/SOURCE/PHILAI.cpp#L4353) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 46 | local | `philAI::QuickCombat` | `remainB` | `expectedDefenderLossValue` | [PHILAI.cpp:4355](../src/SOURCE/PHILAI.cpp#L4355) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 47 | local | `philAI::ChooseEvaluateBattle` | `attackerLoss` | `attackerLossValue` | [PHILAI.cpp:5094](../src/SOURCE/PHILAI.cpp#L5094) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 48 | local | `philAI::ChooseEvaluateBattle` | `defenderLoss` | `defenderLossValue` | [PHILAI.cpp:5092](../src/SOURCE/PHILAI.cpp#L5092) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 49 | local | `philAI::ChooseEvaluateBattle` | `attackerRemaining` | `expectedAttackerLossValue` | [PHILAI.cpp:5092](../src/SOURCE/PHILAI.cpp#L5092) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 50 | local | `philAI::ChooseEvaluateBattle` | `defenderRemaining` | `expectedDefenderLossValue` | [PHILAI.cpp:5092](../src/SOURCE/PHILAI.cpp#L5092) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 51 | local | `philAI::EvaluateMineEvent` | `attackerLoss` | `attackerLossValue` | [PHILAI.cpp:7025](../src/SOURCE/PHILAI.cpp#L7025) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 52 | local | `philAI::EvaluateMineEvent` | `defenderLoss` | `defenderLossValue` | [PHILAI.cpp:7028](../src/SOURCE/PHILAI.cpp#L7028) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 53 | local | `philAI::EvaluateMineEvent` | `attackerRemaining` | `expectedAttackerLossValue` | [PHILAI.cpp:7029](../src/SOURCE/PHILAI.cpp#L7029) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 54 | local | `philAI::EvaluateMineEvent` | `defenderRemaining` | `expectedDefenderLossValue` | [PHILAI.cpp:7030](../src/SOURCE/PHILAI.cpp#L7030) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 55 | local | `philAI::EvaluateMonsterEvent` | `defenderLoss` | `defenderLossValue` | [PHILAI.cpp:7125](../src/SOURCE/PHILAI.cpp#L7125) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 56 | local | `philAI::EvaluateMonsterEvent` | `attackerRemaining` | `expectedAttackerLossValue` | [PHILAI.cpp:7123](../src/SOURCE/PHILAI.cpp#L7123) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 57 | local | `philAI::EvaluateMonsterEvent` | `defenderRemaining` | `expectedDefenderLossValue` | [PHILAI.cpp:7122](../src/SOURCE/PHILAI.cpp#L7122) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 58 | local | `philAI::EvaluateHeroEvent` | `attackerLost` | `attackerLossValue` | [PHILAI.cpp:7234](../src/SOURCE/PHILAI.cpp#L7234) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 59 | local | `philAI::EvaluateHeroEvent` | `defenderLost` | `defenderLossValue` | [PHILAI.cpp:7235](../src/SOURCE/PHILAI.cpp#L7235) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 60 | local | `philAI::EvaluateHeroEvent` | `aliveA` | `expectedAttackerLossValue` | [PHILAI.cpp:7236](../src/SOURCE/PHILAI.cpp#L7236) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 61 | local | `philAI::EvaluateHeroEvent` | `aliveB` | `expectedDefenderLossValue` | [PHILAI.cpp:7237](../src/SOURCE/PHILAI.cpp#L7237) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 62 | local | `philAI::ChooseToFightForArtifact` | `lostA` | `attackerLossValue` | [PHILAI.cpp:5137](../src/SOURCE/PHILAI.cpp#L5137) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 63 | local | `philAI::ChooseToFightForArtifact` | `lostB` | `defenderLossValue` | [PHILAI.cpp:5136](../src/SOURCE/PHILAI.cpp#L5136) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 64 | local | `philAI::ChooseToFightForArtifact` | `remainA` | `expectedAttackerLossValue` | [PHILAI.cpp:5135](../src/SOURCE/PHILAI.cpp#L5135) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 65 | local | `philAI::ChooseToFightForArtifact` | `remainB` | `expectedDefenderLossValue` | [PHILAI.cpp:5134](../src/SOURCE/PHILAI.cpp#L5134) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 66 | local | `philAI::EvaluateArtifactEvent` | `lostA` | `attackerLossValue` | [PHILAI.cpp:6911](../src/SOURCE/PHILAI.cpp#L6911) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 67 | local | `philAI::EvaluateArtifactEvent` | `lostB` | `defenderLossValue` | [PHILAI.cpp:6912](../src/SOURCE/PHILAI.cpp#L6912) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 68 | local | `philAI::EvaluateArtifactEvent` | `remainA` | `expectedAttackerLossValue` | [PHILAI.cpp:6913](../src/SOURCE/PHILAI.cpp#L6913) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 69 | local | `philAI::EvaluateArtifactEvent` | `remainB` | `expectedDefenderLossValue` | [PHILAI.cpp:6914](../src/SOURCE/PHILAI.cpp#L6914) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 70 | local | `philAI::EvaluateTownEvent` | `lostA` | `attackerLossValue` | [PHILAI.cpp:7358](../src/SOURCE/PHILAI.cpp#L7358) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 71 | local | `philAI::EvaluateTownEvent` | `lostB` | `defenderLossValue` | [PHILAI.cpp:7357](../src/SOURCE/PHILAI.cpp#L7357) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 72 | local | `philAI::EvaluateTownEvent` | `remainA` | `expectedAttackerLossValue` | [PHILAI.cpp:7356](../src/SOURCE/PHILAI.cpp#L7356) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 73 | local | `philAI::EvaluateTownEvent` | `remainB` | `expectedDefenderLossValue` | [PHILAI.cpp:7355](../src/SOURCE/PHILAI.cpp#L7355) | ProbableOutcomeOfBattle output slot; units are fight value, and Remaining/alive values are expected losses. |
| 74 | argument | `philAI::QuickCombat` | `attackerDamage` | `attackerCasualtyFraction` | [PHILAI.cpp:4332](../src/SOURCE/PHILAI.cpp#L4332) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 75 | argument | `philAI::QuickCombat` | `defenderDamage` | `defenderCasualtyFraction` | [PHILAI.cpp:4333](../src/SOURCE/PHILAI.cpp#L4333) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 76 | local | `philAI::FightEvent` | `attackerLoss` | `attackerCasualtyFraction` | [PHILAI.cpp:5488](../src/SOURCE/PHILAI.cpp#L5488) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 77 | local | `philAI::FightEvent` | `defenderLoss` | `defenderCasualtyFraction` | [PHILAI.cpp:5491](../src/SOURCE/PHILAI.cpp#L5491) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 78 | local | `philAI::TownEvent` | `attackerLoss` | `attackerCasualtyFraction` | [PHILAI.cpp:5782](../src/SOURCE/PHILAI.cpp#L5782) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 79 | local | `philAI::TownEvent` | `defenderLoss` | `defenderCasualtyFraction` | [PHILAI.cpp:5781](../src/SOURCE/PHILAI.cpp#L5781) | QuickCombat fraction output; not the integer estimate with a similar old name. |
| 80 | local | `philAI::CombatMonsterEvent` | `defenderLoss` | `defenderCasualtyFraction` | [PHILAI.cpp:5435](../src/SOURCE/PHILAI.cpp#L5435) | QuickCombat fraction output; not the integer estimate with a similar old name. |

## Held and deliberately excluded

- `philAI::EvaluateMonsterEvent::attackerLoss`: **no proposed new name**. The
  same slot first receives a battle loss estimate, then receives a creature
  purchase value. A single-role rename would preserve a different lie. Recover
  separate semantic locals and the retail frame before proposing their names.
- `MONSTER_JOIN_FORCED`: accurate existing spelling; no rename proposed.
- Human `monstersFlee`, Visions `creaturesFlee`, and shared
  `MONSTER_STRENGTH_FLEE`: no rename proposed. These retain real flee semantics.
- No original public function names, unknown layout fields, XML/save keys, Lua
  callback names, or unrelated icon-decoder `s_loopCount` variables are renamed.
- No source is split, no algorithms or constants are changed, and no gameplay
  bug is repaired by this proposal. A later implementation needs focused retail
  comparisons and regeneration of the four source/classic exports.
