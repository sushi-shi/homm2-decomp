/*
FlipDimIconToBitmap (RVA 0x004daa20, retail size 571)
Family-structure matrix, 2026-07-27.

Artifacts:
  build/match-variants/icondf2b-structural-axes-20260727.json
  build/match-variants/icondf2b-structural-clean-matrix-20260727.json
  build/match-variants/icondf2b-structural-clean-islands-20260727/results.json

The 16-arm clean-state matrix crossed:
  - four setup orders,
  - local versus published clipping coordinates, and
  - statement versus loop-condition decrement in both dim loops.

The current accessor spelling and the equivalent raw-buffer spelling emitted
identical text: 80.192770%, size 566, 37 relocation records, and 24 exact plus
7 size-only blocks. Moving the decrement into the loop condition was also
byte-neutral.

Reading the clipping coordinates back through gFDX0/gFDY retained the 31-block
flow but fell to 79.981926% and size 564. Taking x before entry construction
retained the 31-block flow but fell to 78.451805% and size 563.

The literal FlipIconToBitmap-family setup order was not a useful descendant:
it emitted size 574 and only 35 relocation records at 65.066260%, with 14
flow-kind mismatches. The version combined with published coordinates emitted
size 572 at 64.072290% with the same contradictory topology.

Disposition: the family resemblance does not justify importing the sibling's
setup order. Keep the current semantic CFG family. These results do not exceed
the hash-scoped retained MAX of 82.6747%, so none is a source/MAX update.
*/

#if 0
// Equivalent raw-buffer setup: byte-identical to the retained accessor source.
u8* src = srcIcon->m_data;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
u8* srcData = src + entry->srcOffset;
i32 x0 = x;
gFDEntry = entry;
gFDSrc = srcData;
i32 w = entry->w;
i32 entryY = entry->y;
x0 = x0 - entry->x;
x0 = x0 - w;
x0++;
gFDX0 = x0;
gFDY = y + entryY;
i32 X = w + x0 - 1;
gFDXEnd = X;

// Sibling-family setup order: rejected by topology and relocation evidence.
u8* src = srcIcon->m_data;
i32 x0 = x;
i32 w;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
w = entry->w;
x0 = x0 - entry->x;
src = src + entry->srcOffset;
i32 entryY = entry->y;
x0 = x0 - w;
gFDEntry = entry;
gFDSrc = src;
x0++;
gFDX0 = x0;
i32 X = w + x0 - 1;
gFDXEnd = X;
y = y + entryY;
gFDY = y;

// Published-coordinate ownership: coherent CFG, but a worse island.
if (gFDX0 < clipX || clipW + clipX < w + gFDX0 || gFDY < clipY
    || entry->h + gFDY > clipY + clipH) {
    // existing clipped body
}

// Counter ownership spelling: byte-identical in both dim loops.
do {
    i32 px = *dst++;
    gFDDst = dst;
    dst[-1] = uDimPal[0][color][px];
} while (--cnt != 0);
#endif
