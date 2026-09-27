// c09-movie-number-store; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.46995301079005 seconds.
// Artifacts: build/source-variant-batch/c09-movie-number-store/results.json
// Disposition: adopt trial 1 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 315 / 61a24ad9d30c9e31 / 2eb050a04de93bcb / 11 / 11 / True
// 1 / 315 / 61a24ad9d30c9e31 / 2eb050a04de93bcb / 11 / 11 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x9592a",
  "axes": [
    {
      "name": "destination-byte",
      "find": "static_cast<i8>(smackNumber)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "smackNumber",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
