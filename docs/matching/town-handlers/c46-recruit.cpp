// Parent 7d4ffd56; 16 complete arms in 12.151050142012537 seconds.
// Selected trial 15: {'source_shape': 'casts15', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (1803, '7135a7b21eb24a12', 'f20f324a0355ad5b').
// Same size and masked payload as baseline: ['casts0', 'casts1', 'casts2', 'casts3', 'casts4', 'casts5', 'casts6', 'casts7', 'casts8', 'casts9', 'casts10', 'casts11', 'casts12', 'casts13', 'casts14', 'casts15'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/c46-recruit-manifest.json; build/source-variant-batch/c46-recruit/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// i32 townManager::RecruitHero(i32 availableHeroIndex, i32 cannotRecruit) {
//     i16 H2_UNUSED(unusedTextState_j) = 1;
//     i16 H2_UNUSED(unusedPortraitState_i) = 2;
//     i16 H2_UNUSED(unusedTextControl_g) = 3;
//     i16 H2_UNUSED(unusedIconState_e) = 4;
//     i16 H2_UNUSED(unusedDimState_d) = 6;
//     i16 H2_UNUSED(unusedPortraitControl_i) = 7;
//     i16 H2_UNUSED(unusedButtonText_h) = 8;
//     i16 H2_UNUSED(unusedButtonIcon_e) = 9;
//     tag_message message_e;
//     i32 artifactCount_h;
//     i32 index_j;
//
//     m_heroWindow1 = new heroWindow(RECRUIT_WINDOW_X, RECRUIT_WINDOW_Y, "rcrthero.bin");
//     if (m_heroWindow1 == NULL)
//         MemError();
//     SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//     m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
//     m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
//     message_e.type = MESSAGE_WIDGET;
//
//     if (cannotRecruit != 0) {
//         message_e.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
//         message_e.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
//         message_e.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
//         m_heroWindow1->BroadcastMessage(message_e);
//         message_e.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
//         m_heroWindow1->BroadcastMessage(message_e);
//         message_e.payload.widget.id = TOWN_DIALOG_CONFIRM;
//         m_heroWindow1->BroadcastMessage(message_e);
//         message_e.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
//         message_e.payload.widget.data.value = IDX(WIDGET_FLAG_GRAYED);
//         message_e.payload.widget.id = RECRUIT_BUTTON_TEXT_CONTROL;
//         m_heroWindow1->BroadcastMessage(message_e);
//         message_e.payload.widget.id = RECRUIT_BUTTON_ICON_CONTROL;
//         m_heroWindow1->BroadcastMessage(message_e);
//         message_e.payload.widget.id = TOWN_DIALOG_CONFIRM;
//         m_heroWindow1->BroadcastMessage(message_e);
//     }
//
//     artifactCount_h = 0;
//     for (index_j = 0; index_j < TOWN_MAX_ARTIFACTS; ++index_j) {
//         if (m_recruitHero->m_artifacts[index_j] != ARTIFACT_NONE
//             && m_recruitHero->m_artifacts[index_j] != ARTIFACT_MAGIC_BOOK)
//             ++artifactCount_h;
//     }
//     sprintf(
//         gText,
//         "%s %d \xf3\xf0\xee\xe2\xed\xff %s. \xc0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xee\xe2: %d." /* "%s %d уровня %s. Артефактов: %d." */,
//         m_recruitHero->m_name,
//         m_recruitHero->m_level,
//         gAlignmentNames[IDX(m_recruitHero->m_cursorType)],
//         artifactCount_h
//     );
//     message_e.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     message_e.payload.widget.id = RECRUIT_DESCRIPTION_CONTROL;
//     message_e.payload.widget.data.text = gText;
//     m_heroWindow1->BroadcastMessage(message_e);
//     sprintf(gText, "port%04d.icn", IDX(m_recruitHero->m_portrait));
//     message_e.payload.widget.command = WIDGET_COMMAND_SET_ICON;
//     message_e.payload.widget.id = RECRUIT_PORTRAIT_CONTROL;
//     message_e.payload.widget.data.text = gText;
//     m_heroWindow1->BroadcastMessage(message_e);
//
//     m_recruitState = -1;
//     gpWindowManager->DoDialog(m_heroWindow1, RecruitHeroHandler, 0);
//     delete m_heroWindow1;
//     if (m_recruitState != -1) {
//         FactionType newHeroClass;
//         i32 townXh;
//         i32 townYWork;
//
//         m_recruitState = availableHeroIndex;
//         gpCurPlayer->m_resources[IDX(RES_GOLD)] -= gHeroGoldCost;
//         gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
//             gpCurPlayer->m_availableHeroIds[m_recruitState];
//         ++gpCurPlayer->m_heroCount;
//
//         townXh = m_town->m_x;
//         townYWork = m_town->m_y;
//         m_recruitHero->m_x = townXh;
//         m_recruitHero->m_y = townYWork;
//         m_recruitHero->m_eventFlags &= HERO_EVENT_SIRENS | HERO_EVENT_ARENA;
//         m_recruitHero->m_direction = MAP_DIRECTION_EAST;
//         m_recruitHero->m_remainingMobility = m_recruitHero->CalcMobility();
//         m_recruitHero->m_mobility = m_recruitHero->m_remainingMobility;
//         m_recruitHero->m_locationType =
//             gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType;
//         m_recruitHero->m_occupiedTown =
//             gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata;
//         gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType =
//             (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION));
//         gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata =
//             gpCurPlayer->m_availableHeroIds[m_recruitState];
//         SendMapChange(
//             MAP_CHANGE_RECRUIT_HERO,
//             m_recruitHero->m_id,
//             townXh,
//             townYWork,
//             MAP_CHANGE_CURRENT_PLAYER,
//             0,
//             0
//         );
//         m_recruitResult = true;
//         m_town->m_occupyingHeroId = m_recruitHero->m_id;
//         gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//             static_cast<i8>(giCurPlayer);
//         CheckValidAvailableHeroes();
//         if (m_town->m_buildings & 1)
//             m_town->GiveSpells(NULL);
//
//         newHeroClass = static_cast<FactionType>(
//             gpCurPlayer->m_availableHeroIds[1 - m_recruitState] / HEROES_PER_FACTION
//         );
//         newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//         gpCurPlayer->m_availableHeroIds[m_recruitState] =
//             static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
//         gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//             AI_HERO_AVAILABLE_FLAG;
//     } else {
//         if (m_castleDialogActive != 0)
//             SetupCastle(m_heroWindow0, 0);
//         if (m_castleDialogActive != 0)
//             m_heroWindow0->DrawWindow();
//     }
//
//     m_bankBox->Update(1);
//     gpWindowManager->BroadcastMessage(
//         MESSAGE_WIDGET,
//         WIDGET_COMMAND_CLEAR_FLAGS,
//         CONTROL_CLOSE,
//         TOWN_INTERFACE_BROADCAST_FLAGS
//     );
//     m_recruitHero->m_owner = -1;
//     if (m_recruitState != -1)
//         m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
//     return m_recruitState != -1;
// }
//
// Option casts0:
//
//
// Option casts1:
// --- baseline
// +++ casts1
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
//
// Option casts2:
// --- baseline
// +++ casts2
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
//
// Option casts3:
// --- baseline
// +++ casts3
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
//
// Option casts4:
// --- baseline
// +++ casts4
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
//
// Option casts5:
// --- baseline
// +++ casts5
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
//
// Option casts6:
// --- baseline
// +++ casts6
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
//
// Option casts7:
// --- baseline
// +++ casts7
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
//
// Option casts8:
// --- baseline
// +++ casts8
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts9:
// --- baseline
// +++ casts9
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts10:
// --- baseline
// +++ casts10
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts11:
// --- baseline
// +++ casts11
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts12:
// --- baseline
// +++ casts12
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts13:
// --- baseline
// +++ casts13
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts14:
// --- baseline
// +++ casts14
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
// Option casts15:
// --- baseline
// +++ casts15
// @@ -16,7 +16,7 @@
//          MemError();
//      SetWinText(m_heroWindow1, RECRUIT_WINDOW_TEXT_ID);
//      m_recruitHero = &gpGame->m_heroRecs[gpCurPlayer->m_availableHeroIds[availableHeroIndex]];
// -    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +    m_recruitHero->m_owner = giCurPlayer;
//      message_e.type = MESSAGE_WIDGET;
//
//      if (cannotRecruit != 0) {
// @@ -104,7 +104,7 @@
//          m_recruitResult = true;
//          m_town->m_occupyingHeroId = m_recruitHero->m_id;
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
// -            static_cast<i8>(giCurPlayer);
// +            giCurPlayer;
//          CheckValidAvailableHeroes();
//          if (m_town->m_buildings & 1)
//              m_town->GiveSpells(NULL);
// @@ -114,7 +114,7 @@
//          );
//          newHeroClass = (newHeroClass + Random(1, IDX(FACTION_COUNT) - 1)) % TOWN_FACTION_COUNT;
//          gpCurPlayer->m_availableHeroIds[m_recruitState] =
// -            static_cast<i8>(gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
// +            gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0);
//          gpGame->m_availableHeroes[gpCurPlayer->m_availableHeroIds[m_recruitState]] =
//              AI_HERO_AVAILABLE_FLAG;
//      } else {
// @@ -133,6 +133,6 @@
//      );
//      m_recruitHero->m_owner = -1;
//      if (m_recruitState != -1)
// -        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
// +        m_recruitHero->m_owner = giCurPlayer;
//      return m_recruitState != -1;
//  }
//
