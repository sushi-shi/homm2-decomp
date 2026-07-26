// Experiment: continue credible lower structures instead of resetting every
// search to the highest-scoring parent.
// Artifacts:
//   build/match-variants/iconf2bc-lower-island-descendants-clean-20260726/
//   build/match-variants/iconf2bc-setup-lower-descendants-clean-20260726/
//
// First complete 2 x 2 x 2 cross:
//   countdown + block-local + open run predicates  87.035260%
//   clipped-root only or forward-index only         86.934510%
//   forward-index + clipped-root                    86.833755%
//   shared run-visibility descendants               83.073050%
// The run helper dominates codegen and masks both downstream choices.
//
// Second complete 2 x 2 x 2 cross added direct sibling-family entry setup:
//   direct setup + countdown                        85.375310%
//   direct setup + forward-index                    85.274560%
// Clipped-root scope is masked by direct setup. All listed non-helper
// descendants retain size 1341, relocations 84/83, and the 76-block skeleton.
//
// Disposition: active branch tree. These children are handed to the greedy
// first-divergence lane for further changes and prefix comparison. They are
// not rejected solely because their total fuzzy scores are below MAX.

#if 0
// Branch seed A: forward-index literal loop.
for (i32 k = 0; k < cmd; k++) {
    CopyLiteralPixel();
}

// Branch seed B: root-scoped clipped dim count.
i32 cmd;
i32 dimCount;
for (;;) {
    // ...
    dimCount = count;
}

// Branch seed C: direct entry setup, crossed with A and B.
u8* src = srcIcon->m_data;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
i32 w = entry->w;
i32 x0 = x - entry->x - w + 1;
gFCEntry = entry;
src += entry->srcOffset;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
#endif
