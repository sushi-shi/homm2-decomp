// Greedy exact-prefix experiment: preserve the icon width through clipping.
//
// Crossing two direct-root setups with entry reload versus local-width clip
// ownership extended the exact prefix from 24 to 26 bytes.  The strongest
// clean global score in that matrix was 86.549120%, below the retained MAX.
// Artifact: build/match-variants/iconf2bc-greedy-width-owner-prefix-20260726/
//
// Three subtraction term orders and five explicit width lifetimes all stayed
// at 26 exact bytes.
// Artifacts:
//   build/match-variants/iconf2bc-greedy-term-order-prefix-20260726/
//   build/match-variants/iconf2bc-greedy-width-lifetime-prefix-20260726/
//
// A complete 50-state forest census of the clean 26-byte arm produced 26 exact
// bytes in every state.  Trial 42 improved fuzzy score to 86.649870%, but did
// not move the first divergence.
// Artifact: build/match-variants/iconf2bc-greedy-prefix26-islands-20260726/
//
// The first divergence is register ownership at offset 0x1a within the width
// load: retail owns width in ECX and entry.x in EAX, while this branch owns
// width in EAX and entry.x in ECX.  This is therefore a structural branch seed,
// not evidence that TU-state randomness can close the setup.

#if 0
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

if (clip != ICON_DRAW_NO_CLIP) {
    i32 currentY = gFCY;
    if (x0 < clipX || clipW + clipX < x0 + w || currentY < clipY
        || clipY + clipH < entry->h + currentY) {
    }
}

// Representative lifetime descendants.
i32 width;
width = entries[frame].w;
x0 = x - entries[frame].x;
x0 -= width;
#endif
