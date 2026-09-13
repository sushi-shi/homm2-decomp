// c11-dd-palette-assertion; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.4637270448729396 seconds.
// Artifacts: build/source-variant-batch/c11-dd-palette-assertion/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 238 / 6c8f71e7564bcdb9 / 03344a17db30e06b / 11 / 11 / True
// 1 / 244 / fa55f1ca2d88ec7a / 9c73e7e6cd9f3efd / 10 / 11 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb0c15",
  "axes": [
    {
      "name": "nonnull-assertion",
      "find": "reinterpret_cast<i32>(lpDDPal)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "lpDDPal != NULL",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
