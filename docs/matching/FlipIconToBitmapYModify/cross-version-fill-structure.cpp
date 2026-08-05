/*
 * FlipIconToBitmapYModify, RVA 0x000d9ce0, retail size 1421.
 * Exact Gold/Buka fill-phase subtraction, 2026-08-05.
 *
 * The exact later source uses direct row predicates and retains the historical
 * four-quadrant solid-run fill spelling. The PoL reconstruction instead owns
 * row visibility in `IconRowVisible` and uses a compact fill body. Because the
 * first live size-only blocks occur in this phase, both whole structures were
 * crossed with clean plus 50 forest/top compiler states.
 *
 * All 102/102 cells completed:
 *
 *   build/iconf2by-cross-version-fill-structure-axes.json
 *   build/iconf2by-cross-version-fill-structure-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-fill-structure/results.json
 *
 * The compact PoL parent reached 94.008040%, size 1419, exact 59/59 CFG, and
 * 144/144 relocations at trials 10 and 19. The later direct-quadrant arm was a
 * distinct lower island: best 93.190346%, size 1399, exact CFG, but 145/144
 * relocations. It did not reproduce the durable 94.4102% MAX.
 *
 * Disposition: reject this later phase as a PoL parent and retain the compact
 * fill body. The result only rejects the full direct-predicate/quadrant product
 * in the current parent orbit; it does not reject other later-derived setup,
 * dim-run, or literal-tail ownership after an evidence-backed parent change.
 *
 * The required parent-orbit follow-up independently crossed the dim-row
 * predicate, solid-fill body, and literal-row predicate. All 2 x 2 x 2 x 51
 * = 408 cells completed:
 *
 *   build/iconf2by-cross-version-row-phase-cartesian-axes.json
 *   build/iconf2by-cross-version-row-phase-cartesian-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-row-phase-cartesian/results.json
 *
 * Direct dim visibility was byte-neutral in the strongest compact-fill state.
 * Every arm containing the later quadrant fill or later literal visibility
 * remained below the compact/helper parent. The complete later row-phase
 * product therefore does not expose a hidden structural orbit in this source
 * parent; the same narrow disposition applies.
 *
 * First-divergence setup follow-up:
 *
 * A second complete 2 x 51 matrix tested the exact later setup prefix as one
 * structural product: raw-entry publication, later subtraction grouping,
 * later operand orders, clipR-before-clipB, and row-product order.
 *
 *   build/iconf2by-cross-version-setup-prefix-axes.json
 *   build/iconf2by-cross-version-setup-prefix-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-setup-prefix/results.json
 *
 * The later prefix preserved size 1419, exact topology, and 144/144
 * relocations, but peaked at 92.852550%; the current prefix reached
 * 94.008040% in the same states. Disposition: reject the exact later prefix
 * as a unit. Previously audited individual setup axes remain reusable only
 * after a different evidence-backed structural parent changes their orbit.
 *
 * Later-parent recovery, performed after that parent changed:
 *
 * The exact later dim-command/fill join was then tested as a whole against the
 * old PoL phase. All 2 x 51 = 102 cells completed:
 *
 *   build/iconf2by-cross-version-negative-command-phase-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-negative-command-phase/results.json
 *
 * It raised the best island from the old 94.4102% durable MAX to 96.332440%,
 * size 1414, exact 59/59 CFG, and 143/144 relocations. This later control-flow
 * parent was retained. Its actual-source 50-state replay reproduced the same
 * score at trials 25 and 39:
 *
 *   build/iconf2by-later-negative-retained-state-summary.json
 *   build/tu-state-noise/iconf2by-later-negative-retained/manifest.json
 *
 * The exact later branch-owned literal tail was byte-identical when clean and
 * slightly lower in the high island (96.305630%). The complete 102-cell result
 * therefore retained the PoL compact literal tail:
 *
 *   build/iconf2by-cross-version-literal-tail-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-literal-tail/results.json
 *
 * Relocation multiplicities then exposed a later-only storage change. The
 * later parent referenced `s_spanCount` 9 times and `s_dimLen` 7 times, while
 * retail requires 16 and 2. Restoring PoL count ownership without changing the
 * later CFG raised the best island to 97.485250%, size 1420, and 145/144
 * relocations. All 102 cells completed and the PoL storage mapping was retained:
 *
 *   build/iconf2by-cross-version-dim-storage-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-dim-storage/results.json
 *   build/iconf2by-pol-storage-retained-state-summary.json
 *   build/tu-state-noise/iconf2by-pol-storage-retained/manifest.json
 *
 * Replaying the exact later setup prefix on this new parent overturned its
 * earlier parent-local rejection. The complete 102-cell matrix raised MAX to
 * 98.471850%, size 1420, exact CFG, and 145/144 relocations at states 25 and
 * 39. The later raw-entry/setup expression structure was retained:
 *
 *   build/iconf2by-cross-version-setup-prefix-new-parent-manifest.json
 *   build/source-variant-batch/iconf2by-cross-version-setup-prefix-new-parent/results.json
 *   build/iconf2by-later-prefix-pol-storage-retained-state-summary.json
 *   build/tu-state-noise/iconf2by-later-prefix-pol-storage-retained/manifest.json
 *   build/tu-state-noise/iconf2by-later-prefix-pol-storage-recorded-max/manifest.json
 *
 * Last-mile follow-ups on that retained parent were complete and negative:
 *
 * - Three fill comparison orders crossed 8 x 51 = 408 cells; all eight arms
 *   were byte-identical within each state.
 * - Direct `s_run`, signed-local, and unsigned-local fill ownership crossed
 *   3 x 51 = 153 cells. The direct form retained 98.471850%; the signed local
 *   peaked at 96.026810% with 144/144 relocations.
 * - An unsigned local with explicitly signed arithmetic crossed 102 cells and
 *   peaked at 98.445040%, size 1414, 143/144 relocations.
 * - The complete PoL compact fill body crossed 102 cells on the new parent and
 *   remained below the later direct-quadrant form.
 *
 *   build/iconf2by-fill-relational-order-manifest.json
 *   build/iconf2by-cross-version-fill-local-signed-manifest.json
 *   build/iconf2by-fill-count-signed-arithmetic-manifest.json
 *   build/iconf2by-pol-compact-fill-new-parent-manifest.json
 *
 * Current disposition: retain later setup and dim/fill control flow with PoL
 * storage and literal-tail ownership. The best disposable island remains one
 * byte short with one extra `s_run` relocation; it is exact in CFG but not an
 * audited exact closure. Further compiler-shape steering is deferred under the
 * bounded last-mile policy.
 */

#if 0
if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
    && s_x - s_run + 1 >= clipX && s_x <= s_clipR) {
    if (s_x <= s_clipR) {
        if (s_x - s_run + 1 >= clipX)
            memset(s_row + s_x - s_run + 1, s_color, s_run);
        else
            memset(s_row + clipX, s_color, s_x - clipX + 1);
    } else {
        if (s_x - s_run + 1 >= clipX)
            memset(s_row + s_x - s_run + 1, s_color, s_clipR - (s_x - s_run));
        else
            memset(s_row + clipX, s_color, clipW);
    }
}
#endif
