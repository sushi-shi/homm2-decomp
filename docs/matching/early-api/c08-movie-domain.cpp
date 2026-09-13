// c08-movie-domain; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 3 arms in 2.2066851928830147 seconds.
// Artifacts: build/source-variant-batch/c08-movie-domain/results.json
// Disposition: adopt trial 1 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 4477 / da881ba390a21b9b / fe5108a947412974 / 206 / 207 / True
// 1 / 4477 / da881ba390a21b9b / 92783b4b2bd02ad5 / 206 / 207 / True
// 2 / 4477 / da881ba390a21b9b / fe5108a947412974 / 206 / 207 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x94769",
  "axes": [
    {
      "name": "movie-domain-boundary",
      "find": "static_cast<i8>(expansionChoice0 + EXPANSION_FIRST_MOVIE)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "unwrap-before-offset",
          "replace": "IDX(expansionChoice0) + EXPANSION_FIRST_MOVIE"
        },
        {
          "name": "unwrap-and-narrow",
          "replace": "static_cast<i8>(IDX(expansionChoice0) + EXPANSION_FIRST_MOVIE)"
        }
      ]
    }
  ]
}
*/
