// C34 game::ClaimTown; parent clean9e33e4659.
// Complete64 arms in67.27839370351285 seconds; restored, no wall-time or generator truncation.
// Retained775 bytes, text381afa20b054797d, normalized relocs 56ab4f51643ec990.
// Disposition: 4 casts removed; both color-argument casts retained; trial60.
// Artifact: build/source-variant-batch/c34-town/results.json.
// Final independent retail bytes/size/DIR32/ordered targets and all98 native objects pass.
// Mine: batch normalized-label hash varies with removed casts; final ordered local
// destinations were checked explicitly, with no added normalization exception.
// See docs/reconstruction/C34-map-G47.md for bounds and rejected alternatives.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x53fd3",
  "axes": [
    {
      "name": "wire-id",
      "find": "MAP_CHANGE_CLAIM_TOWN,\n            static_cast<i8>(townId),",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "MAP_CHANGE_CLAIM_TOWN,\n            townId,"
        }
      ]
    },
    {
      "name": "record-owner",
      "find": "m_castleRecs[townId].m_owner = static_cast<i8>(player);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_castleRecs[townId].m_owner = player;"
        }
      ]
    },
    {
      "name": "table-owner",
      "find": "m_townOwners[townId] = static_cast<i8>(player);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_townOwners[townId] = player;"
        }
      ]
    },
    {
      "name": "player-list",
      "find": "m_players[player].m_townIds[m_players[player].m_townCount] = static_cast<i8>(townId);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_players[player].m_townIds[m_players[player].m_townCount] = townId;"
        }
      ]
    },
    {
      "name": "left-color",
      "find": "GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE,",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "GetPlayerColor(player) * TOWN_FLAG_FRAME_STRIDE,"
        }
      ]
    },
    {
      "name": "right-color",
      "find": "GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE\n            + TOWN_FLAG_RIGHT_FRAME,",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "GetPlayerColor(player) * TOWN_FLAG_FRAME_STRIDE\n            + TOWN_FLAG_RIGHT_FRAME,"
        }
      ]
    }
  ]
}
*/
