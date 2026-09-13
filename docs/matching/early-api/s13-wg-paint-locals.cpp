// s13-wg-paint-locals; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 4 arms in 2.4267597189173102 seconds.
// Artifacts: build/source-variant-batch/s13-wg-paint-locals/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 445 / a28f4c101c82c586 / d59c1906f318a8e4 / 11 / 11 / True
// 1 / 441 / fbc845813e321f61 / 05979b466852f7a5 / 10 / 11 / True
// 2 / 445 / f84d86d6c47a4f65 / d59c1906f318a8e4 / 11 / 11 / True
// 3 / 441 / 8fffb3cee445718c / 05979b466852f7a5 / 10 / 11 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb15d6",
  "axes": [
    {
      "name": "padding8",
      "find": "    i32 H2_UNUSED(padding8);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "unused-byte",
      "find": "    char H2_UNUSED(unusedByte0);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "    unusedByte0 = 0;\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
