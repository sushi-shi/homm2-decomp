// s13-dd-cleanup-result; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.4630708852782845 seconds.
// Artifacts: build/source-variant-batch/s13-dd-cleanup-result/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 330 / f1a383502cfb2eff / 7b67a8d7e1fb0486 / 17 / 17 / True
// 1 / 325 / 11bf34fe4b512768 / 2b3958be2415f69c / 16 / 17 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb0d03",
  "axes": [
    {
      "name": "restore-result",
      "find": "    HRESULT H2_UNUSED(restoreResult);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "        restoreResult = lpDD->RestoreDisplayMode();",
              "replace": "        lpDD->RestoreDisplayMode();"
            }
          ]
        }
      ]
    }
  ]
}
*/
