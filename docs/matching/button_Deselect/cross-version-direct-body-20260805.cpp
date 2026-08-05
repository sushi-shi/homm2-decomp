/*
 * Gold/Buka ownership check for button::Deselect, PoL RVA 0x000ddd10.
 *
 * The exact later source owns the standalone Deselect body directly. PoL's
 * previous reconstruction made the public body a wrapper around the private
 * inline helper needed by Main. The direct public body is now retained while
 * the helper remains the proven inline source for Main.
 *
 * Clean compilation and a complete four-spelling matrix crossed with clean
 * plus 50 forest/top states produced 204/204 cells. Compound/assignment flag
 * clearing and qualified/unqualified Draw calls were all byte-identical:
 *
 *   build/button-deselect-shape-manifest.json
 *   build/source-variant-batch/button-deselect-shapes/results.json
 *   build/tu-state-noise/button-deselect-cross-version
 *
 * Every state remained 99.777780%, size 131, exact 3/3 topology, with the
 * complete ordered 4/4 relocation stream and the same two-byte instruction
 * schedule residual. Disposition: retain the stronger mainline standalone
 * ownership; the residual remains live and the tested spelling surface is
 * exhausted for this structural parent.
 *
 * An independent seed-20260806 expansion completed another 50 forest/top
 * trials and emitted one identical state in all 51 baseline/trial cells. It
 * therefore found no target-local exact closure:
 *
 *   build/button-deselect-seed-20260806-state-summary.json
 *   build/tu-state-noise/button-deselect-seed-20260806/
 *
 * Later-to-current subtraction also identified a surviving source-only
 * distinction in the redraw tail: Gold/Buka spells both coordinate sums as
 * owner-position plus widget-position, while PoL currently spells them in the
 * opposite operand order. A complete four-arm matrix tested both paired
 * spellings and each mixed coordinate against clean plus 50 forest/top states:
 *
 *   build/button-deselect-cross-version-coordinate-order-axes.json
 *   build/button-deselect-cross-version-coordinate-order-manifest.json
 *   build/source-variant-batch/button-deselect-cross-version-coordinate-order/results.json
 *
 * All 204/204 cells completed and were byte-identical at 99.777780%, size
 * 131, exact 3/3 topology, and 4/4 ordered relocations. The coordinate operand
 * order is therefore erased in this structural parent and cannot account for
 * the two-byte scheduling residual. Disposition: retain the current spelling;
 * do not expand state trials for this invariant axis.
 */

H2_ENUM_RETURN(MessageDispatchResult, i16) button::Deselect(tag_message& msg) {
    if (!HAS(m_flags, WIDGET_FLAG_SELECTED))
        return MESSAGE_DISPATCH_CONTINUE;
    m_flags &= ~WIDGET_FLAG_SELECTED;
    Draw();
    // Remaining message construction is unchanged in the retained source.
}
