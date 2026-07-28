/*
 * FlipIconToBitmap branch-owned literal and dim counters, 2026-07-27.
 *
 * Retail B51 does not eagerly copy the literal command, while B62 performs
 * that copy only on the fully visible clipped-literal branch.  Moving the
 * assignment to its semantic owner removed the sole excess relocation and
 * made three additional blocks size-exact.
 *
 * The 48-cell matrix independently crossed:
 *   - four literal-count scopes/initialization sites;
 *   - direct versus cached clip-right ownership;
 *   - four dim-counter test/mutation roles.
 *
 * Artifacts:
 *   build/iconf2b-branch-owner-islands-axes-20260727.json
 *   build/iconf2b-branch-owner-islands-manifest-20260727.json
 *   build/source-variant-batch/iconf2b-branch-owner-islands-20260727/
 *
 * Branch-owned literal initialization alone reached 68/76 exact blocks and
 * 81/81 relocations.  The strongest lower-fuzzy topology island also made
 * the copied dim counter own loop mutation:
 *
 *   84.103450%, size 1261/1265, 81/81 relocations,
 *   exact 76-block CFG, 69 exact plus seven size-only blocks.
 *
 * This was retained despite scoring below the higher-fuzzy branches because
 * it made both dim loops structurally exact.
 */

#if 0
u32 literalCount;
if (clipX <= (currentX - command) + 1) {
    literalCount = command;
    s_srcSkip = 0;
    break;
}

i32 dimCount = count;
s_dimDst = dimDestination;
if (static_cast<i32>(count) > 0) {
    s_loopCount = count;
    do {
        *dimDestination = s_dimPal[*dimDestination];
        dimDestination++;
        s_dimDst = dimDestination;
    } while (--dimCount != 0);
}
#endif
