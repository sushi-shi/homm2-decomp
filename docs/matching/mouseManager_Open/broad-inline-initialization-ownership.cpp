/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Broad inline initialization ownership, 2026-07-27.

Earlier matrices covered value-return helpers, four-bound/vertical helpers,
and single-store setters. This distinct matrix tested broader ownership:

  - direct member stores;
  - one inline helper owning both final center-Y stores;
  - one inline helper owning mouse-X, the two index zeros, cursor-bottom, and
    mouse-Y;
  - one inline helper owning the complete ten-store initialization sequence.

The four structures were compiled against the clean state and all 50 top-of-TU
declaration-forest states, completing 4 x 51 = 204 cells:

    build/mousemgr-open-broad-inline-axes-20260727.json
    build/mousemgr-open-broad-inline-states-manifest-20260727.json
    build/match-variants/mousemgr-open-broad-inline-states-20260727

All 204 cells emitted the identical 93.325584%, 145-byte object with 6/6
aligned blocks and complete relocations. Broad helper ownership is fully
normalized and does not make MSVC encode the two 240 stores independently.
Generated helpers were discarded and direct stores were restored.
*/
