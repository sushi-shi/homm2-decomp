// c02-mouse-handle-tests; parent ffeeb782 plus the typed C02 cursor owners.
// Full product, no TU-state declarations; no source truncation.
// Artifacts: build/source-variant-batch/c02-mouse-handle-tests/results.json
// 4 arms in 2.3724000239744782 seconds; restored.
// Outcome: retain original spelling: every edited arm changes actual bytes.
// Per arm: trial; size; text hash; normalized relocation hash; blocks exact/total; graph exact.
// 0; 1376; 52cf9fbeca58fc6e; 818440dbd78a631a; 47/47; True
// 1; 1384; 434991d957d0f2e0; 5569fbeebb1daa24; 46/47; True
// 2; 1385; 1d0b3aaab69835e1; 5cb3378ddd9dd865; 46/47; True
// 3; 1393; b9f143f07a6b3f21; d2ccfaccc72ec5b0; 45/47; True

/* Exact source axes (JSON string escapes preserve original CP1251 source spelling):
{
  "schema": 1,
  "source": "src/BASE/MOUSEMGR.cpp",
  "rva": "0xb95e0",
  "axes": [
    {
      "name": "hbmpAndMask",
      "find": "reinterpret_cast<i32>(hbmpAndMask[m_cursorSizeIndex])",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "recover",
          "replace": "hbmpAndMask[m_cursorSizeIndex] != NULL",
          "extra_edits": []
        }
      ]
    },
    {
      "name": "hMouseCursor",
      "find": "reinterpret_cast<i32>(hMouseCursor[m_cursorSizeIndex])",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "recover",
          "replace": "hMouseCursor[m_cursorSizeIndex] != NULL",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
