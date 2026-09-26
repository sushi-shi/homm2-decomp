// Parent e6bc8724; 32 complete arms in 37.721738952212036 seconds.
// Selected trial 31: {'source_shape': 'casts31', 'candidate': 'baseline'}; size/text/label-sensitive relocation signature (1780, 'db2c890f8d3078c2', '7dc94a9ef24fdcbb').
// Source restored; all arms compiled; no truncation, AST mutation or TU-state census.
// Selected combined source passes all 98 native objects and full focused retail checks.
// Monster cast signatures contain counter-named local labels: only the selected arm has full resolved-target proof.
// Artifacts: build/c34-monster-manifest.json; build/source-variant-batch/c34-monster/results.json.
// See docs/reconstruction/C34-C39-G49-G51-G54.md for results and disposition.
/* Baseline:
void game::ProcessRandomObjects(void) {
    i32 artifactId;
    i32 minValue;
    i32 mineIndex2;
    i32 x;
    i32 y;
    i32 maxValue;
    mapCell* cell;
    H2_OPEN_CODE_PARAM(MapTriggerCode, i32) randomObjectType8;
    i32 randomType8;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex2 = 0; mineIndex2 < RANDOM_MINE_RESOURCE_COUNT; mineIndex2++)
        RandMineQty[mineIndex2] = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = WORLDMAP->GetCell(x, y);
            switch (cell->m_triggerType) {
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                    giUABaseX = static_cast<i16>(x);
                    giUABaseY = static_cast<i16>(y);
                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
                    cell->m_triggerType = MAP_OBJECT_NONE;
                    cell->m_objectTileset = TILESET_NONE;
                    cell->m_objectIndex = -1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN):
                    RandomizeTown(x, y, 0);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE):
                    RandomizeTown(x, y, 1);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
                    minValue = 80;
                    maxValue = 2000;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                    minValue = 0;
                    maxValue = 400;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                    minValue = 400;
                    maxValue = 1000;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                    minValue = 1000;
                    maxValue = 2500;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                    minValue = 2500;
                    maxValue = 100000;
                    goto randomMonster;
                randomMonster:
                    if (cell->m_objectTileset == TILESET_MONS32
                        && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                        && cell->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
                        randomObjectType8 = MapTriggerCode(
                            cell->m_objectIndex + RANDOM_MONSTER_SPRITE_TO_TRIGGER
                        );
                        switch (randomObjectType8) {
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                                minValue = 0;
                                maxValue = 400;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                                minValue = 400;
                                maxValue = 1000;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                                minValue = 1000;
                                maxValue = 2500;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                                minValue = 2500;
                                maxValue = 100000;
                                goto monsterBoundsReady;
                        }
                    }
                monsterBoundsReady:
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                    while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                           || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
                    randomType8 = Random(0, 6);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNRSRC,
                        16,
                        16,
                        TILESET_OBJNRSRC,
                        randomType8 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNRSRC,
                        17,
                        17,
                        TILESET_OBJNRSRC,
                        randomType8 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    switch (randomType8) {
                        case 0:
                        case 2:
                            cell->m_objectMetadata = Random(8, 16);
                            break;
                        case 6:
                            cell->m_objectMetadata = Random(5, 10);
                            break;
                        default:
                            cell->m_objectMetadata = Random(3, 7);
                            break;
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        162,
                        162,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        163,
                        163,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TREASURE_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        166,
                        166,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        167,
                        167,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MINOR_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        168,
                        168,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        169,
                        169,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MAJOR_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNARTI,
                        170,
                        170,
                        TILESET_OBJNARTI,
                        artifactId * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_OBJNARTI,
                        171,
                        171,
                        TILESET_OBJNARTI,
                        artifactId * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EYE_OF_MAGI):
                    RandomizeMine(x, y);
                    break;
            }
        }
    }
}

Option casts0:

Option casts1:
--- baseline
+++ casts1
@@ -20,7 +20,7 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
Option casts2:
--- baseline
+++ casts2
@@ -21,7 +21,7 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
Option casts3:
--- baseline
+++ casts3
@@ -20,8 +20,8 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseX = x;
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
Option casts4:
--- baseline
+++ casts4
@@ -22,7 +22,7 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
Option casts5:
--- baseline
+++ casts5
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
Option casts6:
--- baseline
+++ casts6
@@ -21,8 +21,8 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
Option casts7:
--- baseline
+++ casts7
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseX = x;
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
Option casts8:
--- baseline
+++ casts8
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts9:
--- baseline
+++ casts9
@@ -20,7 +20,7 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts10:
--- baseline
+++ casts10
@@ -21,7 +21,7 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts11:
--- baseline
+++ casts11
@@ -20,8 +20,8 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseX = x;
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts12:
--- baseline
+++ casts12
@@ -22,7 +22,7 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts13:
--- baseline
+++ casts13
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts14:
--- baseline
+++ casts14
@@ -21,8 +21,8 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts15:
--- baseline
+++ casts15
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseX = x;
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,7 +81,7 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
                         cell->m_objectIndex = static_cast<u8>(Random(0, 65));
Option casts16:
--- baseline
+++ casts16
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts17:
--- baseline
+++ casts17
@@ -20,7 +20,7 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts18:
--- baseline
+++ casts18
@@ -21,7 +21,7 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts19:
--- baseline
+++ casts19
@@ -20,8 +20,8 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseX = x;
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts20:
--- baseline
+++ casts20
@@ -22,7 +22,7 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts21:
--- baseline
+++ casts21
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts22:
--- baseline
+++ casts22
@@ -21,8 +21,8 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts23:
--- baseline
+++ casts23
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseX = x;
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -84,7 +84,7 @@
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts24:
--- baseline
+++ casts24
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts25:
--- baseline
+++ casts25
@@ -20,7 +20,7 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts26:
--- baseline
+++ casts26
@@ -21,7 +21,7 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts27:
--- baseline
+++ casts27
@@ -20,8 +20,8 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
+                    giUABaseX = x;
+                    giUABaseY = y;
                     giUARadius = static_cast<i16>(cell->m_objectMetadata);
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts28:
--- baseline
+++ casts28
@@ -22,7 +22,7 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts29:
--- baseline
+++ casts29
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
+                    giUABaseX = x;
                     giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts30:
--- baseline
+++ casts30
@@ -21,8 +21,8 @@
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                     giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
Option casts31:
--- baseline
+++ casts31
@@ -20,9 +20,9 @@
             cell = WORLDMAP->GetCell(x, y);
             switch (cell->m_triggerType) {
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
-                    giUABaseX = static_cast<i16>(x);
-                    giUABaseY = static_cast<i16>(y);
-                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
+                    giUABaseX = x;
+                    giUABaseY = y;
+                    giUARadius = cell->m_objectMetadata;
                     cell->m_triggerType = MAP_OBJECT_NONE;
                     cell->m_objectTileset = TILESET_NONE;
                     cell->m_objectIndex = -1;
@@ -81,10 +81,10 @@
                     }
                 monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
-                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                    cell->m_objectIndex = Random(0, 65);
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
                            || gMonsterDatabase[cell->m_objectIndex].fightValue >= maxValue)
-                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
+                        cell->m_objectIndex = Random(0, 65);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
*/
