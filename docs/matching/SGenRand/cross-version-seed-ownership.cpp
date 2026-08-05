/*
SGenRand (RVA 0x000c6a60, retail size 72)
Gold/Buka 2.1 seed-ownership reconstruction, 2026-08-05.

The earlier PoL source calculated the mixed seed in a local, generated the
result bits from that local, and published iLastSeed after the loop.  That
source had the retail five-block CFG, size, and 3/3 relocations, but put the
seed store on the exit side of the loop-back branch and scored 90.909090%.

The exact Gold/Buka source exposed the missing structure: iLastSeed is the
mixing accumulator throughout, and a named bitMask local owns `1 << i`.
A complete 2 x 51 matrix compared the current and cross-version structures in
the clean state plus 50 top declaration-forest states.  It completed 102/102.
The Gold/Buka structure was exact in the clean state and in every sampled
state: 100.000000%, size 72, exact 5/5 CFG, and ordered 3/3 relocations.

Artifacts:
  build/sgenrand-cross-version-axes.json
  build/sgenrand-cross-version-manifest.json
  build/source-variant-batch/sgenrand-cross-version/results.json

Disposition: retain the Gold/Buka seed-in-place structure.  No generated
declaration-forest probe is retained.
*/

#if 0
i32 bitMask;
i32 ret = 0;
iLastSeed &= RANDOM_SEED_MASK;
iLastSeed *= RANDOM_MIX_MULTIPLIER;
iLastSeed += (iLastSeed & RANDOM_MIX_MASK) >> RANDOM_MIX_SHIFT;
for (i32 i = RANDOM_TOP_BIT; i >= 0; --i) {
    bitMask = 1 << i;
    if (iLastSeed & bitMask)
        ret |= 1 << i;
}
return ret;
#endif
