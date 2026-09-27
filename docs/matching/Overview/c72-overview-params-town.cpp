// VC6 SP5 Overview source family c72-overview-params-town; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-params-town/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.925186s; input restored.
// Disposition: retain parameter removals EXCEPT creature/dwelling type-to-frame casts.
// Those two casts cross strong enum domains in C++20; retain the original i16 casts.
// Final partial retention passes both dialect checks and combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'implicit_parameters', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 30c151fe4e5e7176 / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
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
// ALTERNATIVE implicit_parameters (unified source delta from BEFORE):
// | --- before
// | +++ implicit_parameters
// | @@ -1,11 +1,11 @@
// |                      TOWN_NAME_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_NAME_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_NAME_Y_OFFSET,
// |                      TOWN_NAME_WIDTH,
// |                      TOWN_NAME_HEIGHT,
// |                      valueText,
// |                      const_cast<char*>("smalfont.fnt"),
// |                      FONT_DRAW_DEFAULT,
// | -                    static_cast<i16>(rowWidgetId + TOWN_NAME_CONTROL),
// | +                    rowWidgetId + TOWN_NAME_CONTROL,
// |                      WIDGET_KIND_TEXT,
// |                      FONT_ALIGN_CENTER
// |                  );
// | @@ -20,13 +20,13 @@
// |                  }
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_ICON_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_ICON_Y_OFFSET,
// |                      TOWN_ICON_WIDTH,
// |                      TOWN_ICON_HEIGHT,
// |                      const_cast<char*>("minitown.icn"),
// |                      townFrame,
// |                      ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_SELECT_WIDGET),
// | +                    rowWidgetId + TOWN_SELECT_WIDGET,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -40,13 +40,13 @@
// |              if (BitTest(gpGame->m_knownTowns, static_cast<u32>(record->m_id))) {
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_LOCATOR_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_LOCATOR_Y_OFFSET,
// |                      0,
// |                      0,
// |                      const_cast<char*>("locators.icn"),
// |                      TOWN_LOCATOR_FRAME,
// |                      ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_LOCATOR_CONTROL),
// | +                    rowWidgetId + TOWN_LOCATOR_CONTROL,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -70,13 +70,13 @@
// |              if (heroData != NULL || capt) {
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_HERO_FRAME_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_FRAME_Y_OFFSET,
// |                      TOWN_HERO_FRAME_WIDTH,
// |                      TOWN_HERO_FRAME_HEIGHT,
// |                      const_cast<char*>("portxtra.icn"),
// |                      0,
// |                      ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_HERO_FIRST),
// | +                    rowWidgetId + TOWN_HERO_FIRST,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -88,13 +88,13 @@
// |
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_HERO_PORTRAIT_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_HERO_PORTRAIT_Y_OFFSET,
// |                      TOWN_HERO_PORTRAIT_WIDTH,
// |                      TOWN_HERO_PORTRAIT_HEIGHT,
// |                      const_cast<char*>(capt ? "minicapt.icn" : "miniport.icn"),
// |                      capt ? static_cast<u8>(IDX(record->m_type)) : IDX(heroData->m_portrait),
// |                      ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_HERO_PORTRAIT_CONTROL),
// | +                    rowWidgetId + TOWN_HERO_PORTRAIT_CONTROL,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -106,13 +106,13 @@
// |
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_MOBILITY_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MOBILITY_Y_OFFSET,
// |                      0,
// |                      0,
// |                      const_cast<char*>("mobility.icn"),
// | -                    static_cast<i16>(GetMobilityFrame(capt ? 0 : heroData->m_remainingMobility)),
// | -                    ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_HERO_MOBILITY_CONTROL),
// | +                    GetMobilityFrame(capt ? 0 : heroData->m_remainingMobility),
// | +                    ICON_DRAW_NORMAL,
// | +                    rowWidgetId + TOWN_HERO_MOBILITY_CONTROL,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -129,13 +129,13 @@
// |                  }
// |                  OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                      TOWN_MANA_X,
// | -                    static_cast<i16>(rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET),
// | +                    rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_MANA_Y_OFFSET,
// |                      0,
// |                      0,
// |                      const_cast<char*>("mana.icn"),
// | -                    static_cast<i16>(GetManaFrame(capt ? captainMana : heroData->m_spellPoints)),
// | -                    ICON_DRAW_NORMAL,
// | -                    static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// | +                    GetManaFrame(capt ? captainMana : heroData->m_spellPoints),
// | +                    ICON_DRAW_NORMAL,
// | +                    rowWidgetId + TOWN_HERO_MANA_CONTROL,
// |                      WIDGET_KIND_ICON_DIRECT,
// |                      OVERVIEW_ICON_FILL_COLOR
// |                  );
// | @@ -152,18 +152,14 @@
// |                      if (record->m_army.m_creatureTypes[i] != CREATURE_NONE
// |                          && record->m_army.m_creatureCounts[i] > 0) {
// |                          OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// | -                            static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
// | -                            ),
// | -                            static_cast<i16>(
// | -                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
// | -                            ),
// | +                            displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X,
// | +                            rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET,
// |                              TROOP_ICON_WIDTH,
// |                              TROOP_ICON_HEIGHT,
// |                              const_cast<char*>("mons32.icn"),
// | -                            static_cast<i16>(record->m_army.m_creatureTypes[i]),
// | +                            record->m_army.m_creatureTypes[i],
// |                              ICON_DRAW_NORMAL,
// | -                            static_cast<i16>(rowWidgetId + i + TOWN_ARMY_FIRST),
// | +                            rowWidgetId + i + TOWN_ARMY_FIRST,
// |                              WIDGET_KIND_ICON_CENTERED,
// |                              OVERVIEW_ICON_FILL_COLOR
// |                          );
// | @@ -180,18 +176,14 @@
// |                              static_cast<i32>(record->m_army.m_creatureCounts[i])
// |                          );
// |                          OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// | -                            static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X
// | -                            ),
// | -                            static_cast<i16>(
// | -                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
// | -                            ),
// | +                            displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_TROOP_FIRST_X,
// | +                            rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET,
// |                              TROOP_TEXT_WIDTH,
// |                              TROOP_TEXT_HEIGHT,
// |                              valueText,
// |                              const_cast<char*>("smalfont.fnt"),
// |                              FONT_DRAW_DEFAULT,
// | -                            static_cast<i16>(rowWidgetId + i + TOWN_ARMY_ALT_FIRST),
// | +                            rowWidgetId + i + TOWN_ARMY_ALT_FIRST,
// |                              WIDGET_KIND_TEXT,
// |                              FONT_ALIGN_CENTER
// |                          );
// | @@ -248,18 +240,14 @@
// |                           & (1 << (IDX(building) + IDX(TOWN_DWELLING_BUILDING_BIT_BASE))))
// |                          != 0) {
// |                          OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// | -                            static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// | -                            ),
// | -                            static_cast<i16>(
// | -                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
// | -                            ),
// | +                            displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X,
// | +                            rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET,
// |                              TROOP_ICON_WIDTH,
// |                              TROOP_ICON_HEIGHT,
// |                              const_cast<char*>("mons32.icn"),
// | -                            static_cast<i16>(gDwellingType[IDX(record->m_type)][building]),
// | +                            gDwellingType[IDX(record->m_type)][building],
// |                              ICON_DRAW_NORMAL,
// | -                            static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_FIRST),
// | +                            rowWidgetId + building + TOWN_RECRUIT_FIRST,
// |                              WIDGET_KIND_ICON_CENTERED,
// |                              OVERVIEW_ICON_FILL_COLOR
// |                          );
// | @@ -272,18 +260,14 @@
// |                          valueText = static_cast<char*>(H2_ALLOC(OVERVIEW_TROOP_TEXT_CAPACITY));
// |                          sprintf(valueText, "%d", static_cast<i32>(record->m_garrison[building]));
// |                          OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// | -                            static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// | -                            ),
// | -                            static_cast<i16>(
// | -                                rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
// | -                            ),
// | +                            displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X,
// | +                            rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET,
// |                              TROOP_TEXT_WIDTH,
// |                              TROOP_TEXT_HEIGHT,
// |                              valueText,
// |                              const_cast<char*>("smalfont.fnt"),
// |                              FONT_DRAW_DEFAULT,
// | -                            static_cast<i16>(rowWidgetId + building + TOWN_RECRUIT_ALT_FIRST),
// | +                            rowWidgetId + building + TOWN_RECRUIT_ALT_FIRST,
// |                              WIDGET_KIND_TEXT,
// |                              FONT_ALIGN_CENTER
// |                          );
// | @@ -364,5 +348,5 @@
// |                  const_cast<char*>("mana.icn"),
// |                  static_cast<i16>(GetManaFrame(curHero->m_spellPoints)),
// |                  ICON_DRAW_NORMAL,
// | -                static_cast<i16>(rowWidgetId + TOWN_HERO_MANA_CONTROL),
// | +                rowWidgetId + TOWN_HERO_MANA_CONTROL,
// |                  WIDGET_KIND_ICON_DIRECT,
