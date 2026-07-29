/*
FightValueOfStack relational and layered declaration-forest search

Target: SOURCE/PHILAI philAI::FightValueOfStack, RVA 0x0003fed2.

The live function already has retail size (3064 bytes), all 72 relocation
owners, and an exact 147-block CFG.  Its remaining instruction differences
were confined to operand orientation at:

    spellCount > scoreIndex15
    armyValue < spellValueMap

A reviewed 2 x 2 source matrix tested both relational spellings independently
against the clean TU state and 50 declaration-forest states.

Artifacts:

    build/fight-value-relational-axes.json
    build/fight-value-relational-manifest.json
    build/source-variant-batch/fight-value-relational/results.json

All 204 candidates completed without truncation.  No exact closure appeared;
the source spellings were often byte-identical within a state.  Forest trial 6
from seed 1213156658 improved the target from 99.827774% to 99.913890% and
reduced the retail byte delta from six to three while preserving exact size,
topology, and relocations:

    build/fight-value-trial6-summary.json
    build/tu-state-noise/fight-value-trial6/

Because this was a near-closing state, a second independent 50-trial forest
(seed 20260729) was layered on trial 6:

    build/fight-value-layer2-summary.json
    build/tu-state-noise/fight-value-layer2/

The layered run completed all trials and observed six distinct target states.
Its best state again scored 99.913890% with a three-byte delta, exact
147-block graph, retail size, and unchanged ordered relocation identity.  It
did not close the remaining inner-loop comparison.

Disposition: measured compiler-state residual.  Source remains unchanged.  A
deinline is contradicted by the exact call graph, size, relocations, and CFG;
the differing block contains only the local loop comparison, not a call or an
/Ob1 inline-accessor continuation.
*/
