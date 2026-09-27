// Parent e6bc8724; 4 complete arms in 5.797065026126802 seconds.
// Selected trial 1: {'source_shape': 'shared0_count1', 'candidate': 'baseline'}; size/text/label-sensitive relocation signature (1613, '00278bb9f3e2b610', '347bead471cac712').
// Source restored; all arms compiled; no truncation, AST mutation or TU-state census.
// Selected combined source passes all 98 native objects and full focused retail checks.
// Monster cast signatures contain counter-named local labels: only the selected arm has full resolved-target proof.
// Artifacts: build/g54-c39-rumour-manifest.json; build/source-variant-batch/g54-c39-rumour/results.json.
// See docs/reconstruction/C34-C39-G49-G51-G54.md for results and disposition.
/* Baseline:
void game::SetupNewRumour(void) {
    i32 roll;
    i32 attempts8;
    rumourEventExtra* event0;
    i32 eventIndex;
    i32 selectionRoll7;
    i32l categoryStats[GAME_PLAYER_COUNT];
    i32 direction;
    i8 categoryOrder[RUMOUR_CATEGORY_ORDER_CAPACITY];
    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
        attempts8 = 0;
        while (attempts8++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex = Random(0, m_rumourEventCount - 1);
            else
                eventIndex = 0;
            event0 =
                reinterpret_cast<rumourEventExtra*>(ppMapExtra[m_rumourEventIndices[eventIndex]]);
            if (strlen(event0->text) > 2 && event0->text[0] != '@') {
                strcpy(m_rumour, event0->text);
                event0->text[0] = '@';
                return;
            }
        }
    }

    if (Random(0, 100) < 30) {
        strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
    } else {
        roll = Random(0, 100);
        if (roll < 80 && giCurTurn > 1) {
            attempts8 = 0;
            while (attempts8++ < 200) {
                selectionRoll7 = Random(
                    IDX(THIEVES_CATEGORY_OBELISKS), IDX(THIEVES_CATEGORY_INCOME)
                );
                GetCategoryStats(
                    static_cast<TownThievesGuildCategory>(selectionRoll7),
                    categoryStats,
                    categoryOrder
                );
                SortStats(categoryStats, categoryOrder);
                if (categoryStats[0] != categoryStats[1]) {
                    if (selectionRoll7 == IDX(THIEVES_CATEGORY_OBELISKS))
                        sprintf(
                            m_rumour,
                            "%s \xed\xe0\xf8\xe5\xeb \xe1\xee\xeb\xfc\xf8\xe5 \xe2\xf1\xe5\xf5 \xee\xe1\xe5\xeb\xe8\xf1\xea\xee\xe2.",
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll7 == IDX(THIEVES_CATEGORY_ARTIFACTS))
                        sprintf(
                            m_rumour,
                            "%s \xed\xe0\xf8\xe5\xeb \xe1\xee\xeb\xfc\xf8\xe5 \xe2\xf1\xe5\xf5 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xee\xe2.",
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll7 == IDX(THIEVES_CATEGORY_ARMY_STRENGTH))
                        sprintf(
                            m_rumour,
                            "%s \xee\xe1\xeb\xe0\xe4\xe0\xe5\xf2 \xf1\xe0\xec\xfb\xec\xe8 \xf1\xe8\xeb\xfc\xed\xfb\xec\xe8 \xe2\xee\xe9\xf1\xea\xe0\xec\xe8.",
                            cPlayerNames[categoryOrder[0]]
                        );
                    else
                        sprintf(
                            m_rumour,
                            "%s \xe7\xe0\xf0\xe0\xe1\xe0\xf2\xfb\xe2\xe0\xe5\xf2 \xe1\xee\xeb\xfc\xf8\xe5 \xe2\xf1\xe5\xf5 \xe4\xe5\xed\xe5\xe3.",
                            cPlayerNames[categoryOrder[0]]
                        );
                    return;
                }
            }
            goto ultimateRumour;
        } else {
        ultimateRumour:
            selectionRoll7 = Random(0, 100);
            if (selectionRoll7 < 33) {
                if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
                      || m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 7;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 5;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33)) {
                    direction = 6;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 1;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 3;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 2;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 0;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.height)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 4;
                } else {
                    direction = 8;
                }
                sprintf(
                    m_rumour,
                    "\xcc\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2 \xec\xee\xe6\xe5\xf2 "
                    "\xe1\xfb\xf2\xfc \xed\xe0\xe9\xe4\xe5\xed \xe2 %s \xf7\xe0\xf1\xf2\xe8 \xec\xe8\xf0\xe0.",
                    cDirections[direction]
                );
            } else if (selectionRoll7 < 66) {
                sprintf(
                    m_rumour,
                    "%s, \xf2\xee \xec\xe5\xf1\xf2\xee \xe3\xe4\xe5 \xec\xee\xe6\xe5\xf2 \xe1\xfb\xf2\xfc "
                    "\xed\xe0\xe9\xe4\xe5\xed \xec\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2.",
                    cRumourTerrainDescriptions
                        [IDX(giGroundToTerrain
                                 [gpAdvManager
                                      ->GetCell(m_ultimateArtifactX, m_ultimateArtifactY)
                                      ->m_terrainImageIndex])]
                );
            } else if (m_ultimateArtifactId != ARTIFACT_NONE) {
                sprintf(
                    m_rumour,
                    "\xce\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xee, \xec\xee\xe3\xf3\xf9\xe5\xf1\xf2\xe2\xe5\xed\xed\xfb\xe9 "
                    "\xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2 \xfd\xf2\xee %s.",
                    gArtifactNames[IDX(m_ultimateArtifactId)]
                );
            } else {
                strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
            }
        }
    }
}

Option shared0_count0:

Option shared0_count1:
--- baseline
+++ shared0_count1
@@ -7,7 +7,7 @@
     i32l categoryStats[GAME_PLAYER_COUNT];
     i32 direction;
     i8 categoryOrder[RUMOUR_CATEGORY_ORDER_CAPACITY];
-    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
+    if (m_rumourEventCount != 0 && Random(0, 9) < m_rumourEventCount) {
         attempts8 = 0;
         while (attempts8++ < 200) {
             if (m_rumourEventCount > 1)
Option shared1_count0:
--- baseline
+++ shared1_count0
@@ -68,9 +68,8 @@
                     return;
                 }
             }
-            goto ultimateRumour;
-        } else {
-        ultimateRumour:
+        }
+        {
             selectionRoll7 = Random(0, 100);
             if (selectionRoll7 < 33) {
                 if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
Option shared1_count1:
--- baseline
+++ shared1_count1
@@ -7,7 +7,7 @@
     i32l categoryStats[GAME_PLAYER_COUNT];
     i32 direction;
     i8 categoryOrder[RUMOUR_CATEGORY_ORDER_CAPACITY];
-    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
+    if (m_rumourEventCount != 0 && Random(0, 9) < m_rumourEventCount) {
         attempts8 = 0;
         while (attempts8++ < 200) {
             if (m_rumourEventCount > 1)
@@ -68,9 +68,8 @@
                     return;
                 }
             }
-            goto ultimateRumour;
-        } else {
-        ultimateRumour:
+        }
+        {
             selectionRoll7 = Random(0, 100);
             if (selectionRoll7 < 33) {
                 if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
*/
