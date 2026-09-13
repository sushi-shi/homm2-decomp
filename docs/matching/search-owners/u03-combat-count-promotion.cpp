// u03-combat-count-promotion; parent e9d0dc9e plus recovered path/queue/target owners.
// Complete 2 arms in 1.557029452174902 seconds; restored, no state probes.
// Artifacts: build/source-variant-batch/u03-combat-count-promotion/results.json
// Disposition: remove redundant comparison cast.
// Full proof: docs/reconstruction/U02-U04-S08-G11.md.
// 0; 406; 08cc14bfc35d8300; f17b2e070d576f15; blocks 22/26; graph False
// 1; 406; 08cc14bfc35d8300; f17b2e070d576f15; blocks 22/26; graph False
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/FINDPATH.cpp",
  "rva": "0x4ae29",
  "axes": [
    {
      "name": "comparison-usual-conversion",
      "find": "static_cast<u32>(middle_a) < m_queueCount",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "middle_a < m_queueCount"
        }
      ]
    }
  ]
}
*/
