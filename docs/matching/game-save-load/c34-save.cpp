// C34 SaveGame scalar conversions; parent 4fd5bdbcb.
// Complete four-arm product; all four compiled, source restored, no truncation.
// 5.516435480676591 seconds; 2906 bytes; text042334ab331accb4 / relocc16d188278d43bc0.
// Retain trial3, both natural destination conversions.
// Artifacts: build/source-variant-batch/c34-save/results.json,
// build/c34-save-manifest.json and build/c34-save-axes.json.
// Independent native/retail disposition is recorded in C34-save-load-S37.md.
// No AST transformations or TU-state probes are claimed.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4d3ae",
  "axes": [
    {
      "name": "player",
      "find": "plBuf[0] = static_cast<char>(giCurPlayer);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "plBuf[0] = giCurPlayer;"
        }
      ]
    },
    {
      "name": "human",
      "find": "humans[iFile] = static_cast<char>(gbHumanPlayer[iFile]);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "humans[iFile] = gbHumanPlayer[iFile];"
        }
      ]
    }
  ]
}
*/
