/*
 * widget::Main structural experiment: recover two developer-level operations
 * as inline helpers instead of leaving their bodies owned by the dispatcher.
 *
 * The first helper factors the dim-region operation duplicated by DRAW and
 * SET_FLAGS:
 *
 *     static inline void DimWidgetArea(widget* target) {
 *         i16 x = target->m_x
 *             + static_cast<i16>(target->m_owner->m_posX);
 *         i16 y = target->m_y
 *             + static_cast<i16>(target->m_owner->m_posY);
 *         DimBitmapArea(gpWindowManager->m_screen, x, y,
 *                       target->m_width, target->m_height, 0);
 *     }
 *
 * Replacing both repeated bodies with DimWidgetArea(this) and crossing that
 * structural version with clean plus 50 target-adjacent forest states produced
 * 15 distinct islands.  Clean improved from the former live 94.496666%/752
 * bytes to 99.100000%/756 bytes.  Trial 13 reached 99.633330% with retail size,
 * eight differing bytes, and all 17 ordered relocations.  Its only residual
 * was the historical register ownership in the mouse hit test; the dispatcher
 * after that test was exact.
 *
 * Artifact:
 *   build/tu-state-noise/widget-main-inline-dim-area-20260727
 *
 * The second helper gives the remaining point-containment expression its own
 * inline ownership boundary:
 *
 *     static inline i32 IsInsideWidget(widget* target, i16 x, i16 y) {
 *         i16 left = target->m_x;
 *         return !(left > x || target->m_y > y
 *                  || left + target->m_width <= x
 *                  || target->m_y + target->m_height <= y);
 *     }
 *
 *     if (!IsInsideWidget(this, x, y))
 *         break;
 *
 * This descendant was also crossed with clean plus 50 states.  The sweep
 * stopped at audited exact trial 29: 100.000000%, retail size 756, zero byte
 * differences, and the complete ordered 17-relocation stream.  Trial 29 was
 * replayed independently with the same seed and permutation and again closed
 * exactly.  Official tooling advanced the retained source-hash MAX from the
 * clean 93.466670% state to 100.000000%; generated declarations were restored.
 *
 * Artifacts:
 *   build/widget-main-inline-dim-and-hit-test-state-summary.json
 *   build/tu-state-noise/widget-main-inline-dim-and-hit-test-20260727
 *   build/tu-state-noise/widget-main-inline-dim-and-hit-test-exact-recorded-20260727
 *
 * Parameters:
 *   seed=0x57494447, insertion=target, trials=50, exact trial=29
 *
 * Disposition: retain both semantic inline boundaries.  The clean TU state is
 * a lower coloring island, but exact retail bytes, size, and relocations were
 * observed for this unchanged effective function source.
 */
