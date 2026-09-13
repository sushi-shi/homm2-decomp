// C36/S37/S38: parent clean d7da2d15c unless noted.
// Complete4 arms5.166312526911497s on parentwithall13 selected C36 cast removals and LoadMap owners. Retainoriginaltrial0:9926 bytes/textc243e648eb042965/relocs8e4d67c9174bf54e. Deleting lower, upper, both arrays+linkedstores emits9881,9884,9839bytes respectively. Real5-entry tileset arrays,10-entry guardianchoices and all count increments remain unchanged.
// All arms compiled; source restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Full bytes/size/ordered targets and all98 native objects: C36-S37-S38.md.
// Artifacts: build/source-variant-batch/s38-randomize/results.json,
// build/s38-randomize-axes.json and -manifest.json.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x513cf",
  "axes": [
    {
      "name": "upperIndexes8",
      "find": "    i32 upperIndexes8[LAYER_SCAN_CAPACITY];\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "                        upperIndexes8[upperCount5] = cell2->m_objectIndex;\n",
              "replace": ""
            },
            {
              "find": "                            upperIndexes8[upperCount5] = extra9->objectIndex;\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "lowerIndexes28",
      "find": "    i32 lowerIndexes28[LAYER_SCAN_CAPACITY];\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "                        lowerIndexes28[lowerCount] = below0->m_objectIndex;\n",
              "replace": ""
            },
            {
              "find": "                            lowerIndexes28[lowerCount] = extra9->objectIndex;\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
