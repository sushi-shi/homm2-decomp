/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Peer-field initialization ownership, 2026-07-27.

The full retail store schedule reuses registers for three equal-value pairs:
saved-left/cursor-left, saved-top/cursor-top, and cursor-right/mouse-X. A new
complete 2 x 2 x 2 structural matrix tested whether each later member was
initialized from its already-written peer or directly from the corresponding
screen constant.

Artifacts:

    build/mousemgr-open-peer-field-ownership-axes-20260727.json
    build/mousemgr-open-peer-field-ownership-manifest-20260727.json
    build/match-variants/mousemgr-open-peer-field-ownership-20260727

All eight clean structures emitted the identical 93.325584%, 145-byte,
six-block object with complete relocations. The peer reads are folded to the
same register schedule before the two unmatched center-Y stores and expose no
new structural orbit. Direct constant ownership remains the clearest source.
*/
