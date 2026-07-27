/*
 * IconToBitmap, RVA 0xd0570:
 * shared forward decoder and solid-run visibility boundaries, 2026-07-27.
 *
 * The next enclosing operation above the retained shared clipped-fill macro
 * is negative-command decoding.  IconToBitmap and IconToBitmapColorTable have
 * the same solid-versus-dim count/flags decoder, so a shared inline function
 * was tested before any spelling permutations:
 */
#if 0
H2_ENUM_BEGIN(IconRleForwardRunType)
    ICON_RLE_FORWARD_SOLID,
    ICON_RLE_FORWARD_DIM
H2_ENUM_END(IconRleForwardRunType)

inline IconRleForwardRunType DecodeIconRleForwardRun(
    u8*& cursor, i32 command, u32& count, i32& flags
) {
    count = command & ICON_RLE_COMMAND_RUN_MASK;
    flags = 0;
    if (count != 0) {
        if (command == ICON_RLE_LONG_SOLID_COMMAND)
            count = ReadIconRleByte(cursor);
        return ICON_RLE_FORWARD_SOLID;
    }
    flags = ReadIconRleByte(cursor);
    count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
    if (count == 0)
        count = ReadIconRleByte(cursor);
    return ICON_RLE_FORWARD_DIM;
}
#endif

/*
 * Each unchanged structural arm received clean plus all 50 target-local
 * declaration-forest states, seed 0x46574431:
 *
 *   build/tu-state-noise/icon2b-shared-forward-decoder-20260727/
 *   build/icon2b-shared-forward-decoder-summary-20260727.json
 *   build/tu-state-noise/icon2bc-shared-forward-decoder-20260727/
 *   build/icon2bc-shared-forward-decoder-summary-20260727.json
 *
 * IconToBitmap produced 29 states and peaked at 70.928570%, size 1323.
 * IconToBitmapColorTable produced 17 states and peaked at 69.659620%, size
 * 1493.  Its clean source emitted 87 blocks against retail's 86 and 93
 * relocations against 91.  No state recovered either retained aligned family.
 *
 * The helper is semantically valid, and its best objects remain lower-island
 * evidence under build/, but its return-value dispatcher separates code which
 * retail keeps in the caller.  It is not retained.  Generalizing the entire
 * run dispatcher would require hooks for color translation, saved destination,
 * dim counters, and publications, so that larger boundary was also rejected
 * as a generator rather than a plausible shared implementation.
 *
 * The surviving single-TU structural descendant owns the complete clipped
 * solid-run visibility calculation, including the right edge, in a nested
 * inline:
 */
#if 0
static inline i32 IconRunVisible(
    i32 currentY,
    i32 clipBottom,
    i32 clipTop,
    i32 currentX,
    u32 runLength,
    i32 clipLeft,
    i32 clipRight
) {
    i32 runRight = currentX + runLength;
    return IconRowVisible(currentY, clipBottom, clipTop)
           && clipLeft < runRight && clipRight >= currentX;
}
#endif

/*
 * H2_ICON_RLE_CLIPPED_FILL now accepts the complete visibility predicate.
 * IconToBitmap calls IconRunVisible; IconToBitmapColorTable supplies its
 * previous expanded predicate.  This keeps one shared fill operation without
 * forcing the two TUs to share different global ownership.
 *
 * Clean IconToBitmap immediately returns to retail's exact 80-block topology:
 * 61 exact blocks and 19 size-only blocks, with 79/83 relocations and no
 * excess owner.  The complete clean-plus-50 census produced 20 states:
 *
 *   build/tu-state-noise/icon2b-run-visible-inline-20260727/
 *   build/icon2b-run-visible-inline-summary-20260727.json
 *
 * Trial 49 retained the best state at 77.961540%, size 1216.  That fuzzy
 * maximum is below older source families, but the clean topology repair makes
 * this a credible structural island; it is retained rather than pruned by
 * score.
 *
 * The macro API is codegen-neutral for IconToBitmapColorTable: clean remains
 * exact 86-block flow (67 exact plus 19 size-only) with 91/91 relocations.
 * Its known trial 8, seed 0x484f4d32, was replayed to preserve 83.936620% for
 * the new generated dependency hash:
 *
 *   build/tu-state-noise/icon2bc-run-visible-macro-api-20260727/
 *   build/icon2bc-run-visible-macro-api-summary-20260727.json
 *
 * Generated declarations were removed.  The match ledger was updated only by
 * the canonical manifest/build and TU-state scripts.
 */

void IconToBitmap_run_visible_inline_owner_attempt() {
}
