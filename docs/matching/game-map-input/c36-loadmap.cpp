// C36/S37/S38: parent clean d7da2d15c unless noted.
// Complete128 arms136.20258380193263s; retainedtrial124. Only32 arms with original8-byte type/junk extents retain original textbcf1c5d84fbb1bba and relocs5a1f6107c63bef7a,1077 bytes. All x/y type+extent combinations and signed i8 type owner remain compatible; selectedx/y u8[1],type i8[8],junk char[8]. Smaller type/junk alters bytes despite all sizes1077.
// All arms compiled; source restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Full bytes/size/ordered targets and all98 native objects: C36-S37-S38.md.
// Artifacts: build/source-variant-batch/c36-loadmap/results.json,
// build/c36-loadmap-axes.json and -manifest.json.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x53b9e",
  "axes": [
    {
      "name": "x",
      "find": "    char x[LOAD_MAP_COORDINATE_SCRATCH_SIZE];",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "minimal_char",
          "replace": "    char x[1];"
        },
        {
          "name": "typed_original",
          "replace": "    u8 x[LOAD_MAP_COORDINATE_SCRATCH_SIZE];",
          "extra_edits": [
            {
              "find": "if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {\n            m_castleRecs[i].m_onMap",
              "replace": "if (x[0] != SAVED_TOWN_OFF_MAP) {\n            m_castleRecs[i].m_onMap"
            },
            {
              "find": "m_castleRecs[i].m_x = static_cast<u8>(x[0]);",
              "replace": "m_castleRecs[i].m_x = x[0];"
            },
            {
              "find": "if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {\n            m_mines[i].guardianType",
              "replace": "if (x[0] != SAVED_TOWN_OFF_MAP) {\n            m_mines[i].guardianType"
            },
            {
              "find": "m_mines[i].x = static_cast<u8>(x[0]);",
              "replace": "m_mines[i].x = x[0];"
            }
          ]
        },
        {
          "name": "typed_minimal",
          "replace": "    u8 x[1];",
          "extra_edits": [
            {
              "find": "if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {\n            m_castleRecs[i].m_onMap",
              "replace": "if (x[0] != SAVED_TOWN_OFF_MAP) {\n            m_castleRecs[i].m_onMap"
            },
            {
              "find": "m_castleRecs[i].m_x = static_cast<u8>(x[0]);",
              "replace": "m_castleRecs[i].m_x = x[0];"
            },
            {
              "find": "if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {\n            m_mines[i].guardianType",
              "replace": "if (x[0] != SAVED_TOWN_OFF_MAP) {\n            m_mines[i].guardianType"
            },
            {
              "find": "m_mines[i].x = static_cast<u8>(x[0]);",
              "replace": "m_mines[i].x = x[0];"
            }
          ]
        }
      ]
    },
    {
      "name": "y",
      "find": "    char y[LOAD_MAP_COORDINATE_SCRATCH_SIZE];",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "minimal_char",
          "replace": "    char y[1];"
        },
        {
          "name": "typed_original",
          "replace": "    u8 y[LOAD_MAP_COORDINATE_SCRATCH_SIZE];",
          "extra_edits": [
            {
              "find": "m_castleRecs[i].m_y = static_cast<u8>(y[0]);",
              "replace": "m_castleRecs[i].m_y = y[0];"
            },
            {
              "find": "m_mines[i].y = static_cast<u8>(y[0]);",
              "replace": "m_mines[i].y = y[0];"
            }
          ]
        },
        {
          "name": "typed_minimal",
          "replace": "    u8 y[1];",
          "extra_edits": [
            {
              "find": "m_castleRecs[i].m_y = static_cast<u8>(y[0]);",
              "replace": "m_castleRecs[i].m_y = y[0];"
            },
            {
              "find": "m_mines[i].y = static_cast<u8>(y[0]);",
              "replace": "m_mines[i].y = y[0];"
            }
          ]
        }
      ]
    },
    {
      "name": "type",
      "find": "    char type[LOAD_MAP_RECORD_SCRATCH_SIZE];",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "minimal_char",
          "replace": "    char type[1];"
        },
        {
          "name": "typed_original",
          "replace": "    i8 type[LOAD_MAP_RECORD_SCRATCH_SIZE];",
          "extra_edits": []
        },
        {
          "name": "typed_minimal",
          "replace": "    i8 type[1];",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "junk",
      "find": "    char junk[LOAD_MAP_RECORD_SCRATCH_SIZE];",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "minimal",
          "replace": "    char junk[sizeof(u16)];"
        }
      ]
    }
  ]
}
*/
