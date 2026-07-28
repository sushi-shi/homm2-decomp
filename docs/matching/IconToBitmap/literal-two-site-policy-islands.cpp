/*
 * Forward literal-run two-site policy experiment, 2026-07-27.
 *
 * Retail IconToBitmap's literal tail has a distinct unclipped setup block before
 * the common memcpy body.  This tested one policy invocation in the unclipped
 * branch and a second invocation after clipped geometry selection:
 */
#if 0
if (clip == ICON_DRAW_NO_CLIP) {
    literalDestination = row + X;
    literalSource = s_src;
    literalCode;
} else {
    literalDestination = row + X;
    literalSource = s_src;
    // Select clipped count, destination, and source.
    if (count != 0)
        literalCode;
}
#endif

/*
 * This is not the older rejected form that duplicated literalCode into every
 * clipping branch.  Both consumers received a complete clean plus 50-state
 * declaration-forest census:
 *
 *   build/tu-state-noise/IconToBitmap-two-site-50/
 *   build/tu-state-noise/IconToBitmapColorTable-two-site-50/
 *
 * IconToBitmap produced 27 byte/relocation states.  Every representative had
 * the same 79/80 block skeleton: 47 exact, 15 size-only, nine target-shift,
 * eight flow-kind, and one missing block.
 *
 * IconToBitmapColorTable produced 22 states.  Every representative had the
 * same 89/86 skeleton: 15 exact, 14 size-only, 24 target-shift, 33 flow-kind,
 * and three extra blocks.  Its best fuzzy island was trial 49 at 57.187794%,
 * but topology remained unchanged.
 *
 * Disposition: reject this exact shared expansion.  The result does not imply
 * that the two TUs require unrelated developer-level blocks; it proves only
 * that duplicating the complete downstream policy at this shared boundary
 * gives the wrong structural orbit for the color-table pixel loop.  Preserve
 * the shared semantic block and search a narrower ownership boundary.
 */

void IconToBitmap_literal_two_site_policy_islands_attempt() {
}
