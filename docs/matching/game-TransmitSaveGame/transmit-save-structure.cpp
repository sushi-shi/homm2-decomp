// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 128 arms compiled in 148.7405918110162 seconds; restored, no truncation.
// Retain trial 0: {'cleanup_join': 'retain', 'unused1d0': 'retain', 'unused208': 'retain', 'unused21c': 'retain', 'unused8': 'retain', 'unused9': 'retain', 'oldTrack_initialization': 'retain', 'candidate': 'baseline'}.
// Selected size 1971, text 8001d38acae64c9a, normalized relocs e44b6f46977541d4.
// Baseline-identical trials: [0].
// Artifacts: build/source-variant-batch/transmit-save-structure/results.json; build/transmit-save-structure-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5da83",
  "axes": [
    {
      "name": "cleanup_join",
      "find": "    if (file == -1) {\n        goto transmitCleanup;\n    }\n    {\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    if (file != -1) {\n",
          "extra_edits": [
            {
              "find": "\ntransmitCleanup:\n",
              "replace": "\n"
            }
          ]
        }
      ]
    },
    {
      "name": "unused1d0",
      "find": "    i32 H2_UNUSED(unused1d0);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused208",
      "find": "    i32 H2_UNUSED(unused208);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused21c",
      "find": "    i32 H2_UNUSED(unused21c);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused8",
      "find": "    i32 H2_UNUSED(unused8);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "unused9",
      "find": "    i32 H2_UNUSED(unused9);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": ""
        }
      ]
    },
    {
      "name": "oldTrack_initialization",
      "find": "    acknowledged = NULL;\n    oldTrack = -1;\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    acknowledged = NULL;\n"
        }
      ]
    }
  ]
}
*/
