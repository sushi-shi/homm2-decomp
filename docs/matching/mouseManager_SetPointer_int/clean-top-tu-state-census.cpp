/*
mouseManager::SetPointer(int), retail RVA 0x000c9630, size 0x405.

This file records an unchanged-source compiler-state experiment. It is
evidence, not compiled source.

Fresh structural evidence
-------------------------

After mouseManager::NewUpdate was reconstructed exactly, the clean SetPointer
object scored 98.402250% at size 1033. Its effective function source was
unchanged and retained an audited historical 100% MAX.

The fresh comparison found:

  - 36 candidate and 36 retail blocks with identical control-flow topology;
  - 35 exact-size blocks and one size-only difference in the nested bitmap
    conversion loop;
  - complete aligned external relocations;
  - the sole instruction divergence was ownership of the row and column loop
    values between eax and esi, plus one extra register-zeroing instruction.

No evidence-supported structural source change was therefore justified.

Unchanged-source census
-----------------------

The expansive declaration-forest census inserted probes at the top of the TU:

    python scripts/tu_state_noise.py \
        --source src/BASE/MOUSEMGR.cpp \
        --rva 0xc9630 \
        --trials 50 \
        --insertion top \
        --state-summary build/mouse-setpointer-top-states-20260727.json \
        --output build/tu-state-noise/mouse-setpointer-top-20260727 \
        --retain-best \
        --record-max

The search observed eight distinct byte/relocation states and stopped at the
first audited exact closure, trial 22. That island has retail size 1029,
100.000000%, zero retail-byte delta, and complete ordered relocation identity.
Trial 3 was the adjacent one-byte island at 99.962400%.

Disposition
-----------

The source was restored unchanged. The exact disposable island independently
reconfirms the existing 100% MAX and shows that the clean 98.402250% object is
a TU-state regression, not a structural defect. No generated probe declaration
or function is retained in reconstructed source.
*/
