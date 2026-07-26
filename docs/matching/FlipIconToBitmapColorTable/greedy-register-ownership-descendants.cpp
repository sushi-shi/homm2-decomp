// Descendant tree for the exact-prefix byte 0x1a register-ownership mismatch.
//
// Retail owns entry width in ECX and entry.x in EAX.  The earlier 26-byte seed
// owned those values in EAX and ECX respectively.
//
// X-before-width sequencing (6 arms) did not change that orbit.
// Artifact: build/match-variants/iconf2bc-prefix26-x-before-width-20260726/
//
// Two materially different inline boundaries were tested:
//   - setup via x0/width output references (6 bodies);
//   - return x0 while publishing width by reference (5 bodies).
// Both retained the 76/76 CFG and 84 relocations, but moved to 17-byte clean
// prefixes.  Their complete 50-state censuses did not re-enter the 26-byte
// orbit; best prefixes remained 17 bytes.
// Artifacts:
//   build/match-variants/iconf2bc-prefix26-inline-outrefs-20260726/
//   build/match-variants/iconf2bc-prefix26-inline-outrefs-states-b-20260726/
//   build/match-variants/iconf2bc-prefix26-inline-return-20260726/
//   build/match-variants/iconf2bc-prefix26-inline-return-states-20260726/
//
// Assignment-expression setup (8 arms) kept 76/76 blocks and 84 relocations.
// Six arms collapsed to one size-1341 object.  A complete 50-state census then
// found a new structural island at state trial 39:
//   exact prefix: 46 bytes (up from 26)
//   fuzzy score: 86.153656%
//   size: 1337, relocs: 84/83
//   blocks: 76/76, 66 exact, 10 size-only, no flow mismatch
// Artifact: build/match-variants/iconf2bc-prefix26-assignment-states-20260726/
//
// State 39 emits the retail register ownership and matches through the complete
// gFCEntry store/relocation.  This lower-fuzzy object is the retained descendant
// seed; trial 39 is disposable TU-state input and is not reconstructed source.

#if 0
// Representative assignment-expression arm that exposes the 46-byte island.
u8* src = srcIcon->m_data;
IconEntry* entries = reinterpret_cast<IconEntry*>(src);
IconEntry* entry = &entries[frame];
i32 w;
i32 x0 = x - entry->x - (w = entry->w);
i32 pitch;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
i32 Y = y + entry->y;
gFCY = Y;

if (clip != ICON_DRAW_NO_CLIP) {
    i32 currentY = gFCY;
    if (x0 < clipX || clipW + clipX < x0 + w || currentY < clipY
        || clipY + clipH < entry->h + currentY) {
    }
}

// Representative assignment-expression descendants in the same clean orbit.
x0 = (w = entry->w, x - entry->x - w);
x0 = x - ((w = entry->w) + entry->x);
x0 = x - (entry->x + (w = entry->w));
#endif
