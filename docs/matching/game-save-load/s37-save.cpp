// S37 SaveGame storage; parent4fd5bdbcb plus four C34 save/load cast removals.
// Complete64 arms in67.81681750435382 seconds, compiled/restored/no truncation.
// Only trials0,8,16,24 preserve text042334ab331accb4 / relocc16d188278d43bc0.
// Select trial24: human array8->6 and current-player array4->1.
// Every unused-declaration removal or workBuf100->40 changes bytes in this parent.
// lastTag's deletion is linked to deletion of its unread initialization.
// WorkBuf40 is also semantically conditional: filename output is not bounded14.
// Keep workBuf100 and all three unread declarations; no invented frame padding.
// Artifacts: build/source-variant-batch/s37-save/results.json,
// build/s37-save-manifest.json and build/s37-save-axes.json.
// All source shapes/retail disposition are detailed in C34-save-load-S37.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4d3ae",
  "axes": [
    {
      "name": "work",
      "find": "    char workBuf[SAVE_LEGACY_SCRATCH_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "    char workBuf[SAVE_LEGACY_CLEAR_SIZE];"
        }
      ]
    },
    {
      "name": "flags",
      "find": "    bchar humans[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "    bchar humans[GAME_PLAYER_COUNT];"
        }
      ]
    },
    {
      "name": "player",
      "find": "    char plBuf[SAVE_CURRENT_PLAYER_SCRATCH_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "    char plBuf[1];"
        }
      ]
    },
    {
      "name": "unused_flag",
      "find": "    i32 H2_UNUSED(saveFlag);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused_array",
      "find": "    i32 H2_UNUSED(scratchVals)[SAVE_SPARE_SLOT_COUNT];\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused_tag",
      "find": "    i32 H2_UNUSED(lastTag);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "",
          "extra_edits": [
            {
              "find": "    lastTag = GAME_UNUSED_FILE_MARKER;\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
