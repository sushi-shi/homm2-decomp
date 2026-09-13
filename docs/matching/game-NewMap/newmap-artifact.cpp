// Parent 508022f87; complete exact-span source product, no AST mutation or TU-state census.
// All 4 arms compiled in 5.122136043384671 seconds; source restored, no truncation.
// Retain trial 3: {'cast12': 'natural', 'cast13': 'natural', 'candidate': 'baseline'}.
// Selected size 7290, text e63f08f05ba7d05c, normalized relocs 953d67f919dce7ba.
// Artifacts: build/source-variant-batch/newmap-artifact/results.json; build/newmap-artifact-manifest.json.
// Full native/retail proof, domains and measured negatives: docs/reconstruction/C34-G48-R07-S36.md.
/* Exact source product:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x4f755",
  "axes": [
    {
      "name": "cast12",
      "find": "    m_ultimateArtifactX = static_cast<i8>(player);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "    m_ultimateArtifactX = player;\n"
        }
      ]
    },
    {
      "name": "cast13",
      "find": "    m_ultimateArtifactY = static_cast<i8>(nTown);\n",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "    m_ultimateArtifactY = nTown;\n"
        }
      ]
    }
  ]
}
*/
