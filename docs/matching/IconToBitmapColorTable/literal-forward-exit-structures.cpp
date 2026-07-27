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
 * A fourth no-do form nested the negative row predicate and deliberately
 * delayed the declaration of `right` until after row visibility was known.
 * It produced exactly the retained do/break bytes and ordered relocations:
 * text hash a5315c989795a12e, 82.295780% clean, size 1437, 89/91
 * relocations, exact 86-block flow, 67 exact blocks plus 19 size-only blocks.
 * Thus the emitted shape does not uniquely prove a macro.  It does establish
 * that a natural nested source form and a safe multiline-macro expansion
 * occupy the same compiler orbit.
 *
 * Artifacts:
 *   build/icon2bc-natural-literal-counter-axes.json
 *   build/match-variants/icon2bc-natural-literal-counter-clean/
 *   build/icon2bc-natural-literal-counter-states.json
 *   build/match-variants/icon2bc-natural-literal-counter-states/
 *   build/icon2bc-literal-forward-goto-clean.json
 *   build/match-variants/icon2bc-literal-forward-goto-clean/
 *
 * A subsequent source-era review identified an important alternative: while
 * do/while (0) would be unusual as ordinary function control flow, it is the
 * conventional safe wrapper for a multiline macro.  The related icon
 * blitters repeat this clipping diamond, so the wrapper may be evidence of a
 * reusable macro expansion rather than a source smell.  The later paired
 * expansion/invocation census in macro-era-reuse-boundaries.cpp proved that
 * this whole literal macro boundary is byte- and relocation-identical in all
 * 51 states.  Retain the stronger do/break structure.  The natural no-do
 * forms and byte-identical goto remain diagnostics only; no generated label
 * or goto is retained.
 */

void IconToBitmapColorTable_literal_forward_exit_structures_attempt() {
}
