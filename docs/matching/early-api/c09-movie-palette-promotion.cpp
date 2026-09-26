// c09-movie-palette-promotion; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 4 arms in 2.5099296383559704 seconds.
// Artifacts: build/source-variant-batch/c09-movie-palette-promotion/results.json
// Disposition: adopt trial 3 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 63 / 44ee08e7d89dd085 / e3b0c44298fc1c14 / 5 / 5 / True
// 1 / 63 / 44ee08e7d89dd085 / e3b0c44298fc1c14 / 5 / 5 / True
// 2 / 63 / 44ee08e7d89dd085 / e3b0c44298fc1c14 / 5 / 5 / True
// 3 / 63 / 44ee08e7d89dd085 / e3b0c44298fc1c14 / 5 / 5 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x94540",
  "axes": [
    {
      "name": "cast-family",
      "find": "static_cast<u8>(static_cast<i32>(paletteData[i]) >> PALETTE_VALUE_SHIFT)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "drop-promotion",
          "replace": "static_cast<u8>(paletteData[i] >> PALETTE_VALUE_SHIFT)"
        },
        {
          "name": "drop-store-cast",
          "replace": "static_cast<i32>(paletteData[i]) >> PALETTE_VALUE_SHIFT"
        },
        {
          "name": "drop-both",
          "replace": "paletteData[i] >> PALETTE_VALUE_SHIFT"
        }
      ]
    }
  ]
}
*/
