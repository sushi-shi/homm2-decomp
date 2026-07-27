// Structural experiment: keep the incoming x parameter as the horizontal
// coordinate owner through initial clipping instead of copying it to x0.
//
// The two credible endpoint lifetimes below were crossed with clean plus all
// 50 declaration-forest TU states (3 setup arms x 51 states).
// Artifacts:
//   build/iconf2bc-x-parameter-owner-axes.json
//   build/match-variants/iconf2bc-x-parameter-owner-clean-20260727/
//   build/match-variants/iconf2bc-x-parameter-owner-states-20260727/
//
// The mutate-width arm re-entered the current 87.035260%, size-1341 orbit in
// three non-clean states.  The preserve-width arm remained a genuine lower
// structural island: best 86.710330%, size 1339, 84/83 relocations, and
// 76/76 blocks (66 exact, 10 size-only, no flow mismatch).
//
// Disposition: do not retain in reconstructed source.  Eliminating x0 did not
// produce the retail width spill, and the mutate-width form is compiler-
// equivalent to the current source in several legitimate TU states.

#if 0
i32 w = entries[frame].w;
x = x - entries[frame].x;
IconEntry* entry = &entries[frame];
x = x - w;
gFCEntry = entry;
src += entry->srcOffset;
x++;
gFCX0 = x;
i32 X = (gFCXEnd = w + x - 1);

if (x < clipX || clipW + clipX < x + w) {
}
#endif
