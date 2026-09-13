// s12-arena-handler-locals; parent638cb091 plus retained API cast changes in other TUs.
// Complete product; no TU-state probes; source restored; no truncation.
// 16 arms in 8.584702053107321 seconds.
// Artifacts: build/source-variant-batch/s12-arena-handler-locals/results.json
// Retain original: EVERY edited arm changes actual native bytes/frames.
// Columns: trial / size / text hash / relocation hash / exact blocks / blocks / graph exact.
// 0 / 481 / db3b4511a2af7977 / 5737004b824d25d6 / 31 / 31 / True
// 1 / 474 / 688d45fb929d7c6b / a7e3bd1339416bcc / 30 / 31 / True
// 2 / 474 / 688d45fb929d7c6b / a7e3bd1339416bcc / 30 / 31 / True
// 3 / 467 / 3ddf60cb9a42e02d / 41120a14a2c27f6b / 30 / 31 / True
// 4 / 481 / e2449008537858b1 / 5737004b824d25d6 / 31 / 31 / True
// 5 / 474 / 0ff31d7d1d5172fc / a7e3bd1339416bcc / 30 / 31 / True
// 6 / 474 / 0ff31d7d1d5172fc / a7e3bd1339416bcc / 30 / 31 / True
// 7 / 467 / b94be0713cfdcded / 41120a14a2c27f6b / 30 / 31 / True
// 8 / 481 / 4e72784eb425b0a1 / 5737004b824d25d6 / 31 / 31 / True
// 9 / 474 / cb459861c32c811c / a7e3bd1339416bcc / 30 / 31 / True
// 10 / 474 / cb459861c32c811c / a7e3bd1339416bcc / 30 / 31 / True
// 11 / 467 / 48fbf6eaa64121ef / 41120a14a2c27f6b / 30 / 31 / True
// 12 / 481 / 323e62bea9272f5c / 5737004b824d25d6 / 31 / 31 / True
// 13 / 474 / aa2f3bb04501c633 / a7e3bd1339416bcc / 30 / 31 / True
// 14 / 474 / aa2f3bb04501c633 / a7e3bd1339416bcc / 30 / 31 / True
// 15 / 467 / 2b7594e2ad0aaca2 / 41120a14a2c27f6b / 30 / 31 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/x_arena.cpp",
  "rva": "0xb2f5b",
  "axes": [
    {
      "name": "message",
      "find": "    tag_message H2_UNUSED(dialogMessage_12);\n",
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
      "name": "widget-index",
      "find": "    i32 H2_UNUSED(widgetIndex_5);\n",
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
      "name": "type",
      "find": "    i32 H2_UNUSED(type_13);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "                type_13 = NORMAL_DIALOG_NO_RESOURCE;\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "extra",
      "find": "    i32 H2_UNUSED(extra_18);\n",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "",
          "extra_edits": [
            {
              "find": "                extra_18 = NORMAL_DIALOG_NO_VALUE;\n",
              "replace": ""
            }
          ]
        }
      ]
    }
  ]
}
*/
