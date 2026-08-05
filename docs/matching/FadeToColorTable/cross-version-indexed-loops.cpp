/*
FadeToColorTable (RVA 0x000c65e0, retail size 184)
Gold/Buka 2.1 indexed-loop reconstruction, 2026-08-05.

The prior PoL source used compact output pointers and nested countdown loops.
It was semantically correct and already had the retail seven-block CFG and
ordered 8/8 relocations, but emitted 186 bytes at 95.709680%.

The exact Gold/Buka source retains the higher-level structure: an indexed
palette loop over 256 colors followed by nested y/x loops over the screen.
A complete 2 x 51 structure/state matrix finished 102/102.  The indexed-loop
arm is clean-state exact at 100.000000%, size 184, exact 7/7 CFG, and ordered
8/8 relocations.  It also remained exact in many sampled states; no compiler
noise is required.

Artifacts:
  build/fadetocolortable-cross-version-axes.json
  build/fadetocolortable-cross-version-manifest.json
  build/source-variant-batch/fadetocolortable-cross-version/results.json

Disposition: retain the Gold/Buka indexed palette and screen-loop structure.
No generated state declarations are retained.
*/

#if 0
for (i = 0; i < MISC_PALETTE_BYTE_COUNT / PALETTE_COMPONENT_COUNT; ++i) {
    // Publish all three indexed color components.
}
for (y = 0; y < BLIT_SCREEN_HEIGHT; ++y)
    for (x = 0; x < BLIT_SCREEN_WIDTH; ++x)
        *pixel++ = colorTable[*pixel];
#endif
