/*
mouseManager::Close, retail RVA 0x000c93f0, size 0xed.

This file records a target-local unchanged-source TU-state census. It is
evidence, not compiled source.

The earlier clean plus 50 declaration-forest census inserted its probes at the
top of MOUSEMGR.cpp. Fresh evidence still showed 17/17 exact blocks, aligned
relocations, and only two differing register-selection bytes: retail assigns
DestroyIcon to edi and DeleteObject to ebx, while the candidate reverses them.

To test whether parser state immediately preceding Close could select another
orbit, the same expansive forest was inserted at the target:

    python scripts/tu_state_noise.py \
        --source src/BASE/MOUSEMGR.cpp \
        --rva 0xc93f0 \
        --trials 50 \
        --insertion target \
        --state-summary build/mouse-close-target-states-20260727.json \
        --output build/tu-state-noise/mouse-close-target-20260727 \
        --retain-best \
        --record-max

All 51 cells emitted one identical state: 99.692310%, size 237, with complete
ordered relocations and a two-byte retail delta. The source was restored.
Target-local declaration state is therefore not the missing lever.
*/
