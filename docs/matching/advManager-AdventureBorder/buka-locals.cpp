/*
Functions:
  advManager::SaveAdventureBorder  PoL 0x00069976 / Buka 0x00013464
  advManager::DrawAdventureBorder  PoL 0x00069abb / Buka 0x000135a6

Retained PoL source
-------------------
SaveAdventureBorder:
  Keep `screenPixel`; literal Buka `screen` is not a VC4.2 invariant.

DrawAdventureBorder:
  Port Buka's function-scope declaration structure and delayed assignments:
    u8* savedPixels;
    u8* screenPixel;
    i32 row;
  Keep the PoL `screenPixel` spelling instead of Buka `screen`.

Complete clean matrices
-----------------------
Save name:
  build/save-adventure-border-name-manifest.json
  build/source-variant-batch/save-adventure-border-name/results.csv
  screenPixel:       100.000000%, 325 bytes, 16/16 blocks, 8/8 relocs
  screen:             99.709305%, 325 bytes, same topology/relocations
  screenPixelValue:  100.000000%, same closure

Draw structure:
  build/draw-adventure-border-structure-manifest.json
  build/source-variant-batch/draw-adventure-border-structure/results.csv
  PoL initialized locals after guards: 100.000000%, 308 bytes,
                                      18/18 blocks, 6/6 relocs
  Buka declarations before guards:    100.000000%, same closure

Draw name with Buka structure fixed:
  build/draw-adventure-border-buka-structure-name-manifest.json
  build/source-variant-batch/draw-adventure-border-buka-structure-name/results.csv
  screenPixel:       100.000000%, 308 bytes, 18/18 blocks, 6/6 relocs
  screen:             99.691360%, 308 bytes, same topology/relocations
  screenPixelValue:  100.000000%, same closure

Disposition: matching-decomp structural port for DrawAdventureBorder only;
both Buka local-name differences are compiler/version-specific. No gameplay
or shipped-byte change.
*/
