/*
DimIconToBitmap, retail RVA 0x004cfd50, size 0x26e.

This file records rejected source experiments. It is evidence, not compiled source.

Canonical structural evidence
-----------------------------

The canonical source and retail image both have 36 basic blocks with identical
flow. Thirty-four blocks also have exact sizes. The two size-only blocks are the
outer clipping predicate and clip-bound setup. The ordered relocation streams are
otherwise aligned, but the candidate has 35 entries against retail's 37: retail
reloads gDimX0 in the horizontal predicate and gDimY in the vertical predicate.
The candidate keeps their earlier values live. The later clip-bound setup is an
equivalent LEA-vs-ADD/DEC materialization difference.

An unchanged-source clean plus 50-state forest census is retained at:

  build/tu-state-noise/icond2b-canonical-20260727/
  build/icond2b-canonical-states-20260727.json

Trial 20 raised the unchanged-source MAX from 85.9138% to 86.2299%. Its object is
608 bytes with 35/37 ordered relocations and retains the same 36-block topology
and the same two missing reloads. The exact replay used to update the generated
baseline is retained at:

  build/tu-state-noise/icond2b-trial20-record-max-20260727/
  build/icond2b-trial20-record-max-states-20260727.json

Reviewed structural/spelling matrix
-----------------------------------

The following three independent axes were tested against the clean state and all
50 forest states (8 source combinations, 408 total candidates):

  build/match-variants/icond2b-reload-loop-20260727/
  build/icond2b-reload-loop-manifest-20260727.json

Axis A, canonical published-coordinate predicate:

    if (gDimX0 < clipX || gDimX0 + entry->w > clipX + clipW
        || clipY > gDimY || gDimY + entry->h > clipY + clipH)

Axis A, mirrored clip-first predicate:

    if (clipX > gDimX0 || clipX + clipW < gDimX0 + entry->w
        || clipY > gDimY || clipY + clipH < gDimY + entry->h)

Axis B, direct bounds:

    gDimClipR = clipX + clipW - 1;
    gDimClipB = clipY + clipH - 1;

Axis B, branch-local materialization:

    const i32 clipRight = clipX + clipW;
    const i32 clipBottom = clipY + clipH;
    gDimClipR = clipRight - 1;
    gDimClipB = clipBottom - 1;

Axis C, canonical dim-loop read:

    i32 px = *dst++;
    cnt--;

Axis C, staged dim-loop read:

    i32 px = *dst;
    dst++;
    cnt--;

The canonical predicate reached 86.2299% at trial 20. The mirrored predicate
reached 85.9138% at the same state. Bound materialization and the staged read were
byte-neutral at the best state. No arm supplied either missing relocation or an
exact island, so none was retained.

Lower structural island: narrowed fill-local lifetimes
------------------------------------------------------

This version moved `right` into the clipped arm and declared a separate `cnt` in
each fill arm:

    if (clip == ICON_DRAW_NO_CLIP) {
        u8* dst = row + X;
        u32 paletteOffset;
        u32 cnt;
        // ...
    } else {
        i32 right;
        // ...
        u32 cnt = right;
        // ...
    }

Its complete clean plus 50-state census is retained at:

  build/tu-state-noise/icond2b-narrow-fill-lifetimes-20260727/
  build/icond2b-narrow-fill-lifetimes-states-20260727.json

It formed a coherent lower orbit: clean was 83.7874%, and trials 4, 5, 17, 21,
and 46 reached 85.3966%, all at 606 bytes and 35/37 relocations. It did not
recover the predicate reloads and was rejected. The canonical function-wide
`right` and `cnt` declarations remain in reconstructed source.

Inline clipping-family matrix
-----------------------------

The two absent retail relocations are predicate reloads of `gDimX0` and
`gDimY`, so four developer-plausible ownership shapes were crossed
independently with the clean state and all 50 forest states:

  build/match-variants/icond2b-inline-clip-family-20260727/
  build/icond2b-inline-clip-family-manifest-20260727.json
  build/icond2b-inline-clip-family-axes-20260727.json

The 204-candidate matrix tested the open predicate, one value-argument inline
helper, zero-argument coordinate accessors used by an inline predicate helper,
and four independent inline clause helpers. The single value helper reproduced
the retained 86.2299% island in five states; the coordinate-accessor helper
peaked at 85.9138%, the open predicate at 84.9943%, and the independent helpers
at 84.3908%. Every candidate retained only 35/37 relocations. None recovered
either missing global reload, improved the 34-exact plus 2-size-only block
classification, or exposed an exact island, so no helper was retained.
*/
