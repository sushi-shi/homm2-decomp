// u03-overland-count-promotion; parent e9d0dc9e plus recovered path/queue/target owners.
// Complete 2 arms in 1.5675833132117987 seconds; restored, no state probes.
// Artifacts: build/source-variant-batch/u03-overland-count-promotion/results.json
// Disposition: remove redundant comparison cast.
// Full proof: docs/reconstruction/U02-U04-S08-G11.md.
// 0; 762; dead17e88c228813; 4977618b61547e90; blocks 32/32; graph True
// 1; 762; dead17e88c228813; 4977618b61547e90; blocks 32/32; graph True
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/FINDPATH.cpp",
  "rva": "0x49e3c",
  "axes": [
    {
      "name": "comparison-usual-conversion",
      "find": "static_cast<u32>(gSearchMiddle) < m_queueCount",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "gSearchMiddle < m_queueCount"
        }
      ]
    }
  ]
}
*/
