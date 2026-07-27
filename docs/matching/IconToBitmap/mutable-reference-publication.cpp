/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Same-family mutable-reference publication matrix, 2026-07-27.

The exact Y-modified and flipped Icon blitters frequently place coordinate
reads behind small inline owners.  IconToBitmap still lacks one ordered retail
relocation each for gIcX0, gIcY, gIcPitch, and gIcCnt2, so this matrix tested
whether reference parameters could make those published values escape the
caller's value tracking without inventing a non-semantic steering expression.

The structural axes were:

  - open source;
  - initial clipping helper taking gIcX0 and gIcY by mutable reference;
  - first-row address helper taking gIcPitch and gIcY by mutable reference;
  - both helpers together;
  - with or without the retail-observed left-clipped gIcCnt2 republication.

All eight structures were crossed with the clean state and 50 expansive
declaration-forest states.  All 408 candidates compiled in 345.65 seconds,
the run was not truncated, and source was restored:

  build/icon2b-mutable-ref-publication-clean-axes-20260727.json
  build/icon2b-mutable-ref-publication-states-manifest-20260727.json
  build/match-variants/icon2b-mutable-ref-publication-states-20260727/results.json

Best state per helper structure:

  open:                    81.631870%, size 1220, 79/83 relocations;
  initial clipping:       81.494510%, size 1220, 79/83 relocations;
  first-row address:      81.494510%, size 1220, 79/83 relocations;
  clipping plus row:      81.494510%, size 1220, 79/83 relocations.

Every best state preserved exact 80-block flow with 63 exact and 17 size-only
block bodies.  The explicit gIcCnt2 republication was byte-identical to its
open counterpart in every reported best state.  Mutable references therefore
produce distinct clean objects, but the complete state product converges on
the existing structural family and supplies none of the four missing ordered
relocations.

Disposition: retain the narrower IconRowVisible family mechanism and the
81.631870% historical MAX.  Do not retain either mutable-reference helper,
the folded republication, or any generated probe declaration.  The next
descendant must change setup/control ownership rather than repeat reference
parameter spellings.
*/

#if 0
static inline i32 IconNeedsClippingMutable(
    IconEntry* entry,
    i32& publishedX,
    i32& publishedY,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    return publishedX < clipX || clipW + clipX < entry->w + publishedX
        || publishedY < clipY || clipY + clipH < entry->h + publishedY;
}

static inline u8* IconRowAddressMutable(
    bitmap* dest, i32& pitch, i32& currentY
) {
    return dest->m_pixels + pitch * currentY;
}
#endif
