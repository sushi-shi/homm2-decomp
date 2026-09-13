// s13-dd-surface-locals; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 4 arms in 2.468520629219711 seconds.
// Artifacts: build/source-variant-batch/s13-dd-surface-locals/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 265 / a1d8a2a219117cef / 43a817da5857214e / 12 / 12 / True
// 1 / 259 / be515776428eb26d / 955049d72700765b / 12 / 12 / True
// 2 / 259 / be515776428eb26d / 955049d72700765b / 12 / 12 / True
// 3 / 256 / 1fa53ac402844b08 / 9058cb4ae3930ced / 12 / 12 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb04f8",
  "axes": [
    {
      "name": "cnt",
      "find": "    i32 H2_UNUSED(cnt);\n",
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
      "name": "unused",
      "find": "    i32 H2_UNUSED(unused);\n    HRESULT rv;",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "    HRESULT rv;",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
