/*
DimIconToBitmap, retail RVA 0x004cfd50, size 0x26e.
Decoder ownership and family-derived inline boundaries, 2026-07-27.

This complete matrix crossed two decoder owners with four inline-boundary
families and the clean state plus 50 declaration-forest states: 8 x 51 = 408
successful builds. The run completed in 286.88 seconds and restored source.

Artifacts:
  build/icond2b-inline-owner-axes-20260727.json
  build/icond2b-inline-owner-states-20260727.json
  build/icond2b-inline-owner-states-20260727/results.json

Every clean arm was 611 bytes with 35/37 relocations. The open accessor arm was
83.6437%; direct cursor ownership was 82.9540%. Palette-only, row-only, and
combined inline boundaries formed lower 78.93-79.65% clean islands without a
size or relocation change, so they were retained for the full state census.

The unchanged open/accessor arm reproduced the durable 86.2299% MAX at forest
trial 20 (608 bytes, 35/37 relocations). The combined palette-and-row arm also
reproduced 86.2299% at trial 32, proving that a lower clean island can reach the
same best orbit under another legitimate TU state. Palette-only peaked at
85.9138%; row-only peaked at 85.8851%. No arm recovered either missing
gDimX0/gDimY relocation and no exact island appeared.

Disposition: keep the open ReadIconRleByte and palette expressions in authored
source. Preserve the inline arms below as evidence-backed alternate branch
seeds; retain no generated state or helper.
*/

#if 0
static inline u8 DimPalettePixel(u32 paletteOffset, i32 pixel) {
    return (&uDimPal[0][0][0])[paletteOffset + pixel];
}

static inline i32 DimRowVisible(i32 clipTop) {
    return clipTop <= gDimY && gDimY <= gDimClipB;
}

// Decoder ownership axis.
i32 cmd = ReadIconRleByte(gDimSrc);
i32 directCmd = *gDimSrc++;

// Palette-boundary axis, used in both fill loops.
dst[-1] = DimPalettePixel(paletteOffset, px);

// Clipped-row boundary axis.
if (DimRowVisible(clipY) && (right = X + cmd, clipX < right) && X <= gDimClipR) {
    // clipped dim run
}
#endif
