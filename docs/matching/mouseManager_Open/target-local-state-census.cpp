/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Target-local unchanged-source TU-state census, 2026-07-27.

The prior state matrices inserted their declaration forests at the top of
MOUSEMGR.cpp. Fresh evidence still showed exact 6/6 control flow, five
exact-size blocks, one size-only block, and complete relocations. The only
semantic-invariant code difference remained one shared ecx materialization for
the two 240 stores instead of retail's two immediate stores.

The same expansive 50-trial forest was therefore inserted immediately before
Open:

    python scripts/tu_state_noise.py \
        --source src/BASE/MOUSEMGR.cpp \
        --rva 0xc9350 \
        --trials 50 \
        --insertion target \
        --state-summary build/mousemgr-open-target-states-20260727.json \
        --output build/tu-state-noise/mousemgr-open-target-20260727 \
        --retain-best \
        --record-max

All 51 cells occupied one identical state at 93.325584%, size 145, with
complete ordered relocations. The source was restored. Combined with the
existing and new structural matrices, no evidence-backed descendant remains;
retain the direct stores and official 93.325584% MAX.
*/
