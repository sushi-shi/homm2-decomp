// c11-wg-bitmap-handle; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.4708796674385667 seconds.
// Artifacts: build/source-variant-batch/c11-wg-bitmap-handle/results.json
// Disposition: adopt trial 1 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 315 / ca0c5681d3cdecf6 / cdf8d59f74333e25 / 7 / 7 / True
// 1 / 315 / ca0c5681d3cdecf6 / cdf8d59f74333e25 / 7 / 7 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xb1138",
  "axes": [
    {
      "name": "bitmap-handle",
      "find": "static_cast<HBITMAP>(SelectObject(hdcImage, bitmap))",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "SelectObject(hdcImage, bitmap)",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
