// Parent with all six named sentinels and three selected C34 hero casts; all16 arms in17.795816550031304 seconds, all962 bytes text68fad32791f3294f/relocsc56b4d54b3368932; retain15(all natural).
// Complete, all compiled/restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Artifacts: build/source-variant-batch/c40-remap/results.json;
// build/c40-remap-axes.json and -manifest.json.
// Native/retail proof and contracts: C34-C40-S41.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x59e9c",
  "axes": [
    {
      "name": "remap_0",
      "find": "static_cast<u8>(cell->m_objectIndex - oldFirstIndex + newFirstIndex)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "cell->m_objectIndex - oldFirstIndex + newFirstIndex"
        }
      ]
    },
    {
      "name": "remap_1",
      "find": "static_cast<u8>(ext->objectIndex - oldFirstIndex + newFirstIndex)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "ext->objectIndex - oldFirstIndex + newFirstIndex"
        }
      ]
    },
    {
      "name": "remap_2",
      "find": "static_cast<u8>(cell->m_overlayIndex - oldFirstIndex + newFirstIndex)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "cell->m_overlayIndex - oldFirstIndex + newFirstIndex"
        }
      ]
    },
    {
      "name": "remap_3",
      "find": "static_cast<u8>(ext->overlayIndex - oldFirstIndex + newFirstIndex)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "natural",
          "replace": "ext->overlayIndex - oldFirstIndex + newFirstIndex"
        }
      ]
    }
  ]
}
*/
