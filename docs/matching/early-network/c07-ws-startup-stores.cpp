// c07-ws-startup-stores; parent 063bce8e plus four signed startup-record fields.
// Complete source family; no TU-state probes or wall-time truncation.
// 4 arms; 3.0956446481868625 seconds; source restored.
// Artifacts: build/source-variant-batch/c07-ws-startup-stores/results.json
// Disposition and full native/retail verification: docs/reconstruction/C07.md.
// Per arm: trial; size; text hash; normalized relocation hash; exact/total blocks; graph exact.
// 0; 1394; 6b3e75fb71c3dc22; 4fa412da1c8f6483; 37/37; True
// 1; 1394; 6b3e75fb71c3dc22; 4fa412da1c8f6483; 37/37; True
// 2; 1394; 6b3e75fb71c3dc22; 4fa412da1c8f6483; 37/37; True
// 3; 1394; 6b3e75fb71c3dc22; 4fa412da1c8f6483; 37/37; True

/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/Wsnetwin.cpp",
  "rva": "0xb1cf0",
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
      "find": "startup.netPosition = static_cast<u8>(plr);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "destination-narrows",
          "replace": "startup.netPosition = plr;"
        }
      ]
    }
  ]
}
*/
