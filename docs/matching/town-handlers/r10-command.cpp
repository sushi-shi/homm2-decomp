// Parent 7d4ffd56; 5 complete arms in 4.204038108699024 seconds.
// Selected trial 0: {'source_shape': 'original', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (1176, 'a2ae2ba1563e531c', '27d617be45d11dcc').
// Same size and masked payload as baseline: ['original'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/r10-command-manifest.json; build/source-variant-batch/r10-command/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// void townManager::DoCommand(TownManagerArmyCommand command) {
//     hero* viewedHero;
//     i32 slot;
//     i32 oldValue;
//
//     switch (command) {
//         case ARMY_COMMAND_SELECT:
//             m_swapStrip = m_selectedStrip;
//             m_swapArmySlot = m_selectedArmySlot;
//             m_swapStrip->m_selectedSlot = m_swapArmySlot;
//             m_swapStrip->Draw();
//             break;
//
//         case ARMY_COMMAND_VIEW:
//             viewedHero = m_selectedStrip == m_heroStrip
//                              ? gpGame->GetHero(m_town->m_occupyingHeroId)
//                              : NULL;
//             gpGame->ViewArmy(
//                 TOWN_ARMY_VIEW_X,
//                 TOWN_ARMY_VIEW_Y,
//                 m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot],
//                 m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot],
//                 m_town,
//                 m_castleDialogActive == 1
//                     || (m_selectedStrip == m_heroStrip
//                         && m_selectedStrip->m_army->GetNumArmies() == 1),
//                 ARMY_FACING_RIGHT,
//                 0,
//                 viewedHero,
//                 NULL,
//                 m_selectedStrip->m_army,
//                 m_selectedArmySlot
//             );
//             m_bankBox->Update(1);
//             if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
//                 m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot] = CREATURE_NONE;
//                 m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot] = 0;
//             }
//             ResetStrips();
//             break;
//
//         case ARMY_COMMAND_MERGE:
//             if (m_pendingStrip != m_swapStrip
//                 && m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]
//                        != m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot]) {
//                 for (slot = 0; slot < TOWN_ARMY_SLOT_COUNT; ++slot) {
//                     if (m_pendingStrip->m_army->m_creatureTypes[slot]
//                         == m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot])
//                         break;
//                 }
//                 if (slot < TOWN_ARMY_SLOT_COUNT)
//                     m_pendingArmySlot = slot;
//             }
//             m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] +=
//                 m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
//             m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = CREATURE_NONE;
//             m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = 0;
//             ResetStrips();
//             break;
//
//         case ARMY_COMMAND_SWAP:
//             oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
//             m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
//                 m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
//             m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
//             oldValue = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
//             m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                 m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//             m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
//                 static_cast<CreatureType>(oldValue);
//             ResetStrips();
//             break;
//
//         case ARMY_COMMAND_VIEW_HERO:
//             HeroView(m_town->m_occupyingHeroId, true, false);
//             RedrawTownScreen();
//             gpWindowManager->FadeScreen(FADE_IN, TOWN_FADE_STEPS, NULL);
//             break;
//
//         case ARMY_COMMAND_SPLIT:
//             SplitArmy();
//             ResetStrips();
//             break;
//     }
//     m_lastHoverId = -1;
// }
//
// Option original:
//
//
// Option function_typed0_i32:
// --- baseline
// +++ function_typed0_i32
// @@ -1,7 +1,8 @@
//  void townManager::DoCommand(TownManagerArmyCommand command) {
//      hero* viewedHero;
//      i32 slot;
// -    i32 oldValue;
// +    i32 oldCount;
// +    i32 oldCreature;
//
//      switch (command) {
//          case ARMY_COMMAND_SELECT:
// @@ -59,15 +60,15 @@
//              break;
//
//          case ARMY_COMMAND_SWAP:
// -            oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
// +            oldCount = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
// -            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
// -            oldValue = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
// +            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldCount;
// +            oldCreature = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
//              m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//              m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
// -                static_cast<CreatureType>(oldValue);
// +                static_cast<CreatureType>(oldCreature);
//              ResetStrips();
//              break;
//
//
// Option function_typed1_i32:
// --- baseline
// +++ function_typed1_i32
// @@ -1,7 +1,8 @@
//  void townManager::DoCommand(TownManagerArmyCommand command) {
//      hero* viewedHero;
//      i32 slot;
// -    i32 oldValue;
// +    i32 oldCount;
// +    CreatureType oldCreature;
//
//      switch (command) {
//          case ARMY_COMMAND_SELECT:
// @@ -59,15 +60,15 @@
//              break;
//
//          case ARMY_COMMAND_SWAP:
// -            oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
// +            oldCount = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
// -            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
// -            oldValue = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
// +            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldCount;
// +            oldCreature = m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//              m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
// -                static_cast<CreatureType>(oldValue);
// +                oldCreature;
//              ResetStrips();
//              break;
//
//
// Option case_typed1_i32:
// --- baseline
// +++ case_typed1_i32
// @@ -1,7 +1,6 @@
//  void townManager::DoCommand(TownManagerArmyCommand command) {
//      hero* viewedHero;
//      i32 slot;
// -    i32 oldValue;
//
//      switch (command) {
//          case ARMY_COMMAND_SELECT:
// @@ -58,18 +57,19 @@
//              ResetStrips();
//              break;
//
// -        case ARMY_COMMAND_SWAP:
// -            oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
// +        case ARMY_COMMAND_SWAP: {
// +            i32 oldCount = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
// -            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
// -            oldValue = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
// +            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldCount;
// +            CreatureType oldCreature = m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//              m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
// -                static_cast<CreatureType>(oldValue);
// +                oldCreature;
//              ResetStrips();
//              break;
// +        }
//
//          case ARMY_COMMAND_VIEW_HERO:
//              HeroView(m_town->m_occupyingHeroId, true, false);
//
// Option case_typed1_i16:
// --- baseline
// +++ case_typed1_i16
// @@ -1,7 +1,6 @@
//  void townManager::DoCommand(TownManagerArmyCommand command) {
//      hero* viewedHero;
//      i32 slot;
// -    i32 oldValue;
//
//      switch (command) {
//          case ARMY_COMMAND_SELECT:
// @@ -58,18 +57,19 @@
//              ResetStrips();
//              break;
//
// -        case ARMY_COMMAND_SWAP:
// -            oldValue = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
// +        case ARMY_COMMAND_SWAP: {
// +            i16 oldCount = m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
// -            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
// -            oldValue = IDX(m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot]);
// +            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldCount;
// +            CreatureType oldCreature = m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot];
//              m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
//                  m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
//              m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] =
// -                static_cast<CreatureType>(oldValue);
// +                oldCreature;
//              ResetStrips();
//              break;
// +        }
//
//          case ARMY_COMMAND_VIEW_HERO:
//              HeroView(m_town->m_occupyingHeroId, true, false);
//
