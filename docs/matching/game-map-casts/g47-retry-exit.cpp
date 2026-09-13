// G47 retry exit structural families, first tested before internal cast changes.
// Full initial2-arm product:3.4483381221070886 seconds, generator not truncated.
// Artifact: build/source-variant-batch/g47-retry-full/results.json.
// Earlier g47-retry run had a conservative generator truncation flag; not used for coverage.
// After retained casts, cross both CFG families with all8 hint-cast combinations:
// complete16 arms in17.62046574894339 seconds; restored, no truncation.
// Artifact: build/source-variant-batch/g47-cast-cross/results.json.
// All goto arms891 bytes/31 blocks, textfe094fd099772765.
// All loop-break arms887 bytes/29 blocks, texte565e4bc3da5f143.
// Both relocation-shape hashes6addc0e17a654358; equal reloc hashes do not prove byte equality.
// Retain actual retry-limit goto; no TU-state or AST-transform census claimed.
/* Final full CFG x cast source family:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4c7a3",
  "axes": [
    {
      "name": "retry-exit",
      "find": "goto saveLocation;",
      "options": [
        {
          "name": "goto"
        },
        {
          "name": "loop-break",
          "replace": "break;",
          "extra_edits": [
            {
              "find": "        saveLocation:\n",
              "replace": ""
            }
          ]
        }
      ]
    },
    {
      "name": "cast-chance",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = chance;",
      "options": [
        {
          "name": "natural"
        },
        {
          "name": "written",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintChance = static_cast<i8>(chance);"
        }
      ]
    },
    {
      "name": "cast-x",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintX = x;",
      "options": [
        {
          "name": "natural"
        },
        {
          "name": "written",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintX = static_cast<i8>(x);"
        }
      ]
    },
    {
      "name": "cast-y",
      "find": "gpGame->m_players[playerIndex].m_ultimateArtifactHintY = y;",
      "options": [
        {
          "name": "natural"
        },
        {
          "name": "written",
          "replace": "gpGame->m_players[playerIndex].m_ultimateArtifactHintY = static_cast<i8>(y);"
        }
      ]
    }
  ]
}
*/
