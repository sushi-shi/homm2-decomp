/*
GoodAdjacent semantic corrections and forward-exit matrices

Target: SOURCE/PHILAI philAI::GoodAdjacent, RVA 0x0003849d.

Retail constants establish three source corrections independently of compiler
shape:

    MAP_OBJECT_MONSTER  -> MAP_OBJECT_STONE_LITHS  (two sites)
    MAP_OBJECT_ARTIFACT -> MAP_OBJECT_WHIRLPOOL

The /Od frame census also places the loop direction in the bucket selected by
the local name dirIdx rather than directionIndex.  These changes raise the
clean current-hash score from 98.1523% to 98.375630%.  They are retained as
semantic and frame recovery even though the function remains structurally
open.

The two interaction exits were tested as reviewed structural alternatives:

    - nested continue, combined guard + continue, and combined guard + break;
    - direct forward gotos to a shared label;
    - a scoped do { ... } while (0) with the two exits expressed as break.

Artifacts:

    build/gadj2-axes.json
    build/gadj2-manifest.json
    build/source-variant-batch/gadj2/results.json
    build/good-adjacent-forward-exit-axes.json
    build/good-adjacent-forward-exit-manifest.json
    build/source-variant-batch/good-adjacent-forward-exit/results.json
    build/good-adjacent-do-break-axes.json
    build/good-adjacent-do-break-manifest.json
    build/source-variant-batch/good-adjacent-do-break/results.json

The initial three-arm matrix completed all 27 requested candidates.  Each
two-arm follow-up completed its full clean-plus-50-forest product (102
candidates apiece) without truncation.  No exact closure appeared.  The
do/break arm was byte-identical to the retained nested source.  The shared-goto
arm regressed to 96.345%; the retained island remains 98.375630%, 754 bytes
versus retail 744, 42/42 relocation owners, and 28 versus 26 blocks.

Disposition: retain the proven constants and dirIdx name.  The forward-exit
ownership remains a measured structural residual; the tested continue, break,
do/break, and shared-goto families do not reproduce the retail island.
*/
