/*
FlipDimIconToBitmap (RVA 0x004daa20, retail size 571)
Clipped-row visibility inline boundary, 2026-07-27.

Artifacts:
  build/match-variants/icondf2b-row-visible-inline/results.json
  build/tu-state-noise/icondf2b-row-visible-inline-record-max/
  build/tu-state-noise/icondf2b-row-visible-inline-trial28-record-max/

The sibling-proven row predicate was crossed with the direct source and the
clean state plus all 50 forest states (102/102 variants, source restored).

The direct arm reproduced the historical 82.674700% state at trial 17. The
inline arm opened a new state at trial 28:
  83.397590%, size 575, relocations 37/37.
That state is not closure and its 32/31 block split is only an island clue.

A second complete matrix with seed 0x4644524f independently found a coherent
31/31 inline-row state at trial 4:
  82.734940%, size 564, 26 exact plus 5 size-only blocks, no flow mismatch.
This both exceeds the previous MAX and preserves the retail CFG family.

The helper is therefore retained as a legitimate sibling-family structure.
The generated workflow migrated the new source hash with `homm2 status update`,
then `tu_state_noise.py --record-max` replayed trial 28 and raised MAX from
77.7892% on the new hash to 83.3976%. No generated probe source or hand-edited
configuration was retained.
*/

#if 0
static inline i32 FlipDimRowVisible(i32 clipTop) {
    return clipTop <= gFDY && gFDY <= gFDClipB;
}

i32 left;
if (FlipDimRowVisible(clipY) && (left = (X - cmd) + 1, clipX <= left)
    && X <= gFDClipR) {
    // Existing clipped dim-run selection and pixel loop.
}
#endif
