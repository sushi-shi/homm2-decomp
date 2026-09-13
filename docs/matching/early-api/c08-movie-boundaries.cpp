// c08-movie-boundaries; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 8 arms in 4.739486417733133 seconds.
// Artifacts: build/source-variant-batch/c08-movie-boundaries/results.json
// Disposition: adopt trial 6 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// Dropping the movie-byte cast also preserves native text, but fails C++20
// because stepped enum arithmetic returns ExpansionCampaignId. The following
// c08-movie-domain family unwraps the campaign index before adding68.
// 0 / 4477 / da881ba390a21b9b / d5a2c358ee11b8bb / 206 / 207 / True
// 1 / 4477 / da881ba390a21b9b / 18afd4f09276339b / 206 / 207 / True
// 2 / 4477 / da881ba390a21b9b / 275776d82305278c / 206 / 207 / True
// 3 / 4477 / da881ba390a21b9b / fb1f5bfe4967273f / 206 / 207 / True
// 4 / 4477 / da881ba390a21b9b / 275776d82305278c / 206 / 207 / True
// 5 / 4477 / da881ba390a21b9b / fb1f5bfe4967273f / 206 / 207 / True
// 6 / 4477 / da881ba390a21b9b / 1efa74483f2f3427 / 206 / 207 / True
// 7 / 4477 / da881ba390a21b9b / d27766c6e6e34f28 / 206 / 207 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x94769",
  "axes": [
    {
      "name": "brother-upcast",
      "find": "static_cast<resource*>(brotherIcon)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "brotherIcon",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "background-upcast",
      "find": "static_cast<resource*>(backImage)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "backImage",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "movie-byte-store",
      "find": "static_cast<i8>(expansionChoice0 + EXPANSION_FIRST_MOVIE)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "expansionChoice0 + EXPANSION_FIRST_MOVIE",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
