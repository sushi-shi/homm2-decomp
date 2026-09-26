// Parent 4d53d37f; complete exact-span structural/scalar product.
// All 72 arms compiled in 77.09057008102536 seconds; source restored.
// No generator/wall-time truncation; no AST mutation or TU-state census.
// Retain trial 70: {'hero0': 'compound', 'hero1': 'compound', 'mask': 'natural', 'autosave': 'natural', 'unused_count': 'original', 'candidate': 'baseline'}.
// Selected size 1290, text 6861ffebf6d88c10, normalized relocs 00ae9e6f8fc1e98d.
// Artifacts: build/source-variant-batch/c42-next/results.json; build/c42-next-manifest.json.
// Unused-count/income deletion changes native code and is not retained.
// Exact native/retail proof, negative scope and bounds: docs/reconstruction/C42-S42.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x56c32",
  "axes": [
    {
      "name": "hero0",
      "find": "m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags = HeroEventFlag(\n        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags)\n        & ~IDX(HERO_EVENT_WEEKLY_VISIT)\n    );",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "typed",
          "replace": "m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags =\n        m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags & ~HERO_EVENT_WEEKLY_VISIT;"
        },
        {
          "name": "compound",
          "replace": "m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags &= ~HERO_EVENT_WEEKLY_VISIT;"
        }
      ]
    },
    {
      "name": "hero1",
      "find": "m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags = HeroEventFlag(\n        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags)\n        & ~IDX(HERO_EVENT_WEEKLY_VISIT)\n    );",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "typed",
          "replace": "m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags =\n        m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags & ~HERO_EVENT_WEEKLY_VISIT;"
        },
        {
          "name": "compound",
          "replace": "m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags &= ~HERO_EVENT_WEEKLY_VISIT;"
        }
      ]
    },
    {
      "name": "mask",
      "find": "giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);\n    for (index",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "natural",
          "replace": "giCurPlayerBit = 1 << giCurPlayer;\n    for (index"
        }
      ]
    },
    {
      "name": "autosave",
      "find": "const_cast<char*>(\"\\xc0\\xe2\\xf2\\xee\\xf1\\xee\\xf5\\xf0\\xe0\\xed\\xe5\\xed\\xe8\\xe5\" /* \"\u0410\u0432\u0442\u043e\u0441\u043e\u0445\u0440\u0430\u043d\u0435\u043d\u0438\u0435\" */)",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "natural",
          "replace": "\"\\xc0\\xe2\\xf2\\xee\\xf1\\xee\\xf5\\xf0\\xe0\\xed\\xe5\\xed\\xe8\\xe5\" /* \"\u0410\u0432\u0442\u043e\u0441\u043e\u0445\u0440\u0430\u043d\u0435\u043d\u0438\u0435\" */"
        }
      ]
    },
    {
      "name": "unused_count",
      "find": "    i32 H2_UNUSED(humansAlive);\n",
      "options": [
        {
          "name": "original"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "        humansAlive = 0;\n        for (index = 0; index < GAME_PLAYER_COUNT; index++) {\n            if (m_playerDead[index] == 0 && gbHumanPlayer[index])\n                humansAlive++;\n        }\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
