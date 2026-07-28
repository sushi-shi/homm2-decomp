/*
 * UNTRIED FlipIconToBitmap decoder-control structures.
 *
 * The current decoder already has the exact retail CFG, so these are fallback
 * islands only after the remaining setup/literal ownership products are
 * exhausted.  They must not displace a 76/76 exact graph merely for a fuzzy
 * increase.
 *
 * Remaining large structures:
 *   1. explicit positive-command early continue, with zero-row handling first;
 *   2. explicit if/else between literal and row transition;
 *   3. structured solid/dim dispatch with no do_fill goto;
 *   4. one shared solid/recolor inline helper instead of a label;
 *   5. switch-like opcode classification with the same semantic phase order;
 *   6. a multiline-macro-shaped do/while(0) around only solid/dim dispatch;
 *   7. command read through a narrow inline accessor, crossed with all above;
 *   8. row-transition helper owning s_right, s_y, s_row, and pitch;
 *   9. complete explicit decoder with no helper boundaries;
 *  10. sibling-derived FlipIconToBitmapYModify phase order with clipping
 *      dialect differences left explicit.
 */

#if 0
// Early row transition.
if (command == 0) {
    currentX = s_right;
    s_y++;
    s_row += pitch;
    continue;
}

// Positive literal handler.
if (command > 0) {
    // existing complete literal geometry
    currentX -= command;
    s_run = command;
    continue;
}

// Negative command handler remains below.
#endif
