// Experiment: give the color-table lookup its own inline owner at the solid,
// unclipped-literal, and clipped-literal sites.
//
// Artifact: build/match-variants/iconf2bc-color-map-inline/
// Coverage: all eight site subsets crossed with clean + 50 TU states
// (408/408 compiled).
// Results:
//   no solid-site helper:     at most 87.035260%, size 1341, blocks 67/9
//   any solid-site helper:    at most 86.052895%, size 1343, blocks 66/10
// The unclipped and clipped literal-only helpers independently and jointly
// collapsed to the direct object in several states. All arms retained 84/83
// relocation counts.
// Disposition: rejected. The solid lookup boundary creates one lower island;
// literal lookup helpers are compiler-neutral and do not explain retail.

#if 0
static inline u8 FlipColorMap(u8* colorTable, i32 color) {
    return colorTable[color];
}

gFCColor = FlipColorMap(colorTable, *src++);
dst[1] = FlipColorMap(colorTable, c);
u8 mapped = FlipColorMap(colorTable, c);
#endif
