// Parent e490bc850; complete structural product, no AST mutations or TU-state census.
// All 4 arms compiled in 5.113810864277184 seconds; source restored, no truncation.
// Retain trial 1: {'unused_sane': 'retain', 'hero_id': 'natural', 'candidate': 'baseline'}.
// Selected size 1225, text c0e16d2fb3722022, normalized relocs ae65056fcecbb3f4.
// Artifacts: build/source-variant-batch/s42-consistency/results.json; build/s42-consistency-manifest.json.
// Dispositions, complete native/retail proofs and bounds: docs/reconstruction/C43-S41-S42-B65.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5d5ba",
  "axes": [
    {
      "name": "unused_sane",
      "find": "    i32 H2_UNUSED(sane);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "        for (slot = 0; slot < m_players[player].m_heroCount; slot++) {\n            if (m_heroRecs[m_players[player].m_heroIds[slot]].m_owner != player)\n                sane = 0;\n        }\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "hero_id",
      "find": "static_cast<i8>(GetNewHeroId(player, FACTION_ANY, 0))",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "natural",
          "replace": "GetNewHeroId(player, FACTION_ANY, 0)"
        }
      ]
    }
  ]
}
*/
