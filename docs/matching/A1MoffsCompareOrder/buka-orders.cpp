/*
Cross-version class: a1/moffs equality operand order
PoL targets:
  UpdateAppSpecificMenus  RVA 0x0009f9c6
  game::SetMapSize       RVA 0x000848bf

Retained Buka source
--------------------
    if (hMenu == hmnuAdv)
    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized)

Complete clean VC4.2 matrices
----------------------------
UpdateAppSpecificMenus:
  axes: build/update-app-menu-a1-order-axes.json
  results: build/source-variant-batch/update-app-menu-a1-order/results.json
  coverage: 2/2; both arms 92.941180%, 41 bytes, 4/4 blocks, 2/2 relocs.

SetMapSize:
  axes: build/set-map-size-a1-order-axes.json
  results: build/source-variant-batch/set-map-size-a1-order/results.json
  coverage: 4/4; every arm 95.789474%, 225 bytes, 11/11 blocks,
            23/23 relocs.

Current-hash exact islands
--------------------------
UpdateAppSpecificMenus:
  build/update-app-menu-a1-recorded-state-summary.json
  forest/top trial 3; 100.000000%, 42 bytes, 4/4 blocks, ordered 2/2
  relocs; MAX 100 retained for hash e849be9940a9.

SetMapSize:
  build/set-map-size-a1-recorded-state-summary.json
  forest/top trial 14; 100.000000%, 227 bytes, 11/11 blocks, ordered 23/23
  relocs; MAX 100 retained for hash df6a6d4f3de3.

Disposition: invariant matching decomp; source port; no gameplay or shipped-byte
change established by either equality reorder.
*/
