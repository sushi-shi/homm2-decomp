// C34 game::CreateBoat; parent clean9e33e4659.
// Complete64 arms in65.57254693191499 seconds; restored, no wall-time or generator truncation.
// Retained281 bytes, text5186f7a905b1ec06, normalized relocs 5b3bb0928c2b11cf.
// Disposition: all6 casts removed; trial63.
// Artifact: build/source-variant-batch/c34-boat/results.json.
// Final independent retail bytes/size/DIR32/ordered targets and all98 native objects pass.
// Mine: batch normalized-label hash varies with removed casts; final ordered local
// destinations were checked explicitly, with no added normalization exception.
// See docs/reconstruction/C34-map-G47.md for bounds and rejected alternatives.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4cde3",
  "axes": [
    {
      "name": "slot",
      "find": "m_boatSlots[boatIdx] = static_cast<i8>(boatIdx);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "m_boatSlots[boatIdx] = boatIdx;"
        }
      ]
    },
    {
      "name": "id",
      "find": "boat->id = static_cast<i8>(boatIdx);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "boat->id = boatIdx;"
        }
      ]
    },
    {
      "name": "x",
      "find": "boat->x = static_cast<i8>(x);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "boat->x = x;"
        }
      ]
    },
    {
      "name": "y",
      "find": "boat->y = static_cast<i8>(y);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "boat->y = y;"
        }
      ]
    },
    {
      "name": "owner",
      "find": "boat->owner = static_cast<i8>(giCurPlayer);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "boat->owner = giCurPlayer;"
        }
      ]
    },
    {
      "name": "metadata",
      "find": "boat->savedEventData = static_cast<u8>(square->m_objectMetadata);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "boat->savedEventData = square->m_objectMetadata;"
        }
      ]
    }
  ]
}
*/
