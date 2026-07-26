/*
 * DoBlur structural experiment: replace the guarded do-while row loop with a
 * top-tested while loop.
 *
 * Reviewed source shape:
 *
 *     i32 lastRow = height - BORDER_RADIUS;
 *     i32 y = BORDER_RADIUS;
 *     i32 rowOffset = BORDER_RADIUS * SCREEN_WIDTH;
 *     while (y < lastRow) {
 *         ...
 *         rowOffset += SCREEN_WIDTH;
 *         y++;
 *     }
 *
 * Result (2026-07-26):
 *   clean fuzzy: 89.586494%, size 1707
 *   first forest state: 88.225740%, size 1708
 *   ordered relocations: 43/43
 *
 * Artifact:
 *   build/tu-state-noise/doblur-top-tested-row-loop-clean
 *
 * Disposition: rejected.  MSVC's rotated top-tested loop preserves the broad
 * control-flow family but lands in a substantially worse allocation orbit.
 * The guarded do-while remains the closer high-level ownership of rowOffset.
 */
