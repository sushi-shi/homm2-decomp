/*
ViewPuzzle coordinate-order and pointer-loop matrix

Target: SOURCE/ADVMGR advManager::ViewPuzzle, RVA 0x0006a1dd.

The live function has the retail size, the same 54 ordered relocation owners,
and an exact 39-block CFG.  The two differing blocks suggested these
semantically equivalent spellings:

    gpGame->m_ultimateArtifactX * PUZZLE_Y_ADJUST_X_FACTOR
        + gpGame->m_ultimateArtifactY * PUZZLE_Y_ADJUST_Y_FACTOR

    gpGame->m_ultimateArtifactY * PUZZLE_Y_ADJUST_Y_FACTOR
        + gpGame->m_ultimateArtifactX * PUZZLE_Y_ADJUST_X_FACTOR

and:

    pixelIterator < rowLimitAddress
    rowLimitAddress > pixelIterator

The reviewed 2 x 2 source product was crossed with the clean TU state and 50
declaration-forest states.

Artifacts:

    build/view-puzzle-axes.json
    build/view-puzzle-manifest.json
    build/source-variant-batch/view-puzzle/results.json

All 204 candidates completed without truncation.  No exact closure appeared.
The best island reproduced the retained 99.333336% score with retail size,
relocation count, and exact graph.  Swapping the coordinate expression was
byte-identical within the same state; the reversed pointer predicate did not
improve the result.

Disposition: measured residual.  The flat coordinate-order distinction is
compiler-erased in this parent orbit, and the pointer predicate alternative is
rejected for the tested parent and state family.  A future retry requires an
evidence-backed structural parent or a distinct state layer.
*/
