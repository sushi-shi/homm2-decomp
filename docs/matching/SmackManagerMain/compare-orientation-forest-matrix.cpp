/*
SmackManagerMain compare-orientation forest matrix

Target: SOURCE/SMACKMGR ?SmackManagerMain@@YIXXZ, RVA 0x0000126d.

The live function has the retail size, an exact 180-block CFG, and the same
relocation count.  The compact byte diff isolates two semantically commutative
choice comparisons:

    if (campaignChoice4 == gbCampaignSideChoice)
    if (expansionChoice1 != xLastChoice)

The reviewed matrix tested both operand orientations at both sites as a
complete 2 x 2 product.  Each source arm was compiled against the clean TU
state and 50 declaration-forest states.

Artifacts:

    build/smack-manager-compare-axes.json
    build/smack-manager-compare-manifest.json
    build/source-variant-batch/smack-manager-compare/results.json

The same complete product was replayed with seed 777, which contains the
previously retained near-closing state:

    build/smack-manager-compare-seed777-manifest.json
    build/source-variant-batch/smack-manager-compare-seed777/results.json

Both 204-candidate products completed without truncation.  Neither found an
exact closure.  Their best topology retained the exact graph but differed in
one size-only block, retail size, and ordered relocation offsets.  The
historical exact arm used OD_STEER and is therefore migration debt, not a
source candidate that may be restored.

Disposition: measured residual.  The four flat compare orientations are
rejected only in these two tested compiler-state families; retrying them
requires an evidence-backed structural parent or a distinct state layer.
*/
