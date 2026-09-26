// C34 LoadGame scalar conversions; parent 4fd5bdbcb.
// Complete four-arm product; all four compiled, source restored, no truncation.
// 5.334152310155332 seconds; 2681 bytes; textcd864eec504ad031; private-label relocation hashes vary.
// Retain trial3, both natural destination conversions.
// Artifacts: build/source-variant-batch/c34-load/results.json,
// build/c34-load-manifest.json and build/c34-load-axes.json.
// Independent native/retail disposition is recorded in C34-save-load-S37.md.
// No AST transformations or TU-state probes are claimed.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4e8d9",
  "axes": [
    {
      "name": "player",
      "find": "close(fd);\n\n    gpAdvManager->m_heroContextLocked = false;\n    gpCurPlayer = &gpGame->m_players[giCurPlayer];\n    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "close(fd);\n\n    gpAdvManager->m_heroContextLocked = false;\n    gpCurPlayer = &gpGame->m_players[giCurPlayer];\n    giCurPlayerBit = 1 << giCurPlayer;"
        }
      ]
    },
    {
      "name": "watch",
      "find": "giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "giCurWatchPlayerBit = 1 << giCurWatchPlayer;"
        }
      ]
    }
  ]
}
*/
