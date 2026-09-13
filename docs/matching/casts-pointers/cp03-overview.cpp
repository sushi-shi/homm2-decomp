// CP03: measured coordinated Overview row-owner recovery, NOT RETAINED.
// Parent: PR59 36ace4cae plus byte-neutral CP01/CP02.
// Full build completed; exact-function count fell from1727 to1725.
// First real divergence: stride imul70 + lea(pointer*4) becomes imul280 + add.
// Overview keeps42 blocks; SetupDynamicStuff's emitted partition235 vs234.
// Private global decoration changes as expected for pointer-to-array ownership;
// instruction differences remain even after resolving those globals by VA.
// Artifacts: build/cp03-build.log and build/cp03-{setup,overview}-after-*.log.
// Only this structural version was measured; no exhaustive state closure claimed.
// Restored with its exact inverse, preserving CP01/CP02; all-native proof rerun.
// Strip the leading // | and trailing | from each patch line below.
// |*** Begin Patch|
// |*** Update File: /tmp/homm2-buka-reconstruction-patterns/include/SOURCE/game.h|
// |@@|
// |      + ((g).m_month - 1) * CALENDAR_DAYS_PER_MONTH)|
// | extern class heroWindow* overWin;|
// | extern char gcCurMapName[GAME_CURRENT_MAP_NAME_SIZE];|
// |-extern class textWidget** textWidgetDynamic;|
// |-extern class iconWidget** iconWidgetDynamic;|
// |+extern OverviewTextWidgetRow* textWidgetDynamic;|
// |+extern OverviewIconWidgetRow* iconWidgetDynamic;|
// | extern OverviewType iLastDynamicType;|
// | extern OverviewType giOverviewType;|
// | extern i32 giOverviewTop[IDX(OVERVIEW_TYPE_COUNT)];|
// |*** Update File: /tmp/homm2-buka-reconstruction-patterns/src/SOURCE/Overview.cpp|
// |@@|
// |     0.4|
// | |
// | #define OVERVIEW_TEXT_WIDGET(row, item)                                                            \|
// |-    (*(textWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))|
// |+    (textWidgetDynamic[(row)][(item)])|
// | #define OVERVIEW_ICON_WIDGET(row, item)                                                            \|
// |-    (*(iconWidgetDynamic + (row) * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + (item)))|
// |+    (iconWidgetDynamic[(row)][(item)])|
// | |
// | VA(0x00479ba0, 0x1e46)|
// | void game::SetupDynamicStuff(i32 redraw, i32 updateKnob, i32 forceUpdate) {|
// |@@|
// | |
// |     for (rowIndex = 0; rowIndex < OVERVIEW_VISIBLE_ROWS; rowIndex++) {|
// |         for (i = 0; i < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; i++) {|
// |-            if (*(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {|
// |+            if (textWidgetDynamic[rowIndex][i] != NULL) {|
// |                 overWin->RemoveWidget(|
// |-                    *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)|
// |+                    textWidgetDynamic[rowIndex][i]|
// |                 );|
// |-                delete *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);|
// |-                *(textWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;|
// |-            }|
// |-            if (*(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) != NULL) {|
// |+                delete textWidgetDynamic[rowIndex][i];|
// |+                textWidgetDynamic[rowIndex][i] = NULL;|
// |+            }|
// |+            if (iconWidgetDynamic[rowIndex][i] != NULL) {|
// |                 overWin->RemoveWidget(|
// |-                    *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i)|
// |+                    iconWidgetDynamic[rowIndex][i]|
// |                 );|
// |-                delete *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i);|
// |-                *(iconWidgetDynamic + rowIndex * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + i) = NULL;|
// |+                delete iconWidgetDynamic[rowIndex][i];|
// |+                iconWidgetDynamic[rowIndex][i] = NULL;|
// |             }|
// |         }|
// |     }|
// |@@|
// |         textWidgetTitle[mine] = NULL;|
// |     }|
// | |
// |-    textWidgetDynamic = static_cast<textWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));|
// |-    iconWidgetDynamic = static_cast<iconWidget**>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));|
// |+    textWidgetDynamic = static_cast<OverviewTextWidgetRow*>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));|
// |+    iconWidgetDynamic = static_cast<OverviewIconWidgetRow*>(H2_ALLOC(DYNAMIC_ARRAY_BYTES));|
// |     for (y = 0; y < OVERVIEW_VISIBLE_ROWS; y++) {|
// |         for (mine = 0; mine < OVERVIEW_DYNAMIC_WIDGETS_PER_ROW; mine++) {|
// |-            *(textWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;|
// |-            *(iconWidgetDynamic + y * OVERVIEW_DYNAMIC_WIDGETS_PER_ROW + mine) = NULL;|
// |+            textWidgetDynamic[y][mine] = NULL;|
// |+            iconWidgetDynamic[y][mine] = NULL;|
// |         }|
// |     }|
// | |
// |@@|
// | }|
// | |
// | DATA(0x005309b0) class heroWindow* overWin = NULL;|
// |-DATA(0x005309b4) class textWidget** textWidgetDynamic = NULL;|
// |-DATA(0x005309b8) class iconWidget** iconWidgetDynamic = NULL;|
// |+DATA(0x005309b4) OverviewTextWidgetRow* textWidgetDynamic = NULL;|
// |+DATA(0x005309b8) OverviewIconWidgetRow* iconWidgetDynamic = NULL;|
// | DATA(0x005309bc) OverviewType giOverviewType = OVERVIEW_HEROES;|
// | DATA(0x005309c0) i32 giOverviewTop[IDX(OVERVIEW_TYPE_COUNT)] = H2_ZERO_INIT;|
// | DATA(0x005309c8) class iconWidget* OVScrollKnob = NULL;|
// |*** End Patch|
