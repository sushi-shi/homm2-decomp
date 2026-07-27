/*
advManager::ValidMove, retail RVA 0x0000fa15, size 0x4f2.

This file records reviewed structural and permutation matrices. It is evidence,
not compiled source.

Fresh residual audit
--------------------

The live function has the retail 1266-byte size, 57/57 exact instruction-count
blocks, matching 60-byte frame and saved registers, and 9/9 relocation counts.
Relocation-masked assembly is identical. The raw clean object has two
accumulator-owner swaps:

    candidate                              retail
    mov centerY_n, ecx                     mov directionY_h, ecx
    add directionY_h, ecx                  add centerY_n, ecx

    mov m_mapOriginY, eax                  mov m_cursorMapY, eax
    add m_cursorMapY, eax                  add m_mapOriginY, eax
    inc eax                                inc eax

Forest trial 22 fixes the local first span and reproduces the official
99.994170% current-hash MAX. The four bytes in the member-field span remain.

Final south-cell accessor ownership
-----------------------------------

The final neighbor lookup was expressed through each semantically equivalent
fullMap API ownership:

    m_mapData->GetCell(x, y)
    m_mapData->Row(y) + x
    m_mapData->Cell(southNeighborCell_g, x, y)

A clean structural screen retained the 57-block CFG for all three forms. They
were therefore crossed with clean plus 50 top-of-TU forest states: 153/153
compiled in 162.10 seconds.

    GetCell:               99.994170%, size 1266, 57/57 blocks
    Row pointer:           97.927120%, size 1272, 56/57 exact blocks
    Cell reference output: 95.670555%, size 1278, 55/57 exact blocks

The alternate APIs form real lower islands but neither restores the final
member load order.

Artifacts:
  build/validmove-south-cell-accessor-axes-20260727.json
  build/validmove-south-cell-accessor-clean-manifest-20260727.json
  build/match-variants/validmove-south-cell-accessor-clean-20260727
  build/validmove-south-cell-accessor-states-manifest-20260727.json
  build/match-variants/validmove-south-cell-accessor-states-20260727

Earlier water-neighbor accessor ownership
-----------------------------------------

Historical exact source used a non-semantic OR_STEER around directionY_h at
the earlier vertical-water GetCell call. It was not reintroduced. Instead,
each horizontal and vertical neighboring access independently used:

    m_mapData->GetCell(x, y)
    m_mapData->Row(y)[x]

The complete 2 * 2 * (clean + 50 states) matrix compiled 204/204 variants in
221.03 seconds. All four forms are byte-identical in every corresponding
state. Trials 22, 35, and 38 reproduce 99.994170%; none closes the member span.
The legitimate accessor choice cannot replace the historical compiler-state
perturbation.

Artifacts:
  build/validmove-water-neighbor-accessor-axes-20260727.json
  build/validmove-water-neighbor-accessor-clean-manifest-20260727.json
  build/match-variants/validmove-water-neighbor-accessor-clean-20260727
  build/validmove-water-neighbor-accessor-states-manifest-20260727.json
  build/match-variants/validmove-water-neighbor-accessor-states-20260727

Final Y literal ownership
-------------------------

The prior arithmetic-tree matrix covered every tree with 1 retained at the
right-hand member branch. The remaining literal-owner trees were permuted:

    (m_cursorMapY + m_mapOriginY) + 1
    (m_cursorMapY + 1) + m_mapOriginY
    (m_mapOriginY + 1) + m_cursorMapY
    1 + (m_cursorMapY + m_mapOriginY)
    1 + (m_mapOriginY + m_cursorMapY)

All five arms received clean plus the same 50 top forest states: 255/255
compiled in 271.42 seconds. Every tree is byte-identical in each corresponding
state. Trials 22, 35, and 38 remain 99.994170%, size 1266, with 57/57 blocks
and 9/9 relocations.

Artifacts:
  build/validmove-south-y-literal-ownership-axes-20260727.json
  build/validmove-south-y-literal-ownership-manifest-20260727.json
  build/match-variants/validmove-south-y-literal-ownership-20260727

Disposition
-----------

The authored source is restored. No generated declarations, configuration
change, accessor substitution, or arithmetic tree is retained. The durable
current-hash MAX remains 99.994170%. Exact closure is still known historically,
but the only known selector was non-semantic steering; every legitimate local
tree and available fullMap accessor ownership is now exhausted.
*/
