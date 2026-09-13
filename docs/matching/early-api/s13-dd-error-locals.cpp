// s13-dd-error-locals; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 4 arms in 2.4464242421090603 seconds.
// Artifacts: build/source-variant-batch/s13-dd-error-locals/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 1556 / f2d4ae8a9f3eac1a / 446c54a9d349f340 / 71 / 72 / False
// 1 / 1549 / 1631fccf9500cb20 / e4934e2def728316 / 70 / 72 / False
// 2 / 1553 / aedd16d69c7edc73 / fefa68e34be92001 / 70 / 72 / False
// 3 / 1546 / caf394a4e01a6d24 / 5cc1ca591c15765b / 70 / 72 / False

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb0601",
  "axes": [
    {
      "name": "hres",
      "find": "    HRESULT H2_UNUSED(hres);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "\n    hres = lpDD->RestoreDisplayMode();",
              "replace": "\n    lpDD->RestoreDisplayMode();"
            }
          ]
        }
      ]
    },
    {
      "name": "unused",
      "find": "    i32 H2_UNUSED(unused);\n    char errorMessage",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "    char errorMessage",
          "extra_edits": [
            {
              "find": "    unused = 0;\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
