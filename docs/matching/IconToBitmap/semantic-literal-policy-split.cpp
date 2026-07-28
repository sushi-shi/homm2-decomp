/*
 * Forward literal-run semantic policy split, 2026-07-27.
 *
 * One callback-shaped literal handler hid a real family distinction:
 *
 *   - IconToBitmap copies raw literal bytes with directly nested memcpy calls.
 *   - IconToBitmapColorTable selects clipped geometry, then maps every source
 *     byte through colorTable in one counted pixel loop.
 *
 * The retained abstraction therefore names two semantic Tier-3 handlers while
 * sharing decoder state, opcode dispatch, and the X/source/run tail:
 */
#if 0
RLE_HANDLER_FORWARD_COPY_LITERAL_RUN();
RLE_HANDLER_FORWARD_MAPPED_LITERAL_RUN();
#endif

/*
 * Raw-copy result:
 *
 *   build/tu-state-noise/IconToBitmap-copy-policy-50/
 *
 * The 50-trial census produced 18 byte/relocation states and two emitted CFG
 * partitions.  Fifteen states have the exact retail 80-block labeled graph;
 * three have an 81-block divergent graph.  Trial 40 is both the fuzzy winner
 * (74.739010%) and topology winner: canonical graph hashes are identical,
 * labeled-edge delta is zero, and predecessor delta is zero.
 *
 * Mapped do/while geometry descendant:
 *
 *   build/tu-state-noise/IconToBitmapColorTable-mapped-policy-50/
 *
 * Its 22 states have 87 or 88 blocks versus retail's 86.  Trial 11 is the
 * best observed island at 76.887320%; the canonical comparison reports one
 * extra block, labeled-edge delta 25, and predecessor delta nine.  This branch
 * remains credible historical macro evidence, but it is not yet the retail
 * CFG and needs a narrower ownership change.
 *
 * Disposition: retain the semantic policy split and the raw-copy handler.
 * Continue structure recovery inside the mapped handler; do not collapse the
 * two policies back into a callback solely to make their source look uniform.
 */

void IconToBitmap_semantic_literal_policy_split_attempt() {
}
