/*
DimIconToBitmap, retail RVA 0x000cfd50, size 0x26e.
Dim-pixel and destination-cursor ownership, 2026-07-27.

The task's proposed RVA 0xd94f0 is unmapped in the current symbol inventory;
source, generated symbols, and sema resolve DimIconToBitmap to RVA 0xcfd50.

Fresh audit reproduced exact 36/36 flow, 34 instruction-count-exact blocks,
two size-only clipping-setup blocks, and 35/37 relocations. Retail's two
additional sites are the already-classified gDimX0/gDimY predicate reloads.
The prior 561 decoder, inline-predicate, entry, and canonical-state variants
were not repeated.

The successful IconToBitmapColorTable structure suggested two mechanisms that
had not been crossed on this function:

  - one destination cursor shared by the unclipped and clipped dim loops,
    either uninitialized or initialized early from gDimDst;
  - a narrow inline helper owning pixel read, destination advance, count
    decrement, gDimDst publication, and palette write.

These form six reviewed structures with the retained branch-local/direct arm.
Every structure was crossed with clean plus all 50 top-of-TU forest states:
306/306 compilations completed in 208.86 seconds and source was restored.

Clean results:

  branch-local direct          83.643680%, 611 bytes, 35/37 relocations
  shared destination direct    83.902300%, 611 bytes, 35/37 relocations
  any dim-pixel helper         71.310350%, 625 bytes, 36/37 relocations

The initialized and uninitialized shared owners are byte-identical at clean
state (text SHA 39e06b72a6d65e6e) and retain 36 blocks with 34 exact plus two
size-only. They are a credible stronger clean island, but their top-state best
is only 84.649420%, 611 bytes, with 33 exact plus three size-only blocks.
The branch-local top-state control reaches 84.994255%.

Unlike ColorTableDimPixel, the faithful DimIconPixel helper crosses VC4.2's
/Ob1 threshold. Every destination lifetime converges to the same clean helper
hash 930cbbe46fc6151b. Its best top state is 75.103450%, 621 bytes, and 36/37
relocations, with 37 candidate blocks versus 36 retail blocks (13 exact,
13 size-only, eight target shifts, two flow-kind mismatches, one missing).
The additional relocation and block are the visible out-of-line call.

Artifacts:

  build/icond2b-dim-pixel-destination-axes-20260727.json
  build/icond2b-dim-pixel-destination-top-states-20260727.json
  build/match-variants/icond2b-dim-pixel-destination-top-states-20260727/results.json

The top-of-TU state family requested for this pass does not reproduce the
86.229880% durable record from the earlier default-insertion census; it does
not supersede or lower that MAX. No arm reaches retail size 622, restores the
two predicate relocations, or closes exact bytes.

Disposition: preserve the shared-cursor clean object and helper objects as
lower structural evidence, but retain the branch-local direct source. No
generated helper, forest probe, configuration change, or MAX change remains.
*/

#if 0
static inline u8* DimIconPixel(
    u8* dst, u32 paletteOffset, u32& count
) {
    i32 px = *dst;
    dst = dst + 1;
    count--;
    gDimDst = dst;
    dst[-1] = (&uDimPal[0][0][0])[paletteOffset + px];
    return dst;
}

u8* savedDst = gDimDst; // Rejected early shared owner.
#endif
