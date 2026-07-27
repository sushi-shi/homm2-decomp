/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Sibling-derived clipped-literal source boundary, 2026-07-27.

The seven earlier ledgers cover broad literal sharing, decoder reads, setup,
fill, dim/palette, predicates, and coordinate publication. This matrix instead
tested a mechanism recovered later from the now-exact IconToBitmapYModify:
a parameterized inline source-pointer expression at only the left-outside
clipped literal-copy boundary.

The current source and retail have exact 80/80 flow. The relevant retail
quadrant is the shared copy tail in blocks 72-76, so the helper was kept at
that existing boundary rather than duplicating memcpy into its two predecessor
arms. Five reviewed structures were crossed with the clean state and all 50
declaration-forest states: 255/255 builds completed in 178.33 seconds and
source was restored.

  build/icon2b-literal-source-inline-axes-20260727.json
  build/icon2b-literal-source-inline-states-20260727.json
  build/match-variants/icon2b-literal-source-inline-states-20260727/results.json

Clean results:

  open expression                    79.3187%, 1220 bytes, 79/83 relocations
  parameterized expression           80.9451%, 1219 bytes, 78/83 relocations
  parameterized delta expression     80.9451%, 1219 bytes, 78/83 relocations
  parameterized incremental          81.0000%, 1217 bytes, 78/83 relocations
  branch-local incremental           79.1539%, 1218 bytes, 79/83 relocations

The expression helper reproduced the durable 81.494510% MAX at trials 1, 7,
16, 32, and 49; the delta helper reached the same state at trial 49. The open
source reproduced MAX in its known states. Incremental helpers formed a
distinct 81.329670%, 1218-byte, 79/83-relocation island. No arm improved MAX,
reached retail size 1261, or supplied the four missing ordered relocations.

Disposition: preserve these helpers as credible sibling-derived branch seeds,
but retain the existing open expression because the helpers provide no MAX or
relocation gain. No generated source, probe declaration, or configuration is
retained.
*/

#if 0
static inline u8* IconOutsideCopySource(
    u8* src, i32 currentX, i32 clipX
) {
    return src + (clipX - currentX);
}

memcpy(
    row + clipX,
    IconOutsideCopySource(gIcSrc, X, clipX),
    count
);

static inline u8* IconOutsideCopySourceIncremental(
    u8* src, i32 currentX, i32 clipX
) {
    src -= currentX;
    src += clipX;
    return src;
}
#endif
