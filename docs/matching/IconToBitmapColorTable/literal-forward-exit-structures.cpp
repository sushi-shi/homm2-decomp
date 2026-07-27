/*
 * IconToBitmapColorTable, RVA 0xd32a0:
 * natural literal-run forward-exit structures and dim-loop counters.
 *
 * Retail blocks B63-B79 contain forward exits to the shared literal tail.
 * There is no loop backedge or emitted loop condition.  The retained
 * do { ... break; } while (0) must therefore not be read as evidence that the
 * developer wrote a loop.
 *
 * A clean structural matrix replaced that wrapper with three no-do forms:
 *
 *   1. cnt = 0 followed by one combined positive visibility/overlap guard;
 *   2. cnt = 0 followed by nested positive row and overlap guards;
 *   3. the negative predicates as an if / else-if / else chain.
 *
 * Each structure was crossed independently with both decrement placements in
 * each dim loop:
 *
 *   count--; } while (count != 0);
 *   } while (--count != 0);
 *
 * and equivalently for cn, for four counter combinations per structure.
 *
 * Clean results:
 *
 *   combined positive guard:
 *     81.145540%, size 1435, 90/91 relocations, 85/86 blocks,
 *     six flow-kind mismatches and one missing block
 *
 *   nested positive guards:
 *     81.673706%, size 1425, 88/91 relocations, 85/86 blocks,
 *     six flow-kind mismatches and one missing block
 *
 *   negative if/else chain:
 *     82.201880%, size 1437, 89/91 relocations, exact 86-block flow,
 *     65 exact-size and 21 size-only blocks
 *
 * The first two structures contradict the retail topology and were pruned
 * after the complete clean matrix.  The surviving negative chain received a
 * complete four-counter-shape by clean-plus-50 forest census: 204/204
 * candidates compiled.  All four counter combinations were byte-identical in
 * every state.  Its best state was trial 14 at 83.760570%, size 1426,
 * 89/91 relocations, below the retained 83.9836% MAX.
 *
 * A forward-goto diagnostic replaced the two breaks with gotos to a label
 * immediately before gCTCnt publication.  It compiled byte-for-byte
 * identically to the retained do/while form: 82.295780% clean, size 1437,
 * 89/91 relocations, with the same exact 86-block flow.  This proves that the
 * do/while is only a source-level spelling of a forward-exit diamond.  It
 * does not establish whether retail source used either spelling.
 *
 * Artifacts:
 *   build/icon2bc-natural-literal-counter-axes.json
 *   build/match-variants/icon2bc-natural-literal-counter-clean/
 *   build/icon2bc-natural-literal-counter-states.json
 *   build/match-variants/icon2bc-natural-literal-counter-states/
 *   build/icon2bc-literal-forward-goto-clean.json
 *   build/match-variants/icon2bc-literal-forward-goto-clean/
 *
 * Disposition: retain the do/while spelling because it preserves the stronger
 * audited MAX orbit without retaining a generated label or goto.  Treat it as
 * a single-pass forward-exit scope, never as a reconstructed runtime loop.
 */

void IconToBitmapColorTable_literal_forward_exit_structures_attempt() {
}
