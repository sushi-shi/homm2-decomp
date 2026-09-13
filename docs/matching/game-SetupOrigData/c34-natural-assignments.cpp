// C34 SetupOrigData: seven independent explicit-to-natural assignment axes.
// Parent: clean053271d7 plus U12/U13 game owner recovery.
// Completed all128 arms in138.56228588335216 seconds; source restored; no truncation.
// Every arm:2513 bytes, text17aba7a1cfda9b7e, normalized relocs89f70eba2c384768,
// 59/59 blocks and graph-exact. No state probes or generated declarations retained.
// Artifact: build/source-variant-batch/c34-setup/results.json.
// Retain all seven natural assignments (trial127).
// Batch exact=false reflects its stricter identity comparison even on baseline;
// final independent retail check proves all2513 bytes/sites/ordered targets.
// Final all98-object native comparison also passes. See docs/reconstruction/C34-setup.md.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4df08",
  "axes": [
    {
      "name": "player-color",
      "find": "m_players[i].m_color = static_cast<i8>(i);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_players[i].m_color = i;"
        }
      ]
    },
    {
      "name": "hero-id",
      "find": "m_heroRecs[i].m_id = static_cast<i8>(i);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_heroRecs[i].m_id = i;"
        }
      ]
    },
    {
      "name": "hero-portrait",
      "find": "m_heroRecs[i].m_portrait = static_cast<u8>(i);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_heroRecs[i].m_portrait = i;"
        }
      ]
    },
    {
      "name": "town-id",
      "find": "m_castleRecs[i].m_id = static_cast<u8>(i);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_castleRecs[i].m_id = i;"
        }
      ]
    },
    {
      "name": "boat-id",
      "find": "m_boats[i].id = static_cast<i8>(i);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_boats[i].id = i;"
        }
      ]
    },
    {
      "name": "current-player-mask",
      "find": "giCurPlayer = 0;\n    gpCurPlayer = &gpGame->m_players[giCurPlayer];\n    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);\n    giCurWatchPlayer = giCurPlayer;",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "giCurPlayer = 0;\n    gpCurPlayer = &gpGame->m_players[giCurPlayer];\n    giCurPlayerBit = 1 << giCurPlayer;\n    giCurWatchPlayer = giCurPlayer;"
        }
      ]
    },
    {
      "name": "watch-player-mask",
      "find": "giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);\n    gpAdvManager->CheckSetEvilInterface(0, -1);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "giCurWatchPlayerBit = 1 << giCurWatchPlayer;\n    gpAdvManager->CheckSetEvilInterface(0, -1);"
        }
      ]
    }
  ]
}
*/
