// VC6 SP5 Overview source family r24-overview-dynamic-roles; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r24-overview-dynamic-roles/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.732169s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'dwelling_skill_artifact', 'candidate': 'baseline'} / 7771 / a1c5f4059887c1b0 / c4dac0b35b01445f / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
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
// |                         ICON_DRAW_NORMAL,
// |                         static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FIRST),
// |                         WIDGET_KIND_ICON_DIRECT,
// ALTERNATIVE dwelling_skill_artifact (unified source delta from BEFORE):
// | --- before
// | +++ dwelling_skill_artifact
// | @@ -1,6 +1,6 @@
// |                  displayedTroops = 0;
// | -                for (i = 0; i < OVERVIEW_DWELLING_SLOTS; i++) {
// | -                    switch (i) {
// | +                for (i32 dwellingOrder = 0; dwellingOrder < OVERVIEW_DWELLING_SLOTS; dwellingOrder++) {
// | +                    switch (dwellingOrder) {
// |                          case DWELLING_ORDER_LEVEL_1:
// |                              building = DWELLING_INDEX_LEVEL_1;
// |                              break;
// | @@ -260,11 +260,11 @@
// |                  }
// |              }
// |
// | -            for (i = 0; i < OVERVIEW_SECONDARY_SKILL_SLOTS; i++) {
// | -                skillIndex = curHero->GetNthSS(i);
// | +            for (i32 skillSlot = 0; skillSlot < OVERVIEW_SECONDARY_SKILL_SLOTS; skillSlot++) {
// | +                skillIndex = curHero->GetNthSS(skillSlot);
// |                  if (skillIndex != HERO_SKILL_NONE) {
// | -                    detailRow = i / OVERVIEW_SECONDARY_SKILL_COLUMNS;
// | -                    column = i % OVERVIEW_SECONDARY_SKILL_COLUMNS;
// | +                    detailRow = skillSlot / OVERVIEW_SECONDARY_SKILL_COLUMNS;
// | +                    column = skillSlot % OVERVIEW_SECONDARY_SKILL_COLUMNS;
// |                      OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                          static_cast<i16>(column * DETAIL_COLUMN_STRIDE + HERO_SKILL_FRAME_FIRST_X),
// |                          static_cast<i16>(
// | @@ -276,7 +276,7 @@
// |                          const_cast<char*>("overview.icn"),
// |                          DETAIL_FRAME_ICON,
// |                          ICON_DRAW_NORMAL,
// | -                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_FRAME_FIRST),
// | +                        static_cast<i16>(rowWidgetId + skillSlot + HERO_SKILL_FRAME_FIRST),
// |                          WIDGET_KIND_ICON_DIRECT,
// |                          OVERVIEW_ICON_FILL_COLOR
// |                      );
// | @@ -297,7 +297,7 @@
// |                          const_cast<char*>("miniss.icn"),
// |                          static_cast<i16>(skillIndex),
// |                          ICON_DRAW_NORMAL,
// | -                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_FIRST),
// | +                        static_cast<i16>(rowWidgetId + skillSlot + HERO_SKILL_FIRST),
// |                          WIDGET_KIND_ICON_DIRECT,
// |                          OVERVIEW_ICON_FILL_COLOR
// |                      );
// | @@ -320,7 +320,7 @@
// |                          valueText,
// |                          const_cast<char*>("smalfont.fnt"),
// |                          FONT_DRAW_DEFAULT,
// | -                        static_cast<i16>(rowWidgetId + i + HERO_SKILL_LEVEL_FIRST),
// | +                        static_cast<i16>(rowWidgetId + skillSlot + HERO_SKILL_LEVEL_FIRST),
// |                          WIDGET_KIND_TEXT,
// |                          FONT_ALIGN_RIGHT
// |                      );
// | @@ -330,8 +330,8 @@
// |              }
// |
// |              i32 displayedArtifacts = 0;
// | -            for (i = 0; i < OVERVIEW_ARTIFACT_SLOTS; i++) {
// | -                if (curHero->m_artifacts[i] != ARTIFACT_NONE) {
// | +            for (i32 artifactSlot = 0; artifactSlot < OVERVIEW_ARTIFACT_SLOTS; artifactSlot++) {
// | +                if (curHero->m_artifacts[artifactSlot] != ARTIFACT_NONE) {
// |                      detailRow = displayedArtifacts / OVERVIEW_ARTIFACT_COLUMNS;
// |                      column = displayedArtifacts % OVERVIEW_ARTIFACT_COLUMNS;
// |                      OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// | @@ -347,7 +347,7 @@
// |                          const_cast<char*>("overview.icn"),
// |                          DETAIL_FRAME_ICON,
// |                          ICON_DRAW_NORMAL,
// | -                        static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FRAME_FIRST),
// | +                        static_cast<i16>(rowWidgetId + artifactSlot + HERO_ARTIFACT_FRAME_FIRST),
// |                          WIDGET_KIND_ICON_DIRECT,
// |                          OVERVIEW_ICON_FILL_COLOR
// |                      );
// | @@ -368,7 +368,7 @@
// |                          DETAIL_ICON_WIDTH,
// |                          DETAIL_ICON_HEIGHT,
// |                          const_cast<char*>("artfx.icn"),
// | -                        static_cast<i16>(curHero->m_artifacts[i]),
// | +                        static_cast<i16>(curHero->m_artifacts[artifactSlot]),
// |                          ICON_DRAW_NORMAL,
// | -                        static_cast<i16>(rowWidgetId + i + HERO_ARTIFACT_FIRST),
// | +                        static_cast<i16>(rowWidgetId + artifactSlot + HERO_ARTIFACT_FIRST),
// |                          WIDGET_KIND_ICON_DIRECT,
