// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 4 arms compiled in 5.834304862655699 seconds; restored, no truncation.
// Retain trial 3: {'packet_index': 'remove', 'batch_index': 'remove', 'candidate': 'baseline'}.
// Selected size 1971, text 8001d38acae64c9a, normalized relocs e44b6f46977541d4.
// Baseline-identical trials: [0, 1, 2, 3].
// Artifacts: build/source-variant-batch/transmit-save-indices/results.json; build/transmit-save-indices-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5da83",
  "axes": [
    {
      "name": "packet_index",
      "find": "                        *reinterpret_cast<i16*>(header) = static_cast<i16>(packet);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "                        *reinterpret_cast<i16*>(header) = packet;\n"
        }
      ]
    },
    {
      "name": "batch_index",
      "find": "                    static_cast<i16>(batch * REMOTE_PACKET_BATCH_SIZE);\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "                    batch * REMOTE_PACKET_BATCH_SIZE;\n"
        }
      ]
    }
  ]
}
*/
