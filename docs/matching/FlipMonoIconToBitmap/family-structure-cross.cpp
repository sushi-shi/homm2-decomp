/*
 * FlipMonoIconToBitmap same-family structural cross, 2026-07-27.
 *
 * Retail evidence:
 *   RVA 0xda800, retail size 530, 27 relocations, 26 basic blocks.
 *   Starting clean source: 75.874250%, size 514, 26/27 relocations;
 *   blocks 19 exact plus seven size-only.
 *   Starting durable MAX: 80.419160%.
 *
 * This experiment crossed two complete setup lifetimes with all eight
 * combinations of three credible inline-ownership boundaries:
 *
 *   setup:
 *     - the retained FlipDim-family setup;
 *     - MonoIconToBitmap-style entryX/entryY/source snapshots.
 *
 *   ownership:
 *     - an initial-row helper reading published gFMY;
 *     - an initial-clip helper reading published gFMY twice;
 *     - a clipped-fill row-visibility helper reading gFMY and gFMClipB.
 *
 * Every valid shape was compiled against clean state and the same 50
 * top-inserted declaration-forest states. The first matrix's four
 * initial-clip arms had a generator-only signature typo, so those eight
 * shapes were repeated correctly in a separate complete 408-build subset.
 * No malformed generated source was retained.
 *
 * Complete artifacts:
 *   build/flipmono-family-structure-cross-axes-20260727.json
 *   build/flipmono-family-structure-cross-manifest-20260727.json
 *   build/match-variants/flipmono-family-structure-cross-20260727/
 *   build/flipmono-initial-clip-cross-axes-20260727.json
 *   build/flipmono-initial-clip-cross-manifest-20260727.json
 *   build/match-variants/flipmono-initial-clip-cross-20260727/
 *
 * The new best orbit used Mono setup plus initial-row and row-visibility
 * ownership, with the initial clip left open:
 *
 *   trial 9, seed 0x464d6f4e, top forest;
 *   81.580840%, size 513, 26/27 relocations;
 *   exact 26-block flow, 21 exact plus five size-only blocks.
 *
 * This is +1.161680 points over the old MAX. Its clean state is lower at
 * 74.107790%, size 521, 26/27 relocations, with 19+7 blocks. MAX rather than
 * a transient clean score is the durable evidence. The family structure is
 * retained because it is semantically direct, mirrors MonoIconToBitmap, and
 * opens the strongest observed compiler-state orbit.
 *
 * The initial-clip helper was independently useful but lower. Retained setup
 * plus clip-only compiled clean to the old 80.419160% island and peaked at
 * 81.395210% (trial 16), size 512, 26/27 relocations, 21+5 blocks. Combining
 * it with the two winning helpers fell to 80.167660% or below. The initial
 * clip therefore remains open in live source.
 *
 * Deterministic official MAX replay:
 *   scripts/tu_state_noise.py
 *     --source src/BASE/Iconmf2b.cpp --rva 0xda800
 *     --trials 50 --only-trial 9 --seed 1179479886
 *     --insertion top --families forest --record-max --retain-best
 *   build/tu-state-noise/
 *     iconmf2b-mono-row-helpers-trial9-record-max-20260727/
 *
 * The script generated the new source/dependency hash and updated
 * config/match_baseline.tsv to 81.5808. Generated probe declarations were
 * restored and are not part of reconstructed source.
 *
 * Residual: retail still has one additional ordered gFMY relocation in the
 * second initial vertical-clip read. The retained best island is 17 bytes
 * short. Exact closure remains open.
 */

#if 0
static inline u8* FlipMonoInitialRow(bitmap* dest, i16 pitch) {
    return dest->m_pixels + gFMY * pitch;
}

static inline i32 FlipMonoNeedsClipping(
    IconEntry* entry, i32 x0, i32 right,
    i32 clipX, i32 clipY, i32 clipW, i32 clipH
) {
    return x0 < clipX || clipX + clipW < right || gFMY < clipY
        || entry->h + gFMY > clipY + clipH;
}

static inline i32 FlipMonoRowVisible(i32 clipTop) {
    return clipTop <= gFMY && gFMY <= gFMClipB;
}

// Retained Mono-family setup arm.
IconEntry* entries = srcIcon->Entries();
i32 entryX = entries[frame].x;
IconEntry* const entry = &entries[frame];
u8* const srcData =
    reinterpret_cast<u8*>(entries) + entries[frame].srcOffset;
gFMEntry = entry;
const i32 entryY = entry->y;
gFMSrc = srcData;
i32 w = entry->w;
i32 x0 = x - entryX;

// Retained ownership calls.
gFMRow = FlipMonoInitialRow(dest, pitch);
if (FlipMonoRowVisible(clipY)
    && (left = (X - cmd) + 1, clipX <= left) && gFMClipR >= X) {
}

// Rejected lower initial-clip owner.
if (FlipMonoNeedsClipping(
    entry, x0, right, clipX, clipY, clipW, clipH
)) {
}
#endif
