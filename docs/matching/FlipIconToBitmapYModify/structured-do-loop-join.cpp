/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: structured literal joins.
 *
 * Two zero-goto spellings based only on nested if/else blocks were rejected:
 *
 *   shared copy tail: 91.739944%, 1443 bytes, 58/59 blocks;
 *   duplicated loops: 72.214480%, 1670 bytes, 63/59 blocks.
 *
 * Splitting the two horizontal rejection predicates into separate early
 * continues produced 59 blocks, but not retail topology: labeled-edge delta
 * 104, predecessor delta 33, flow-kind delta 11, target-shift delta 6.
 *
 * A scoped do/while(0) preserves the two semantic joins without source
 * labels. Direct paths break to the shared copy tail; clipped paths fall
 * through the shared s_srcSkip publication first. `break` and `continue`
 * spellings inside this constant-false loop are byte-identical.
 *
 * Clean 2 declaration orders x 2 join exits:
 *
 *   build/iconf2by-do-loop-axes-20260727.json
 *   build/match-variants/iconf2by-do-loop-clean-20260727/
 *
 * All four forms have the exact retail CFG: 59/59 blocks with zero edge,
 * predecessor, flow-kind, and target-shift deltas.
 *
 * The complete 2 x 2 x (clean + 50 top-state) Cartesian matrix likewise
 * preserves exact topology:
 *
 *   build/iconf2by-do-loop-states-manifest-20260727.json
 *   build/match-variants/iconf2by-do-loop-states-20260727/
 *
 * With right-before-bottom clip stores, the strongest state is 92.450400%,
 * 1420 bytes, and 144/144 relocation count. Forward-relative and historical
 * flipped declaration orders, and break/continue, collapse to the same
 * strongest object.
 *
 * Replaying the previous bottom-before-right orbit recovers the historical
 * best object exactly:
 *
 *   build/iconf2by-do-loop-previous-orbit-manifest-20260727.json
 *   build/match-variants/iconf2by-do-loop-previous-orbit-20260727/
 *
 * Trials 10 and 19 reach 93.605896%, 1420 bytes, 144/144 relocations, exact
 * 59/59 CFG, and target text hash 6c176f78ade4bf3c. That text hash is
 * identical to the old two-goto best island. The structured source therefore
 * removes the labels without sacrificing the strongest known compiler orbit.
 *
 * A stale configured normalized object briefly reported exact topology for
 * an unrelated guard-continue spelling. Fresh disposable compiles rejected
 * it at 58/59 blocks; the matrices above are the authoritative evidence.
 */

void FlipIconToBitmapYModify_structured_do_loop_join_attempt() {
}
