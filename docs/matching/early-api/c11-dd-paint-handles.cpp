// c11-dd-paint-handles; parent638cb091 (domain follow-up uses retained API casts).
// Full source product, no TU-state probe declarations; restored, no truncation.
// 8 arms in 4.3763386476784945 seconds.
// Artifacts: build/source-variant-batch/c11-dd-paint-handles/results.json
// Disposition: adopt trial 7 subject to full native/dialect/retail proof in C04-C05-C08-C11-S11-S13-G14.md.
// Columns: trial / size / text hash / normalized relocation hash / exact blocks / total blocks / graph exact.
// 0 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 1 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 2 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 3 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 4 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 5 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 6 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True
// 7 / 1242 / 5498f6afa8f8f641 / 112f1de8a89d77e7 / 55 / 55 / True

/* Exact source choices:
{
  "schema": 1,
  "source": "src/SOURCE/wingraph.cpp",
  "rva": "0xafe8f",
  "axes": [
    {
      "name": "begin-window",
      "find": "BeginPaint(reinterpret_cast<HWND>(window), &paint7)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "BeginPaint(window, &paint7)",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "client-window",
      "find": "GetClientRect(reinterpret_cast<HWND>(window), &gDDClientRect)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "GetClientRect(window, &gDDClientRect)",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "end-window",
      "find": "EndPaint(reinterpret_cast<HWND>(window), &paint7)",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "remove",
          "replace": "EndPaint(window, &paint7)",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
