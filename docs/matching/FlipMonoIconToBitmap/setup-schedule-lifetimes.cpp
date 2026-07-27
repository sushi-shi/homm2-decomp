/*
 * FlipMonoIconToBitmap setup-schedule and local-lifetime campaign, 2026-07-27.
 *
 * Starting evidence:
 *   retained source MAX 80.646706%, size 507, 26/27 relocations;
 *   retained structural trial 30 77.323360%, size 514, 27/27 relocations,
 *   exact 26-block flow, 19 exact plus seven size-only blocks.
 *
 * 1. Shared-right setup screen and complete state cross
 *
 *   build/flipmono-setup-schedule-clean-axes-20260727.json
 *   build/flipmono-setup-schedule-clean-manifest-20260727.json
 *   build/match-variants/flipmono-setup-schedule-clean-20260727/
 *   build/flipmono-setup-schedule-state-axes-20260727.json
 *   build/flipmono-setup-schedule-state-manifest-20260727.json
 *   build/match-variants/flipmono-setup-schedule-state-20260727/
 *
 * Eight clean setup schedules tested entry/source snapshot order, direct
 * versus accessor source calculation, pre- versus post-increment right-edge
 * construction, and early Y calculation. The direct pre-increment form
 *
 *   right = w + x0 + 1; x0++; X = right - 1;
 *
 * improved clean code from 70.239520% to 73.676640% with 27/27 relocations.
 * The retained, direct, and accessor forms then received clean plus 50 top
 * forest states (153/153 complete). Direct trial 48 produced 79.784430%,
 * size 518, 27/27 ordered relocations, exact 26-block flow, and 20 exact plus
 * six size-only blocks. Its clue artifacts are retained at:
 *
 *   build/tu-state-noise/
 *     iconmf2b-direct-preincrement-trial48-structural-clue-20260727/
 *
 * 2. Neighbor-family and entry/X materialization screens
 *
 *   build/flipmono-family-setup-clean-axes-20260727.json
 *   build/flipmono-family-setup-clean-manifest-20260727.json
 *   build/match-variants/flipmono-family-setup-clean-20260727/
 *   build/flipmono-entry-x-materialization-clean-axes-20260727.json
 *   build/flipmono-entry-x-materialization-clean-manifest-20260727.json
 *   build/match-variants/flipmono-entry-x-materialization-clean-20260727/
 *
 * The eight neighbor-family shapes included FlipIcon's m_data/sourceOffset
 * lifetime and FlipDim's entry/source/x0 publication order. A second 30-arm
 * matrix crossed six typed/byte-base entry-pointer materializations with five
 * ways of deriving X from the shared right edge. Split typed expressions and
 * X decrement spellings collapsed to existing code. Byte-base forms were
 * worse; the FlipIcon-shaped form was 68.401200%, size 521. No generated
 * pointer arithmetic or decrement-only spelling was retained.
 *
 * 3. Complete surviving lifetime census
 *
 *   build/flipmono-entry-lifetimes-state-axes-20260727.json
 *   build/flipmono-entry-lifetimes-state-manifest-20260727.json
 *   build/match-variants/flipmono-entry-lifetimes-state-20260727/
 *
 * Typed split-entry and FlipDim-style schedules each received clean plus 50
 * top forest states (102/102 complete). The FlipDim schedule moved x0's
 * initialization before global publication, then loaded w and entryY before
 * subtracting entry->x and w. With the old source helper still present, its
 * trial 2 reached 79.317370%, size 517, 27/27 relocations, exact 26-block
 * flow, and 21 exact plus five size-only blocks. The whole 32-instruction
 * setup block matched retail's instruction count. Trial 15 reached a separate
 * 81.257484% fuzzy island with 26/27 relocations and 22 exact blocks.
 *
 * 4. Clean-source helper audit
 *
 * The schedule no longer used FlipMonoSource. It was removed rather than
 * retained as compiler-state steering, and the actual helperless source
 * received a fresh unchanged-source clean plus 50-state census:
 *
 *   build/tu-state-noise/
 *     iconmf2b-flipdim-schedule-clean-helperless-census-20260727/
 *
 * Helperless trial 2 remains the strongest honest structural result:
 * 75.035930%, size 519, 27/27 ordered relocation identities/addends, exact
 * 26-block flow, 21 exact plus five size-only blocks, and a 32/32-instruction
 * setup block. Its paired artifacts are retained at:
 *
 *   build/tu-state-noise/
 *     iconmf2b-flipdim-helperless-trial2-structural-clue-20260727/
 *
 * Helperless trial 37 is the new hash-scoped MAX: 81.113770%, size 507,
 * 26/27 relocations, exact 26-block flow, 22 exact plus four size-only blocks.
 * It was replayed through the official recorder at:
 *
 *   build/tu-state-noise/
 *     iconmf2b-flipdim-helperless-trial37-record-max-20260727/
 *
 * The generated MAX is 81.1138 for source/dependency hash
 * cee6daf5b3d2.b833f2b08aa6. Generated forests and the unused-helper state
 * were not retained.
 *
 * Disposition: retain the FlipDim-family local schedule, shared
 * pre-increment right edge, and the prior clipRight/entryHeight structure.
 * It advances both structural campaign memory and clean-source fuzzy MAX.
 */

#if 0
// Retained setup structure.
IconEntry* entries = srcIcon->Entries();
IconEntry* entry = &entries[frame];
u8* srcData = reinterpret_cast<u8*>(entries) + entry->srcOffset;
i32 x0 = x;
gFMEntry = entry;
gFMSrc = srcData;
i32 w = entry->w;
i32 entryY = entry->y;
x0 = x0 - entry->x;
x0 = x0 - w;
i32 right = w + x0 + 1;
x0++;
gFMX0 = x0;
i32 X = right - 1;
gFMXEnd = X;
gFMY = y + entryY;

// Rejected FlipIcon-family lifetime.
u8* familySrc = srcIcon->m_data;
i32 familyX0 = x;
i32 familyW;
IconEntry* familyEntry = &srcIcon->Entries()[frame];
familyW = familyEntry->w;
i32 familyEntryX = familyEntry->x;
i32 familyEntryY = familyEntry->y;
i32 familySourceOffset = familyEntry->srcOffset;
gFMEntry = familyEntry;
familySrc += familySourceOffset;
familyX0 = familyX0 - familyEntryX;
gFMSrc = familySrc;
#endif
