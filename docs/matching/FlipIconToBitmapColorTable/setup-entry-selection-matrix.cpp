// Experiment: entry-table/root selection and deinlining.
// Artifact: build/match-variants/iconf2bc-setup-islands-20260726/
// Coverage: five source shapes crossed with the requested TU-state forest.
// Best: inline Entries() accessor, 86.455920%.
// Disposition: all alternatives rejected; none beat the previous maximum.

#if 0
// Arm 1: inline accessor (baseline).
IconEntry* entries = srcIcon->Entries();
u8* src = reinterpret_cast<u8*>(entries);

// Arm 2: deinline to a typed root.
IconEntry* entries = reinterpret_cast<IconEntry*>(srcIcon->m_data);
u8* src = reinterpret_cast<u8*>(entries);

// Arm 3: deinline to a byte root.
u8* src = srcIcon->m_data;
IconEntry* entries = reinterpret_cast<IconEntry*>(src);

// Arm 4: retail-style field snapshots.
i32 w = entries[frame].w;
i32 entryX = entries[frame].x;
IconEntry* entry = &entries[frame];
i32 x0 = x - entryX;
x0 = x0 - w;
i32 widthEnd = w + ++x0;
i32 X = (gFCXEnd = widthEnd - 1);
i32 entryY = entry->y;
i32 Y = y + entryY;

// Arm 5: direct entry pointer.
u8* src = srcIcon->m_data;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
i32 w = entry->w;
i32 x0 = x - entry->x - w + 1;
i32 X = (gFCXEnd = w + x0 - 1);
#endif
