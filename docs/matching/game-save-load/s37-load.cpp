// S37 LoadGame storage; parent4fd5bdbcb plus four C34 save/load cast removals.
// Complete64 arms in66.24839116912335 seconds, compiled/restored/no truncation.
// Only trials0,8,16,24 preserve textcd864eec504ad031 / reloce78d04bd45323af3.
// Select trial24: human array8->6 and current-player array4->1.
// Every unused-declaration removal or workData40->36 changes bytes in this parent.
// Keep workData40 and all three unread declarations; no invented frame padding.
// The four-byte chunkTag owner is not an axis: its reads genuinely need four bytes.
// Artifacts: build/source-variant-batch/s37-load/results.json,
// build/s37-load-manifest.json and build/s37-load-axes.json.
// All source shapes/retail disposition are detailed in C34-save-load-S37.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4e8d9",
  "axes": [
    {
      "name": "work",
      "find": "    char workData[SAVE_LEGACY_CLEAR_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "    char workData[SAVE_LEGACY_SERIALIZED_SIZE];"
        }
      ]
    },
    {
      "name": "flags",
      "find": "    char isHuman[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "    char isHuman[GAME_PLAYER_COUNT];"
        }
      ]
    },
    {
      "name": "player",
      "find": "    char plBuf[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];",
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
      "find": "    i32 H2_UNUSED(oldFlag);\n",
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
      "name": "unused_value",
      "find": "    i32 H2_UNUSED(saveVal);\n",
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
      "find": "    char H2_UNUSED(junkBuf)[SAVE_LEGACY_CLEAR_SIZE];\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": ""
        }
      ]
    }
  ]
}
*/
