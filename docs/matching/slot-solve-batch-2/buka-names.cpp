/*
Cross-version local-name batch 2
================================

Functions:
  combatManager::GetOutOfItMask PoL 0x000c2c0b / Buka 0x000179cf
  armyGroup::IsHomogeneous      PoL 0x0008c44b / Buka 0x00021a96

Retained mixed GetOutOfItMask source
------------------------------------
  PoL:  armyIndex2, bit1, flattened m_armies[0] + side * stride + index
  Buka: result, currentArmy

Structural matrix:
  build/get-out-of-it-mask-structure-manifest.json
  PoL flat address: 100.000000%
  Buka row address:  90.507460%

Complete 16/16 name products in both structures:
  build/get-out-of-it-mask-flat-names-manifest.json
  build/get-out-of-it-mask-row-names-manifest.json
  build/source-variant-batch/get-out-of-it-mask-{flat,row}-names/results.csv

Flat structure:
  PoL names:                                      100.000000%
  armyIndex2 + bit1 + result + currentArmy:       100.000000%
  idx + bitMask + result + currentArmy:            99.880600%
Row structure:
  best current-index/current-bit arms:             90.507460%
  literal Buka names:                              90.417910%

The retained normal build is exact at 233 bytes, 12/12 blocks, 0/0 relocs.

IsHomogeneous complete 4/4 name product
---------------------------------------
Manifest: build/is-homogeneous-buka-names-manifest.json
Results: build/source-variant-batch/is-homogeneous-buka-names/results.csv
  nRaces + last:      99.976746%
  numRaces + prev:    99.872090%
  either Buka name:   99.720930%
All arms are 334 bytes, 27/27 blocks, and 2/2 relocation counts. Branch
mnemonics and symbolic targets agree; the pre-existing instruction/slot
residual remains live and is not classified by this name matrix.

Disposition: matching-decomp port of GetOutOfItMask `result` and
`currentArmy` only. Its Buka row expression and other two names, plus both
IsHomogeneous names, are compiler/version-specific. No gameplay or shipped-
byte change.
*/
