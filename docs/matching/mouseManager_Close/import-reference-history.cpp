/*
mouseManager::Close, retail RVA 0x000c93f0: prior import-reference history.

Because the remaining divergence is exactly the order in which the compiler
loads DestroyIcon and DeleteObject into callee-saved registers, this disposable
state matrix tested whether earlier references to those imports influence the
target:

  - clean;
  - a pre-target function referencing DestroyIcon only;
  - a pre-target function referencing DeleteObject only;
  - a function referencing DestroyIcon then DeleteObject;
  - a function referencing DeleteObject then DestroyIcon.

Artifacts:

    build/mouse-close-import-history-axes-20260727.json
    build/mouse-close-import-history-manifest-20260727.json
    build/match-variants/mouse-close-import-history-20260727

All five target bodies were identical at 99.692310%, size 237, with complete
relocations. Import encounter order is not the state dimension selecting the
retail register assignment. The probe functions were discarded.
*/
