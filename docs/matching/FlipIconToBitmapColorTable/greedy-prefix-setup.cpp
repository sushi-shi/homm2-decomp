// Greedy exact-prefix experiment: recover the entry-selection/setup bytes first.
//
// Baseline: 17 exact leading bytes, 87.035260%, size 1341, relocs 84/83.
// Broad setup matrix (13 arms): no arm exceeded 17 exact bytes.
// Artifact: build/match-variants/iconf2bc-greedy-setup-prefix-20260726/
//
// Expression-root matrix (7 arms): the direct m_data root and one complete
// subtraction expression extended the exact prefix to 24 bytes.  Its clean
// score was 85.209070%, so fuzzy score alone would have hidden this structure.
// Artifact: build/match-variants/iconf2bc-greedy-expression-prefix-20260726/
//
// Field-load descendants (8 arms): snapshots, references, repeated casts, and
// summed fields did not exceed 24 bytes.
// Artifact: build/match-variants/iconf2bc-greedy-field-load-prefix-20260726/
//
// Disposition: preserve as a live structural clue, not reconstructed source.

#if 0
// The first arm to extend the exact retail prefix from 17 to 24 bytes.
u8* src = srcIcon->m_data;
IconEntry* entries = reinterpret_cast<IconEntry*>(src);
i32 x0 = (x - entries[frame].w) - entries[frame].x;
IconEntry* entry = &entries[frame];
i32 w = entry->w;
i32 pitch;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
i32 Y = y + entry->y;
gFCY = Y;

// Representative field-load descendants.
i32 entryX = entries[frame].x;
i32 width = entries[frame].w;
x0 = (x - width) - entryX;

IconEntry& selectedEntry = entries[frame];
x0 = (x - selectedEntry.w) - selectedEntry.x;
#endif
