/*
Cross-version local-name batch 4
================================

Function                                  PoL RVA     Buka RVA
resourceManager::GetBitmap                0x0c82b0    0x0b81c0
resourceManager::GetMIDIWrap              0x0c8610    0x0b85b0
game::GiveArmy                            0x080e6c    0x05ba0c

Complete clean identifier matrices
----------------------------------
GetBitmap:
  build/get-bitmap-file-id-manifest.json
  build/source-variant-batch/get-bitmap-file-id/results.csv
  id:       100.000000%, 151 bytes, 8/8 blocks, 5/5 relocs
  fileId:    99.775510%, same size/topology/relocations
  idValue:  100.000000%, same closure

GetMIDIWrap:
  build/get-midi-wrap-file-id-manifest.json
  build/source-variant-batch/get-midi-wrap-file-id/results.csv
  id:       100.000000%, 151 bytes, 8/8 blocks, 5/5 relocs
  fileId:    99.795920%, same size/topology/relocations
  idValue:  100.000000%, same closure

GiveArmy:
  build/give-army-swap-manifest.json
  build/source-variant-batch/give-army-swap/results.csv
  tmp:       100.000000%, 252 bytes, 19/19 blocks, 0/0 relocs
  swap:       99.750000%, same size/topology/relocations
  tmpValue:  100.000000%, same closure

Disposition: no port. Buka's `fileId` and dead-slot `swap` spellings are
compiler/version-specific. PoL retains its exact `id` and `tmp` names. No
gameplay or shipped-byte change.
*/
