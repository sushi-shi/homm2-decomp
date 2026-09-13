// g14-movie-playback-tail; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 2 arms in 1.7065476104617119 seconds.
// Artifacts: build/source-variant-batch/g14-movie-playback-tail/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 4477 / da881ba390a21b9b / d5a2c358ee11b8bb / 206 / 207 / True
// 1 / 4489 / 285b8543076b79f3 / 8cc61c059301af7c / 161 / 207 / False

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x94769",
  "axes": [
    {
      "name": "structured-loop-exit",
      "find": "                goto playbackDone;",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "                playing17 = false;",
          "extra_edits": [
            {
              "find": "        if (bSmackNum == CONGRATS && smk1->FrameNum + 1 == smk1->Frames && !musicStarted0) {",
              "replace": "        if (!playing17)\n            break;\n\n        if (bSmackNum == CONGRATS && smk1->FrameNum + 1 == smk1->Frames && !musicStarted0) {"
            },
            {
              "find": "playbackDone:\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
