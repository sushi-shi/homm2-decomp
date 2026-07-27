/*
mouseManager::Close, retail RVA 0x000c93f0: cursor cleanup loop structure.

The current for-loop, an equivalent pre-tested while-loop, and a
count-known-nonzero do/while-loop were compiled as a complete three-arm
structural matrix. These are credible high-level spellings with the potential
to preserve the retail CFG while changing loop-lifetime scheduling.

Artifacts:

    build/mouse-close-loop-structure-axes-20260727.json
    build/mouse-close-loop-structure-manifest-20260727.json
    build/match-variants/mouse-close-loop-structure-20260727

All three structures emitted the identical 17-block, 237-byte body at
99.692310%, including the same reversed DestroyIcon/DeleteObject register
assignment. The direct for-loop source was restored. Together with the prior
pointer-lifetime matrix and the newer state/wrapper/history matrices, this
leaves no evidence-backed descendant to retain; the historical 99.692310% MAX
remains authoritative.
*/
