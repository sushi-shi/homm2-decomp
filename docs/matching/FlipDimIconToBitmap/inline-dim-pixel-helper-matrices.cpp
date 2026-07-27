/*
FlipDimIconToBitmap (RVA 0x004daa20, retail size 571)
Inline dim-pixel helper matrices, 2026-07-27.

This file records rejected source experiments. It is evidence, not compiled
source.

Full pixel-step helper
----------------------

Artifacts:
  build/icondf2b-inline-pixel-helper-axes-20260727.json
  build/icondf2b-inline-pixel-helper-clean-manifest-20260727.json
  build/match-variants/icondf2b-inline-pixel-helper-clean-20260727/results.json

The clean four-arm decoder-owner by helper matrix preserved the 31-block flow
and 37 relocation records, but putting destination advance, count decrement,
global publication, and palette lookup behind one inline boundary expanded the
function to 643 bytes with the accessor decoder and 641 bytes with the direct
decoder. Both scored 55.463856%. Their block classifications fell to 19 exact
plus 12 size-only and 20 exact plus 11 size-only respectively. The 70-byte
excess contradicts retail, so this structure was pruned without a state sweep.

Palette-lookup-only helper
--------------------------

Artifacts:
  build/icondf2b-inline-palette-helper-axes-20260727.json
  build/icondf2b-inline-palette-helper-clean-manifest-20260727.json
  build/match-variants/icondf2b-inline-palette-helper-clean-20260727/results.json
  build/icondf2b-inline-palette-helper-states-manifest-20260727.json
  build/match-variants/icondf2b-inline-palette-helper-states-20260727/results.json

Leaving destination and counter ownership in the caller while moving only the
uDimPal lookup behind an inline helper was structurally credible: clean emitted
568 bytes, all 31 flow blocks, and 37 relocations. Both the accessor and direct
decoder versions were therefore compiled against the clean state and all 50
forest states (102/102 variants, not truncated, source restored).

The strongest island used the accessor decoder and trial 38. It reached
82.132530%, size 565, 37/37 relocations, and 26 exact plus 5 size-only blocks.
Its relocation identities/counts remain aligned, but relocation sites/addends
and bytes are not exact. It is below the retained 82.6747% MAX.

Parameter order
---------------

Artifacts:
  build/icondf2b-inline-palette-parameter-order-states-axes-20260727.json
  build/icondf2b-inline-palette-parameter-order-states-manifest-20260727.json
  build/match-variants/icondf2b-inline-palette-parameter-order-states-20260727/results.json

The reversed helper signature, pixel then color, was also crossed with both
decoder owners and the complete clean plus 50-state forest. All 102 variants
compiled without truncation and reproduced the same text islands byte for byte:
trial 38 again reached 82.132530%, size 565, and 26 exact plus 5 size-only
blocks. Parameter order is therefore byte-neutral after this helper is inlined.

Disposition: retain the open palette expression in reconstructed source. The
small helper remains a coherent lower structural orbit, but it neither raises
MAX nor supplies exact bytes, exact size, and ordered relocation closure.
*/

#if 0
// Rejected full pixel-step boundary.
static inline void WriteFlipDimPixel(u8*& dst, u32& cnt, i32 color) {
    i32 px = *dst++;
    cnt--;
    gFDDst = dst;
    dst[-1] = uDimPal[0][color][px];
}

// Coherent lower island; rejected because no exact state was found.
static inline u8 FlipDimPalettePixel(i32 color, i32 pixel) {
    return uDimPal[0][color][pixel];
}

do {
    i32 px = *dst++;
    cnt--;
    gFDDst = dst;
    dst[-1] = FlipDimPalettePixel(color, px);
} while (cnt != 0);
#endif
