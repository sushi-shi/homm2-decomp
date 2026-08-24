/*
Function: combatManager::SetupAdjacencyArray
PoL RVA: 0x0009089d
Buka RVA: 0x0002683f

Retained mixed source
---------------------
  Buka structure:
    direction, fromHex, and rowIndex are function-scope locals;
    rowIndex is assigned at the outer-loop head.
  Buka name retained:
    sourceHex -> fromHex.
  PoL names retained:
    destinationHex (not Buka toHex), rowIndex (not Buka row).

Measured structural states
--------------------------
  original PoL scope/names:       100.000000%, 515 bytes, 41/41 blocks,
                                  ordered 7/7 relocs
  full exact-Buka scope/names:     99.9103%, 515 bytes, 41/41 blocks,
                                  ordered 7/7 relocs
  Buka scope + PoL names:          100.000000%, same closure
  retained scope + fromHex:        100.000000%, same closure

Complete identifier product
---------------------------
Manifest: build/setup-adjacency-buka-names-manifest.json
Results: build/source-variant-batch/setup-adjacency-buka-names/results.csv
Coverage: 125/125 clean source shapes from all collision-safe combinations of
destinationHex/sourceHex/rowIndex with Buka toHex/fromHex/row plus conservative
controls. In the permuter's clean compilation orbit:
  sourceHex -> fromHex alone:       ties the best baseline (99.850426%)
  destinationHex -> toHex alone:    99.649574%
  rowIndex -> row alone:             99.799150%
  all three Buka names:              99.760680%

The clean matrix score differs from the normal mainline object because the
permuter occupies a different legitimate CMBTMGR TU-state orbit; the paired
relative name result is consistent, and the retained normal build is audited
exact.

Disposition: matching-decomp structural/name port with compiler-specific PoL
names for two slots; no gameplay or shipped-byte change.
*/
