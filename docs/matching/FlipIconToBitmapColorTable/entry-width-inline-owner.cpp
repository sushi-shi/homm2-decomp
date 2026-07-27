// Experiment: use a sibling-derived inline entry selector that returns the
// selected IconEntry while publishing width by reference.
//
// Motivation: retail enters B0 with x owned by EBP and width by ECX, advances
// src with [entry+9], then spills width. A selector/out-reference boundary could
// plausibly produce that ownership without raw steering.
//
// Artifact: build/match-variants/iconf2bc-entry-width-inline/
// Coverage: direct/helper crossed with clean + 50 TU states
// (102/102 compiled).
// Best:
//   direct                 87.035260%, size 1341, blocks 67 exact / 9 size
//   inline selector        85.838790%, size 1341, blocks 67 exact / 9 size
// Both retained 84/83 relocation counts and 76/76 topology.
// Disposition: retained as a lower structural island only; it did not reproduce
// the retail B0 ownership and does not improve MAX.

#if 0
static inline IconEntry* FlipColorSelectEntry(
    u8* iconData, i32 frame, i32& width
) {
    IconEntry* entry = reinterpret_cast<IconEntry*>(iconData) + frame;
    width = entry->w;
    return entry;
}

u8* src = srcIcon->m_data;
i32 w;
IconEntry* entry = FlipColorSelectEntry(src, frame, w);
i32 x0 = x - entry->x;
x0 = x0 - w;
#endif
