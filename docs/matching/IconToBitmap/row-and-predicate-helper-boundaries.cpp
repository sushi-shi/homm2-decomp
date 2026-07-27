/*
IconToBitmap (RVA 0x000d0570, retail size 1261)
Row-address and initial-predicate helper boundaries, 2026-07-27.

The retained source has exact 80/80 CFG flow.  Its best known state remains
80.807690%, size 1216, 79/83 relocations, with 62 exact and 18 size-only
blocks.  Retail still has one additional reload each of gIcCnt2, gIcPitch,
gIcX0, and gIcY.

The row-address census crossed four ownership boundaries with clean plus 50
declaration-forest states:

  - published pitch/Y snapshots,
  - direct published globals,
  - an inline helper taking pitch/Y by const reference, and
  - an inline helper taking pitch/Y by value.

The open-predicate half completed 204 valid variants.  Published snapshots
reproduced the retained 80.807690% state at forest trial 14.  The other three
row-address structures reached the same 80.780220%, size 1216, 79/83
relocations, and 62-exact/18-size-only partition.  Neither helper boundary
materialized the missing retail reloads.

The original combined run reported 408 completed variants, but its four
predicate-helper arms contained a malformed generated signature and were
compile failures.  They are not counted as coverage.  A corrected, independent
4 x 51 rerun compiled all 204 predicate-helper candidates in 179.46 seconds,
was not truncated, and restored source.  Every row-address arm again peaked at
80.780220%, size 1216, 79/83 relocations, with exact flow.  Moving the initial
four-way clipping expression into an inline helper therefore changes the clean
object but reaches no new useful state or relocation topology.

A separate four-arm argument/product-order census completed 204/204 variants
in 180.01 seconds:

  IconRowAddress(dest, pitch, y):       by value and by reference
  IconRowAddress(dest, y, pitch):       by value and by reference
  helper product: pitch * y or y * pitch

All four structures were byte-identical within each TU state and peaked at
80.780220%, size 1216, 79/83 relocations.  Argument order, parameter reference
kind, and multiplication order are therefore neutral on this compiler orbit.

Artifacts:
  build/match-variants/icon2b-row-clip-helper-20260727/results.json
  build/match-variants/icon2b-row-helper-order-20260727/results.json
  build/match-variants/icon2b-clip-helper-valid-20260727/results.json

Disposition: keep the existing published row snapshots and official
80.807690% MAX.  No helper source, probe declaration, or generated
configuration is retained.
*/

#if 0
static inline u8* IconRowAddress(
    bitmap* dest,
    const i32& pitch,
    const i32& rowY
) {
    return dest->m_pixels + pitch * rowY;
}

static inline i32 IconNeedsClipping(
    IconEntry* entry,
    i32 x0,
    i32 currentY,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    return x0 < clipX || clipW + clipX < entry->w + x0 || currentY < clipY
           || clipY + clipH < entry->h + currentY;
}
#endif
