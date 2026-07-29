/*
GetClosestArmy indexing, comparison, and layered forest searches

Target: SOURCE/AI combatManager::GetClosestArmy, RVA 0x000c307d.

The retired OD_STEER expression left a non-idiomatic commuted subscript:

    &armyIndex2[m_armies[IDX(side)]]

The retained reconstruction restores conventional developer-facing indexing:

    &m_armies[IDX(side)][armyIndex2]

A complete address-expression matrix compared that form with the inherited
commuted subscript and direct pointer addition across the clean state and 50
declaration forests:

    build/get-closest-army-index-axes.json
    build/get-closest-army-index-manifest.json
    build/source-variant-batch/get-closest-army-index/results.json

All 153 candidates completed.  The three source forms were byte-identical
within each state; none closed.  Direct pointer addition was diagnostic only
and is rejected as an implausible reconstruction.

Forest trial 1 from seed 1213156658 improved the conventional source from
94.270270% to 99.441444%, with retail size (367 bytes), both relocations, the
exact nine-block graph, and a three-byte residual at the final best-value
comparison:

    build/get-closest-army-trial1-summary.json
    build/tu-state-noise/get-closest-army-trial1/

Both positive relational spellings were then tested against the same 51
states:

    closestValue29 > value19
    value19 < closestValue29

Artifacts:

    build/get-closest-army-compare-axes.json
    build/get-closest-army-compare-manifest.json
    build/source-variant-batch/get-closest-army-compare/results.json

All 102 candidates completed.  The compiler erased the source distinction in
every tested state.  A structural three-arm follow-up tested the positive if
and empty-if/else ownership with both natural negative predicates:

    build/get-closest-army-branch-axes.json
    build/get-closest-army-branch-manifest.json
    build/source-variant-batch/get-closest-army-branch/results.json

All 153 candidates completed.  Neither else-owned version improved the
positive if.  Finally, an independent 50-state forest (seed 20260729) was
layered on the near-closing trial-1 state:

    build/get-closest-army-layer2-summary.json
    build/tu-state-noise/get-closest-army-layer2/

The layered run observed five states but did not close; its best remained
99.441444%, exact size/relocations/topology, and a three-byte compare residual.

Disposition: retain conventional two-dimensional indexing.  The commuted
subscript and pointer addition provide no unique byte evidence, and the
negative-branch structures are rejected for this parent orbit.  The remaining
three bytes are a bounded compiler-state residual.
*/
