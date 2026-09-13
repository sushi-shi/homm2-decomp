// Parent 508022f87; complete exact-span source product, no AST mutation or TU-state census.
// All 32 arms compiled in 35.51454720180482 seconds; source restored, no truncation.
// Retain trial 31: {'cast0': 'natural', 'cast1': 'natural', 'cast2': 'natural', 'cast3': 'natural', 'cast4': 'natural', 'candidate': 'baseline'}.
// Selected size 7290, text e63f08f05ba7d05c, normalized relocs 953d67f919dce7ba.
// Artifacts: build/source-variant-batch/newmap-setup/results.json; build/newmap-setup-manifest.json.
// Full native/retail proof, domains and measured negatives: docs/reconstruction/C34-G48-R07-S36.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4f755",
  "axes": [
    {
      "name": "cast0",
      "find": "    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "    giCurPlayerBit = 1 << giCurPlayer;\n"
        }
      ]
    },
    {
      "name": "cast1",
      "find": "                gbSetupGamePosToRealGamePos[player] = static_cast<i8>(humanPos++);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                gbSetupGamePosToRealGamePos[player] = humanPos++;\n"
        }
      ]
    },
    {
      "name": "cast2",
      "find": "        gcColorToSetupPos[m_setupPlayerColor[player]] = static_cast<i8>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "        gcColorToSetupPos[m_setupPlayerColor[player]] = player;\n"
        }
      ]
    },
    {
      "name": "cast3",
      "find": "        gcColorToPlayerPos[m_players[player].m_color] = static_cast<i8>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "        gcColorToPlayerPos[m_players[player].m_color] = player;\n"
        }
      ]
    },
    {
      "name": "cast4",
      "find": "                m_mapHeader.victorySideThreshold = static_cast<u16>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                m_mapHeader.victorySideThreshold = player;\n"
        }
      ]
    }
  ]
}
*/
