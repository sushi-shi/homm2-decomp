// s13-dd-query-local; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.459412065334618 seconds.
// Artifacts: build/source-variant-batch/s13-dd-query-local/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 45 / 957cb53252701a84 / 22c95a9e0608ec78 / 6 / 6 / True
// 1 / 42 / 57445af3b2e9a4a9 / 0f1a1f91e8f62787 / 4 / 6 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xafc1a",
  "axes": [
    {
      "name": "unused-slot",
      "find": "i32 DDQueryNewPalette(void) {\n    i32 H2_UNUSED(unused);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "i32 DDQueryNewPalette(void) {",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
