// C34 game::ClaimMine; parent clean9e33e4659.
// Complete16 arms in18.81679157819599 seconds; restored, no wall-time or generator truncation.
// Retained1044 bytes, text94d57d53e11d5943, normalized relocs variable temporary-label names.
// Disposition: 3 casts removed; color-argument cast retained; trial14.
// Artifact: build/source-variant-batch/c34-mine/results.json.
// Final independent retail bytes/size/DIR32/ordered targets and all98 native objects pass.
// Mine: batch normalized-label hash varies with removed casts; final ordered local
// destinations were checked explicitly, with no added normalization exception.
// See docs/reconstruction/C34-map-G47.md for bounds and rejected alternatives.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x542da",
  "axes": [
    {
      "name": "wire-id",
      "find": "MAP_CHANGE_CLAIM_MINE,\n        static_cast<i8>(mineId),",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "MAP_CHANGE_CLAIM_MINE,\n        mineId,"
        }
      ]
    },
    {
      "name": "record-owner",
      "find": "m_mines[mineId].owner = static_cast<i8>(player);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_mines[mineId].owner = player;"
        }
      ]
    },
    {
      "name": "table-owner",
      "find": "m_mineOwners[mineId] = static_cast<i8>(player);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_mineOwners[mineId] = player;"
        }
      ]
    },
    {
      "name": "color",
      "find": "flag1 + GetPlayerColor(static_cast<i8>(player)),",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "flag1 + GetPlayerColor(player),"
        }
      ]
    }
  ]
}
*/
