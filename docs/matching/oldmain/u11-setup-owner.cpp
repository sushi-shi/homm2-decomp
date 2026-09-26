// U11 oldmain buffer owner: complete2-arm family;3.518003514967859 seconds.
// Parent a0438967. Actual serialized setup212 bytes; existing allocation256.
// union256:5329 bytes, text3c6798433424b23b, normalized relocs c7839c78c4a9eb48.
// record212:5329 bytes, text8e548f19880fea96, normalized relocs d4b703a1a21baca8.
// Both252/253 diagnostic blocks with the same EH-tail frontier; ordinary baseline
// retail bytes pass but four EH/FS identities remain outside the general checker.
// Frame is decisive: baseline/retail81 ec74 010000 (sub esp,0x174),
// record212 81 ec48 010000 (sub esp,0x148):44 bytes missing.
// Retain original owner. No padding/filler declarations introduced.
// Full results: build/source-variant-batch/u11-owner/results.json.
// One-arm diagnostic replay: build/source-variant-batch/u11-record-replay/best.asm,
// complete1/1 in2.48029107414186 seconds; same target text/reloc hashes, restored.
// AST frontend rejects the pre-existing VC6 string-literal conditional in oldmain;
// run this exact-span manifest with homm2.permute.batch_source_variants.
// No parse allowance or source workaround was added; no AST/state coverage claimed.
/* Exact source family:
{
  "schema": 1,
  "source": "src/SOURCE/KB.cpp",
  "rva": "0x66767",
  "axes": [
    {
      "name": "setup-buffer-owner",
      "find": "OldMainNetBuffer netBuffer_f;",
      "options": [
        {
          "name": "union256"
        },
        {
          "name": "record212",
          "replace": "OldMainNetSetup netBuffer_f;",
          "extra_edits": [
            {
              "find": "memcpy(netBuffer_f.setup.gamePosToNetPos, gbGamePosToNetPos, OLD_MAIN_PLAYER_COUNT);",
              "replace": "memcpy(netBuffer_f.gamePosToNetPos, gbGamePosToNetPos, OLD_MAIN_PLAYER_COUNT);"
            },
            {
              "find": "memcpy(\n                    netBuffer_f.setup.players,\n                    gsNetPlayerInfo,\n                    sizeof(netBuffer_f.setup.players)\n                );",
              "replace": "memcpy(\n                    netBuffer_f.players,\n                    gsNetPlayerInfo,\n                    sizeof(netBuffer_f.players)\n                );"
            },
            {
              "find": "netBuffer_f.setup.useRegularCompression = gbUseRegularCompression;",
              "replace": "netBuffer_f.useRegularCompression = gbUseRegularCompression;"
            },
            {
              "find": "netBuffer_f.setup.useDiffCompression = gbUseDiffCompression;",
              "replace": "netBuffer_f.useDiffCompression = gbUseDiffCompression;"
            },
            {
              "find": "netBuffer_f.bytes",
              "replace": "reinterpret_cast<char*>(&netBuffer_f)"
            }
          ]
        }
      ]
    }
  ]
}
*/
