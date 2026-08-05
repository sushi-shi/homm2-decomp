/*
SRandom (RVA 0x000c6930, retail size 184)
Seed publication and mix/range lifetime matrices, 2026-07-27.

The live and historical-MAX object is 92.000000%, with the retail nine-block
CFG, all nine block sizes exact, retail size, and 3/3 raw relocations. Its only
differing block is the final range reduction and seed publication:

  candidate materializes the final mix in a temporary and stores iLastSeed
  before idiv; retail mutates the mix register in place, keeps it across idiv,
  then stores iLastSeed.

The first complete matrix tested five publication owners:

  - statement after rangedResult,
  - statement before the modulus/result expression,
  - assignment in a comma return,
  - publication through an i32 reference,
  - publication through an i32 pointer.

All five forms crossed with clean plus 50 expansive top declaration-forest
states completed 255/255. The three scalar forms emitted the identical
92.000000%, size-184, 3/3-relocation object in every useful state. The
reference and pointer forms emitted the same contradictory 73.320000%,
size-218 object with 9/3 raw relocations.

Artifacts:
  build/srandom-publication-tail-axes-20260727.json
  build/srandom-publication-tail-manifest-20260727.json
  build/match-variants/srandom-publication-tail-20260727/results.json

The disassembly suggested that range/mix statement ownership, rather than the
store expression itself, might control the final register lifetime. A second
complete matrix tested:

  - range before compound mix update,
  - compound mix update before range,
  - assigned mix update,
  - commuted multiplication,
  - an inclusive range local,
  - direct high-low modulus expression.

All six shapes crossed with clean plus 50 states completed 306/306 and emitted
the same 92.000000%, size-184, 3/3-relocation object. The optimized compiler
fully canonicalizes these natural range/mix structures.

Artifacts:
  build/srandom-mix-range-lifetime-axes-20260727.json
  build/srandom-mix-range-lifetime-manifest-20260727.json
  build/match-variants/srandom-mix-range-lifetime-20260727/results.json

Disposition: retain the current scalar source and existing MAX. No generated
source shape or declaration-forest probe is retained.

Cross-version follow-up, 2026-08-05:

The exact Gold/Buka reconstruction delegates to SIncRandomize and SGenRand.
A complete 2 x 51 structure/state matrix tested that body against the current
PoL inline body.  It completed 102/102.  The Gold body compiled as two real
calls here (31.933332% at best, size 66, 4 relocations), while PoL retail has
no direct call and is size 184.  The current inline body remained best at
92.000000%.  This is a genuine version-specific ownership change, not a
missing PoL helper boundary.

Artifacts:
  build/srandom-cross-version-axes.json
  build/srandom-cross-version-manifest.json
  build/source-variant-batch/srandom-cross-version/results.json
*/

#if 0
i32 rangedResult = low + result % (range + 1);
return (iLastSeed = mix, rangedResult);

i32& seed = iLastSeed;
seed = mix;
return rangedResult;

mix += high * RANDOM_HIGH_MIX_MULTIPLIER;
i32 range = high - low;

i32 range = high - low;
++range;
mix += high * RANDOM_HIGH_MIX_MULTIPLIER;
i32 rangedResult = low + result % range;
#endif
