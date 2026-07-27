/*
DimIconToBitmap, retail RVA 0x004cfd50, size 622.
Origin publication and loop-owner lifetime matrix, 2026-07-27.

Retail reloads gDimX0 and gDimY in both halves of the clipping predicate,
whereas the candidate keeps each first load live.  This new matrix tested the
FlipDimIconToBitmap-style setup mechanism: compute local origins or the loop X
owner first, then publish them to gDimX0/gDimY.  Four structures received the
clean state plus the same 50-state declaration forest: 4 x 51 = 204/204.

Representative attempted structure:

    i32 x0 = x + entryX;
    i32 currentY = y + entryY;
    gDimX0 = x0;
    i32 X = x0;
    gDimY = currentY;

Results:
  published globals then loop X  86.229880%, 608 bytes, 35/37 relocations
  local origins then publish      82.517240%, 599 bytes, 34/37 relocations
  Flip-family staged origins      82.517240%, 599 bytes, 34/37 relocations
  loop X/local Y then publish     80.706894%, 599 bytes, 34/37 relocations

The local-origin arms retain 36/36 flow blocks but fall from 34 exact plus two
size-only blocks to 32 exact plus four size-only blocks.  They remove another
global reference rather than recovering either missing retail reload.

Artifacts:
  build/icond2b-origin-publication-axes.json
  build/icond2b-origin-publication-manifest.json
  build/match-variants/icond2b-origin-publication

Disposition: rejected.  The unchanged arm reproduces the durable 86.229880%
MAX; all sibling-style publication owners form lower, relocation-poorer
islands.  No experimental source or generated state is retained.
*/

void DimIconToBitmap_origin_publication_lifetime_record() {
}
