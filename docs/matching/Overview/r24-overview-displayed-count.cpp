// VC6 SP5 Overview source family r24-overview-displayed-count; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r24-overview-displayed-count/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.848449s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'dwelling_count', 'candidate': 'baseline'} / 7756 / 33a3076ede902924 / 7c76c4f0a767c674 / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
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
// ALTERNATIVE dwelling_count (unified source delta from BEFORE):
// | --- before
// | +++ dwelling_count
// | @@ -1,5 +1,5 @@
// |              {
// | -                displayedTroops = 0;
// | +                i32 displayedDwellings = 0;
// |                  for (i = 0; i < OVERVIEW_DWELLING_SLOTS; i++) {
// |                      switch (i) {
// |                          case DWELLING_ORDER_LEVEL_1:
// | @@ -45,7 +45,7 @@
// |                          != 0) {
// |                          OVERVIEW_ICON_WIDGET(rowIndex, icons) = new iconWidget(
// |                              static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// | +                                displayedDwellings * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// |                              ),
// |                              static_cast<i16>(
// |                                  rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_ICON_Y_OFFSET
// | @@ -69,7 +69,7 @@
// |                          sprintf(valueText, "%d", static_cast<i32>(record->m_garrison[building]));
// |                          OVERVIEW_TEXT_WIDGET(rowIndex, texts) = new textWidget(
// |                              static_cast<i16>(
// | -                                displayedTroops * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// | +                                displayedDwellings * TOWN_TROOP_COLUMN_STRIDE + TOWN_DWELLING_FIRST_X
// |                              ),
// |                              static_cast<i16>(
// |                                  rowIndex * OVERVIEW_ROW_HEIGHT + TOWN_TROOP_TEXT_Y_OFFSET
// | @@ -85,5 +85,5 @@
// |                          );
// |                          overWin->AddWidget(OVERVIEW_TEXT_WIDGET(rowIndex, texts), -1);
// |                          texts++;
// | -                        displayedTroops++;
// | +                        displayedDwellings++;
// |                      }
