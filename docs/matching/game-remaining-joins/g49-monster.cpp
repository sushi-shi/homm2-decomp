// Parent e6bc8724; 6 complete arms in 8.145001878030598 seconds.
// Selected trial 0: {'source_shape': 'original_inner0', 'candidate': 'baseline'}; size/text/label-sensitive relocation signature (1780, 'db2c890f8d3078c2', '417bed1c3e0c1ed1').
// Source restored; all arms compiled; no truncation, AST mutation or TU-state census.
// Selected combined source passes all 98 native objects and full focused retail checks.
// Monster cast signatures contain counter-named local labels: only the selected arm has full resolved-target proof.
// Artifacts: build/g49-monster-manifest.json; build/source-variant-batch/g49-monster/results.json.
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

Option original_inner0:

Option original_inner1:
--- baseline
+++ original_inner1
@@ -64,22 +64,21 @@
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                                 minValue = 0;
                                 maxValue = 400;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                                 minValue = 400;
                                 maxValue = 1000;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                                 minValue = 1000;
                                 maxValue = 2500;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                                 minValue = 2500;
                                 maxValue = 100000;
-                                goto monsterBoundsReady;
+                                break;
                         }
                     }
-                monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
Option last_fallthrough_inner0:
--- baseline
+++ last_fallthrough_inner0
@@ -52,7 +52,6 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                     minValue = 2500;
                     maxValue = 100000;
-                    goto randomMonster;
                 randomMonster:
                     if (cell->m_objectTileset == TILESET_MONS32
                         && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
Option last_fallthrough_inner1:
--- baseline
+++ last_fallthrough_inner1
@@ -52,7 +52,6 @@
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                     minValue = 2500;
                     maxValue = 100000;
-                    goto randomMonster;
                 randomMonster:
                     if (cell->m_objectTileset == TILESET_MONS32
                         && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
@@ -64,22 +63,21 @@
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                                 minValue = 0;
                                 maxValue = 400;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                                 minValue = 400;
                                 maxValue = 1000;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                                 minValue = 1000;
                                 maxValue = 2500;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                                 minValue = 2500;
                                 maxValue = 100000;
-                                goto monsterBoundsReady;
+                                break;
                         }
                     }
-                monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
Option grouped_switch_inner0:
--- baseline
+++ grouped_switch_inner0
@@ -34,26 +34,32 @@
                     RandomizeTown(x, y, 1);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
-                    minValue = 80;
-                    maxValue = 2000;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
-                    minValue = 0;
-                    maxValue = 400;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
-                    minValue = 400;
-                    maxValue = 1000;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
-                    minValue = 1000;
-                    maxValue = 2500;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
-                    minValue = 2500;
-                    maxValue = 100000;
-                    goto randomMonster;
-                randomMonster:
+                    switch (cell->m_triggerType) {
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
+                            minValue = 80;
+                            maxValue = 2000;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
+                            minValue = 0;
+                            maxValue = 400;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
+                            minValue = 400;
+                            maxValue = 1000;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
+                            minValue = 1000;
+                            maxValue = 2500;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
+                            minValue = 2500;
+                            maxValue = 100000;
+                            break;
+                    }
                     if (cell->m_objectTileset == TILESET_MONS32
                         && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                         && cell->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
Option grouped_switch_inner1:
--- baseline
+++ grouped_switch_inner1
@@ -34,26 +34,32 @@
                     RandomizeTown(x, y, 1);
                     break;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
-                    minValue = 80;
-                    maxValue = 2000;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
-                    minValue = 0;
-                    maxValue = 400;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
-                    minValue = 400;
-                    maxValue = 1000;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
-                    minValue = 1000;
-                    maxValue = 2500;
-                    goto randomMonster;
                 case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
-                    minValue = 2500;
-                    maxValue = 100000;
-                    goto randomMonster;
-                randomMonster:
+                    switch (cell->m_triggerType) {
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
+                            minValue = 80;
+                            maxValue = 2000;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
+                            minValue = 0;
+                            maxValue = 400;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
+                            minValue = 400;
+                            maxValue = 1000;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
+                            minValue = 1000;
+                            maxValue = 2500;
+                            break;
+                        case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
+                            minValue = 2500;
+                            maxValue = 100000;
+                            break;
+                    }
                     if (cell->m_objectTileset == TILESET_MONS32
                         && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                         && cell->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
@@ -64,22 +70,21 @@
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                                 minValue = 0;
                                 maxValue = 400;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                                 minValue = 400;
                                 maxValue = 1000;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                                 minValue = 1000;
                                 maxValue = 2500;
-                                goto monsterBoundsReady;
+                                break;
                             case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                                 minValue = 2500;
                                 maxValue = 100000;
-                                goto monsterBoundsReady;
+                                break;
                         }
                     }
-                monsterBoundsReady:
                     cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
                     cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                     while (gMonsterDatabase[cell->m_objectIndex].fightValue <= minValue
*/
