// Structural descendants of the 46-byte setup island.
//
// The retail opening stores the selected width at [esp+0x14] after advancing
// the RLE source, then preserves x0 in EBP while it computes the horizontal
// right edge.  The current candidate has one surplus gFCY relocation in the
// initial clipping predicates.  These matrices tested source mechanisms that
// could explain both facts before any internal AST permutations.
//
// Exact FlipIconToBitmap sibling ordering x vertical ownership:
//   build/match-variants/iconf2bc-family-vertical-structure-clean-20260727/
// The exact sibling statement order formed a distinct lower island
// (84.516370%, size 1324, relocations 81/83).  It propagates setup locals too
// aggressively and therefore contradicts the retail relocation structure.
// An inline vertical helper and an ordered split predicate retained 84
// relocations but regressed to 83.969770% and 82.793450%, respectively.
//
// Mixed local/global Y ownership:
//   build/match-variants/iconf2bc-mixed-y-owner-clean-20260727/
// All four mixed-source arms compiled identically at 84.287155%, size 1338,
// relocations 81/83.  MSVC store-forwarded Y through gFCY, removing all three
// initial gFCY reads instead of retaining the one retail read.
//
// Addressable Y snapshot boundary:
//   build/match-variants/iconf2bc-y-snapshot-boundary-clean-20260727/
//   build/match-variants/iconf2bc-pointer-y-snapshot-states-20260727/
// Pointer and reference copies formed the same new clean island at 86.632240%,
// size 1341, relocations 84/83, blocks 76/76 (66 exact, 10 size-only).  The
// complete clean-plus-50 forest census found 51 states and no improvement over
// clean.  A const scalar reproduced the prior object; an inline value-parameter
// helper regressed to 84.188920%.
//
// Published gFCX0 horizontal ownership, matching the sibling family spelling:
//   build/match-variants/iconf2bc-published-x0-owner-clean-20260727/
//   build/match-variants/iconf2bc-published-x0-owner-states-20260727/
// Clean MSVC kept EBP intact and materialized the horizontal right edge in ECX,
// but did not spill width.  The complete clean-plus-50 forest census produced
// 51 states; trial 42 was best at 86.531490%, size 1341, relocations 84/83,
// blocks 76/76 (66 exact, 10 size-only).  Trial 39 reproduced the pre-existing
// 46-byte-prefix object byte-for-byte (text hash 652f62e98b0ad1ce), proving
// that published gFCX0 and local x0 are the same compiler orbit here.
//
// Disposition: none of these structures is retained in reconstructed source.
// The pointer/reference Y snapshot is a real lower structural island, but its
// exhaustive state census did not close or improve MAX.  The next B0 branch
// must explain both retail instructions at offsets 0x2e and 0x32:
//   add esi, [edi+9]
//   mov [esp+0x14], ecx

#if 0
// Exact adapted sibling setup order.
u8* src = srcIcon->m_data;
i32 x0 = x;
i32 w;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
w = entry->w;
x0 = x0 - entry->x;
src += entry->srcOffset;
i32 entryY = entry->y;
x0 = x0 - w;
gFCEntry = entry;
x0++;
gFCX0 = x0;
i32 X = w + x0 - 1;
gFCXEnd = X;
y += entryY;
i32 Y = y;
gFCY = Y;

// Addressable snapshot boundary; reference-copy compiled identically.
const i32* currentYSource = &gFCY;
i32 currentY = *currentYSource;
if (x0 < clipX || clipW + clipX < x0 + w || currentY < clipY
    || clipY + clipH < entry->h + currentY) {
}

// Sibling-family publication ownership; compiler-equivalent to local x0.
if (gFCX0 < clipX || clipW + clipX < gFCX0 + w || currentY < clipY
    || clipY + clipH < entry->h + currentY) {
}
#endif
