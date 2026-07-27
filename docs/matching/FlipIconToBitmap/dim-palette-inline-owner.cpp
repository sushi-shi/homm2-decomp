// FlipIconToBitmap dim-palette lookup inline boundary.
//
// Matrix:
//   build/iconf2b-dim-palette-inline-axes-20260727.json
// Results:
//   build/match-variants/iconf2b-dim-palette-inline-clean-20260727/results.json
//
// Both dim loops were changed together because the sibling DimIconToBitmap and
// FlipDimIconToBitmap functions use the lookup as one repeated family
// mechanism. The helper arm produced a coherent lower island at 84.578250%,
// 1264 bytes, 79/81 relocations, and the same 75/76 block classification as the
// direct 86.846150%, 1251-byte source.
//
// The helper adds 13 bytes without supplying the missing block or either
// missing relocation owner, so it was pruned before compiler-state trials.

static inline u8 FlipDimPalettePixel(u8* palette, i32 pixel) {
    return palette[pixel];
}

void attempted_dim_loop(u8* dp, u8* dimPalette) {
    i32 px = *dp++;
    dp[-1] = FlipDimPalettePixel(dimPalette, px);
}
