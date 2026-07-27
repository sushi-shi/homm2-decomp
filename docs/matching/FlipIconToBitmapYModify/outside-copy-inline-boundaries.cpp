/*
FlipIconToBitmapYModify, retail RVA 0x000d9ce0, size 0x58d.
Mirrored right-outside copy-source inline campaign, 2026-07-27.

IconToBitmapYModify closed exactly with a parameterized expression helper used
only by its both-sides-clipped literal-copy arm. FlipY's current source differs:
the reverse-copy loop must publish its adjusted global cursor, and the
right-outside source advance is shared by both descendants of that outer arm.
The live function has exact 59/59 flow, while blocks 44, 45, and 48-50 in this
literal quadrant are among its eleven size-only blocks.

The first matrix tested whether the source advance should instead be duplicated
into the two inner arms, with the helper restricted to the both-sides-clipped
descendant. It crossed seven open, parameterized, global, and incremental
owners with the clean state and all 50 top-of-TU forest states: 357/357 builds
completed in 247.23 seconds and source was restored.

Artifacts:
  build/iconf2by-outside-copy-inline-axes-20260727.json
  build/iconf2by-outside-copy-inline-manifest-20260727.json
  build/match-variants/iconf2by-outside-copy-inline-20260727/results.json

Splitting the advance contradicted retail shape. The best expression variants
were 90.8847%, 1438 bytes, and 146/144 relocations. Incremental split variants
were lower at 89.7587%, 1442 bytes, and 147/144 relocations. Only the current
shared expression reproduced the retained 93.6059% state.

The second matrix preserved the shared outer-arm ownership and varied only its
inline boundary. Five structures times clean plus 50 top states produced
255/255 successful builds in 181.42 seconds:

  build/iconf2by-shared-outside-inline-axes-20260727.json
  build/iconf2by-shared-outside-inline-manifest-20260727.json
  build/match-variants/iconf2by-shared-outside-inline-20260727/results.json

The open expression, parameterized coordinate expression, parameterized delta
expression, and zero-argument global expression all reproduced 93.6059%, 1420
bytes, and 144/144 relocations in one or more states. Their best objects are
byte-identical compiler states, so the helpers add no structural evidence.
The parameterized incremental helper formed a lower 92.7882%, 1413-byte,
143/144-relocation orbit.

No arm reached retail's 1421-byte size, no candidate passed ordered relocation
identity, and no exact closure appeared. Retain the current shared open
expression and the durable 93.6059% MAX; retain no generated state or helper.
*/

#if 0
// Best byte-neutral inline boundary at the existing outer-arm site.
static inline u8* FlipOutsideCopySource(
    u8* src, i32 currentX, i32 clipRight
) {
    return src + (currentX - clipRight);
}

gFYSrc = FlipOutsideCopySource(gFYSrc, gFYX, gFYClipR);

// Rejected incremental form.
static inline u8* FlipOutsideCopySourceIncremental(
    u8* src, i32 currentX, i32 clipRight
) {
    src += currentX;
    src -= clipRight;
    return src;
}

// Rejected structural split: duplicating either expression in both branches
// widened the right-outside quadrant and introduced excess relocations.
if (clipX <= (gFYX - gFYRun)) {
    gFYSrc = gFYSrc + (gFYX - gFYClipR);
    // right-only clipped copy
} else {
    gFYSrc = FlipOutsideCopySource(gFYSrc, gFYX, gFYClipR);
    // both-sides-clipped copy
}
#endif
