// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 16 arms compiled in 19.44384785555303 seconds; restored, no truncation.
// Retain trial 0: {'unused2080': 'retain', 'file_initialization': 'retain', 'packet_initialization': 'retain', 'oldTrack_initialization': 'retain', 'candidate': 'baseline'}.
// Selected size 1736, text fcae3c3c634cd051, normalized relocs 19a3f19dffe7fd75.
// Baseline-identical trials: [0].
// Artifacts: build/source-variant-batch/receive-save-structure/results.json; build/receive-save-structure-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5e236",
  "axes": [
    {
      "name": "unused2080",
      "find": "    i32 H2_UNUSED(unused2080);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "    unused2080 = 0;\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "file_initialization",
      "find": "    file = 0;\n    finished = false;\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    finished = false;\n"
        }
      ]
    },
    {
      "name": "packet_initialization",
      "find": "    decodedData = NULL;\n    packet = NULL;\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    decodedData = NULL;\n"
        }
      ]
    },
    {
      "name": "oldTrack_initialization",
      "find": "    success = false;\n    oldTrack = -1;\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    success = false;\n"
        }
      ]
    }
  ]
}
*/
