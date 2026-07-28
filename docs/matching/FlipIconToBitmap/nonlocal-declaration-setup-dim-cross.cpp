/*
 * FlipIconToBitmap nonlocal declaration/setup/dim/literal cross, 2026-07-27.
 *
 * This matrix deliberately crossed source regions that are far apart because
 * MSVC 4.2 register allocation and code scheduling are TU-wide and nonlocal.
 * It did not rank candidates by fuzzy score alone.
 *
 * Axes:
 *   - six function-static declaration orders;
 *   - four setup/publication structures;
 *   - four dim-pixel ownership structures;
 *   - three clipped-literal joins.
 *
 * All 288 source combinations completed:
 *   build/iconf2b-nonlocal-islands-axes-20260727.json
 *   build/iconf2b-nonlocal-islands-manifest-20260727.json
 *   build/source-variant-batch/iconf2b-nonlocal-islands-20260727/
 *
 * The highest-fuzzy clean candidate was 87.254640%, size 1255, 83/81
 * relocations.  It combined the historical declaration order with staged
 * field locals and direct dim expressions.
 *
 * A deliberately retained lower island was more structurally useful:
 * 84.291780%, size 1238, 82/81 relocations, exact 76-block CFG, 65 exact and
 * 11 size-only blocks, with the first 33 instructions structurally aligned.
 * It combined the historical declaration order, publications interleaved
 * with entry-field loads, and the direct dim expression below.
 *
 * Both selected source shapes then received clean plus the same 50 forest
 * states (102/102 cells).  No state improved either clean shape:
 *   build/iconf2b-nonlocal-selected-manifest-20260727.json
 *   build/source-variant-batch/iconf2b-nonlocal-selected-20260727/
 */

#if 0
// Lower-fuzzy topology descendant retained as the next branch seed.
IconEntry* entry = &srcIcon->Entries()[frame];
s_entry = entry;
u8* src = srcIcon->m_data;
i32 entryX = entry->x;
i32 x0 = x - entryX;
src += entry->srcOffset;
i32 width = entry->w;
x0 = (x0 - width) + 1;
s_left = x0;
i32 currentX = (width - 1) + x0;
s_right = currentX;
s_y = y + entry->y;

do {
    *dimDestination = s_dimPal[*dimDestination];
    dimDestination++;
    s_dimDst = dimDestination;
} while (--count != 0);
#endif
