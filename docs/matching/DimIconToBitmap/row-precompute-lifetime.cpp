/*
DimIconToBitmap, retail RVA 0x004cfd50, size 622.
Destination row precompute lifetime matrix, 2026-07-27.

This experiment extended legitimate destination state across the clipping
predicate to test whether retail's additional register pressure explains its
gDimX0/gDimY reloads.  Pitch-only, pitch plus row offset, and the complete row
pointer were moved before clipping; the current post-clipping construction was
the control.  All four structures received clean plus 50 forest states:
4 x 51 = 204/204 complete compilations.

Representative arms:

    i16 pitch = dest->m_width;
    // clipping predicate
    i32 rowOffset = gDimY * pitch;

    i16 pitch = dest->m_width;
    i32 rowOffset = gDimY * pitch;
    u8* row = dest->m_pixels + rowOffset;
    // clipping predicate

Best per family:
  row after clipping          85.913795%, 608 bytes, 35/37 relocations
  pitch before clipping       83.132190%, 611 bytes, 35/37 relocations
  row offset before clipping  47.810345%, 604 bytes, 34/37 relocations
  row pointer before clipping 47.781610%, 604 bytes, 34/37 relocations

Pitch-only retains the 36-block CFG but worsens the classifier to 33 exact
plus three size-only blocks.  Early row-offset ownership has only 32 exact
blocks, and early row-pointer ownership changes the CFG to 35 blocks with
flow/target mismatches.  None produces the missing published-coordinate
reloads.

Artifacts:
  build/icond2b-row-precompute-lifetime-axes.json
  build/icond2b-row-precompute-lifetime-manifest.json
  build/match-variants/icond2b-row-precompute-lifetime

Disposition: rejected.  Row construction belongs after clipping in the
surviving family; durable MAX remains 86.229880%.  No experimental source or
generated state is retained.
*/

void DimIconToBitmap_row_precompute_lifetime_record() {
}
