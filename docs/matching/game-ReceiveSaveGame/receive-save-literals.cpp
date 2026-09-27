// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 32 arms compiled in 37.78174417745322 seconds; restored, no truncation.
// Retain trial 31: {'LogStr': 'remove', 'LogInt': 'remove', 'AiPrint': 'remove', 'BVResMsg': 'remove', 'NormalDialog': 'remove', 'candidate': 'baseline'}.
// Selected size 1736, text fcae3c3c634cd051, normalized relocs 19a3f19dffe7fd75.
// Baseline-identical trials: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31].
// Artifacts: build/source-variant-batch/receive-save-literals/results.json; build/receive-save-literals-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5e236",
  "axes": [
    {
      "name": "LogStr",
      "find": "    LogStr(const_cast<char*>(\"RSG1\"));\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    LogStr(\"RSG1\");\n",
          "extra_edits": [
            {
              "find": "    LogStr(const_cast<char*>(\"Begin Transmit Init Confirm\"));\n",
              "replace": "    LogStr(\"Begin Transmit Init Confirm\");\n"
            },
            {
              "find": "    LogStr(const_cast<char*>(\"End Transmit Init Confirm\"));\n",
              "replace": "    LogStr(\"End Transmit Init Confirm\");\n"
            }
          ]
        }
      ]
    },
    {
      "name": "LogInt",
      "find": "    LogInt(\n        const_cast<char*>(\"FW1\"),\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    LogInt(\n        \"FW1\",\n",
          "extra_edits": [
            {
              "find": "    LogInt(\n        const_cast<char*>(\"FW2\"),\n",
              "replace": "    LogInt(\n        \"FW2\",\n"
            },
            {
              "find": "                    LogInt(\n                        const_cast<char*>(\"FW3\"),\n",
              "replace": "                    LogInt(\n                        \"FW3\",\n"
            },
            {
              "find": "    receivedCrc = calc_crc_long(incomingData, dataSize);\n    LogInt(\n        const_cast<char*>(\"Receive\"),\n",
              "replace": "    receivedCrc = calc_crc_long(incomingData, dataSize);\n    LogInt(\n        \"Receive\",\n"
            },
            {
              "find": "    }\n    LogInt(\n        const_cast<char*>(\"Receive\"),\n",
              "replace": "    }\n    LogInt(\n        \"Receive\",\n"
            }
          ]
        }
      ]
    },
    {
      "name": "AiPrint",
      "find": "    AiPrint(const_cast<char*>(\"Receive Start - Getting Data\"));\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    AiPrint(\"Receive Start - Getting Data\");\n",
          "extra_edits": [
            {
              "find": "    AiPrint(const_cast<char*>(\"Receive Start - Decompressing Data\"));\n",
              "replace": "    AiPrint(\"Receive Start - Decompressing Data\");\n"
            },
            {
              "find": "    AiPrint(const_cast<char*>(\"Receive End\"));\n",
              "replace": "    AiPrint(\"Receive End\");\n"
            }
          ]
        }
      ]
    },
    {
      "name": "BVResMsg",
      "find": "        BVResMsg(const_cast<char*>(\"\\xcf\\xee\\xeb\\xf3\\xf7\\xe5\\xed\\xe8\\xe5 \\xe4\\xe0\\xed\\xed\\xfb\\xf5\"), RES_NONE, 0);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "        BVResMsg(\"\\xcf\\xee\\xeb\\xf3\\xf7\\xe5\\xed\\xe8\\xe5 \\xe4\\xe0\\xed\\xed\\xfb\\xf5\", RES_NONE, 0);\n"
        }
      ]
    },
    {
      "name": "NormalDialog",
      "find": "            NormalDialog(\n                const_cast<char*>(\"\\xce\\xf8\\xe8\\xe1\\xea\\xe0 \\xef\\xee\\xeb\\xf3\\xf7\\xe5\\xed\\xe8\\xff \\xe8\\xed\\xf4\\xee\\xf0\\xec\\xe0\\xf6\\xe8\\xe8. \\xcf\\xf0\\xee\\xe4\\xee\\xeb\\xe6\\xe0\\xf2\\xfc?\"),\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "            NormalDialog(\n                \"\\xce\\xf8\\xe8\\xe1\\xea\\xe0 \\xef\\xee\\xeb\\xf3\\xf7\\xe5\\xed\\xe8\\xff \\xe8\\xed\\xf4\\xee\\xf0\\xec\\xe0\\xf6\\xe8\\xe8. \\xcf\\xf0\\xee\\xe4\\xee\\xeb\\xe6\\xe0\\xf2\\xfc?\",\n"
        }
      ]
    }
  ]
}
*/
