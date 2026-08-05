/*
FadeIn (RVA 0x000c45e0, retail size 234)
Gold/Buka 2.1 loop structure and exact compiler-state island, 2026-08-05.

The prior PoL source expressed the final forced palette level as an infinite
loop with an internal return.  It had the retail 21-block graph and 11/11
relocations but emitted 236 bytes at 95.555560%.

The exact Gold/Buka source preserves a counted for loop, a `fadeStep` label,
and a tail that forces the last level and jumps back once.  A complete 2 x 51
structure/state matrix finished 102/102.  The donor structure improved the
clean object to 99.382710%, size 233, while retaining the exact CFG and ordered
relocation stream.  Multiple donor states closed exactly; the first was trial
2 (seed 1213156658), at retail size 234.

A focused replay of trial 2 proved 100.000000%, exact 21/21 CFG, and ordered
11/11 relocations for retained source hash 85728fb0e5e3.  The hash-scoped MAX
was recorded as 100.  The declaration-forest probe is not retained.

Artifacts:
  build/fadein-cross-version-axes.json
  build/fadein-cross-version-manifest.json
  build/source-variant-batch/fadein-cross-version/results.json
  build/fadein-exact-replay-summary.json
  build/tu-state-noise/fadein-exact-replay/

Disposition: retain the donor loop/tail structure and the audited exact MAX;
retain no generated state declarations.
*/

#if 0
for (level = 0; level < MISC_PALETTE_LEVEL_COUNT; level += increment) {
fadeStep:
    // Fade one level and wait for the frame deadline.
}
if (done == 0) {
    level = MISC_PALETTE_MAX_LEVEL;
    goto fadeStep;
}
#endif
