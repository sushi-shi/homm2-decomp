// s13-wg-palette-result; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.4608065318316221 seconds.
// Artifacts: build/source-variant-batch/s13-wg-palette-result/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 449 / 2ae922daefba2b92 / 42da7f9d185c491e / 16 / 16 / True
// 1 / 446 / 390e7b3b607bb420 / e1c206d8d9cd5181 / 15 / 16 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb1273",
  "axes": [
    {
      "name": "realize-result",
      "find": "    i32 H2_UNUSED(result);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "    result = RealizePalette(dc0);",
              "replace": "    RealizePalette(dc0);"
            }
          ]
        }
      ]
    }
  ]
}
*/
