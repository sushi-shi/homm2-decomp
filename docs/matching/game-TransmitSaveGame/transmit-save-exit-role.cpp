// Parent 68ed2e25a; complete reviewed source product, no AST mutation or TU-state census.
// All 4 arms compiled in 5.7536596562713385 seconds; restored, no truncation.
// Retain trial 3: {'exit_parameter': 'remove', 'exit_word': 'remove', 'candidate': 'baseline'}.
// Selected size 1971, text 8001d38acae64c9a, normalized relocs e44b6f46977541d4.
// Baseline-identical trials: [0, 1, 2, 3].
// Artifacts: build/source-variant-batch/transmit-save-exit-role/results.json; build/transmit-save-exit-role-manifest.json.
// Scope, semantic review and full native/retail proof: docs/reconstruction/C44-G50-S43.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x5da83",
  "axes": [
    {
      "name": "exit_parameter",
      "find": "i32 game::TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave) {",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "i32 game::TransmitSaveGame(i32 remotePlayer, i32 playerExited, i32 useCurrentSave) {",
          "extra_edits": [
            {
              "find": " = player;\n        result = TransmitAndWait(",
              "replace": " = playerExited;\n        result = TransmitAndWait("
            }
          ]
        }
      ]
    },
    {
      "name": "exit_word",
      "find": "    REMOTE_SAVE_HEADER_PLAYER        = 3,\n",
      "options": [
        {
          "name": "retain"
        },
        {
          "name": "remove",
          "replace": "    REMOTE_SAVE_HEADER_PLAYER_EXITED = 3,\n",
          "extra_edits": [
            {
              "find": "header[REMOTE_SAVE_HEADER_PLAYER]",
              "replace": "header[REMOTE_SAVE_HEADER_PLAYER_EXITED]"
            }
          ]
        }
      ]
    }
  ]
}
*/
