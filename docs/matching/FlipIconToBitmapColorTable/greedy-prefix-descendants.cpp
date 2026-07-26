// Descendants of the 26-byte greedy-prefix branch.
//
// Literal FlipIconToBitmap-family setup (4 arms): at most 26 exact bytes,
// 85.919395%, size 1339, relocs 84/83.
// Artifact: build/match-variants/iconf2bc-greedy-literal-sibling-20260726/
//
// Setup inline helpers (4 arms): regressed to 11 exact bytes; the added inline
// declarations changed the compiler orbit before the target setup.
// Artifact: build/match-variants/iconf2bc-greedy-inline-prefix-20260726/
//
// Reusing icon width as destination pitch (2 valid arms): at most 26 exact
// bytes; the reuse arm scored 86.052895% and emitted size 1331.
// Artifact: build/match-variants/iconf2bc-greedy-width-reuse-20260726/
//
// Width type matrix (i32, const i32, i16, i32l): the three 32-bit forms were
// identical at 26 exact bytes; i16 regressed to 17.
// Artifact: build/match-variants/iconf2bc-greedy-width-type-20260726/
//
// Explicit entry.x/width ownership snapshots (4 arms): all stayed at 26 exact
// bytes, 85.823680%, size 1335, relocs 84/83.
// Artifact: build/match-variants/iconf2bc-greedy-register-owner-20260726/
//
// Disposition: none of these descendants closes or extends the first register
// byte.  Keep the 26-byte direct-root/local-width parent as the branch seed;
// do not replace the current reconstructed source or lower its retained MAX.

#if 0
// Literal sibling-family order.
u8* src = srcIcon->m_data;
i32 x0 = x;
i32 w;
IconEntry* entry = reinterpret_cast<IconEntry*>(src) + frame;
w = entry->w;
x0 -= entry->x;
x0 -= w;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
i32 Y = y + entry->y;
gFCY = Y;

// Snapshot ownership descendants.
IconEntry* entries = reinterpret_cast<IconEntry*>(src);
i32 entryX = entries[frame].x;
i32 width = entries[frame].w;
x0 = (x - entryX) - width;

// Width/pitch lifetime descendant.
width = dest->m_width;
gFCRow = dest->m_pixels + gFCY * width;
#endif
