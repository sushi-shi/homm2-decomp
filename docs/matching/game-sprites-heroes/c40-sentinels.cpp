// Parent cleanb080908d8; all64 arms in68.88719606399536 seconds, all962 bytes text68fad32791f3294f/relocsc56b4d54b3368932; retain63(all named).
// Complete, all compiled/restored; no generator or wall-time truncation.
// No generated AST transformations or TU-state census is claimed.
// Artifacts: build/source-variant-batch/c40-sentinels/results.json;
// build/c40-sentinels-axes.json and -manifest.json.
// Native/retail proof and contracts: C34-C40-S41.md.
/* Exact axes:
{
  "schema": 1,
  "source": "src/SOURCE/GAME.cpp",
  "rva": "0x59e9c",
  "axes": [
    {
      "name": "sentinel_0",
      "find": "cell->m_objectIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "cell->m_objectIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    },
    {
      "name": "sentinel_1",
      "find": "WORLDMAP->Extra(cell->m_extraIndex)->objectIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "WORLDMAP->Extra(cell->m_extraIndex)->objectIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    },
    {
      "name": "sentinel_2",
      "find": "WORLDMAP->Extra(ext->nextIndex)->objectIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "WORLDMAP->Extra(ext->nextIndex)->objectIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    },
    {
      "name": "sentinel_3",
      "find": "cell->m_overlayIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "cell->m_overlayIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    },
    {
      "name": "sentinel_4",
      "find": "WORLDMAP->Extra(cell->m_extraIndex)->overlayIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "WORLDMAP->Extra(cell->m_extraIndex)->overlayIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    },
    {
      "name": "sentinel_5",
      "find": "WORLDMAP->Extra(ext->nextIndex)->overlayIndex != static_cast<u8>(-1)",
      "options": [
        {
          "name": "cast"
        },
        {
          "name": "named",
          "replace": "WORLDMAP->Extra(ext->nextIndex)->overlayIndex != MAPCELL_SPRITE_NONE"
        }
      ]
    }
  ]
}
*/
