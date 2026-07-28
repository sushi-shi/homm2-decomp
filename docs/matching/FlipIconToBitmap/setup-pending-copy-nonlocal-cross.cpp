/*
 * FlipIconToBitmap setup x pending-skip x copy-counter cross, 2026-07-27.
 *
 * This 80-cell matrix retried setup forms only after the dim and literal
 * counter ownership had changed.  The result demonstrates why source levers
 * cannot be rejected permanently based on one compiler-state basin.
 *
 * Axes:
 *   - five setup/publication structures;
 *   - signed, unsigned, volatile, or dispatch-scoped pendingSkip;
 *   - four clipped-literal copy-counter roles.
 *
 * Artifacts:
 *   build/iconf2b-final-block-islands-axes-20260727.json
 *   build/iconf2b-final-block-islands-manifest-20260727.json
 *   build/source-variant-batch/iconf2b-final-block-islands-20260727/
 *
 * The combined staged-setup/unsigned-pending/copied-loop shape reached the
 * campaign's highest observed fuzzy island:
 *
 *   89.310350%, size 1257, 82/81 relocations,
 *   exact 76-block CFG, 70 exact plus six size-only blocks.
 *
 * It is preserved as MAX/clue evidence, not as the live source: another
 * candidate had the same 70/6 topology, exact 81/81 relocation count, and
 * size 1263.  That lower 85.416440% shape was retained instead.
 *
 * Volatile pendingSkip was rejected.  It expanded to 77 blocks, changed flow,
 * and grew to 1330-1344 bytes.  The repeated retail stack publications must
 * therefore come from ordinary ownership/compiler state, not volatility.
 */

#if 0
// Highest-fuzzy island setup.
u8* src = srcIcon->m_data;
i32 x0 = x;
i32 width;
IconEntry* entry = &srcIcon->Entries()[frame];
width = entry->w;
i32 entryX = entry->x;
i32 entryY = entry->y;
i32 sourceOffset = entry->srcOffset;
x0 -= entryX;
src += sourceOffset;
x0 -= width;
s_entry = entry;
x0++;
s_left = x0;
i32 currentX = width + x0 - 1;
s_right = currentX;
s_y = entryY + y;

// Retained lower-fuzzy literal ownership.
u32 pendingSkip;
i32 copyCount = literalCount;
if (static_cast<i32>(literalCount) > 0) {
    s_loopCount = literalCount;
    do {
        // literal pixel publication
    } while (--copyCount != 0);
}
#endif
