/*
 * MonoIconToBitmap, RVA 0xcfae0: fill owner and row-visibility boundaries.
 *
 * After the setup-clipping helper raised MAX to 82.8619%, the remaining fill
 * path still used open row bounds.  The sibling icon blitters repeatedly
 * benefit from an inline row-visibility boundary, so four coherent structures
 * were crossed with clean plus 50 top-of-TU declaration-forest states:
 */

#if 0
static inline i32 MonoRowVisible(i32 clipTop) {
    return clipTop <= gMonoY && gMonoY <= gMonoClipB;
}

static inline void MonoFillRun(
    u8* row,
    i32 cmd,
    i32 color,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW
) {
    // The complete unclipped/clipped solid-run body was tested here.
}
#endif

/*
 * Artifacts:
 *
 *   build/iconm2b-fill-inline-owner-axes.json
 *   build/iconm2b-fill-inline-owner-manifest.json
 *   build/match-variants/iconm2b-fill-inline-owner/
 *
 * Complete product: direct, row-helper, whole-fill-helper, and combined-helper
 * structures times 51 states, 204/204 compilations in 173.54 seconds.  Source
 * was restored after the matrix.
 *
 * Best by structure:
 *
 *   row-visibility helper       83.607735%, size 601, forest trial 34
 *   whole-fill helper           83.386740%, size 602, forest trial 15
 *   direct body                 82.861880%, size 602, forest trial 1
 *   both helpers                82.640884%, size 604, forest trial 1
 *
 * Every best object retains 36/38 relocations and the exact 31-block CFG with
 * 24 exact-size plus seven size-only blocks.  The whole-fill owner is a real
 * lower structural island, but composing it with the row helper is not
 * additive.  The simpler row-visibility helper is retained because it is the
 * sibling-family structure and establishes the higher island.
 *
 * The new source hash was refreshed and trial 34 replayed only through the
 * official generated-state tools:
 *
 *   homm2 status update --force-refresh
 *   scripts/tu_state_noise.py --trials 50 --only-trial 34 \
 *       --insertion top --record-max --retain-best
 *
 * Replay artifacts:
 *
 *   build/tu-state-noise/iconm2b-row-visible-trial34-record-max-20260727/
 *
 * MAX advanced from 82.8619% to 83.6077%.  No generated probe declarations or
 * generated source are retained.
 */

void MonoIconToBitmap_fill_owner_and_row_visibility_attempts() {
}
