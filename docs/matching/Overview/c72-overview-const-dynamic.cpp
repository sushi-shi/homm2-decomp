// VC6 SP5 Overview source family c72-overview-const-dynamic; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-const-dynamic/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.923935s; input restored.
// Disposition: retain literal-name removals EXCEPT the conditional captain/portrait name.
// That conditional is const char* in C++98 and needs the original const_cast for the
// mutable legacy constructor. Whole-arm VC6 equivalence alone was insufficient;
// final partial retention passes both dialect checks and combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'literal_names', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 7749014774c8c1de / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |             ROW_BACKGROUND_HEIGHT,
// |             const_cast<char*>("overview.icn"),
// |             static_cast<i16>(IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
// |             ICON_DRAW_NORMAL,
// |             static_cast<i16>(rowWidgetId + ROW_BACKGROUND_CONTROL),
// |             WIDGET_KIND_ICON_DIRECT,
// |             OVERVIEW_ICON_FILL_COLOR
// |         );
// |         if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |             MemError();
// |         }
// |         overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |         icons++;
// |
// |         if (giOverviewType == OVERVIEW_TOWNS) {
// |             OverviewDwellingIndex building;
// |             i32 displayedTroops;
// |             hero* heroData;
// |             town* record =
// |                 GetTown(gpCurPlayer->m_townIds[giOverviewTop[IDX(giOverviewType)] + rowIndex]);
// |             i32 townFrame;
// |             b32 capt;
// |             i32 captainMana;
// |             {
// |                 valueText = static_cast<char*>(H2_ALLOC(strlen(record->m_name) + 1));
// |                 strcpy(valueText, record->m_name);
// |
// |                 OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                     TOWN_NAME_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_NAME_Y_OFFSET),
// |                     TOWN_NAME_WIDTH,
// |                     TOWN_NAME_HEIGHT,
// |                     valueText,
// |                     const_cast<char*>("smalfont.fnt"),
// |                     FONT_DRAW_DEFAULT,
// |                     static_cast<i16>(rowWidgetId + TOWN_NAME_CONTROL),
// |                     WIDGET_KIND_TEXT,
// |                     FONT_ALIGN_CENTER
// |                 );
// |                 overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                 texts++;
// |             }
// |
// |             {
// |                 townFrame = IDX(record->m_type);
// |                 if ((record->m_buildings & IDX(TOWN_BUILDING_CASTLE)) == 0) {
// |                     townFrame += TOWN_UNFORTIFIED_FRAME_OFFSET;
// |                 }
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_ICON_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET),
// |                     TOWN_ICON_WIDTH,
// |                     TOWN_ICON_HEIGHT,
// |                     const_cast<char*>("minitown.icn"),
// |                     townFrame,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_SELECT_WIDGET),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |             }
// |
// |             if (BitTest(gpGame->m_knownTowns, static_cast<u32>(record->m_id))) {
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_LOCATOR_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET),
// |                     0,
// |                     0,
// |                     const_cast<char*>("locators.icn"),
// |                     TOWN_LOCATOR_FRAME,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_LOCATOR_CONTROL),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |             }
// |
// |             capt = false;
// |             heroData = NULL;
// |             if (record->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
// |                 heroData = GetHero(record->m_occupyingHeroId);
// |             } else {
// |                 if ((record->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS)) != 0) {
// |                     capt = true;
// |                 }
// |             }
// |
// |             if (heroData != NULL || capt) {
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_HERO_FRAME_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET),
// |                     TOWN_HERO_FRAME_WIDTH,
// |                     TOWN_HERO_FRAME_HEIGHT,
// |                     const_cast<char*>("portxtra.icn"),
// |                     0,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_HERO_FIRST),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_HERO_PORTRAIT_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET),
// |                     TOWN_HERO_PORTRAIT_WIDTH,
// |                     TOWN_HERO_PORTRAIT_HEIGHT,
// |                     const_cast<char*>(capt ? "minicapt.icn" : "miniport.icn"),
// |                     capt ? static_cast<u8>(IDX(record->m_type)) : IDX(heroData->m_portrait),
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_HERO_PORTRAIT_CONTROL),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_MOBILITY_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET),
// |                     0,
// |                     0,
// |                     const_cast<char*>("mobility.icn"),
// |                     static_cast<i16>(GetMobilityFrame(capt ? 0 : heroData->m_remainingMobility)),
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_HERO_MOBILITY_CONTROL),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |
// |                 if (record->m_type == FACTION_BARBARIAN || record->m_type == FACTION_KNIGHT) {
// |                     captainMana = CAPTAIN_MANA_LOW;
// |                 } else {
// |                     captainMana = CAPTAIN_MANA_HIGH;
// |                 }
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     TOWN_MANA_X,
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET),
// |                     0,
// |                     0,
// |                     const_cast<char*>("mana.icn"),
// |                     static_cast<i16>(GetManaFrame(capt ? captainMana : heroData->m_spellPoints)),
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |             }
// |
// |             {
// |                 displayedTroops = 0;
// |                 for (i = 0; i < OVERVIEW_TROOP_SLOTS; i++) {
// |                     if (record->m_army.m_creatureTypes[i] != CREATURE_NONE
// |                         && record->m_army.m_creatureCounts[i] > 0) {
// |                         OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                             static_cast<i16>(
// |                                 displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
// |                             ),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
// |                             ),
// |                             TROOP_ICON_WIDTH,
// |                             TROOP_ICON_HEIGHT,
// |                             const_cast<char*>("mons32.icn"),
// |                             static_cast<i16>(record->m_army.m_creatureTypes[i]),
// |                             ICON_DRAW_NORMAL,
// |                             static_cast<i16>(rowWidgetId + i + TOWN_ARMY_FIRST),
// |                             WIDGET_KIND_ICON_CENTERED,
// |                             OVERVIEW_ICON_FILL_COLOR
// |                         );
// |                         if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                             MemError();
// |                         }
// |                         overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                         icons++;
// |
// |                         valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
// |                         sprintf(
// |                             valueText,
// |                             "%d",
// |                             static_cast<i32>(record->m_army.m_creatureCounts[i])
// |                         );
// |                         OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                             static_cast<i16>(
// |                                 displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
// |                             ),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
// |                             ),
// |                             TROOP_TEXT_WIDTH,
// |                             TROOP_TEXT_HEIGHT,
// |                             valueText,
// |                             const_cast<char*>("smalfont.fnt"),
// |                             FONT_DRAW_DEFAULT,
// |                             static_cast<i16>(rowWidgetId + i + TOWN_ARMY_ALT_FIRST),
// |                             WIDGET_KIND_TEXT,
// |                             FONT_ALIGN_CENTER
// |                         );
// |                         overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                         texts++;
// |                         displayedTroops++;
// |                     }
// |                 }
// |             }
// |
// |             {
// |                 displayedTroops = 0;
// |                 for (i = 0; i < OVERVIEW_DWELLING_SLOTS; i++) {
// |                     switch (i) {
// |                         case DWELLING_ORDER_LEVEL_1:
// |                             building = DWELLING_INDEX_LEVEL_1;
// |                             break;
// |                         case DWELLING_ORDER_LEVEL_2:
// |                             building = DWELLING_INDEX_LEVEL_2;
// |                             break;
// |                         case DWELLING_ORDER_UPGRADED_LEVEL_2:
// |                             building = DWELLING_INDEX_UPGRADED_LEVEL_2;
// |                             break;
// |                         case DWELLING_ORDER_LEVEL_3:
// |                             building = DWELLING_INDEX_LEVEL_3;
// |                             break;
// |                         case DWELLING_ORDER_UPGRADED_LEVEL_3:
// |                             building = DWELLING_INDEX_UPGRADED_LEVEL_3;
// |                             break;
// |                         case DWELLING_ORDER_LEVEL_4:
// |                             building = DWELLING_INDEX_LEVEL_4;
// |                             break;
// |                         case DWELLING_ORDER_UPGRADED_LEVEL_4:
// |                             building = DWELLING_INDEX_UPGRADED_LEVEL_4;
// |                             break;
// |                         case DWELLING_ORDER_LEVEL_5:
// |                             building = DWELLING_INDEX_LEVEL_5;
// |                             break;
// |                         case DWELLING_ORDER_UPGRADED_LEVEL_5:
// |                             building = DWELLING_INDEX_UPGRADED_LEVEL_5;
// |                             break;
// |                         case DWELLING_ORDER_LEVEL_6:
// |                             building = DWELLING_INDEX_LEVEL_6;
// |                             break;
// |                         case DWELLING_ORDER_UPGRADED_LEVEL_6:
// |                             building = DWELLING_INDEX_UPGRADED_LEVEL_6;
// |                             break;
// |                         default:
// |                             building = DWELLING_INDEX_ALTERNATE_LEVEL_6;
// |                             break;
// |                     }
// |
// |                     if ((record->m_buildings
// |                          & (1 << (IDX(building) + IDX(TOWN_DWELLING_BUILDING_BIT_BASE))))
// |                         != 0) {
// |                         OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                             static_cast<i16>(
// |                                 displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// |                             ),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
// |                             ),
// |                             TROOP_ICON_WIDTH,
// |                             TROOP_ICON_HEIGHT,
// |                             const_cast<char*>("mons32.icn"),
// |                             static_cast<i16>(gDwellingType[IDX(record->m_type)][building]),
// |                             ICON_DRAW_NORMAL,
// |                             static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_FIRST),
// |                             WIDGET_KIND_ICON_CENTERED,
// |                             OVERVIEW_ICON_FILL_COLOR
// |                         );
// |                         if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                             MemError();
// |                         }
// |                         overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                         icons++;
// |
// |                         valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
// |                         sprintf(valueText, "%d", static_cast<i32>(record->m_garrison[building]));
// |                         OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                             static_cast<i16>(
// |                                 displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// |                             ),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
// |                             ),
// |                             TROOP_TEXT_WIDTH,
// |                             TROOP_TEXT_HEIGHT,
// |                             valueText,
// |                             const_cast<char*>("smalfont.fnt"),
// |                             FONT_DRAW_DEFAULT,
// |                             static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_ALT_FIRST),
// |                             WIDGET_KIND_TEXT,
// |                             FONT_ALIGN_CENTER
// |                         );
// |                         overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                         texts++;
// |                         displayedTroops++;
// |                     }
// |                 }
// |             }
// |         } else {
// |             i32 shown;
// |             hero* curHero;
// |             i32 column;
// |             HeroSecondarySkill skillIndex;
// |             i32 detailRow;
// |             curHero =
// |                 GetHero(gpCurPlayer->m_heroIds[giOverviewTop[IDX(giOverviewType)] + rowIndex]);
// |
// |             OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                 HERO_FRAME_X,
// |                 static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_FRAME_Y_OFFSET),
// |                 HERO_FRAME_WIDTH,
// |                 HERO_FRAME_HEIGHT,
// |                 const_cast<char*>("portxtra.icn"),
// |                 0,
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(rowWidgetId + HERO_SELECT_FIRST),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 OVERVIEW_ICON_FILL_COLOR
// |             );
// |             if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                 MemError();
// |             }
// |             overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |             icons++;
// |
// |             OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                 HERO_PORTRAIT_X,
// |                 static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PORTRAIT_Y_OFFSET),
// |                 HERO_PORTRAIT_WIDTH,
// |                 HERO_PORTRAIT_HEIGHT,
// |                 const_cast<char*>("miniport.icn"),
// |                 static_cast<i16>(curHero->m_portrait),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(rowWidgetId + HERO_PORTRAIT_CONTROL),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 OVERVIEW_ICON_FILL_COLOR
// |             );
// |             if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                 MemError();
// |             }
// |             overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |             icons++;
// |
// |             OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                 HERO_MOBILITY_X,
// |                 static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MOBILITY_Y_OFFSET),
// |                 0,
// |                 0,
// |                 const_cast<char*>("mobility.icn"),
// |                 static_cast<i16>(GetMobilityFrame(curHero->m_remainingMobility)),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(rowWidgetId + HERO_MOBILITY_CONTROL),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 OVERVIEW_ICON_FILL_COLOR
// |             );
// |             if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                 MemError();
// |             }
// |             overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |             icons++;
// |
// |             OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                 HERO_MANA_X,
// |                 static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MANA_Y_OFFSET),
// |                 0,
// |                 0,
// |                 const_cast<char*>("mana.icn"),
// |                 static_cast<i16>(GetManaFrame(curHero->m_spellPoints)),
// |                 ICON_DRAW_NORMAL,
// |                 static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 OVERVIEW_ICON_FILL_COLOR
// |             );
// |             if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                 MemError();
// |             }
// |             overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |             icons++;
// |
// |             for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
// |                 valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_PRIMARY_TEXT_CAPACITY));
// |                 sprintf(valueText, "%d", static_cast<i32>(curHero->Stats(HeroPrimaryStat(i))));
// |                 OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                     static_cast<i16>(i * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_TEXT_FIRST_X),
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_TEXT_Y_OFFSET),
// |                     HERO_PRIMARY_TEXT_WIDTH,
// |                     HERO_PRIMARY_TEXT_HEIGHT,
// |                     valueText,
// |                     const_cast<char*>("smalfont.fnt"),
// |                     FONT_DRAW_DEFAULT,
// |                     static_cast<i16>(rowWidgetId + i + HERO_PRIMARY_TEXT_FIRST),
// |                     WIDGET_KIND_TEXT,
// |                     FONT_ALIGN_RIGHT
// |                 );
// |                 overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                 texts++;
// |             }
// |
// |             for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
// |                 OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                     static_cast<i16>(i * HERO_PRIMARY_COLUMN_STRIDE + HERO_PRIMARY_ICON_FIRST_X),
// |                     static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_ICON_Y_OFFSET),
// |                     HERO_PRIMARY_ICON_WIDTH,
// |                     HERO_PRIMARY_ICON_HEIGHT,
// |                     const_cast<char*>("overview.icn"),
// |                     HERO_PRIMARY_ICON_FRAME,
// |                     ICON_DRAW_NORMAL,
// |                     static_cast<i16>(rowWidgetId + i + HERO_STAT_FIRST),
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     OVERVIEW_ICON_FILL_COLOR
// |                 );
// |                 if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                     MemError();
// |                 }
// |                 overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                 icons++;
// |             }
// |
// |             {
// |                 shown = 0;
// |                 for (i = 0; i < OVERVIEW_TROOP_SLOTS; i++) {
// |                     if (curHero->m_army.m_creatureTypes[i] != CREATURE_NONE
// |                         && curHero->m_army.m_creatureCounts[i] > 0) {
// |                         OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                             static_cast<i16>(shown * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + HERO_TROOP_ICON_Y_OFFSET
// |                             ),
// |                             TROOP_ICON_WIDTH,
// |                             TROOP_ICON_HEIGHT,
// |                             const_cast<char*>("mons32.icn"),
// |                             static_cast<i16>(curHero->m_army.m_creatureTypes[i]),
// |                             ICON_DRAW_NORMAL,
// |                             static_cast<i16>(rowWidgetId + i + HERO_ARMY_FIRST),
// |                             WIDGET_KIND_ICON_CENTERED,
// |                             OVERVIEW_ICON_FILL_COLOR
// |                         );
// |                         if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                             MemError();
// |                         }
// |                         overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                         icons++;
// |
// |                         valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
// |                         sprintf(
// |                             valueText,
// |                             "%d",
// |                             static_cast<i32>(curHero->m_army.m_creatureCounts[i])
// |                         );
// |                         OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                             static_cast<i16>(shown * HERO_TROOP_COLUMN_STRIDE + HERO_TROOP_FIRST_X),
// |                             static_cast<i16>(
// |                                 rowIndex * OVERVIEW_ROW_HEIGHT + HERO_TROOP_TEXT_Y_OFFSET
// |                             ),
// |                             TROOP_TEXT_WIDTH,
// |                             TROOP_TEXT_HEIGHT,
// |                             valueText,
// |                             const_cast<char*>("smalfont.fnt"),
// |                             FONT_DRAW_DEFAULT,
// |                             static_cast<i16>(rowWidgetId + i + HERO_ARMY_ALT_FIRST),
// |                             WIDGET_KIND_TEXT,
// |                             FONT_ALIGN_CENTER
// |                         );
// |                         overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                         texts++;
// |                         shown++;
// |                     }
// |                 }
// |             }
// |
// |             for (i = 0; i < OVERVIEW_SECONDARY_SKILL_SLOTS; i++) {
// |                 skillIndex = curHero->GetNthSS(i);
// |                 if (skillIndex != HERO_SKILL_NONE) {
// |                     detailRow = i / OVERVIEW_SECONDARY_SKILL_COLUMNS;
// |                     column = i % OVERVIEW_SECONDARY_SKILL_COLUMNS;
// |                     OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                         static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_FRAME_FIRST_X),
// |                         static_cast<i16>(
// |                             detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
// |                             + HERO_SKILL_FRAME_Y_OFFSET
// |                         ),
// |                         DETAIL_FRAME_WIDTH,
// |                         DETAIL_FRAME_HEIGHT,
// |                         const_cast<char*>("overview.icn"),
// |                         DETAIL_FRAME_ICON,
// |                         ICON_DRAW_NORMAL,
// |                         static_cast<i16>(rowWidgetId + i + HERO_SKILL_FRAME_FIRST),
// |                         WIDGET_KIND_ICON_DIRECT,
// |                         OVERVIEW_ICON_FILL_COLOR
// |                     );
// |                     if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                         MemError();
// |                     }
// |                     overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                     icons++;
// |
// |                     OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                         static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_ICON_FIRST_X),
// |                         static_cast<i16>(
// |                             detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
// |                             + HERO_SKILL_ICON_Y_OFFSET
// |                         ),
// |                         DETAIL_ICON_WIDTH,
// |                         DETAIL_ICON_HEIGHT,
// |                         const_cast<char*>("miniss.icn"),
// |                         static_cast<i16>(skillIndex),
// |                         ICON_DRAW_NORMAL,
// |                         static_cast<i16>(rowWidgetId + i + HERO_SKILL_FIRST),
// |                         WIDGET_KIND_ICON_DIRECT,
// |                         OVERVIEW_ICON_FILL_COLOR
// |                     );
// |                     if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                         MemError();
// |                     }
// |                     overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                     icons++;
// |
// |                     valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_SKILL_LEVEL_CAPACITY));
// |                     sprintf(valueText, "%d", static_cast<i32>(curHero->GetSSLevel(skillIndex)));
// |                     OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                         static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_LEVEL_FIRST_X),
// |                         static_cast<i16>(
// |                             detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
// |                             + HERO_SKILL_LEVEL_Y_OFFSET
// |                         ),
// |                         HERO_SKILL_LEVEL_WIDTH,
// |                         HERO_SKILL_LEVEL_HEIGHT,
// |                         valueText,
// |                         const_cast<char*>("smalfont.fnt"),
// |                         FONT_DRAW_DEFAULT,
// |                         static_cast<i16>(rowWidgetId + i + HERO_SKILL_LEVEL_FIRST),
// |                         WIDGET_KIND_TEXT,
// |                         FONT_ALIGN_RIGHT
// |                     );
// |                     overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                     texts++;
// |                 }
// |             }
// |
// |             i32 displayedArtifacts = 0;
// |             for (i = 0; i < OVERVIEW_ARTIFACT_SLOTS; i++) {
// |                 if (curHero->m_artifacts[i] != ARTIFACT_NONE) {
// |                     detailRow = displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS;
// |                     column = displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS;
// |                     OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                         static_cast<i16>(
// |                             column * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_FRAME_FIRST_X
// |                         ),
// |                         static_cast<i16>(
// |                             detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
// |                             + HERO_ARTIFACT_FRAME_Y_OFFSET
// |                         ),
// |                         DETAIL_FRAME_WIDTH,
// |                         DETAIL_FRAME_HEIGHT,
// |                         const_cast<char*>("overview.icn"),
// |                         DETAIL_FRAME_ICON,
// |                         ICON_DRAW_NORMAL,
// |                         static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FRAME_FIRST),
// |                         WIDGET_KIND_ICON_DIRECT,
// |                         OVERVIEW_ICON_FILL_COLOR
// |                     );
// |                     if (OVERVIEW_ICON_WIDGET(rowIndex, icons) == NULL) {
// |                         MemError();
// |                     }
// |                     overWin->AddWidget(OVERVIEW_ICON_WIDGET(rowIndex, icons), -1);
// |                     icons++;
// |
// |                     OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                         static_cast<i16>(
// |                             column * DETAIL_COLUMN_STRIDE + HERO_ARTIFACT_ICON_FIRST_X
// |                         ),
// |                         static_cast<i16>(
// |                             detailRow * DETAIL_ROW_STRIDE + rowIndex * OVERVIEW_ROW_HEIGHT
// |                             + HERO_ARTIFACT_ICON_Y_OFFSET
// |                         ),
// |                         DETAIL_ICON_WIDTH,
// |                         DETAIL_ICON_HEIGHT,
// |                         const_cast<char*>("artfx.icn"),
// |                         static_cast<i16>(curHero->m_artifacts[i]),
// ALTERNATIVE literal_names (unified source delta from BEFORE):
// | --- before
// | +++ literal_names
// | @@ -1,5 +1,5 @@
// |              ROW_BACKGROUND_HEIGHT,
// | -            const_cast<char*>("overview.icn"),
// | +            "overview.icn",
// |              static_cast<i16>(IDX(giOverviewType) + ROW_BACKGROUND_FRAME_BASE),
// |              ICON_DRAW_NORMAL,
// |              static_cast<i16>(rowWidgetId + ROW_BACKGROUND_CONTROL),
// | @@ -31,7 +31,7 @@
// |                      TOWN_NAME_WIDTH,
// |                      TOWN_NAME_HEIGHT,
// |                      valueText,
// | -                    const_cast<char*>("smalfont.fnt"),
// | +                    "smalfont.fnt",
// |                      FONT_DRAW_DEFAULT,
// |                      static_cast<i16>(rowWidgetId + TOWN_NAME_CONTROL),
// |                      WIDGET_KIND_TEXT,
// | @@ -51,7 +51,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET),
// |                      TOWN_ICON_WIDTH,
// |                      TOWN_ICON_HEIGHT,
// | -                    const_cast<char*>("minitown.icn"),
// | +                    "minitown.icn",
// |                      townFrame,
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_SELECT_WIDGET),
// | @@ -71,7 +71,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET),
// |                      0,
// |                      0,
// | -                    const_cast<char*>("locators.icn"),
// | +                    "locators.icn",
// |                      TOWN_LOCATOR_FRAME,
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_LOCATOR_CONTROL),
// | @@ -101,7 +101,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET),
// |                      TOWN_HERO_FRAME_WIDTH,
// |                      TOWN_HERO_FRAME_HEIGHT,
// | -                    const_cast<char*>("portxtra.icn"),
// | +                    "portxtra.icn",
// |                      0,
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_HERO_FIRST),
// | @@ -119,7 +119,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET),
// |                      TOWN_HERO_PORTRAIT_WIDTH,
// |                      TOWN_HERO_PORTRAIT_HEIGHT,
// | -                    const_cast<char*>(capt ? "minicapt.icn" : "miniport.icn"),
// | +                    capt ? "minicapt.icn" : "miniport.icn",
// |                      capt ? static_cast<u8>(IDX(record->m_type)) : IDX(heroData->m_portrait),
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_HERO_PORTRAIT_CONTROL),
// | @@ -137,7 +137,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET),
// |                      0,
// |                      0,
// | -                    const_cast<char*>("mobility.icn"),
// | +                    "mobility.icn",
// |                      static_cast<i16>(GetMobilityFrame(capt ? 0 : heroData->m_remainingMobility)),
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_HERO_MOBILITY_CONTROL),
// | @@ -160,7 +160,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET),
// |                      0,
// |                      0,
// | -                    const_cast<char*>("mana.icn"),
// | +                    "mana.icn",
// |                      static_cast<i16>(GetManaFrame(capt ? captainMana : heroData->m_spellPoints)),
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// | @@ -188,7 +188,7 @@
// |                              ),
// |                              TROOP_ICON_WIDTH,
// |                              TROOP_ICON_HEIGHT,
// | -                            const_cast<char*>("mons32.icn"),
// | +                            "mons32.icn",
// |                              static_cast<i16>(record->m_army.m_creatureTypes[i]),
// |                              ICON_DRAW_NORMAL,
// |                              static_cast<i16>(rowWidgetId + i + TOWN_ARMY_FIRST),
// | @@ -217,7 +217,7 @@
// |                              TROOP_TEXT_WIDTH,
// |                              TROOP_TEXT_HEIGHT,
// |                              valueText,
// | -                            const_cast<char*>("smalfont.fnt"),
// | +                            "smalfont.fnt",
// |                              FONT_DRAW_DEFAULT,
// |                              static_cast<i16>(rowWidgetId + i + TOWN_ARMY_ALT_FIRST),
// |                              WIDGET_KIND_TEXT,
// | @@ -284,7 +284,7 @@
// |                              ),
// |                              TROOP_ICON_WIDTH,
// |                              TROOP_ICON_HEIGHT,
// | -                            const_cast<char*>("mons32.icn"),
// | +                            "mons32.icn",
// |                              static_cast<i16>(gDwellingType[IDX(record->m_type)][building]),
// |                              ICON_DRAW_NORMAL,
// |                              static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_FIRST),
// | @@ -309,7 +309,7 @@
// |                              TROOP_TEXT_WIDTH,
// |                              TROOP_TEXT_HEIGHT,
// |                              valueText,
// | -                            const_cast<char*>("smalfont.fnt"),
// | +                            "smalfont.fnt",
// |                              FONT_DRAW_DEFAULT,
// |                              static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_ALT_FIRST),
// |                              WIDGET_KIND_TEXT,
// | @@ -335,7 +335,7 @@
// |                  static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_FRAME_Y_OFFSET),
// |                  HERO_FRAME_WIDTH,
// |                  HERO_FRAME_HEIGHT,
// | -                const_cast<char*>("portxtra.icn"),
// | +                "portxtra.icn",
// |                  0,
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(rowWidgetId + HERO_SELECT_FIRST),
// | @@ -353,7 +353,7 @@
// |                  static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PORTRAIT_Y_OFFSET),
// |                  HERO_PORTRAIT_WIDTH,
// |                  HERO_PORTRAIT_HEIGHT,
// | -                const_cast<char*>("miniport.icn"),
// | +                "miniport.icn",
// |                  static_cast<i16>(curHero->m_portrait),
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(rowWidgetId + HERO_PORTRAIT_CONTROL),
// | @@ -371,7 +371,7 @@
// |                  static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MOBILITY_Y_OFFSET),
// |                  0,
// |                  0,
// | -                const_cast<char*>("mobility.icn"),
// | +                "mobility.icn",
// |                  static_cast<i16>(GetMobilityFrame(curHero->m_remainingMobility)),
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(rowWidgetId + HERO_MOBILITY_CONTROL),
// | @@ -389,7 +389,7 @@
// |                  static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_MANA_Y_OFFSET),
// |                  0,
// |                  0,
// | -                const_cast<char*>("mana.icn"),
// | +                "mana.icn",
// |                  static_cast<i16>(GetManaFrame(curHero->m_spellPoints)),
// |                  ICON_DRAW_NORMAL,
// |                  static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// | @@ -411,7 +411,7 @@
// |                      HERO_PRIMARY_TEXT_WIDTH,
// |                      HERO_PRIMARY_TEXT_HEIGHT,
// |                      valueText,
// | -                    const_cast<char*>("smalfont.fnt"),
// | +                    "smalfont.fnt",
// |                      FONT_DRAW_DEFAULT,
// |                      static_cast<i16>(rowWidgetId + i + HERO_PRIMARY_TEXT_FIRST),
// |                      WIDGET_KIND_TEXT,
// | @@ -427,7 +427,7 @@
// |                      static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + HERO_PRIMARY_ICON_Y_OFFSET),
// |                      HERO_PRIMARY_ICON_WIDTH,
// |                      HERO_PRIMARY_ICON_HEIGHT,
// | -                    const_cast<char*>("overview.icn"),
// | +                    "overview.icn",
// |                      HERO_PRIMARY_ICON_FRAME,
// |                      ICON_DRAW_NORMAL,
// |                      static_cast<i16>(rowWidgetId + i + HERO_STAT_FIRST),
// | @@ -453,7 +453,7 @@
// |                              ),
// |                              TROOP_ICON_WIDTH,
// |                              TROOP_ICON_HEIGHT,
// | -                            const_cast<char*>("mons32.icn"),
// | +                            "mons32.icn",
// |                              static_cast<i16>(curHero->m_army.m_creatureTypes[i]),
// |                              ICON_DRAW_NORMAL,
// |                              static_cast<i16>(rowWidgetId + i + HERO_ARMY_FIRST),
// | @@ -480,7 +480,7 @@
// |                              TROOP_TEXT_WIDTH,
// |                              TROOP_TEXT_HEIGHT,
// |                              valueText,
// | -                            const_cast<char*>("smalfont.fnt"),
// | +                            "smalfont.fnt",
// |                              FONT_DRAW_DEFAULT,
// |                              static_cast<i16>(rowWidgetId + i + HERO_ARMY_ALT_FIRST),
// |                              WIDGET_KIND_TEXT,
// | @@ -506,7 +506,7 @@
// |                          ),
// |                          DETAIL_FRAME_WIDTH,
// |                          DETAIL_FRAME_HEIGHT,
// | -                        const_cast<char*>("overview.icn"),
// | +                        "overview.icn",
// |                          DETAIL_FRAME_ICON,
// |                          ICON_DRAW_NORMAL,
// |                          static_cast<i16>(rowWidgetId + i + HERO_SKILL_FRAME_FIRST),
// | @@ -527,7 +527,7 @@
// |                          ),
// |                          DETAIL_ICON_WIDTH,
// |                          DETAIL_ICON_HEIGHT,
// | -                        const_cast<char*>("miniss.icn"),
// | +                        "miniss.icn",
// |                          static_cast<i16>(skillIndex),
// |                          ICON_DRAW_NORMAL,
// |                          static_cast<i16>(rowWidgetId + i + HERO_SKILL_FIRST),
// | @@ -551,7 +551,7 @@
// |                          HERO_SKILL_LEVEL_WIDTH,
// |                          HERO_SKILL_LEVEL_HEIGHT,
// |                          valueText,
// | -                        const_cast<char*>("smalfont.fnt"),
// | +                        "smalfont.fnt",
// |                          FONT_DRAW_DEFAULT,
// |                          static_cast<i16>(rowWidgetId + i + HERO_SKILL_LEVEL_FIRST),
// |                          WIDGET_KIND_TEXT,
// | @@ -577,7 +577,7 @@
// |                          ),
// |                          DETAIL_FRAME_WIDTH,
// |                          DETAIL_FRAME_HEIGHT,
// | -                        const_cast<char*>("overview.icn"),
// | +                        "overview.icn",
// |                          DETAIL_FRAME_ICON,
// |                          ICON_DRAW_NORMAL,
// |                          static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FRAME_FIRST),
// | @@ -600,5 +600,5 @@
// |                          ),
// |                          DETAIL_ICON_WIDTH,
// |                          DETAIL_ICON_HEIGHT,
// | -                        const_cast<char*>("artfx.icn"),
// | +                        "artfx.icn",
// |                          static_cast<i16>(curHero->m_artifacts[i]),
