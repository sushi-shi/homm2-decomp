// VC6 SP5 COMMAND source family s62-reset-cycling; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-reset-cycling/results.json; RVA 0x312c9
// Complete 8 arms; elapsed 6.062897s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 529 / d131ce8594c60996 / a90547f53e5cb33e / 26:26 / 26,0,0,0 / True
// 1 / {'local_role_subset': 'repeated_pointer', 'candidate': 'baseline'} / 487 / 55fed00aceefb100 / 5b05736d320ad1f3 / 26:26 / 25,1,0,0 / True
// 2 / {'local_role_subset': 'initial_pointer', 'candidate': 'baseline'} / 522 / 1c19e646a414b5b2 / 3c8d863aa52b7b2b / 26:26 / 25,1,0,0 / True
// 3 / {'local_role_subset': 'initial_pointer_repeated_pointer', 'candidate': 'baseline'} / 480 / ab8c0e996b3a3e13 / 4c1ea1c2c6417d56 / 26:26 / 24,2,0,0 / True
// 4 / {'local_role_subset': 'word', 'candidate': 'baseline'} / 529 / 34c7cc1bdfd73210 / a90547f53e5cb33e / 26:26 / 26,0,0,0 / True
// 5 / {'local_role_subset': 'word_repeated_pointer', 'candidate': 'baseline'} / 487 / a452263060317981 / 5b05736d320ad1f3 / 26:26 / 25,1,0,0 / True
// 6 / {'local_role_subset': 'word_initial_pointer', 'candidate': 'baseline'} / 522 / 496e3c36b6e24c5c / 3c8d863aa52b7b2b / 26:26 / 25,1,0,0 / True
// 7 / {'local_role_subset': 'word_initial_pointer_repeated_pointer', 'candidate': 'baseline'} / 480 / dea460f9334a9297 / 4c1ea1c2c6417d56 / 26:26 / 24,2,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::ResetCyclingCreatures(void) {
// |     army* currentTroop = NULL;
// |     i32 rotateCount = 0;
// |     i32 index;
// |     CombatSide sideIndex;
// |     i32 H2_UNUSED(unusedRotateWord);
// |
// |     for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |         for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// |             currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |             if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                     == 0
// |                 && currentTroop->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
// |                 && currentTroop->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) {
// |                 ++rotateCount;
// |                 ++gpCombatManager->m_limitCreatureCount[IDX(sideIndex)][index];
// |             }
// |         }
// |     }
// |     if (rotateCount == 0)
// |         return;
// |
// |     gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
// |     for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |         for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// |             currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |             if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                 == 0) {
// |                 currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |                 currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
// |                 currentTroop->m_animationFrame = 0;
// |                 currentTroop->m_lastAnimationTime = KBTickCount();
// |             }
// |         }
// |     }
// |     m_heroCycleTimer[IDX(COMBAT_ATTACKER_SIDE)] = KBTickCount();
// |     m_heroCycleTimer[IDX(COMBAT_DEFENDER_SIDE)] = KBTickCount();
// |     gpCombatManager->DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
// | }
// ALTERNATIVE repeated_pointer (unified source delta from BEFORE):
// | --- before
// | +++ repeated_pointer
// | @@ -26,7 +26,6 @@
// |              currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |              if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                  == 0) {
// | -                currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |                  currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
// |                  currentTroop->m_animationFrame = 0;
// |                  currentTroop->m_lastAnimationTime = KBTickCount();
// ALTERNATIVE initial_pointer (unified source delta from BEFORE):
// | --- before
// | +++ initial_pointer
// | @@ -1,5 +1,5 @@
// |  void combatManager::ResetCyclingCreatures(void) {
// | -    army* currentTroop = NULL;
// | +    army* currentTroop;
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// ALTERNATIVE initial_pointer_repeated_pointer (unified source delta from BEFORE):
// | --- before
// | +++ initial_pointer_repeated_pointer
// | @@ -1,5 +1,5 @@
// |  void combatManager::ResetCyclingCreatures(void) {
// | -    army* currentTroop = NULL;
// | +    army* currentTroop;
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// | @@ -26,7 +26,6 @@
// |              currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |              if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                  == 0) {
// | -                currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |                  currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
// |                  currentTroop->m_animationFrame = 0;
// |                  currentTroop->m_lastAnimationTime = KBTickCount();
// ALTERNATIVE word (unified source delta from BEFORE):
// | --- before
// | +++ word
// | @@ -3,7 +3,6 @@
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// | -    i32 H2_UNUSED(unusedRotateWord);
// |
// |      for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |          for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// ALTERNATIVE word_repeated_pointer (unified source delta from BEFORE):
// | --- before
// | +++ word_repeated_pointer
// | @@ -3,7 +3,6 @@
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// | -    i32 H2_UNUSED(unusedRotateWord);
// |
// |      for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |          for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// | @@ -26,7 +25,6 @@
// |              currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |              if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                  == 0) {
// | -                currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |                  currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
// |                  currentTroop->m_animationFrame = 0;
// |                  currentTroop->m_lastAnimationTime = KBTickCount();
// ALTERNATIVE word_initial_pointer (unified source delta from BEFORE):
// | --- before
// | +++ word_initial_pointer
// | @@ -1,9 +1,8 @@
// |  void combatManager::ResetCyclingCreatures(void) {
// | -    army* currentTroop = NULL;
// | +    army* currentTroop;
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// | -    i32 H2_UNUSED(unusedRotateWord);
// |
// |      for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |          for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// ALTERNATIVE word_initial_pointer_repeated_pointer (unified source delta from BEFORE):
// | --- before
// | +++ word_initial_pointer_repeated_pointer
// | @@ -1,9 +1,8 @@
// |  void combatManager::ResetCyclingCreatures(void) {
// | -    army* currentTroop = NULL;
// | +    army* currentTroop;
// |      i32 rotateCount = 0;
// |      i32 index;
// |      CombatSide sideIndex;
// | -    i32 H2_UNUSED(unusedRotateWord);
// |
// |      for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
// |          for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
// | @@ -26,7 +25,6 @@
// |              currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |              if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
// |                  == 0) {
// | -                currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
// |                  currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
// |                  currentTroop->m_animationFrame = 0;
// |                  currentTroop->m_lastAnimationTime = KBTickCount();
