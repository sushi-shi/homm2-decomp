// Parent 508022f87; complete exact-span source product, no AST mutation or TU-state census.
// All 128 arms compiled in 134.68258106242865 seconds; source restored, no truncation.
// Retain trial 127: {'cast5': 'natural', 'cast6': 'natural', 'cast7': 'natural', 'cast8': 'natural', 'cast9': 'natural', 'cast10': 'natural', 'cast11': 'natural', 'candidate': 'baseline'}.
// Selected size 7290, text e63f08f05ba7d05c, normalized relocs 953d67f919dce7ba.
// Artifacts: build/source-variant-batch/newmap-heroes/results.json; build/newmap-heroes-manifest.json.
// Full native/retail proof, domains and measured negatives: docs/reconstruction/C34-G48-R07-S36.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4f755",
  "axes": [
    {
      "name": "cast5",
      "find": "                static_cast<i8>(GetNewHeroId(\n                    player,\n                    m_castleRecs[m_players[player].m_townIds[selectedTown]].m_type,\n                    0\n                ));\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                GetNewHeroId(\n                    player,\n                    m_castleRecs[m_players[player].m_townIds[selectedTown]].m_type,\n                    0\n                );\n"
        }
      ]
    },
    {
      "name": "cast6",
      "find": "            m_availableHeroes[m_players[player].m_heroIds[m_players[player].m_heroCount]] =\n                static_cast<i8>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "            m_availableHeroes[m_players[player].m_heroIds[m_players[player].m_heroCount]] =\n                player;\n"
        }
      ]
    },
    {
      "name": "cast7",
      "find": "            m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_owner =\n                static_cast<i8>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "            m_heroRecs[m_players[player].m_heroIds[m_players[player].m_heroCount]].m_owner =\n                player;\n"
        }
      ]
    },
    {
      "name": "cast8",
      "find": "                m_players[player].m_availableHeroIds[0] = static_cast<char>(awardHero);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                m_players[player].m_availableHeroIds[0] = awardHero;\n"
        }
      ]
    },
    {
      "name": "cast9",
      "find": "                            static_cast<char>(awardHero);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                            awardHero;\n"
        }
      ]
    },
    {
      "name": "cast10",
      "find": "                static_cast<char>(GetNewHeroId(player, startClass, 0));\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "                GetNewHeroId(player, startClass, 0);\n"
        }
      ]
    },
    {
      "name": "cast11",
      "find": "        m_players[player].m_availableHeroIds[1] =\n            static_cast<char>(GetNewHeroId(player, startClass, 0));\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "        m_players[player].m_availableHeroIds[1] =\n            GetNewHeroId(player, startClass, 0);\n"
        }
      ]
    }
  ]
}
*/
