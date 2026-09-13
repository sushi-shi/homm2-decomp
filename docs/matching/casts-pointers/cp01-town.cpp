// CP01: measured town conversion families; parent 36ace4cae.
// Comment-only source archive. Baselines appear once per function.
// All 326 arms compiled/restored; five complete independent families, not their Cartesian product.
// Line spans below are zero-based half-open baseline splitlines(keepends=True) positions.
// Apply all enabled bit edits in descending span order; before-text assertions reject drift.
// Sort bit0 removes the assignment cast; bit1 narrows the temporary to i8.
// Retained: all BuyBuild/category bits; Sort mask1 (narrow0_omit1).
// Native whole-object proof, retail checker limitations and owner rationale: CP01.md.
// Matrix text/relocation hashes and block views are diagnostics, not standalone retail closure.
//
// BEGIN BASELINE buy
// i32 townManager::BuyBuild(
//     H2_ENUM_PARAM(BuildingSlotType, i32) building,
//     i32 cannotBuy,
//     i32 quickView
// ) {
//     u32l prerequisiteMask_c;
//     i32 prerequisiteCount_p;
//     char* description_b;
//     i16 H2_UNUSED(dialogWidth_e);
//     textWidget* amountWidgets_b[TOWN_RESOURCE_COUNT];
//     char* amountText_n[TOWN_RESOURCE_COUNT];
//     iconWidget* resourceWidgets_m[TOWN_RESOURCE_COUNT];
//     i32 rowResourceTypes_a[BUILD_ROW_RESOURCE_CAPACITY];
//     i32 topRowCount_c;
//     tag_message message_m;
//     i32 widgetIndex_f;
//     i32 xStart_b;
//     i16 H2_UNUSED(dialogLeft_a);
//     i16 H2_UNUSED(dialogButtonCount_m);
//     i32 row_l;
//     i32 resourcesInRow_l;
//     i32 rowY_o;
//     i32 windowY_m;
//     i8 resourceTypes_o[TOWN_RESOURCE_COUNT];
//     i32 costCount_o;
//     i16 H2_UNUSED(dialogResult_b);
//     i32 index_h;
//     i32 spacing_h;
//     i32 bottomRowCount_o;
//     heroWindow* window_a;
//     char iconName_o[TOWN_OBJECT_FILENAME_SIZE];
//     i32 rowWidth_h;
//     i16 H2_UNUSED(dialogButtonWidth_l);
//     i32 lineCount_j;
//     i32 windowHeight_a;
//     icon* resourceIcon_c;
//     i16 H2_UNUSED(dialogControl_g);
//     i32 windowRows_b;
//     i32 mageLevel_k;
//     i32 x_d;
//     i32 resourceCount_a;
//     i32 dwelling_k;
//     i16 H2_UNUSED(dialogHeight_f);
//     i32 entryWidth_o;
//     i16 costs_e[TOWN_RESOURCE_COUNT];
//     widget* descriptionWidget_g;
//
//     mageLevel_k = 0;
//     index_h = 0;
//     costCount_o = 0;
//     description_b = static_cast<char*>(H2_ALLOC(BUILDING_DESCRIPTION_CAPACITY));
//
//     for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//         costs_e[index_h] = -1;
//         resourceTypes_o[index_h] = -1;
//     }
//
//     dwelling_k = -1;
//     if (building >= TOWN_OBJECT_DWELLING_1 && building <= TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6)
//         dwelling_k = IDX(building) - IDX(TOWN_OBJECT_DWELLING_1);
//
//     if (building == BUILDING_SLOT_TAVERN && m_town->m_type == FACTION_NECROMANCER) {
//         for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//             if (xShrineBuildingCost[index_h] > 0) {
//                 resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
//                 costs_e[costCount_o] = static_cast<i16>(xShrineBuildingCost[index_h]);
//                 ++costCount_o;
//             }
//         }
//     } else if (building == BUILDING_SLOT_MAGE_GUILD) {
//         mageLevel_k = gpTownManager->m_town->m_buildState;
//         for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//             if (gMageBuildingCosts[NEXT_MAGE_GUILD_LEVEL(mageLevel_k)][index_h] > 0) {
//                 resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
//                 costs_e[costCount_o] = static_cast<i16>(
//                     gMageBuildingCosts[NEXT_MAGE_GUILD_LEVEL(mageLevel_k)][index_h]
//                 );
//                 ++costCount_o;
//             }
//         }
//     } else if (building == BUILDING_SLOT_SPECIAL) {
//         for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//             if (gSpecialBuildingCosts[IDX(gpTownManager->m_town->m_type)][index_h] > 0) {
//                 resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
//                 costs_e[costCount_o] = static_cast<i16>(
//                     gSpecialBuildingCosts[IDX(gpTownManager->m_town->m_type)][index_h]
//                 );
//                 ++costCount_o;
//             }
//         }
//     } else if (building <= BUILDING_SLOT_NEUTRAL_LAST) {
//         for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//             if (gNeutralBuildingCosts[IDX(building)][index_h] > 0) {
//                 resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
//                 costs_e[costCount_o] =
//                     static_cast<i16>(gNeutralBuildingCosts[IDX(building)][index_h]);
//                 ++costCount_o;
//             }
//         }
//     } else {
//         for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//             if (gDwellingCosts[IDX(gpTownManager->m_town->m_type)][dwelling_k][index_h] > 0) {
//                 resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
//                 costs_e[costCount_o] = static_cast<i16>(
//                     gDwellingCosts[IDX(gpTownManager->m_town->m_type)][dwelling_k][index_h]
//                 );
//                 ++costCount_o;
//             }
//         }
//     }
//
//     dialogWidth_e = 80;
//     dialogHeight_f = 40;
//     dialogLeft_a = 32;
//     dialogControl_g = 289;
//     dialogResult_b = 0;
//     dialogButtonWidth_l = 2;
//     dialogButtonCount_m = 3;
//     widgetIndex_f = 0;
//     resourceCount_a = 0;
//     topRowCount_c = 0;
//     bottomRowCount_o = 0;
//
//     for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
//         if (resourceTypes_o[index_h] != -1)
//             ++resourceCount_a;
//     }
//     if (resourceCount_a <= 4) {
//         topRowCount_c = resourceCount_a;
//     } else if (resourceCount_a == 5) {
//         topRowCount_c = 2;
//         bottomRowCount_o = 3;
//     } else if (resourceCount_a == 6) {
//         topRowCount_c = 3;
//         bottomRowCount_o = 3;
//     } else if (resourceCount_a == TOWN_RESOURCE_COUNT) {
//         topRowCount_c = 3;
//         bottomRowCount_o = 4;
//     }
//
//     sprintf(description_b, GetBuildingInfo(m_town->m_type, building, 0));
//     if (dwelling_k >= 0) {
//         prerequisiteCount_p = 0;
//         prerequisiteMask_c = gHierarchyMask[IDX(m_town->m_type)][dwelling_k];
//         for (index_h = 0; index_h < TOWN_BUILDING_COUNT; ++index_h) {
//             if (prerequisiteMask_c & (1L << index_h)) {
//                 if (prerequisiteCount_p == 0)
//                     strcat(description_b, localization::Tr("town.build.requires"));
//                 ++prerequisiteCount_p;
//                 strcat(description_b, "\n");
//                 strcat(description_b, GetBuildingName(m_town->m_type, BuildingSlotType(index_h)));
//             }
//         }
//         if (m_town->m_type == FACTION_NECROMANCER
//             && building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE
//             && m_town->m_buildState <= NECROMANCER_PREREQUISITE_MAX_MAGE_LEVEL)
//             strcat(description_b, localization::Tr("town.build.requires_mage_guild_level_2"));
//     }
//     strcat(description_b, "\n ");
//
//     lineCount_j = bigFont->LineLength(description_b, BUILD_DESCRIPTION_WIDTH);
//     windowY_m = BUILD_WINDOW_BASE_Y;
//     windowHeight_a = windowY_m;
//     windowHeight_a += lineCount_j << BUILD_TEXT_LINE_SHIFT;
//     if (resourceCount_a <= BUILD_ROW_RESOURCE_CAPACITY)
//         windowHeight_a += BUILD_SINGLE_RESOURCE_ROW_HEIGHT;
//     else
//         windowHeight_a += BUILD_DOUBLE_RESOURCE_ROW_HEIGHT;
//     if (quickView == 0)
//         windowHeight_a += BUILD_BUTTON_AREA_HEIGHT;
//     windowRows_b = (windowHeight_a - BUILD_WINDOW_ROW_BASE_HEIGHT) / BUILD_WINDOW_ROW_HEIGHT;
//     if (windowRows_b < BUILD_WINDOW_MIN_ROWS)
//         windowRows_b = BUILD_WINDOW_MIN_ROWS;
//     if (windowRows_b > BUILD_WINDOW_MAX_ROWS)
//         windowRows_b = BUILD_WINDOW_MAX_ROWS;
//
//     sprintf(gText, "buybuil%d.bin", windowRows_b);
//     window_a = new heroWindow(BUILD_WINDOW_X, BUILD_WINDOW_Y, gText);
//     if (window_a == NULL)
//         MemError();
//
//     SET_WIDGET_MESSAGE(message_m, WIDGET_COMMAND_SET_ICON, BUILD_ICON_CONTROL);
//     sprintf(iconName_o, "cstl%s.icn", cHeroTypeShortName[IDX(m_town->m_type)]);
//     message_m.payload.widget.data.text = iconName_o;
//     window_a->BroadcastMessage(message_m);
//     message_m.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
//     message_m.payload.widget.id = BUILD_ICON_CONTROL;
//     message_m.payload.widget.data.value = IDX(building);
//     window_a->BroadcastMessage(message_m);
//
//     if (building == BUILDING_SLOT_MAGE_GUILD) {
//         sprintf(
//             gText,
//             localization::Tr("castle.mage_guild.level")
//             ,
//             NEXT_MAGE_GUILD_LEVEL(mageLevel_k)
//         );
//     } else {
//         strcpy(gText, GetBuildingName(m_town->m_type, building));
//     }
//     message_m.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
//     message_m.payload.widget.id = BUILD_NAME_CONTROL;
//     message_m.payload.widget.data.text = gText;
//     window_a->BroadcastMessage(message_m);
//
//     descriptionWidget_g = new textWidget(
//         BUILD_DESCRIPTION_X,
//         static_cast<i16>(windowY_m + BUILD_DESCRIPTION_Y_OFFSET),
//         BUILD_DESCRIPTION_WIDTH,
//         static_cast<i16>(lineCount_j << BUILD_TEXT_LINE_SHIFT),
//         description_b,
//         "bigfont.fnt",
//         FONT_DRAW_DEFAULT,
//         -1,
//         WIDGET_KIND_UNDIMMED,
//         FONT_ALIGN_CENTER
//     );
//     if (descriptionWidget_g == NULL)
//         MemError();
//     window_a->AddWidget(descriptionWidget_g, -1);
//
//     widgetIndex_f = 0;
//     resourceIcon_c = gpResourceManager->GetIcon("resource.icn");
//     for (row_l = 0; row_l < BUILD_RESOURCE_ROW_COUNT; ++row_l) {
//         rowY_o = lineCount_j * BUILD_TEXT_LINE_HEIGHT + windowY_m
//                  + row_l * BUILD_RESOURCE_ROW_HEIGHT + BUILD_RESOURCE_FIRST_Y_OFFSET;
//         resourcesInRow_l = row_l == 0 ? topRowCount_c : bottomRowCount_o;
//         if (resourcesInRow_l > 0) {
//             rowWidth_h = 0;
//             costCount_o = widgetIndex_f;
//             for (index_h = 0; index_h < BUILD_ROW_RESOURCE_CAPACITY; ++index_h) {
//                 if (index_h < resourcesInRow_l) {
//                     while (resourceTypes_o[costCount_o] == -1)
//                         ++costCount_o;
//                     rowResourceTypes_a[index_h] = resourceTypes_o[costCount_o];
//                     ++costCount_o;
//                 } else {
//                     rowResourceTypes_a[index_h] = -1;
//                 }
//             }
//             for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
//                 rowWidth_h += GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->w;
//             }
//             spacing_h = (BUILD_RESOURCE_AREA_WIDTH - rowWidth_h) / (resourcesInRow_l + 1);
//             xStart_b = spacing_h + BUILD_RESOURCE_AREA_LEFT;
//             x_d = xStart_b;
//             for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
//                 entryWidth_o = GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->w;
//                 amountText_n[widgetIndex_f] =
//                     static_cast<char*>(H2_ALLOC(BUILD_AMOUNT_TEXT_CAPACITY));
//                 sprintf(amountText_n[widgetIndex_f], "%d", costs_e[widgetIndex_f]);
//                 i32 widgetXOffset = 0;
//                 amountWidgets_b[widgetIndex_f] = new textWidget(
//                     static_cast<i16>(x_d + widgetXOffset),
//                     static_cast<i16>(rowY_o + BUILD_AMOUNT_Y_OFFSET),
//                     static_cast<i16>(entryWidth_o),
//                     BUILD_RESOURCE_WIDGET_HEIGHT,
//                     amountText_n[widgetIndex_f],
//                     "smalfont.fnt",
//                     FONT_DRAW_DEFAULT,
//                     -1,
//                     WIDGET_KIND_UNDIMMED,
//                     FONT_ALIGN_CENTER
//                 );
//                 if (amountWidgets_b[widgetIndex_f] == NULL)
//                     MemError();
//                 resourceWidgets_m[widgetIndex_f] = new iconWidget(
//                     static_cast<i16>(
//                         x_d + widgetXOffset
//                         - GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->x
//                     ),
//                     static_cast<i16>(rowY_o),
//                     static_cast<i16>(entryWidth_o),
//                     BUILD_RESOURCE_WIDGET_HEIGHT,
//                     "resource.icn",
//                     resourceTypes_o[widgetIndex_f],
//                     ICON_DRAW_NORMAL,
//                     -1,
//                     WIDGET_KIND_ICON_DIRECT,
//                     1
//                 );
//                 if (resourceWidgets_m[widgetIndex_f] == NULL)
//                     MemError();
//                 window_a->AddWidget(amountWidgets_b[widgetIndex_f], -1);
//                 window_a->AddWidget(resourceWidgets_m[widgetIndex_f], -1);
//                 ++widgetIndex_f;
//                 x_d += entryWidth_o + spacing_h;
//             }
//         }
//     }
//     gpResourceManager->Dispose(resourceIcon_c);
//
//     if (quickView == 0)
//         gpWindowManager->BroadcastMessage(
//             MESSAGE_WIDGET,
//             WIDGET_COMMAND_SET_FLAGS,
//             CONTROL_CLOSE,
//             TOWN_INTERFACE_BROADCAST_FLAGS
//         );
//     m_selectedBuilding = BUILDING_SLOT_NONE;
//     if (quickView != 0) {
//         message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
//         message_m.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
//         message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
//         window_a->BroadcastMessage(message_m);
//         message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
//         message_m.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
//         message_m.payload.widget.id = IDX(DIALOG_CANCEL_ID);
//         window_a->BroadcastMessage(message_m);
//         message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
//         message_m.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
//         message_m.payload.widget.id = 0;
//         window_a->BroadcastMessage(message_m);
//         gpWindowManager->AddWindow(window_a, -1, 1);
//         QuickViewWait();
//         gpWindowManager->RemoveWindow(window_a);
//     } else {
//         if (cannotBuy != 0) {
//             message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
//             message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
//             message_m.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
//             window_a->BroadcastMessage(message_m);
//             message_m.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
//             message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
//             message_m.payload.widget.data.value = IDX(WIDGET_FLAG_GRAYED);
//             window_a->BroadcastMessage(message_m);
//         }
//         gpWindowManager->DoDialog(window_a, TrueFalseDialogHandler, 0);
//         if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
//             m_selectedBuilding = building;
//             for (index_h = 0; index_h < resourceCount_a; ++index_h)
//                 gpCurPlayer->m_resources[resourceTypes_o[index_h]] -= costs_e[index_h];
//         }
//     }
//     if (quickView == 0)
//         gpWindowManager->BroadcastMessage(
//             MESSAGE_WIDGET,
//             WIDGET_COMMAND_CLEAR_FLAGS,
//             CONTROL_CLOSE,
//             TOWN_INTERFACE_BROADCAST_FLAGS
//         );
//     delete window_a;
//     if (quickView != 0) {
//         return 0;
//     } else {
//         return gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM;
//     }
// }
// END BASELINE buy
//
// BEGIN BASELINE category
// void GetCategoryStats(
//     H2_ENUM_PARAM(TownThievesGuildCategory, i32) category,
//     i32l* const stats,
//     i8* const order
// ) {
//     i32 player;
//     i32 townIndex_c;
//     hero* playerHero_h;
//     i32 heroIndex_n;
//     i32 townCount_k;
//     i32 castleCount_p;
//     i32 armyStrength;
//     town* playerTown;
//
//     for (player = 0; player < gpGame->m_playerCount; ++player) {
//         townCount_k = 0;
//         castleCount_p = 0;
//         order[player] = static_cast<i8>(player);
//         if (gpGame->m_playerDead[player]) {
//             stats[player] = TOWN_THIEVES_DEAD_PLAYER_STAT;
//         } else {
//             switch (category) {
//                 case THIEVES_CATEGORY_TOWNS:
//                     for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT; ++townIndex_c) {
//                         if (gpGame->m_castleRecs[townIndex_c].m_owner == player
//                             && HAS(
//                                 gpGame->m_castleRecs[townIndex_c].m_buildings,
//                                 IDX(TOWN_BUILDING_TENT)
//                             )) {
//                             ++townCount_k;
//                         }
//                     }
//                     stats[player] = townCount_k;
//                     break;
//                 case THIEVES_CATEGORY_CASTLES:
//                     for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT; ++townIndex_c) {
//                         if (gpGame->m_castleRecs[townIndex_c].m_owner == player
//                             && HAS(
//                                 gpGame->m_castleRecs[townIndex_c].m_buildings,
//                                 IDX(TOWN_BUILDING_CASTLE)
//                             )) {
//                             ++castleCount_p;
//                         }
//                     }
//                     stats[player] = castleCount_p;
//                     break;
//                 case THIEVES_CATEGORY_HEROES:
//                     stats[player] = gpGame->m_players[player].m_heroCount;
//                     break;
//                 case THIEVES_CATEGORY_GOLD:
//                     stats[player] = gpGame->m_players[player].m_resources[IDX(RES_GOLD)];
//                     break;
//                 case THIEVES_CATEGORY_WOOD_AND_ORE:
//                     stats[player] = gpGame->m_players[player].m_resources[IDX(RES_WOOD)]
//                                     + gpGame->m_players[player].m_resources[IDX(RES_ORE)];
//                     break;
//                 case THIEVES_CATEGORY_RARE_RESOURCES:
//                     stats[player] = gpGame->m_players[player].m_resources[IDX(RES_GEMS)]
//                                     + gpGame->m_players[player].m_resources[IDX(RES_CRYSTAL)]
//                                     + gpGame->m_players[player].m_resources[IDX(RES_SULFUR)]
//                                     + gpGame->m_players[player].m_resources[IDX(RES_MERCURY)];
//                     break;
//                 case THIEVES_CATEGORY_OBELISKS:
//                     stats[player] = GetNumObelisks(player);
//                     break;
//                 case THIEVES_CATEGORY_ARTIFACTS:
//                     stats[player] = 0;
//                     for (townIndex_c = 0; townIndex_c < gpGame->m_players[player].m_heroCount;
//                          ++townIndex_c) {
//                         playerHero_h =
//                             gpGame->GetHero(gpGame->m_players[player].m_heroIds[townIndex_c]);
//                         for (heroIndex_n = 0; heroIndex_n < TOWN_MAX_ARTIFACTS; ++heroIndex_n) {
//                             if (playerHero_h->m_artifacts[heroIndex_n] != ARTIFACT_NONE
//                                 && playerHero_h->m_artifacts[heroIndex_n] != ARTIFACT_MAGIC_BOOK) {
//                                 ++stats[player];
//                             }
//                         }
//                     }
//                     break;
//                 case THIEVES_CATEGORY_ARMY_STRENGTH:
//                     armyStrength = 0;
//                     for (heroIndex_n = 0; heroIndex_n < gpGame->m_players[player].m_heroCount;
//                          ++heroIndex_n) {
//                         playerHero_h = gpGame->GetPlayerHero(player, heroIndex_n);
//                         armyStrength += gpPhilAI->FightValueOfStack(
//                             &playerHero_h->m_army,
//                             playerHero_h,
//                             0
//                         );
//                     }
//                     for (heroIndex_n = 0; heroIndex_n < gpGame->m_players[player].m_townCount;
//                          ++heroIndex_n) {
//                         playerTown = gpGame->GetPlayerTown(player, heroIndex_n);
//                         if (playerTown->HasGarrison()) {
//                             armyStrength +=
//                                 gpPhilAI->FightValueOfStack(&playerTown->m_army, NULL, 0);
//                         }
//                     }
//                     stats[player] = armyStrength;
//                     break;
//                 case THIEVES_CATEGORY_INCOME:
//                     stats[player] = gpGame->ComputeDailyGold(player);
//                     break;
//             }
//         }
//     }
// }
// END BASELINE category
//
// BEGIN BASELINE sort
// void SortStats(i32l* const stats, i8* const order) {
//     i32 temporaryOrder;
//     i32 secondPlayer;
//     i32 firstPlayer;
//     i32l tempStat;
//
//     for (firstPlayer = 0; firstPlayer < gpGame->m_playerCount - 1; ++firstPlayer) {
//         for (secondPlayer = firstPlayer + 1; secondPlayer < gpGame->m_playerCount; ++secondPlayer) {
//             if (stats[secondPlayer] > stats[firstPlayer]) {
//                 tempStat = stats[firstPlayer];
//                 stats[firstPlayer] = stats[secondPlayer];
//                 stats[secondPlayer] = tempStat;
//                 temporaryOrder = order[firstPlayer];
//                 order[firstPlayer] = order[secondPlayer];
//                 order[secondPlayer] = static_cast<i8>(temporaryOrder);
//             }
//         }
//     }
// }
// END BASELINE sort
//
// BEGIN DATA
// {
//   "parent_commit": "36ace4cae",
//   "source": "src/SOURCE/TOWNMGR.cpp",
//   "measured_arms": 326,
//   "families": [
//     {
//       "tag": "cp01-buy-resources",
//       "base": "buy",
//       "rva": "0xa89fe",
//       "dimensions": 5,
//       "mask_names": "omit{mask}",
//       "parent_tu_sha256": "ca7335cc51cd40f60ce1c2f5eda561a09ea2b2a5b8597d6bd7ef802659b739b4",
//       "baseline_body_sha256": "bd9ffa742ab018914bc06e19456a5bb034f1b7a7fcad820d4ae6a475d733e453",
//       "ordered_arm_digest_sha256": "a297bbe12ce0f90cf38a916ba3d9bc961a172898237ab2a0ad3ae987d192f273",
//       "artifacts_sha256": {
//         "build/cp01-buy-resources-axes.json": "1e5aa1414d31b54920d0219dd96f642c694b1d2f317495a18facbf952d1ca287",
//         "build/cp01-buy-resources-manifest.json": "b274c8166ca087c84392706ddf2b42e2a162cc0a17e77ed12643e21303aaf326",
//         "build/source-variant-batch/cp01-buy-resources/results.json": "d0f4d43de1b4fe33e79c70b6c7cf885a4eb4f02eda05077b8519fe82f17fa81d"
//       },
//       "independent_edits": [
//         [
//           {
//             "start": 64,
//             "end": 65,
//             "before": [
//               "                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);\n"
//             ],
//             "after": [
//               "                resourceTypes_o[costCount_o] = index_h;\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 73,
//             "end": 74,
//             "before": [
//               "                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);\n"
//             ],
//             "after": [
//               "                resourceTypes_o[costCount_o] = index_h;\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 83,
//             "end": 84,
//             "before": [
//               "                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);\n"
//             ],
//             "after": [
//               "                resourceTypes_o[costCount_o] = index_h;\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 93,
//             "end": 94,
//             "before": [
//               "                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);\n"
//             ],
//             "after": [
//               "                resourceTypes_o[costCount_o] = index_h;\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 102,
//             "end": 103,
//             "before": [
//               "                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);\n"
//             ],
//             "after": [
//               "                resourceTypes_o[costCount_o] = index_h;\n"
//             ]
//           }
//         ]
//       ],
//       "observations": [
//         {
//           "masks": [
//             0
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "701a5b52cd8851b8",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             1,
//             2,
//             4,
//             8,
//             16
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "8cfdea2f3823fbc0",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             3,
//             5,
//             6,
//             9,
//             10,
//             12,
//             17,
//             18,
//             20,
//             24
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "f182d6dd7e43dab2",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             7,
//             11,
//             13,
//             14,
//             19,
//             21,
//             22,
//             25,
//             26,
//             28
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "118af3e7bb98d073",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             15,
//             23,
//             27,
//             29,
//             30
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "5dc2b5d173d5e68b",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             31
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "e2db64a59711492e",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         }
//       ]
//     },
//     {
//       "tag": "cp01-buy-costs",
//       "base": "buy",
//       "rva": "0xa89fe",
//       "dimensions": 5,
//       "mask_names": "omit{mask}",
//       "parent_tu_sha256": "ca7335cc51cd40f60ce1c2f5eda561a09ea2b2a5b8597d6bd7ef802659b739b4",
//       "baseline_body_sha256": "bd9ffa742ab018914bc06e19456a5bb034f1b7a7fcad820d4ae6a475d733e453",
//       "ordered_arm_digest_sha256": "873b10efd7445809e2785151fdfe1f4126723c1579e042012c993cfb8b2eac71",
//       "artifacts_sha256": {
//         "build/cp01-buy-costs-axes.json": "071c06d59faae6cd7bb2261b83ccfe7144db70917dc93c02d23826819d2a7aa7",
//         "build/cp01-buy-costs-manifest.json": "afa88c85309b4afd74f34641ac171cca4cc1c3871e47fa8a7df90a46fa7085f1",
//         "build/source-variant-batch/cp01-buy-costs/results.json": "9ecc3528f7b55bf3107039769bff0aabeb22a3e96e22d81fafc98e0189248173"
//       },
//       "independent_edits": [
//         [
//           {
//             "start": 65,
//             "end": 66,
//             "before": [
//               "                costs_e[costCount_o] = static_cast<i16>(xShrineBuildingCost[index_h]);\n"
//             ],
//             "after": [
//               "                costs_e[costCount_o] = xShrineBuildingCost[index_h];\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 74,
//             "end": 77,
//             "before": [
//               "                costs_e[costCount_o] = static_cast<i16>(\n",
//               "                    gMageBuildingCosts[NEXT_MAGE_GUILD_LEVEL(mageLevel_k)][index_h]\n",
//               "                );\n"
//             ],
//             "after": [
//               "                costs_e[costCount_o] = gMageBuildingCosts[NEXT_MAGE_GUILD_LEVEL(mageLevel_k)][index_h];\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 84,
//             "end": 87,
//             "before": [
//               "                costs_e[costCount_o] = static_cast<i16>(\n",
//               "                    gSpecialBuildingCosts[IDX(gpTownManager->m_town->m_type)][index_h]\n",
//               "                );\n"
//             ],
//             "after": [
//               "                costs_e[costCount_o] = gSpecialBuildingCosts[IDX(gpTownManager->m_town->m_type)][index_h];\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 95,
//             "end": 96,
//             "before": [
//               "                    static_cast<i16>(gNeutralBuildingCosts[IDX(building)][index_h]);\n"
//             ],
//             "after": [
//               "                    gNeutralBuildingCosts[IDX(building)][index_h];\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 103,
//             "end": 106,
//             "before": [
//               "                costs_e[costCount_o] = static_cast<i16>(\n",
//               "                    gDwellingCosts[IDX(gpTownManager->m_town->m_type)][dwelling_k][index_h]\n",
//               "                );\n"
//             ],
//             "after": [
//               "                costs_e[costCount_o] = gDwellingCosts[IDX(gpTownManager->m_town->m_type)][dwelling_k][index_h];\n"
//             ]
//           }
//         ]
//       ],
//       "observations": [
//         {
//           "masks": [
//             0
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "701a5b52cd8851b8",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             1,
//             2,
//             4,
//             8,
//             16
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "8cfdea2f3823fbc0",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             3,
//             5,
//             6,
//             9,
//             10,
//             12,
//             17,
//             18,
//             20,
//             24
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "f182d6dd7e43dab2",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             7,
//             11,
//             13,
//             14,
//             19,
//             21,
//             22,
//             25,
//             26,
//             28
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "118af3e7bb98d073",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             15,
//             23,
//             27,
//             29,
//             30
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "5dc2b5d173d5e68b",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             31
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "e2db64a59711492e",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         }
//       ]
//     },
//     {
//       "tag": "cp01-buy-widgets",
//       "base": "buy",
//       "rva": "0xa89fe",
//       "dimensions": 8,
//       "mask_names": "omit{mask}",
//       "parent_tu_sha256": "ca7335cc51cd40f60ce1c2f5eda561a09ea2b2a5b8597d6bd7ef802659b739b4",
//       "baseline_body_sha256": "bd9ffa742ab018914bc06e19456a5bb034f1b7a7fcad820d4ae6a475d733e453",
//       "ordered_arm_digest_sha256": "57c6d48846a851a0ddc98282b5b29da4fe6313e420cb6effd349d4bb3c27e41c",
//       "artifacts_sha256": {
//         "build/cp01-buy-widgets-axes.json": "09de97cf89c3daf9b63ec8ea5fb0cb942d90be101fb6144708f2de3e6f824b0c",
//         "build/cp01-buy-widgets-manifest.json": "cf08493905a4f9e2eedbb710ec9ce798d9b9907ec063b8e870d67fb54d85c0ba",
//         "build/source-variant-batch/cp01-buy-widgets/results.json": "29a0a85612caa0d57ea4f7e0c25b70ad1b349aac8f5080f1c88719c8d0e498a4"
//       },
//       "independent_edits": [
//         [
//           {
//             "start": 207,
//             "end": 208,
//             "before": [
//               "        static_cast<i16>(windowY_m + BUILD_DESCRIPTION_Y_OFFSET),\n"
//             ],
//             "after": [
//               "        windowY_m + BUILD_DESCRIPTION_Y_OFFSET,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 209,
//             "end": 210,
//             "before": [
//               "        static_cast<i16>(lineCount_j << BUILD_TEXT_LINE_SHIFT),\n"
//             ],
//             "after": [
//               "        lineCount_j << BUILD_TEXT_LINE_SHIFT,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 253,
//             "end": 254,
//             "before": [
//               "                    static_cast<i16>(x_d + widgetXOffset),\n"
//             ],
//             "after": [
//               "                    x_d + widgetXOffset,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 254,
//             "end": 255,
//             "before": [
//               "                    static_cast<i16>(rowY_o + BUILD_AMOUNT_Y_OFFSET),\n"
//             ],
//             "after": [
//               "                    rowY_o + BUILD_AMOUNT_Y_OFFSET,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 255,
//             "end": 256,
//             "before": [
//               "                    static_cast<i16>(entryWidth_o),\n"
//             ],
//             "after": [
//               "                    entryWidth_o,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 267,
//             "end": 271,
//             "before": [
//               "                    static_cast<i16>(\n",
//               "                        x_d + widgetXOffset\n",
//               "                        - GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->x\n",
//               "                    ),\n"
//             ],
//             "after": [
//               "                    x_d + widgetXOffset\n",
//               "                        - GetIconEntry(resourceIcon_c, rowResourceTypes_a[index_h])->x,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 271,
//             "end": 272,
//             "before": [
//               "                    static_cast<i16>(rowY_o),\n"
//             ],
//             "after": [
//               "                    rowY_o,\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 272,
//             "end": 273,
//             "before": [
//               "                    static_cast<i16>(entryWidth_o),\n"
//             ],
//             "after": [
//               "                    entryWidth_o,\n"
//             ]
//           }
//         ]
//       ],
//       "observations": [
//         {
//           "masks": [
//             0
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "701a5b52cd8851b8",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             1,
//             2,
//             4,
//             8,
//             16,
//             32,
//             64,
//             128
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "8cfdea2f3823fbc0",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             3,
//             5,
//             6,
//             9,
//             10,
//             12,
//             17,
//             18,
//             20,
//             24,
//             33,
//             34,
//             36,
//             40,
//             48,
//             65,
//             66,
//             68,
//             72,
//             80,
//             96,
//             129,
//             130,
//             132,
//             136,
//             144,
//             160,
//             192
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "f182d6dd7e43dab2",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             7,
//             11,
//             13,
//             14,
//             19,
//             21,
//             22,
//             25,
//             26,
//             28,
//             35,
//             37,
//             38,
//             41,
//             42,
//             44,
//             49,
//             50,
//             52,
//             56,
//             67,
//             69,
//             70,
//             73,
//             74,
//             76,
//             81,
//             82,
//             84,
//             88,
//             97,
//             98,
//             100,
//             104,
//             112,
//             131,
//             133,
//             134,
//             137,
//             138,
//             140,
//             145,
//             146,
//             148,
//             152,
//             161,
//             162,
//             164,
//             168,
//             176,
//             193,
//             194,
//             196,
//             200,
//             208,
//             224
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "118af3e7bb98d073",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             15,
//             23,
//             27,
//             29,
//             30,
//             39,
//             43,
//             45,
//             46,
//             51,
//             53,
//             54,
//             57,
//             58,
//             60,
//             71,
//             75,
//             77,
//             78,
//             83,
//             85,
//             86,
//             89,
//             90,
//             92,
//             99,
//             101,
//             102,
//             105,
//             106,
//             108,
//             113,
//             114,
//             116,
//             120,
//             135,
//             139,
//             141,
//             142,
//             147,
//             149,
//             150,
//             153,
//             154,
//             156,
//             163,
//             165,
//             166,
//             169,
//             170,
//             172,
//             177,
//             178,
//             180,
//             184,
//             195,
//             197,
//             198,
//             201,
//             202,
//             204,
//             209,
//             210,
//             212,
//             216,
//             225,
//             226,
//             228,
//             232,
//             240
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "5dc2b5d173d5e68b",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             31,
//             47,
//             55,
//             59,
//             61,
//             62,
//             79,
//             87,
//             91,
//             93,
//             94,
//             103,
//             107,
//             109,
//             110,
//             115,
//             117,
//             118,
//             121,
//             122,
//             124,
//             143,
//             151,
//             155,
//             157,
//             158,
//             167,
//             171,
//             173,
//             174,
//             179,
//             181,
//             182,
//             185,
//             186,
//             188,
//             199,
//             203,
//             205,
//             206,
//             211,
//             213,
//             214,
//             217,
//             218,
//             220,
//             227,
//             229,
//             230,
//             233,
//             234,
//             236,
//             241,
//             242,
//             244,
//             248
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "e2db64a59711492e",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             63,
//             95,
//             111,
//             119,
//             123,
//             125,
//             126,
//             159,
//             175,
//             183,
//             187,
//             189,
//             190,
//             207,
//             215,
//             219,
//             221,
//             222,
//             231,
//             235,
//             237,
//             238,
//             243,
//             245,
//             246,
//             249,
//             250,
//             252
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "fbcc10299ea4ae3d",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             127,
//             191,
//             223,
//             239,
//             247,
//             251,
//             253,
//             254
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "3c4b503d57158a53",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             255
//           ],
//           "size": 3692,
//           "text_sha": "ccda50ff0fa43557",
//           "normalized_reloc_sha": "5b084d78f9c2d949",
//           "candidate_blocks": 156,
//           "retail_blocks": 156,
//           "matrix_exact": false
//         }
//       ]
//     },
//     {
//       "tag": "cp01-category",
//       "base": "category",
//       "rva": "0xac127",
//       "dimensions": 1,
//       "mask_names": "omit{mask}",
//       "parent_tu_sha256": "ca7335cc51cd40f60ce1c2f5eda561a09ea2b2a5b8597d6bd7ef802659b739b4",
//       "baseline_body_sha256": "c5ca622078044a127af80988ed47659e11a4847f7794cba8655ea8685f5a14e5",
//       "ordered_arm_digest_sha256": "ee81f40ce94516aa3402451669d97749bf29ed3b151e68fc806393c779aa9aad",
//       "artifacts_sha256": {
//         "build/cp01-category-axes.json": "5c62cff4ea8d98bc5312415b8743c9b1d72680ea1c19f83e608388b712107ff6",
//         "build/cp01-category-manifest.json": "c99217cdee6737f1041df054fac8ed0bb6bf0da1bcae6df2edfec53396c704b8",
//         "build/source-variant-batch/cp01-category/results.json": "302246c0e3166a56c168a4bf36806724200660ed4c19dcb70098055efb163f25"
//       },
//       "independent_edits": [
//         [
//           {
//             "start": 17,
//             "end": 18,
//             "before": [
//               "        order[player] = static_cast<i8>(player);\n"
//             ],
//             "after": [
//               "        order[player] = player;\n"
//             ]
//           }
//         ]
//       ],
//       "observations": [
//         {
//           "masks": [
//             0
//           ],
//           "size": 1184,
//           "text_sha": "405dd7cac126b351",
//           "normalized_reloc_sha": "1519ca187e74259d",
//           "candidate_blocks": 55,
//           "retail_blocks": 56,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             1
//           ],
//           "size": 1184,
//           "text_sha": "405dd7cac126b351",
//           "normalized_reloc_sha": "173dd4ae99791c97",
//           "candidate_blocks": 55,
//           "retail_blocks": 56,
//           "matrix_exact": false
//         }
//       ]
//     },
//     {
//       "tag": "cp01-sort",
//       "base": "sort",
//       "rva": "0xac5c7",
//       "dimensions": 2,
//       "mask_names": "narrow{mask>>1}_omit{mask&1}",
//       "parent_tu_sha256": "ca7335cc51cd40f60ce1c2f5eda561a09ea2b2a5b8597d6bd7ef802659b739b4",
//       "baseline_body_sha256": "c851774bb4fcdb4e668298a5f5d04538494868131692deecf3bcd366f39aa474",
//       "ordered_arm_digest_sha256": "30467b2dcb0d6dadb201985f8f61e7354edae3b7b88ca2d46a3e874a329426ba",
//       "artifacts_sha256": {
//         "build/cp01-sort-axes.json": "604f0bd4e4b67abe696c021a44c36c309f36695b6e7c2ca1376d9147a1581b98",
//         "build/cp01-sort-manifest.json": "4476b06ed644b91c14ab9cd08e7f072c608cce3a021c773355ff385762dd0e27",
//         "build/source-variant-batch/cp01-sort/results.json": "b6eadc3ac5d7102622efe140189b2c66f9d015a8468121fd7b47e1a398fb7415"
//       },
//       "independent_edits": [
//         [
//           {
//             "start": 14,
//             "end": 15,
//             "before": [
//               "                order[secondPlayer] = static_cast<i8>(temporaryOrder);\n"
//             ],
//             "after": [
//               "                order[secondPlayer] = temporaryOrder;\n"
//             ]
//           }
//         ],
//         [
//           {
//             "start": 1,
//             "end": 2,
//             "before": [
//               "    i32 temporaryOrder;\n"
//             ],
//             "after": [
//               "    i8 temporaryOrder;\n"
//             ]
//           }
//         ]
//       ],
//       "observations": [
//         {
//           "masks": [
//             0,
//             1
//           ],
//           "size": 210,
//           "text_sha": "27c9d803f55eac2c",
//           "normalized_reloc_sha": "6027f01bca1f571e",
//           "candidate_blocks": 15,
//           "retail_blocks": 11,
//           "matrix_exact": false
//         },
//         {
//           "masks": [
//             2,
//             3
//           ],
//           "size": 209,
//           "text_sha": "9c088cdb070d8a1b",
//           "normalized_reloc_sha": "5f59545844652b63",
//           "candidate_blocks": 15,
//           "retail_blocks": 11,
//           "matrix_exact": false
//         }
//       ]
//     }
//   ],
//   "selected_body_sha256": {
//     "buy": "30b43b54a5605bc776d68fea43a54a3809c317e3dd22d37b1a27fa968a06421d",
//     "category": "09d01cf0806dee5b2f1171338e81e911d83cf5e91556d67c1583b48bfa4b9b94",
//     "sort": "df2b954e9eb0429897d76de2261de1ccd5619dd5b094e4e07c9871b5eb5d8cc2"
//   }
// }
// END DATA
//
// BEGIN REPLAY
// import hashlib, json
// from pathlib import Path
// archive = Path("docs/matching/casts-pointers/cp01-town.cpp")
// raw = "\n".join(line[3:] for line in archive.read_text().splitlines())
// def section(name):
//     return raw.split("BEGIN " + name + "\n", 1)[1].split("\nEND " + name, 1)[0]
// def sha(text):
//     return hashlib.sha256(text.encode()).hexdigest()
// def apply(base, edits):
//     lines = base.splitlines(keepends=True)
//     last = len(lines) + 1
//     for e in sorted(edits, key=lambda e: e["start"], reverse=True):
//         assert e["end"] <= last
//         assert lines[e["start"]:e["end"]] == e["before"]
//         lines[e["start"]:e["end"]] = e["after"]
//         last = e["start"]
//     return "".join(lines)
// data = json.loads(section("DATA"))
// count = 0
// selected = {}
// for f in data["families"]:
//     base = section("BASELINE " + f["base"])
//     assert sha(base) == f["baseline_body_sha256"]
//     arms = []
//     digest = ""
//     masks_seen = []
//     for group in f["observations"]:
//         masks_seen.extend(group["masks"])
//     assert sorted(masks_seen) == list(range(1 << f["dimensions"]))
//     for mask in range(1 << f["dimensions"]):
//         name = (f"narrow{mask >> 1}_omit{mask & 1}"
//                 if f["base"] == "sort" else f"omit{mask}")
//         changes = [e for bit, ee in enumerate(f["independent_edits"])
//                    if mask & (1 << bit) for e in ee]
//         body = apply(base, changes)
//         arms.append({"name": name, "replace": body})
//         digest += name + "\0" + sha(body) + "\n"
//         count += 1
//     assert sha(digest) == f["ordered_arm_digest_sha256"]
//     for path, expected in f["artifacts_sha256"].items():
//         p = Path(path)
//         if p.exists():
//             assert hashlib.sha256(p.read_bytes()).hexdigest() == expected, path
//     axes_path = Path("build/" + f["tag"] + "-axes.json")
//     if axes_path.exists():
//         axis = json.loads(axes_path.read_text())["axes"][0]
//         assert axis["find"] == base and axis["options"] == arms
//     selected.setdefault(f["base"], [])
//     selected[f["base"]].extend(
//         e for bit, ee in enumerate(f["independent_edits"])
//         if f["base"] != "sort" or bit == 0 for e in ee)
// for key, changes in selected.items():
//     body = apply(section("BASELINE " + key), changes)
//     assert sha(body) == data["selected_body_sha256"][key]
// assert count == data["measured_arms"] == 326
// print("PASS 326 exact-span source arms and three selected body digests")
// END REPLAY
