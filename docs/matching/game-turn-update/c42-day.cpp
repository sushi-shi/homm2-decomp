// Parent 4d53d37f; complete exact-span structural/scalar product.
// All 24 arms compiled in 26.462598172947764 seconds; source restored.
// No generator/wall-time truncation; no AST mutation or TU-state census.
// Retain trial 19: {'daily_flag': 'compound', 'unused_income': 'original', 'restored_mana': 'natural', 'town_mana': 'natural', 'candidate': 'baseline'}.
// Selected size 2576, text b412aa0c9090779b, normalized relocs 79ee460cc3b42937.
// Artifacts: build/source-variant-batch/c42-day/results.json; build/c42-day-manifest.json.
// Unused-count/income deletion changes native code and is not retained.
// Exact native/retail proof, negative scope and bounds: docs/reconstruction/C42-S42.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x574c6",
  "axes": [
    {
      "name": "daily_flag",
      "find": "m_heroRecs[player].m_eventFlags = HeroEventFlag(\n            static_cast<i32>(m_heroRecs[player].m_eventFlags) & ~IDX(WEEKLY_HERO_RESERVED_FLAG)\n        );",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "typed",
          "replace": "m_heroRecs[player].m_eventFlags =\n        m_heroRecs[player].m_eventFlags & ~WEEKLY_HERO_RESERVED_FLAG;"
        },
        {
          "name": "compound",
          "replace": "m_heroRecs[player].m_eventFlags &= ~WEEKLY_HERO_RESERVED_FLAG;"
        }
      ]
    },
    {
      "name": "unused_income",
      "find": "    i32 H2_UNUSED(income8);\n",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "restored_mana",
      "find": "static_cast<i16>(restoredSpellPoints14)",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "natural",
          "replace": "restoredSpellPoints14"
        }
      ]
    },
    {
      "name": "town_mana",
      "find": "static_cast<i16>(maxSpellPoints9)",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "natural",
          "replace": "maxSpellPoints9"
        }
      ]
    }
  ]
}
*/
