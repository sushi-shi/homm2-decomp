// c07-dp-startup-stores; parent 063bce8e plus four signed startup-record fields.
// Complete source family; no TU-state probes or wall-time truncation.
// 4 arms; 2.8175299279391766 seconds; source restored.
// Artifacts: build/source-variant-batch/c07-dp-startup-stores/results.json
// Disposition and full native/retail verification: docs/reconstruction/C07.md.
// Per arm: trial; size; text hash; normalized relocation hash; exact/total blocks; graph exact.
// 0; 703; 705be52d2e969452; f3cc7715e9856637; 27/27; True
// 1; 703; 705be52d2e969452; f3cc7715e9856637; 27/27; True
// 2; 703; 705be52d2e969452; f3cc7715e9856637; 27/27; True
// 3; 703; 705be52d2e969452; f3cc7715e9856637; 27/27; True

/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/dpnetwin.cpp",
  "rva": "0x36866",
  "axes": [
    {
      "name": "playerCount",
      "find": "startup.playerCount = static_cast<u8>(giNumHumanPlayers);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "destination-narrows",
          "replace": "startup.playerCount = giNumHumanPlayers;"
        }
      ]
    },
    {
      "name": "netPosition",
      "find": "startup.netPosition = static_cast<u8>(guestIndex);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "destination-narrows",
          "replace": "startup.netPosition = guestIndex;"
        }
      ]
    }
  ]
}
*/
