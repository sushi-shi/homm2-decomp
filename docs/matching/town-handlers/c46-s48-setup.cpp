// Parent 7d4ffd56; 12 complete arms in 9.499170825816691 seconds.
// Selected trial 0: {'source_shape': 'original_casts0', 'candidate': 'baseline'}; size/text/label-sensitive reloc signature (2527, '8eba63e38288d863', '7fbdd17f8fbf872a').
// Same size and masked payload as baseline: ['original_casts0'].
// All arms compiled; source restored; no generator/wall truncation, AST mutation or TU-state census.
// Only selected combined source has all-98-object resolved native equivalence.
// Private counter-label hashes are not resolved-target proof; five existing retail EH/site gaps remain.
// Artifacts: build/c46-s48-setup-manifest.json; build/source-variant-batch/c46-s48-setup/results.json.
// See docs/reconstruction/C46-R10-S48-G56.md for exact per-function dispositions.
// Comment-only archive: strip the leading // and optional one space to recover the text below.
//
// Baseline:
// void townManager::SetupTown(void) {
//     tag_message message;
//     i32 i;
//     i32 H2_UNUSED(crestFrame);
//
//     sprintf(gText, GetTownName(m_town->m_id));
//     message.type = MESSAGE_WIDGET;
//     message.payload.widget.command = TOWN_WIDGET_SET_TEXT;
//     message.payload.widget.id = TOWN_WINDOW_TEXT_CONTROL;
//     message.payload.widget.data.text = gText;
//     m_townWindow->BroadcastMessage(message);
//     strcpy(gText, "\xdd\xea\xf0\xe0\xed \xe3\xee\xf0\xee\xe4\xe0");
//     message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
//     message.payload.widget.data.text = gText;
//     m_townWindow->BroadcastMessage(message);
//     m_townWindow->DrawWindow(0, TOWN_WINDOW_DRAW_WIDTH, TOWN_WINDOW_DRAW_RIGHT);
//
//     if (gpCurPlayer->m_townCount == 1) {
//         message.payload.widget.command = TOWN_WIDGET_DISABLE;
//         message.payload.widget.data.value = TOWN_WIDGET_DISABLED_VALUE;
//         message.payload.widget.id = CONTROL_PREVIOUS_TOWN;
//         m_townWindow->BroadcastMessage(message);
//         message.payload.widget.id = CONTROL_NEXT_TOWN;
//         m_townWindow->BroadcastMessage(message);
//         message.payload.widget.command = TOWN_WIDGET_ENABLE;
//         message.payload.widget.data.value = TOWN_WIDGET_ENABLED_VALUE;
//         message.payload.widget.id = CONTROL_PREVIOUS_TOWN;
//         m_townWindow->BroadcastMessage(message);
//         message.payload.widget.id = CONTROL_NEXT_TOWN;
//         m_townWindow->BroadcastMessage(message);
//     }
//
//     if (m_lastTownType != m_town->m_type) {
//         if (m_lastTownType != FACTION_NONE)
//             UnloadTown();
//         m_bankBox = new bankBox(TOWN_BANK_BOX_X, TOWN_GARRISON_STRIP_Y, gpCurPlayer);
//         if (m_bankBox == NULL)
//             MemError();
//         sprintf(gText, "townbkg%d.icn", IDX(m_town->m_type));
//         m_backgroundIcon = gpResourceManager->GetIcon(gText);
//         m_townObjectCount = 0;
//         for (i = 0; i < TOWN_BUILDING_COUNT; ++i) {
//             H2_ENUM_STORAGE(BuildingSlotType, i32) buildId = gTownObjectOrder[IDX(m_town->m_type)][i];
//             if (buildId != TOWN_OBJECT_NONE) {
//                 sprintf(
//                     gText,
//                     "%s%s",
//                     gTownPrefixNames[IDX(m_town->m_type)],
//                     gTownObjNames[IDX(buildId)]
//                 );
//                 m_townObjects[m_townObjectCount] = new townObject(m_town->m_type, buildId, gText);
//                 if (m_townObjects[m_townObjectCount] == NULL)
//                     MemError();
//                 if (m_townObjects[m_townObjectCount]->m_border != NULL) {
//                     if (!(m_town->m_buildings & BIT(buildId))) {
//                         m_townObjects[m_townObjectCount]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
//                         m_townObjects[m_townObjectCount]->m_visible = 0;
//                     }
//                     m_townWindow->AddWidget(
//                         m_townObjects[m_townObjectCount]->m_border,
//                         TOWN_WIDGET_INSERT_DEFAULT
//                     );
//                 }
//                 ++m_townObjectCount;
//             }
//         }
//         gpWindowManager->AddWindow(m_townWindow, 0, 1);
//     } else {
//         m_townObjectCount = 0;
//         for (i = 0; i < TOWN_BUILDING_COUNT; ++i) {
//             H2_ENUM_STORAGE(BuildingSlotType, i32) buildId = gTownObjectOrder[IDX(m_town->m_type)][i];
//             if (buildId != TOWN_OBJECT_NONE) {
//                 if (m_townObjects[m_townObjectCount]->m_border != NULL) {
//                     if (!(m_town->m_buildings & BIT(buildId))) {
//                         m_townObjects[m_townObjectCount]->m_border->m_flags &= ~WIDGET_FLAG_ENABLED;
//                         m_townObjects[m_townObjectCount]->m_visible = 0;
//                     } else {
//                         m_townObjects[m_townObjectCount]->m_border->m_flags |= WIDGET_FLAG_ENABLED;
//                         m_townObjects[m_townObjectCount]->m_visible = 1;
//                     }
//                 }
//                 ++m_townObjectCount;
//             }
//         }
//         if (m_heroStrip != NULL)
//             delete m_heroStrip;
//         m_heroStrip = NULL;
//         if (m_garrisonStrip != NULL)
//             delete m_garrisonStrip;
//         m_garrisonStrip = NULL;
//     }
//
//     crestFrame = gpCurPlayer->m_color;
//     if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
//         crestFrame *= CREST_PORTRAITS_PER_COLOR;
//         crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
//     } else {
//         crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
//     }
//     m_garrisonStrip = new strip(
//         0,
//         TOWN_GARRISON_STRIP_Y,
//         m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
//             ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
//             : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
//         gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//         gpCurPlayer->m_color,
//         &m_town->m_army,
//         TOWN_GARRISON_FIRST_CONTROL,
//         0,
//         -1
//     );
//     if (m_garrisonStrip == NULL)
//         MemError();
//
//     if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
//         sprintf(gText, "port%04d.icn", IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_portrait));
//         m_heroStrip = new strip(
//             0,
//             TOWN_HERO_STRIP_Y,
//             TOWN_HERO_STRIP_FRAME_COUNT,
//             gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
//             0,
//             &gpGame->GetHero(m_town->m_occupyingHeroId)->m_army,
//             TOWN_HERO_FIRST_CONTROL,
//             0,
//             -1
//         );
//         if (m_heroStrip == NULL)
//             MemError();
//         if (m_town->m_buildings & IDX(TOWN_BUILDING_MAGE_GUILD))
//             m_town->GiveSpells(NULL);
//     } else if (m_town->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS)) {
//         sprintf(gText, "port%04d.icn", IDX(m_town->m_type) + TOWN_PORTRAIT_FRAME_BASE);
//         m_heroStrip = new strip(
//             0,
//             TOWN_HERO_STRIP_Y,
//             TOWN_HERO_STRIP_FRAME_COUNT,
//             gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
//             0,
//             NULL,
//             -1,
//             0,
//             gpCurPlayer->m_color
//         );
//         if (m_heroStrip == NULL)
//             MemError();
//     } else {
//         m_heroStrip = new strip(
//             0,
//             TOWN_HERO_STRIP_Y,
//             TOWN_HERO_STRIP_FRAME_COUNT,
//             gpResourceManager->MakeId("strip.icn", TOWN_ICON_RESOURCE_TYPE),
//             TOWN_HERO_STRIP_FRAME_COUNT,
//             NULL,
//             -1,
//             0,
//             -1
//         );
//         if (m_heroStrip == NULL)
//             MemError();
//     }
//
//     m_lastTownType = m_town->m_type;
//     m_pendingStrip = NULL;
//     m_swapStrip = NULL;
//     m_selectedStrip = NULL;
//     m_pendingArmySlot = TOWN_ARMY_SLOT_NONE;
//     m_swapArmySlot = TOWN_ARMY_SLOT_NONE;
//     m_selectedArmySlot = TOWN_ARMY_SLOT_NONE;
//     DrawTown(0, 0);
//     gpWindowManager->UpdateScreenRegion(0, 0, TOWN_SCREEN_WIDTH, TOWN_SCREEN_HEIGHT);
// }
//
// Option original_casts0:
//
//
// Option original_casts1:
// --- baseline
// +++ original_casts1
// @@ -101,7 +101,7 @@
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
//              : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//
// Option original_casts2:
// --- baseline
// +++ original_casts2
// @@ -102,7 +102,7 @@
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
//              ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
// Option original_casts3:
// --- baseline
// +++ original_casts3
// @@ -101,8 +101,8 @@
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
// Option call_only_casts0:
// --- baseline
// +++ call_only_casts0
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,13 +89,8 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        gpGame->GetHero(m_town->m_occupyingHeroId);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//
// Option call_only_casts1:
// --- baseline
// +++ call_only_casts1
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,18 +89,13 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        gpGame->GetHero(m_town->m_occupyingHeroId);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
//              : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//
// Option call_only_casts2:
// --- baseline
// +++ call_only_casts2
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,19 +89,14 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        gpGame->GetHero(m_town->m_occupyingHeroId);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
//              ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
// Option call_only_casts3:
// --- baseline
// +++ call_only_casts3
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,19 +89,14 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        gpGame->GetHero(m_town->m_occupyingHeroId);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
// Option reads_casts0:
// --- baseline
// +++ reads_casts0
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,13 +89,9 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    gpCurPlayer->m_color;
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//
// Option reads_casts1:
// --- baseline
// +++ reads_casts1
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,18 +89,14 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    gpCurPlayer->m_color;
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
//              : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//
// Option reads_casts2:
// --- baseline
// +++ reads_casts2
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,19 +89,15 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    gpCurPlayer->m_color;
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
//              ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
// Option reads_casts3:
// --- baseline
// +++ reads_casts3
// @@ -1,7 +1,6 @@
//  void townManager::SetupTown(void) {
//      tag_message message;
//      i32 i;
// -    i32 H2_UNUSED(crestFrame);
//
//      sprintf(gText, GetTownName(m_town->m_id));
//      message.type = MESSAGE_WIDGET;
// @@ -90,19 +89,15 @@
//          m_garrisonStrip = NULL;
//      }
//
// -    crestFrame = gpCurPlayer->m_color;
// -    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// -        crestFrame *= CREST_PORTRAITS_PER_COLOR;
// -        crestFrame += IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
// -    } else {
// -        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
// -    }
// +    gpCurPlayer->m_color;
// +    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
// +        IDX(gpGame->GetHero(m_town->m_occupyingHeroId)->m_cursorType);
//      m_garrisonStrip = new strip(
//          0,
//          TOWN_GARRISON_STRIP_Y,
//          m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
// -            ? static_cast<i8>(TOWN_CREST_FRAME_WITHOUT_HERO)
// -            : static_cast<i8>(TOWN_CREST_FRAME_WITH_HERO),
// +            ? TOWN_CREST_FRAME_WITHOUT_HERO
// +            : TOWN_CREST_FRAME_WITH_HERO,
//          gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
//          gpCurPlayer->m_color,
//          &m_town->m_army,
//
