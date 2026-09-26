// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 16 arms compiled in 19.5103369243443 seconds; restored, no truncation.
// Retain trial 15: {'LogStr': 'remove', 'LogInt': 'remove', 'AiPrint': 'remove', 'BVResMsg': 'remove', 'candidate': 'baseline'}.
// Selected size 1971, text 8001d38acae64c9a, normalized relocs e44b6f46977541d4.
// Baseline-identical trials: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15].
// Artifacts: build/source-variant-batch/transmit-save-literals/results.json; build/transmit-save-literals-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5da83",
  "axes": [
    {
      "name": "LogStr",
      "find": "    LogStr(const_cast<char*>(\"Transmit Game Start\"));\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    LogStr(\"Transmit Game Start\");\n",
          "extra_edits": [
            {
              "find": "                LogStr(const_cast<char*>(\"PreWait\"));\n",
              "replace": "                LogStr(\"PreWait\");\n"
            },
            {
              "find": "                LogStr(const_cast<char*>(\"PostWait\"));\n",
              "replace": "                LogStr(\"PostWait\");\n"
            }
          ]
        }
      ]
    },
    {
      "name": "LogInt",
      "find": "    LogInt(\n        const_cast<char*>(\"PostDiffFileSize\"),\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    LogInt(\n        \"PostDiffFileSize\",\n",
          "extra_edits": [
            {
              "find": "        LogInt(\n            const_cast<char*>(\"Send\"),\n",
              "replace": "        LogInt(\n            \"Send\",\n"
            }
          ]
        }
      ]
    },
    {
      "name": "AiPrint",
      "find": "    AiPrint(const_cast<char*>(\"Transmit Start - Compressing\"));\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    AiPrint(\"Transmit Start - Compressing\");\n",
          "extra_edits": [
            {
              "find": "        AiPrint(const_cast<char*>(\"Transmit Start - Sending\"));\n",
              "replace": "        AiPrint(\"Transmit Start - Sending\");\n"
            },
            {
              "find": "    AiPrint(const_cast<char*>(\"Transmit End\"));\n",
              "replace": "    AiPrint(\"Transmit End\");\n"
            }
          ]
        }
      ]
    },
    {
      "name": "BVResMsg",
      "find": "        BVResMsg(const_cast<char*>(\"\\xcf\\xe5\\xf0\\xe5\\xf1\\xfb\\xeb\\xea\\xe0 \\xe4\\xe0\\xed\\xed\\xfb\\xf5\"), RES_NONE, 0);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "        BVResMsg(\"\\xcf\\xe5\\xf0\\xe5\\xf1\\xfb\\xeb\\xea\\xe0 \\xe4\\xe0\\xed\\xed\\xfb\\xf5\", RES_NONE, 0);\n"
        }
      ]
    }
  ]
}
*/
