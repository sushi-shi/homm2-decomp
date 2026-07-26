// Experiment: factor the repeated clipped-row predicate into the same inline
// accessor shape already used by the sibling icon-family implementation.
// Artifact: build/tu-state-noise/iconf2bc-row-visible-inline-50-20260726/
// Isolated clean baseline: 87.010080%; 50 trials, 39 unique states, no gain.
// Official generated status after the full build: 87.0227%.
// Clean blocks: 76/76, 67 exact and 9 size-only.
// Disposition: retained; plausible family structure and a new MAX.

#if 0
static inline i32 FlipColorRowVisible(i32 clipTop)
{
    return clipTop <= gFCY && gFCY <= gFCClipB;
}

if (FlipColorRowVisible(clipY)
    && (left = (X - count) + 1, clipX <= left)
    && X <= gFCClipR) {
}

if (FlipColorRowVisible(clipY)
    && clipX <= static_cast<i32>((X - count) + 1)
    && X <= gFCClipR) {
}

if (FlipColorRowVisible(clipY)) {
}
#endif
