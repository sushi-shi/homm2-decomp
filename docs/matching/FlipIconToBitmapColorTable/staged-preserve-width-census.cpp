// Completed census for the previously clean-only staged setup.
//
// This source order is structurally credible because it mirrors the retail
// opening's load order: snapshot x, assign width, then consume entry.x.  It
// also preserves width through the clipping predicate, where retail reads a
// spilled width from [esp+0x14].
//
// Artifact:
//   build/match-variants/iconf2bc-staged-preserve-width-states-20260727/
// Coverage: baseline plus this arm, each in clean plus 50 forest states.
// Best staged result: 86.710330%, size 1339, 84/83 relocations, 76/76 blocks
// (66 exact, 10 size-only, no flow mismatch).
//
// Disposition: do not retain in reconstructed source.  The complete state
// census found no retail width-spill orbit, but this corrects the earlier
// premature disposition based only on its lower clean fuzzy score.

#if 0
i32 x0 = x;
i32 w;
i32 pitch;
w = entries[frame].w;
x0 = x0 - entries[frame].x;
IconEntry* entry = &entries[frame];
x0 = x0 - w;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
#endif
