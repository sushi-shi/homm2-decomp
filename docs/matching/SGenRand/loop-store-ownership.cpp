/*
SGenRand (RVA 0x000c6a60, retail size 72)
Loop-tail seed-store ownership matrices, 2026-07-27.

The original reconstruction redundantly wrote the same invariant mix value to
iLastSeed on every source iteration. MSVC hoisted that body store before the
loop. Retail instead emits:

    dec bitIndex
    mov [iLastSeed], mix
    jns loop

The candidate and retail already shared a five-block CFG, retail size, and
3/3 raw relocations. The initial 5 x 51 complete matrix compared:

  - body-local store,
  - decrement/store and store/decrement for-increment expressions,
  - explicit while-tail ownership,
  - explicit do/while-tail ownership.

All 255/255 builds canonicalized to the same 90.409090%, size-72 object.

Artifacts:
  build/sgenrand-loop-tail-axes-20260727.json
  build/sgenrand-loop-tail-manifest-20260727.json
  build/match-variants/sgenrand-loop-tail-20260727/results.json

A second 4 x 51 matrix moved the invariant store across structural boundaries:

  body store:          90.409090%
  before-loop store:   90.409090%
  inline store helper: 90.409090%
  after-loop store:    90.909090%

All arms retained retail size and 3/3 raw relocations. The after-loop spelling
formed the only distinct object, made the entire setup block exact, and left
only the store on the opposite side of the loop-back branch. It is also the
natural source structure: calculate the mixed result, generate its bits, then
publish the new seed once.

Artifacts:
  build/sgenrand-store-boundary-axes-20260727.json
  build/sgenrand-store-boundary-manifest-20260727.json
  build/match-variants/sgenrand-store-boundary-20260727/results.json

The final 4 x 51 matrix tested whether retail's dec/store/test order came from
a comma expression owned by the for or do/while condition. All three
condition-owned forms canonicalized back to the old 90.409090% object in every
state; only the after-loop source remained 90.909090%.

Artifacts:
  build/sgenrand-condition-store-axes-20260727.json
  build/sgenrand-condition-store-manifest-20260727.json
  build/match-variants/sgenrand-condition-store-20260727/results.json

Disposition: retain the after-loop source and advance its new hash-scoped MAX
through the generated status workflow. No probe declarations or generated
source variants are retained.
*/

#if 0
// Canonicalized loop-tail alternatives.
for (i32 i = RANDOM_TOP_BIT; i >= 0; --i, iLastSeed = mix) {
}

i32 i = RANDOM_TOP_BIT;
do {
    --i;
} while (iLastSeed = mix, i >= 0);

static inline void SStoreLastSeed(i32 seed) {
    iLastSeed = seed;
}

// Retained source ownership.
for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
    if (mix & (1 << i))
        result |= 1 << i;
}
iLastSeed = mix;
#endif
