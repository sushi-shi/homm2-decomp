// VC6 SP5 Castle source family g84-castle-build-join; parent f8b6c956.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g84-castle-build-join/results.json; RVA 0x25230
// Complete 2 arms; elapsed 1.613493s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2925 / 1a52b2757d2aa26c / ca88afec6a8ea9c0 / 127:130 / 123,0,0,4 / False
// 1 / {'source_family': 'shared_case_guard', 'candidate': 'baseline'} / 2898 / cfc7d8a172b6fe53 / ef6cc4e750402ae0 / 124:130 / 77,1,17,29 / False

// AXIS source_family
// BEFORE (complete exact span):
// | MessageDispatchResult CastleHandler(tag_message& message) {
// |     i32 objIndex;
// |     b32 hoverMessage;
// |     i32 heroChoiceIndex;
// |     i32 whichBuilding;
// |     b32 quickFlag;
// |     i32 ret;
// |     i16 H2_UNUSED(statusWidgetId);
// |
// |     statusWidgetId = IDX(CONTROL_STATUS_TEXT);
// |     whichBuilding = IDX(BUILDING_SLOT_NONE);
// |     ret = 0;
// |     hoverMessage = false;
// |
// |     if (message.type == MESSAGE_MOUSE_MOVE || message.type == MESSAGE_WIDGET) {
// |         if (message.type == MESSAGE_MOUSE_MOVE) {
// |             gpWindowManager->ConvertToHover(message);
// |             hoverMessage = true;
// |         }
// |         if (message.payload.widget.id == CONTROL_CAPTAIN_ICON)
// |             whichBuilding = IDX(CASTLE_CAPTAIN);
// |         else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_GROUPED)
// |             whichBuilding = CONTROL_CAPTAIN_FORMATION_GROUPED;
// |         else if (message.payload.widget.id == CONTROL_CAPTAIN_FORMATION_SPREAD)
// |             whichBuilding = CONTROL_CAPTAIN_FORMATION_SPREAD;
// |         else {
// |             if (message.payload.widget.id >= CONTROL_BUILDING_NAME_FIRST
// |                 && message.payload.widget.id
// |                        < CONTROL_BUILDING_NAME_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
// |                 whichBuilding = message.payload.widget.id - CONTROL_BUILDING_NAME_FIRST;
// |             else if (message.payload.widget.id >= CONTROL_BUILDING_ICON_FIRST
// |                      && message.payload.widget.id
// |                             < CONTROL_BUILDING_ICON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
// |                 whichBuilding = message.payload.widget.id - CONTROL_BUILDING_ICON_FIRST;
// |             else if (message.payload.widget.id >= CONTROL_BUILDING_BUTTON_FIRST
// |                      && message.payload.widget.id
// |                             < CONTROL_BUILDING_BUTTON_FIRST + static_cast<i32>(CASTLE_SLOT_COUNT))
// |                 whichBuilding = message.payload.widget.id - CONTROL_BUILDING_BUTTON_FIRST;
// |             if (whichBuilding != IDX(BUILDING_SLOT_NONE))
// |                 whichBuilding = IDX(castleSlotsUse[whichBuilding]);
// |         }
// |     }
// |
// |     if (hoverMessage) {
// |         if (gpTownManager->m_lastHoverId == message.payload.widget.id)
// |             return MESSAGE_DISPATCH_CONSUME;
// |         gpTownManager->m_lastHoverId = message.payload.widget.id;
// |         switch (whichBuilding) {
// |
// |             case CONTROL_CAPTAIN_FORMATION_GROUPED:
// |                 sprintf(gText, cCastleInfo[IDX(INFO_GROUPED_FORMATION)]);
// |                 break;
// |
// |             case CONTROL_CAPTAIN_FORMATION_SPREAD:
// |                 sprintf(gText, cCastleInfo[IDX(INFO_SPREAD_FORMATION)]);
// |                 break;
// |
// |             case IDX(TOWN_OBJECT_MAGE_GUILD):
// |                 if (!(gpTownManager->m_buildableBuildings & BIT(whichBuilding))) {
// |                     sprintf(
// |                         gText,
// |                         cCastleInfo[IDX(INFO_CANNOT_BUILD)],
// |                         GetBuildingName(
// |                             gpTownManager->m_town->m_type,
// |                             static_cast<BuildingSlotType>(whichBuilding)
// |                         )
// |                     );
// |                 } else if (!(gpTownManager->m_affordableBuildings & BIT(whichBuilding))) {
// |                     sprintf(
// |                         gText,
// |                         cCastleInfo[IDX(INFO_CANNOT_AFFORD)],
// |                         GetBuildingName(
// |                             gpTownManager->m_town->m_type,
// |                             static_cast<BuildingSlotType>(whichBuilding)
// |                         )
// |                     );
// |                 } else {
// |                     if (!(gpTownManager->m_town->m_buildings & 1L))
// |                         objIndex = IDX(INFO_BUILD_MAGE_GUILD);
// |                     else if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)
// |                         objIndex = IDX(INFO_MAGE_GUILD_MAX_LEVEL);
// |                     else if (!CanBuy(gpTownManager->m_town, CASTLE_MAGE_GUILD))
// |                         objIndex = IDX(INFO_CANNOT_AFFORD_MAGE_LEVEL);
// |                     else
// |                         objIndex = IDX(INFO_ADD_MAGE_GUILD_LEVEL);
// |                     strcpy(gText, cCastleInfo[objIndex]);
// |                 }
// |                 break;
// |
// |             case IDX(BUILDING_SLOT_SPECIAL_ONE):
// |             case IDX(BUILDING_SLOT_NECROMANCER_SHRINE):
// |             case IDX(BUILDING_SLOT_DOCK):
// |             case IDX(BUILDING_SLOT_SPECIAL_FOUR):
// |             case IDX(BUILDING_SLOT_SPECIAL_SEVEN):
// |             case IDX(BUILDING_SLOT_SPECIAL_EIGHT):
// |             case IDX(BUILDING_SLOT_SPECIAL_NINE):
// |             case IDX(BUILDING_SLOT_SPECIAL_TEN):
// |             case IDX(BUILDING_SLOT_WELL_EXTRA):
// |             case IDX(BUILDING_SLOT_SPECIAL_TWELVE):
// |             case IDX(BUILDING_SLOT_SPECIAL):
// |             case IDX(BUILDING_SLOT_NEUTRAL_LAST):
// |             case IDX(BUILDING_SLOT_DWELLING_FIRST):
// |             case IDX(BUILDING_SLOT_DWELLING_SECOND):
// |             case IDX(BUILDING_SLOT_DWELLING_THIRD):
// |             case IDX(BUILDING_SLOT_DWELLING_FOURTH):
// |             case IDX(BUILDING_SLOT_DWELLING_FIFTH):
// |             case IDX(BUILDING_SLOT_DWELLING_SIXTH):
// |             case IDX(BUILDING_SLOT_UPGRADE_FIRST):
// |             case IDX(BUILDING_SLOT_UPGRADE_SECOND):
// |             case IDX(BUILDING_SLOT_UPGRADE_THIRD):
// |             case IDX(BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE):
// |             case IDX(BUILDING_SLOT_SPECIAL_TWENTY_NINE):
// |             case IDX(BUILDING_SLOT_SPECIAL_THIRTY):
// |                 if (BitTest(gpGame->m_dailyEventFlags, gpTownManager->m_town->m_id)) {
// |                     sprintf(
// |                         gText,
// |                         "\xcd\xe5\xeb\xfc\xe7\xff \xef\xee\xf1\xf2\xf0\xee\xe8\xf2\xfc. \xc2\xfb \xf3\xe6\xe5 "
// |                             "\xf1\xf2\xf0\xee\xe8\xeb\xe8 \xe7\xe4\xe5\xf1\xfc \xe2 \xfd\xf2\xee\xec \xf5\xee\xe4\xf3."
// |                             /* "Нельзя построить. Вы уже строили здесь в этом ходу." */
// |                     );
// |                 } else if (gpTownManager->m_town->m_buildings & BIT(whichBuilding)) {
// |                     sprintf(
// |                         gText,
// |                         cCastleInfo[IDX(INFO_ALREADY_BUILT)],
// |                         GetBuildingName(
// |                             gpTownManager->m_town->m_type,
// |                             static_cast<BuildingSlotType>(whichBuilding)
// |                         )
// |                     );
// |                 } else {
// |                     if (!(gpTownManager->m_buildableBuildings & BIT(whichBuilding)))
// |                         sprintf(
// |                             gText,
// |                             cCastleInfo[IDX(INFO_CANNOT_BUILD)],
// |                             GetBuildingName(
// |                                 gpTownManager->m_town->m_type,
// |                                 static_cast<BuildingSlotType>(whichBuilding)
// |                             )
// |                         );
// |                     else if (!(gpTownManager->m_affordableBuildings & BIT(whichBuilding)))
// |                         sprintf(
// |                             gText,
// |                             cCastleInfo[IDX(INFO_CANNOT_AFFORD)],
// |                             GetBuildingName(
// |                                 gpTownManager->m_town->m_type,
// |                                 static_cast<BuildingSlotType>(whichBuilding)
// |                             )
// |                         );
// |                     else
// |                         sprintf(
// |                             gText,
// |                             cCastleInfo[IDX(INFO_BUILD)],
// |                             GetBuildingName(
// |                                 gpTownManager->m_town->m_type,
// |                                 static_cast<BuildingSlotType>(whichBuilding)
// |                             )
// |                         );
// |                 }
// |                 break;
// |
// |             case IDX(BUILDING_SLOT_NONE):
// |                 switch (message.payload.widget.id) {
// |                     case CONTROL_HERO_FIRST:
// |                     case CONTROL_HERO_FIRST + 1:
// |                         heroChoiceIndex = message.payload.widget.id - CONTROL_HERO_FIRST;
// |                         goto hero_hover_text;
// |                     case CONTROL_RECRUIT_FIRST:
// |                     case CONTROL_RECRUIT_FIRST + 1:
// |                         heroChoiceIndex = message.payload.widget.id - CONTROL_RECRUIT_FIRST;
// |                     hero_hover_text:
// |                         if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost) {
// |                             strcpy(gText, cCastleInfo[IDX(INFO_CANNOT_AFFORD_HERO)]);
// |                         } else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY) {
// |                             sprintf(
// |                                 gText,
// |                                 cCastleInfo[IDX(INFO_TOO_MANY_HEROES)],
// |                                 PLAYER_HERO_CAPACITY
// |                             );
// |                         } else if (gpTownManager->m_town->m_occupyingHeroId != -1) {
// |                             strcpy(gText, cCastleInfo[IDX(INFO_TOWN_OCCUPIED)]);
// |                         } else {
// |                             sprintf(
// |                                 gText,
// |                                 cCastleInfo[IDX(INFO_RECRUIT_HERO)],
// |                                 gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(heroChoiceIndex)]
// |                                     .m_name,
// |                                 gAlignmentNames
// |                                     [IDX(gpGame
// |                                              ->m_heroRecs[gpCurPlayer
// |                                                               ->AvailableHeroId(heroChoiceIndex)]
// |                                              .m_cursorType)]
// |                             );
// |                         }
// |                         break;
// |                     case CONTROL_CLOSE:
// |                         strcpy(gText, cCastleInfo[IDX(INFO_EXIT)]);
// |                         break;
// |                     default:
// |                         strcpy(gText, cCastleInfo[IDX(INFO_OPTIONS)]);
// |                         break;
// |                 }
// |                 break;
// |         }
// |
// |         message.type = MESSAGE_WIDGET;
// |         message.payload.widget.command = CASTLE_WIDGET_TEXT;
// |         message.payload.widget.id = CONTROL_STATUS_TEXT;
// |         message.payload.widget.data.text = gText;
// |         gpTownManager->m_heroWindow0->BroadcastMessage(message);
// |         gpTownManager->m_heroWindow0->DrawWindow(0, CONTROL_STATUS_FIRST, CONTROL_STATUS_TEXT);
// |         gpWindowManager
// |             ->UpdateScreenRegion(INTERFACE_X, INTERFACE_Y, INTERFACE_WIDTH, INTERFACE_HEIGHT);
// |         return MESSAGE_DISPATCH_CONSUME;
// |     }
// |
// |     if (message.type == MESSAGE_WIDGET) {
// |         switch (message.payload.widget.command) {
// |             case WIDGET_COMMAND_DESELECT:
// |                 if (message.payload.widget.id == CONTROL_CLOSE)
// |                     ret = 1;
// |                 break;
// |             case WIDGET_COMMAND_SELECT:
// |             case WIDGET_COMMAND_ALTERNATE_SELECT:
// |                 quickFlag = (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0;
// |                 switch (whichBuilding) {
// |
// |                     case CONTROL_CAPTAIN_FORMATION_SPREAD:
// |                         if (quickFlag) {
// |                             NormalDialog(
// |                                 "{\xd8\xe8\xf0\xee\xea\xe8\xe5 \xf0\xff\xe4\xfb}\n\n\xcf"
// |                                     "\xf0\xe8 \xf2\xe0\xea\xee\xec \xe1\xee\xe5\xe2\xee\xec "
// |                                     "\xef\xee\xf0\xff\xe4\xea\xe5 \xe2\xe0\xf8\xe5 \xe2\xee\xe9"
// |                                     "\xf1\xea\xee \xe7\xe0\xed\xe8\xec\xe0\xe5\xf2 \xef\xee\xe7"
// |                                     "\xe8\xf6\xe8\xe8 \xef\xee \xe2\xf1\xe5\xe9 \xf8\xe8\xf0"
// |                                     "\xe8\xed\xe5 \xef\xee\xeb\xff \xe1\xee\xff \xe8 \xec\xe5"
// |                                     "\xe6\xe4\xf3 \xf1\xee\xf1\xe5\xe4\xed\xe8\xec\xe8 \xee\xf2"
// |                                     "\xf0\xff\xe4\xe0\xec\xe8 \xe8\xec\xe5\xe5\xf2\xf1\xff \xf5"
// |                                     "\xee\xf2\xff \xe1\xfb \xee\xe4\xed\xe0 \xef\xf3\xf1\xf2"
// |                                     "\xe0\xff \xea\xeb\xe5\xf2\xea\xe0."
// |                                     /* "{Широкие ряды}\n\nПри таком боевом порядке ваше войско занимает позиции по всей ширине поля боя и между соседними отрядами имеется хотя бы одна пустая клетка." */,
// |                                 NORMAL_DIALOG_QUICK_VIEW,
// |                                 -1,
// |                                 -1,
// |                                 -1,
// |                                 0,
// |                                 -1,
// |                                 0,
// |                                 -1,
// |                                 0
// |                             );
// |                             break;
// |                         }
// |                         gpTownManager->m_town->m_formation = TOWN_FORMATION_SPREAD;
// |                         gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
// |                         gpTownManager->m_heroWindow0->DrawWindow();
// |                         break;
// |
// |                     case CONTROL_CAPTAIN_FORMATION_GROUPED:
// |                         if (quickFlag) {
// |                             NormalDialog(
// |                                 "{\xcf\xeb\xee\xf2\xed\xfb\xe5 \xf0\xff\xe4\xfb}\n\n\xcf"
// |                                     "\xf0\xe8 \xf2\xe0\xea\xee\xec \xe1\xee\xe5\xe2\xee\xec "
// |                                     "\xef\xee\xf0\xff\xe4\xea\xe5 \xf0\xff\xe4\xfb \xe2\xe0\xf8"
// |                                     "\xe5\xe9 \xe0\xf0\xec\xe8\xe8 \xf1\xec\xfb\xea\xe0\xfe\xf2"
// |                                     "\xf1\xff \xe2\xee\xea\xf0\xf3\xe3 \xf6\xe5\xed\xf2\xf0\xe0"
// |                                     "\xeb\xfc\xed\xee\xe3\xee \xee\xf2\xf0\xff\xe4\xe0 \xed\xe0"
// |                                     " \xe2\xe0\xf8\xe5\xec \xea\xf0\xe0\xfe \xef\xee\xeb\xff "
// |                                     "\xe1\xee\xff."
// |                                     /* "{Плотные ряды}\n\nПри таком боевом порядке ряды вашей армии смыкаются вокруг центрального отряда на вашем краю поля боя." */,
// |                                 NORMAL_DIALOG_QUICK_VIEW,
// |                                 -1,
// |                                 -1,
// |                                 -1,
// |                                 0,
// |                                 -1,
// |                                 0,
// |                                 -1,
// |                                 0
// |                             );
// |                             break;
// |                         }
// |                         gpTownManager->m_town->m_formation = TOWN_FORMATION_GROUPED;
// |                         gpTownManager->SetupCastle(gpTownManager->m_heroWindow0, 1);
// |                         gpTownManager->m_heroWindow0->DrawWindow();
// |                         break;
// |
// |                     case IDX(TOWN_OBJECT_MAGE_GUILD):
// |                         if (!quickFlag) {
// |                             if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL
// |                                 || !(gpTownManager->m_buildableBuildings & BIT(whichBuilding)))
// |                                 break;
// |                         }
// |                         goto buy_building;
// |
// |                     case IDX(BUILDING_SLOT_SPECIAL_ONE):
// |                     case IDX(BUILDING_SLOT_NECROMANCER_SHRINE):
// |                     case IDX(BUILDING_SLOT_DOCK):
// |                     case IDX(BUILDING_SLOT_SPECIAL_FOUR):
// |                     case IDX(BUILDING_SLOT_SPECIAL_SEVEN):
// |                     case IDX(BUILDING_SLOT_SPECIAL_EIGHT):
// |                     case IDX(BUILDING_SLOT_SPECIAL_NINE):
// |                     case IDX(BUILDING_SLOT_SPECIAL_TEN):
// |                     case IDX(BUILDING_SLOT_WELL_EXTRA):
// |                     case IDX(BUILDING_SLOT_SPECIAL_TWELVE):
// |                     case IDX(BUILDING_SLOT_SPECIAL):
// |                     case IDX(BUILDING_SLOT_NEUTRAL_LAST):
// |                     case IDX(BUILDING_SLOT_DWELLING_FIRST):
// |                     case IDX(BUILDING_SLOT_DWELLING_SECOND):
// |                     case IDX(BUILDING_SLOT_DWELLING_THIRD):
// |                     case IDX(BUILDING_SLOT_DWELLING_FOURTH):
// |                     case IDX(BUILDING_SLOT_DWELLING_FIFTH):
// |                     case IDX(BUILDING_SLOT_DWELLING_SIXTH):
// |                     case IDX(BUILDING_SLOT_UPGRADE_FIRST):
// |                     case IDX(BUILDING_SLOT_UPGRADE_SECOND):
// |                     case IDX(BUILDING_SLOT_UPGRADE_THIRD):
// |                     case IDX(BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE):
// |                     case IDX(BUILDING_SLOT_SPECIAL_TWENTY_NINE):
// |                     case IDX(BUILDING_SLOT_SPECIAL_THIRTY):
// |                         if (!quickFlag) {
// |                             if ((gpTownManager->m_town->m_buildings & BIT(whichBuilding))
// |                                 || !(gpTownManager->m_buildableBuildings & BIT(whichBuilding)))
// |                                 break;
// |                         }
// |                     buy_building:
// |                         for (objIndex = 0; objIndex < gpTownManager->m_townObjectCount;
// |                              ++objIndex) {
// |                             if (IDX(gpTownManager->m_townObjects[objIndex]->m_buildingId)
// |                                 == whichBuilding)
// |                                 break;
// |                         }
// |                         ret = gpTownManager->BuyBuild(
// |                             static_cast<BuildingSlotType>(whichBuilding),
// |                             (gpTownManager->m_affordableBuildings & BIT(whichBuilding)) == 0,
// |                             quickFlag
// |                         );
// |                         break;
// |
// |                     case IDX(BUILDING_SLOT_NONE):
// |                         switch (message.payload.widget.id) {
// |                             case CONTROL_HERO_FIRST:
// |                             case CONTROL_HERO_FIRST + 1:
// |                                 heroChoiceIndex = message.payload.widget.id - CONTROL_HERO_FIRST;
// |                                 goto hero_selected;
// |                             case CONTROL_RECRUIT_FIRST:
// |                             case CONTROL_RECRUIT_FIRST + 1:
// |                                 heroChoiceIndex = message.payload.widget.id - CONTROL_RECRUIT_FIRST;
// |                             hero_selected:
// |                                 if (quickFlag) {
// |                                     HeroView(gpCurPlayer->AvailableHeroId(heroChoiceIndex), true, false);
// |                                     casWin->DrawWindow();
// |                                     gpTownManager->m_bankBox->Update(1);
// |                                     gpWindowManager
// |                                         ->FadeScreen(FADE_IN, TOWN_FADE_STEPS, NULL);
// |                                 } else {
// |                                     ret = gpTownManager->RecruitHero(
// |                                         heroChoiceIndex,
// |                                         CannotRecruitHero()
// |                                     );
// |                                 }
// |                                 break;
// |                         }
// |                         break;
// |                 }
// |                 break;
// |         }
// |     }
// |
// |     if (ret != 0) {
// |         message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
// |         message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
// |         return MESSAGE_DISPATCH_FORWARD;
// |     }
// |     return MESSAGE_DISPATCH_CONSUME;
// | }
// ALTERNATIVE shared_case_guard (unified source delta from BEFORE):
// | --- before
// | +++ shared_case_guard
// | @@ -285,13 +285,6 @@
// |                          break;
// |
// |                      case IDX(TOWN_OBJECT_MAGE_GUILD):
// | -                        if (!quickFlag) {
// | -                            if (gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL
// | -                                || !(gpTownManager->m_buildableBuildings & BIT(whichBuilding)))
// | -                                break;
// | -                        }
// | -                        goto buy_building;
// | -
// |                      case IDX(BUILDING_SLOT_SPECIAL_ONE):
// |                      case IDX(BUILDING_SLOT_NECROMANCER_SHRINE):
// |                      case IDX(BUILDING_SLOT_DOCK):
// | @@ -317,11 +310,12 @@
// |                      case IDX(BUILDING_SLOT_SPECIAL_TWENTY_NINE):
// |                      case IDX(BUILDING_SLOT_SPECIAL_THIRTY):
// |                          if (!quickFlag) {
// | -                            if ((gpTownManager->m_town->m_buildings & BIT(whichBuilding))
// | +                            if ((whichBuilding == IDX(TOWN_OBJECT_MAGE_GUILD)
// | +                                     ? gpTownManager->m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL
// | +                                     : (gpTownManager->m_town->m_buildings & BIT(whichBuilding)) != 0)
// |                                  || !(gpTownManager->m_buildableBuildings & BIT(whichBuilding)))
// |                                  break;
// |                          }
// | -                    buy_building:
// |                          for (objIndex = 0; objIndex < gpTownManager->m_townObjectCount;
// |                               ++objIndex) {
// |                              if (IDX(gpTownManager->m_townObjects[objIndex]->m_buildingId)
