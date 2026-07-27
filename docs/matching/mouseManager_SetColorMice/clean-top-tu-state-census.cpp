/*
mouseManager::SetColorMice(int), retail RVA 0x000ca230, size 0x191.

This file records an unchanged-source compiler-state experiment. It is
evidence, not compiled source.

Fresh structural evidence
-------------------------

After mouseManager::NewUpdate was reconstructed exactly, the clean
SetColorMice object scored 94.809525% at size 399. Its effective function
source was unchanged and retained an audited historical 100% MAX.

The fresh comparison found:

  - 14 candidate and 14 retail blocks with identical control-flow topology;
  - 13 exact-size blocks and one size-only prologue difference;
  - complete aligned external relocations;
  - register and input-lifetime differences in the prologue and around the
    saved cursor fields, with no semantic or CFG divergence.

No evidence-supported structural source change was therefore justified.

Unchanged-source census
-----------------------

The expansive declaration-forest census inserted probes at the top of the TU:

    python scripts/tu_state_noise.py \
        --source src/BASE/MOUSEMGR.cpp \
        --rva 0xca230 \
        --trials 50 \
        --insertion top \
        --state-summary build/mouse-setcolormice-top-states-20260727.json \
        --output build/tu-state-noise/mouse-setcolormice-top-20260727 \
        --retain-best \
        --record-max

The search observed three distinct byte/relocation states and stopped at the
first audited exact closure, trial 7. That island has retail size 401,
100.000000%, zero retail-byte delta, and complete ordered relocation identity.
Trials 1 through 6 all occupied a different 91.866670% state at size 399.

Disposition
-----------

The source was restored unchanged. The exact disposable island independently
reconfirms the existing 100% MAX and shows that the clean 94.809525% object is
a TU-state regression, not a structural defect. No generated probe declaration
or function is retained in reconstructed source.
*/
