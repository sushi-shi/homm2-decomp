/*
 * MonoIconToBitmap, RVA 0xcfae0: destination and setup lifetimes, 2026-07-27.
 *
 * Retail has 31 blocks.  The live source already had exact successor flow,
 * but its setup lacked retail's second gMonoX0 and gMonoY relocation owners.
 * Three complete structural matrices were run with clean plus 50 top-inserted
 * declaration-forest states.  Generated input was restored after every
 * compile.
 *
 * The first matrix crossed:
 *
 *   - value-parameter and published-global clipping helpers;
 *   - direct, product-local, and staged-sibling first-row construction;
 *   - four merged memset calls and two explicit shared destination/count
 *     spellings.
 *
 * All 18 x 51 = 918 cells completed in 638.99 seconds.  The ordinary merged
 * calls retained exact 31-block flow and reproduced 83.607735%, size 601,
 * 36/38 relocations.  Published-global clipping plus either local row-offset
 * form reached the same island.  Explicit shared destination/count ownership
 * formed a real lower structural orbit: 73.690605%, size 568, 35/38
 * relocations, 31 blocks with two shifted branch targets.  It was preserved
 * as evidence but rejected for live source.
 *
 * Artifacts:
 *
 *   build/iconm2b-destination-lifetime-axes-20260727.json
 *   build/iconm2b-destination-lifetime-manifest-20260727.json
 *   build/match-variants/iconm2b-destination-lifetime-20260727/
 *
 * The second matrix crossed three setup-lifetime families with an open or
 * inline first-row boundary.  The successful setup mirrors DimIconToBitmap:
 */

#if 0
IconEntry* entries = srcIcon->Entries();
i32 entryX = entries[frame].x;
IconEntry* const entry = &entries[frame];
u8* const srcData = reinterpret_cast<u8*>(entries) + entries[frame].srcOffset;
gMonoEntry = entry;
const i32 entryY = entry->y;
gMonoSrc = srcData;
gMonoX0 = x + entryX;
gMonoX = gMonoX0;
gMonoY = y + entryY;

static inline u8* MonoInitialRow(bitmap* dest, i16 pitch) {
    return dest->m_pixels + gMonoY * pitch;
}
#endif

/*
 * All 6 x 51 = 306 cells completed in 216.76 seconds.  The sibling setup
 * alone reached 83.779010%, size 609, and 37/38 relocations.  Combined with
 * the first-row helper it reached 84.524864%, size 606, 37/38 relocations,
 * and exact 31-block flow with 25 exact plus six size-only block bodies.
 * This is retained: it restores retail's second gMonoX0 load and advances
 * structural and byte evidence.
 *
 * Artifacts:
 *
 *   build/iconm2b-setup-row-helper-axes-20260727.json
 *   build/iconm2b-setup-row-helper-manifest-20260727.json
 *   build/match-variants/iconm2b-setup-row-helper-20260727/
 *
 * Trial 25 was replayed against the retained source with the official state
 * tool.  It recorded MAX 84.5249% for source hash
 * eec82b659fac.065eb853b62d without retaining generated declarations:
 *
 *   build/tu-state-noise/
 *     iconm2b-dim-setup-row-helper-trial25-record-max-20260727/
 *
 * A final descendant crossed the retained setup and row helper with a
 * published-global clipping helper.  All 4 x 51 = 204 cells completed in
 * 152.17 seconds.  It reproduced 84.524864% but never produced the final
 * relocation.  Ordered relocation audit shows that only retail's second
 * setup gMonoY read remains absent.
 *
 * Descendant artifacts:
 *
 *   build/iconm2b-global-clip-descendant-axes-20260727.json
 *   build/iconm2b-global-clip-descendant-manifest-20260727.json
 *   build/match-variants/iconm2b-global-clip-descendant-20260727/
 *
 * Exact closure remains open at that gMonoY reload and the resulting
 * allocator/body differences.  No generated source or configuration is
 * retained.
 */

void MonoIconToBitmap_destination_and_setup_lifetime_attempts() {
}
