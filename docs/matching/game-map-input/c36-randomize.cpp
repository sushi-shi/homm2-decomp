// C36/S37/S38: parent clean d7da2d15c unless noted.
// Complete128 arms139.95387980900705s; retainedtrial127,all7 natural assignments. All9926 bytes/textc243e648eb042965, normalized-label relocation hashes vary. Event coordinates are u16, boat coordinates and guardian count u8; original signed intermediate casts disappear without changing low bits. Original block partition300/301 does not negate full byte/ordered-target proof.
// All arms compiled; source restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Full bytes/size/ordered targets and all98 native objects: C36-S37-S38.md.
// Artifacts: build/source-variant-batch/c36-randomize/results.json,
// build/c36-randomize-axes.json and -manifest.json.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x513cf",
  "axes": [
    {
      "name": "event_x",
      "find": "mapEvent0->x = static_cast<i16>(xPos);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "mapEvent0->x = xPos;"
        }
      ]
    },
    {
      "name": "event_y",
      "find": "mapEvent0->y = static_cast<i16>(yPos);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "mapEvent0->y = yPos;"
        }
      ]
    },
    {
      "name": "boat_left_x",
      "find": "townRec->m_boatX = static_cast<i8>(xPos - CASTLE_BOAT_X_OFFSET);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "townRec->m_boatX = xPos - CASTLE_BOAT_X_OFFSET;"
        }
      ]
    },
    {
      "name": "boat_left_y",
      "find": "townRec->m_boatY = static_cast<i8>(yPos + CASTLE_BOAT_Y_OFFSET);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "townRec->m_boatY = yPos + CASTLE_BOAT_Y_OFFSET;"
        }
      ]
    },
    {
      "name": "boat_right_x",
      "find": "townRec->m_boatX =\n                                    static_cast<i8>(xPos + CASTLE_BOAT_X_OFFSET);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "townRec->m_boatX =\n                                    xPos + CASTLE_BOAT_X_OFFSET;"
        }
      ]
    },
    {
      "name": "boat_right_y",
      "find": "townRec->m_boatY =\n                                    static_cast<i8>(yPos + CASTLE_BOAT_Y_OFFSET);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "townRec->m_boatY =\n                                    yPos + CASTLE_BOAT_Y_OFFSET;"
        }
      ]
    },
    {
      "name": "guardian_count",
      "find": "m_mines[mineId6].guardianCount = static_cast<u8>(Random(\n                        ABANDONED_MINE_GUARDIAN_COUNT_MIN,\n                        ABANDONED_MINE_GUARDIAN_COUNT_MAX\n                    ));",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_mines[mineId6].guardianCount = Random(\n                        ABANDONED_MINE_GUARDIAN_COUNT_MIN,\n                        ABANDONED_MINE_GUARDIAN_COUNT_MAX\n                    );"
        }
      ]
    }
  ]
}
*/
