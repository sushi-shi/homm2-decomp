// s11-movie-unused-locals; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 16 arms in 8.666986950673163 seconds.
// Artifacts: build/source-variant-batch/s11-movie-unused-locals/results.json
// Disposition: retain original; every edited arm changes native text.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 4477 / da881ba390a21b9b / d5a2c358ee11b8bb / 206 / 207 / True
// 1 / 4477 / 1e46174b251b7ebb / 6a26210ca48d82bb / 206 / 207 / True
// 2 / 4477 / 98b3e7e381277df1 / 6a26210ca48d82bb / 206 / 207 / True
// 3 / 4477 / f49b3503144e8b78 / 587fc2aeef5c337a / 206 / 207 / True
// 4 / 4477 / 4c542f9fb3b7313e / 6a26210ca48d82bb / 206 / 207 / True
// 5 / 4477 / 6e6d0184089e64e5 / 587fc2aeef5c337a / 206 / 207 / True
// 6 / 4477 / da0a57e3cf0e7334 / 587fc2aeef5c337a / 206 / 207 / True
// 7 / 4477 / 0b42705c0ba08586 / 5f879f6fbcc116cf / 206 / 207 / True
// 8 / 4467 / 431aad3f63785ed7 / f025d2f9393075b4 / 205 / 207 / True
// 9 / 4467 / 05c254bde22c69b2 / 83f932effe7ab65a / 205 / 207 / True
// 10 / 4467 / 63f65f48afde94b0 / 83f932effe7ab65a / 205 / 207 / True
// 11 / 4467 / 22800c1e4f825f9c / ff07b2565ad42826 / 205 / 207 / True
// 12 / 4467 / f93e5b4cf043194e / 83f932effe7ab65a / 205 / 207 / True
// 13 / 4467 / 8afb4a779aaacba0 / ff07b2565ad42826 / 205 / 207 / True
// 14 / 4467 / 3273b1c38c6f534a / ff07b2565ad42826 / 205 / 207 / True
// 15 / 4467 / 8a95d562b4644f9e / e5889a6c3617c237 / 205 / 207 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/SMACKMGR.cpp",
  "rva": "0x94769",
  "axes": [
    {
      "name": "unusedOne8",
      "find": "    i32 H2_UNUSED(unusedOne8);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "    unusedOne8 = 1;\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "unusedPlaybackState0",
      "find": "    i32 H2_UNUSED(unusedPlaybackState0);\n",
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
      "name": "unusedFrameCount18",
      "find": "    i32 H2_UNUSED(unusedFrameCount18);\n",
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
      "name": "unusedFrameHead04",
      "find": "    i32 H2_UNUSED(unusedFrameHead04);\n",
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
    }
  ]
}
*/
