/*
 * FlipMonoIconToBitmap vertical/publication/source cross, 2026-07-27.
 *
 * Starting retained structure:
 *   clean 74.107790%, size 521, 26/27 relocations, 26 blocks (19+7);
 *   durable MAX 81.580840%.
 *
 * Four independent structural axes were crossed explicitly:
 *
 *   1. open upper vertical comparison vs FlipMonoAboveTop reading gFMY;
 *   2. open lower extent vs FlipMonoBelowBottom reading gFMY;
 *   3. gFMY-before-gFMXEnd vs retail-observed gFMXEnd-before-gFMY publication;
 *   4. open source-pointer expression vs narrow FlipMonoSource owner.
 *
 * The complete 16 * (clean + 50 top forest states) matrix compiled all
 * 816/816 candidates:
 *
 *   build/flipmono-vertical-publication-source-cross-axes-20260727.json
 *   build/flipmono-vertical-publication-source-cross-manifest-20260727.json
 *   build/match-variants/
 *     flipmono-vertical-publication-source-cross-20260727/
 *
 * The publication reorder alone improved clean source to 76.874250% and
 * reached 82.191620%. Combining retail publication order with the narrow
 * source-pointer helper opened the strongest island:
 *
 *   trial 18, seed 0x464d6f4e, top forest;
 *   82.580840%, size 518/530, 26/27 relocations;
 *   exact 26-block flow, 22 exact plus four size-only blocks.
 *
 * Both mechanisms are retained. The two vertical helper boundaries, alone
 * and together, never added the missing relocation and never beat this MAX.
 * The separate upper/lower pair peaked at 81.425150%, size 527, 26/27.
 *
 * A second complete 4 * 51 staged-clip matrix tested:
 *
 *   - the retained flat short-circuit;
 *   - horizontal-then-vertical boolean staging;
 *   - vertical-then-horizontal boolean staging;
 *   - nested horizontal/vertical branches with duplicated clip tails.
 *
 *   build/flipmono-staged-clip-axes-20260727.json
 *   build/flipmono-staged-clip-manifest-20260727.json
 *   build/match-variants/flipmono-staged-clip-20260727/
 *
 * The staged boolean forms expanded to 33 blocks and were rejected. The
 * nested form was the first source shape to emit 27/27 relocations, but it
 * expanded to 28 blocks versus retail's 26; ordered relocation positions
 * still differed. Clean was 74.233536%, size 535, and its best state was
 * 80.197600%, size 536. This lower island is preserved as evidence that
 * branching ownership, rather than an accessor alone, can force the second
 * gFMY load. It is not retained because its CFG family is incompatible.
 *
 * Official MAX replay:
 *   scripts/tu_state_noise.py
 *     --source src/BASE/Iconmf2b.cpp --rva 0xda800
 *     --trials 50 --only-trial 18 --seed 1179479886
 *     --insertion top --families forest --record-max --retain-best
 *   build/tu-state-noise/
 *     iconmf2b-publication-source-trial18-record-max-20260727/
 *
 * The replay reproduced 82.580840% and generated MAX 82.5808 for source and
 * dependency hash f75a01f02714.2995e36488e1. No probe source is retained.
 */

#if 0
static inline i32 FlipMonoAboveTop(i32 clipTop) {
    return gFMY < clipTop;
}

static inline i32 FlipMonoBelowBottom(
    IconEntry* entry, i32 clipTop, i32 clipHeight
) {
    return entry->h + gFMY > clipTop + clipHeight;
}

// Retained narrow source owner.
static inline u8* FlipMonoSource(IconEntry* entries, i32 frame) {
    return reinterpret_cast<u8*>(entries) + entries[frame].srcOffset;
}

// Retained retail-observed publication order.
gFMX0 = x0;
i32 X = w + x0 - 1;
gFMXEnd = X;
gFMY = y + entryY;

// Rejected 27-relocation / 28-block lower island.
if (x0 < clipX || clipX + clipW < right) {
    clip = ICON_DRAW_CLIP;
    gFMClipR = clipX + clipW - 1;
    gFMClipB = clipY + clipH - 1;
} else if (gFMY < clipY || entry->h + gFMY > clipY + clipH) {
    clip = ICON_DRAW_CLIP;
    gFMClipR = clipX + clipW - 1;
    gFMClipB = clipY + clipH - 1;
} else {
    clip = ICON_DRAW_NO_CLIP;
}
#endif
