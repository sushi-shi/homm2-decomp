// C34 ComputeUALoc; parent clean9e33e4659.
// Complete8 arms in9.168350788764656 seconds; restored, no wall-time or generator truncation.
// Retained891 bytes, textfe094fd099772765, normalized relocs 6addc0e17a654358.
// Disposition: all3 casts removed; trial7.
// Artifact: build/source-variant-batch/c34-hint/results.json.
// Final independent retail bytes/size/DIR32/ordered targets and all98 native objects pass.
// Mine: batch normalized-label hash varies with removed casts; final ordered local
// destinations were checked explicitly, with no added normalization exception.
// See docs/reconstruction/C34-map-G47.md for bounds and rejected alternatives.
/* Exact source axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4c7a3",
  "axes": [
    {
      "name": "chance",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = static_cast<i8>(chance);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = chance;"
        }
      ]
    },
    {
      "name": "x",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintX = static_cast<i8>(x);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintX = x;"
        }
      ]
    },
    {
      "name": "y",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintY = static_cast<i8>(y);",
      "options": [
        {
          "name": "keep"
        },
        {
          "name": "natural",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintY = y;"
        }
      ]
    }
  ]
}
*/
