// c06-fizzle-index; parent ffeeb782 plus the typed C02 cursor owners.
// Full product, no TU-state declarations; no source truncation.
// Artifacts: build/source-variant-batch/c06-fizzle-index/results.json
// 2 arms in 1.4690887331962585 seconds; restored.
// Outcome: retain original spelling: every edited arm changes actual bytes.
// Per arm: trial; size; text hash; normalized relocation hash; blocks exact/total; graph exact.
// 0; 1133; c4896c25d14c1c09; 6ceda19e6fb9399e; 44/44; True
// 1; 1128; 1644103d833bc619; 47de20585cc8e53c; 43/44; True

/* Exact source axes (JSON string escapes preserve original CP1251 source spelling):
{
  "schema": 1,
  "source": "src/BASE/WINMGR.cpp",
  "rva": "0xb79b0",
  "axes": [
    {
      "name": "index",
      "find": "static_cast<u16>(\n                    *workPixel | (*savePixel << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT)\n                )",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "recover",
          "replace": "*workPixel | (*savePixel << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT)",
          "extra_edits": []
        }
      ]
    }
  ]
}
*/
