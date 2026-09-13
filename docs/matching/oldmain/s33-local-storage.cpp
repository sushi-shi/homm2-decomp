// S33 oldmain: complete32-arm product,39.55248038843274 seconds; restored, no truncation.
// Parent a0438967; U11 record212 structural family already rejected for frame mismatch.
// All four flag-array extent combinations preserve text3c6798433424b23b and
// normalized relocation hash c7839c78c4a9eb48 when all three scalar slots stay.
// Any scalar deletion changes target bytes; every arm remains5329 bytes.
// Retain trial3: both local arrays6, all scalar slots retained.
// All rows252/253 diagnostic blocks with existing EH-tail frontier.
// Artifacts: build/source-variant-batch/s33-startup/results.json.
// Exact-span runner used because AST frontend rejects an existing VC6 literal conditional.
// No AST or TU-state census claimed; no parser allowance added.
// Full evidence: docs/reconstruction/U11-S33.md.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/KB.cpp",
  "rva": "0x66767",
  "axes": [
    {
      "name": "unused-main-state",
      "find": "    i32 H2_UNUSED(unusedMainState_o);\n",
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
      "name": "unused-menu-state",
      "find": "    i32 H2_UNUSED(unusedMenuState);\n",
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
      "name": "unused-player-state",
      "find": "    i32 H2_UNUSED(unusedPlayerState_f);\n",
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
      "name": "network-player-flags",
      "find": "char matchedNetPlayers_d[OLD_MAIN_MATCH_BUFFER_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "char matchedNetPlayers_d[OLD_MAIN_PLAYER_COUNT];"
        }
      ]
    },
    {
      "name": "game-player-flags",
      "find": "char matchedGamePlayers_c[OLD_MAIN_MATCH_BUFFER_SIZE];",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "minimal",
          "replace": "char matchedGamePlayers_c[OLD_MAIN_PLAYER_COUNT];"
        }
      ]
    }
  ]
}
*/
