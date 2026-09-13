// c04-screen-void-boundary; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.5244506690651178 seconds.
// Artifacts: build/source-variant-batch/c04-screen-void-boundary/results.json
// Disposition: adopt trial 1 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 302 / b89a8aeb979fb35d / f01f67da9cffeaa6 / 6 / 6 / True
// 1 / 302 / b89a8aeb979fb35d / f01f67da9cffeaa6 / 6 / 6 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/BASE/WINMGR.cpp",
  "rva": "0xb7020",
  "axes": [
    {
      "name": "void-boundary-kind",
      "find": "reinterpret_cast<u8*>(lpInitWin)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "static_cast<u8*>(lpInitWin)",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
