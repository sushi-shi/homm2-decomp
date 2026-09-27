// R08 RandomizeEvents roles, parentd7da2d15c plus all selected C36 changes.
// Complete4 arms in5.167623003944755s; all compile/restore, no truncation.
// Retain shared-role originaltrial0,9926 bytes/textc243e648eb042965.
// Scoped layer index emits9951, scoped town ID9935, both9960 bytes.
// These are real semantic scopes, not name-only stack steering or padding.
// No high-fuzzy score is treated as byte equivalence.
// Artifact: build/source-variant-batch/r08-randomize/results.json;
// build/r08-randomize-axes.json and -manifest.json.
// Final proof and narrower negative interpretation: C36-S37-S38.md.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x513cf",
  "axes": [
    {
      "name": "castle_id",
      "find": "                case MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE):\n                    mineId6 = GetTownId(xPos, yPos);\n                    for (row = yPos - CASTLE_METADATA_TOP_OFFSET;\n                         row <= yPos + CASTLE_METADATA_BOTTOM_OFFSET;\n                         row++) {\n                        for (column3 = xPos - CASTLE_METADATA_X_RADIUS;\n                             column3 <= xPos + CASTLE_METADATA_X_RADIUS;\n                             column3++) {\n                            if (m_worldMap.GetCell(column3, row)->m_objectMetadata == 0)\n                                m_worldMap.GetCell(column3, row)->m_objectMetadata = mineId6;\n                        }\n                    }\n                    townRec = GetCastleSlot(this, mineId6);\n                    townRec->m_boatY = -1;\n                    townRec->m_boatX = -1;\n                    if (yPos <= MAP_HEIGHT - CASTLE_BOAT_Y_OFFSET - 1) {\n                        townEntrance2 = gpAdvManager->GetCell(\n                            xPos - CASTLE_BOAT_X_OFFSET,\n                            yPos + CASTLE_BOAT_Y_OFFSET\n                        );\n                        if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]\n                            == TERRAIN_WATER) {\n                            townRec->m_boatX = xPos - CASTLE_BOAT_X_OFFSET;\n                            townRec->m_boatY = yPos + CASTLE_BOAT_Y_OFFSET;\n                        } else {\n                            townEntrance2 = gpAdvManager->GetCell(\n                                xPos + CASTLE_BOAT_X_OFFSET,\n                                yPos + CASTLE_BOAT_Y_OFFSET\n                            );\n                            if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]\n                                == TERRAIN_WATER) {\n                                townRec->m_boatX =\n                                    xPos + CASTLE_BOAT_X_OFFSET;\n                                townRec->m_boatY =\n                                    yPos + CASTLE_BOAT_Y_OFFSET;\n                            }\n                        }\n                    }\n                    break;\n",
      "options": [
        {
          "name": "shared"
        },
        {
          "name": "scoped_town",
          "replace": "                case MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE): {\n                    i32 townId = GetTownId(xPos, yPos);\n                    for (row = yPos - CASTLE_METADATA_TOP_OFFSET;\n                         row <= yPos + CASTLE_METADATA_BOTTOM_OFFSET;\n                         row++) {\n                        for (column3 = xPos - CASTLE_METADATA_X_RADIUS;\n                             column3 <= xPos + CASTLE_METADATA_X_RADIUS;\n                             column3++) {\n                            if (m_worldMap.GetCell(column3, row)->m_objectMetadata == 0)\n                                m_worldMap.GetCell(column3, row)->m_objectMetadata = townId;\n                        }\n                    }\n                    townRec = GetCastleSlot(this, townId);\n                    townRec->m_boatY = -1;\n                    townRec->m_boatX = -1;\n                    if (yPos <= MAP_HEIGHT - CASTLE_BOAT_Y_OFFSET - 1) {\n                        townEntrance2 = gpAdvManager->GetCell(\n                            xPos - CASTLE_BOAT_X_OFFSET,\n                            yPos + CASTLE_BOAT_Y_OFFSET\n                        );\n                        if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]\n                            == TERRAIN_WATER) {\n                            townRec->m_boatX = xPos - CASTLE_BOAT_X_OFFSET;\n                            townRec->m_boatY = yPos + CASTLE_BOAT_Y_OFFSET;\n                        } else {\n                            townEntrance2 = gpAdvManager->GetCell(\n                                xPos + CASTLE_BOAT_X_OFFSET,\n                                yPos + CASTLE_BOAT_Y_OFFSET\n                            );\n                            if (giGroundToTerrain[townEntrance2->m_terrainImageIndex]\n                                == TERRAIN_WATER) {\n                                townRec->m_boatX =\n                                    xPos + CASTLE_BOAT_X_OFFSET;\n                                townRec->m_boatY =\n                                    yPos + CASTLE_BOAT_Y_OFFSET;\n                            }\n                        }\n                    }\n                    break;\n                }\n"
        }
      ]
    },
    {
      "name": "layer_index",
      "find": "                    for (randomValue5 = 0; randomValue5 < upperCount5; randomValue5++) {\n                        for (j4 = 0; j4 < lowerCount; j4++) {\n                            if (upperTilesets0[randomValue5] == lowerTilesets5[j4]\n                                || (upperTilesets0[randomValue5] >= TILESET_OBJNTOWN\n                                    && upperTilesets0[randomValue5] <= TILESET_OBJNTWRD\n                                    && lowerTilesets5[j4] >= TILESET_OBJNTOWN\n                                    && lowerTilesets5[j4] <= TILESET_OBJNTWRD))\n                                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);\n                        }\n                    }",
      "options": [
        {
          "name": "shared"
        },
        {
          "name": "scoped_layer",
          "replace": "                    i32 layerIndex;\n                    for (layerIndex = 0; layerIndex < upperCount5; layerIndex++) {\n                        for (j4 = 0; j4 < lowerCount; j4++) {\n                            if (upperTilesets0[layerIndex] == lowerTilesets5[j4]\n                                || (upperTilesets0[layerIndex] >= TILESET_OBJNTOWN\n                                    && upperTilesets0[layerIndex] <= TILESET_OBJNTWRD\n                                    && lowerTilesets5[j4] >= TILESET_OBJNTOWN\n                                    && lowerTilesets5[j4] <= TILESET_OBJNTWRD))\n                                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);\n                        }\n                    }"
        }
      ]
    }
  ]
}
*/
