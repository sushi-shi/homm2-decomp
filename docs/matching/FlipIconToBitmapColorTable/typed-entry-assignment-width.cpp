// Structural fusion: combine the current typed icon::Entries() root with the
// assignment-based width lifetime from the historical 46-byte-prefix branch.
//
// This combination had not been tested: earlier assignment matrices changed
// the root back to raw icon::m_data, while the current typed root is what lets
// MSVC emit the retail-like direct packed-source operation:
//     add esi, dword ptr [edi + 9]
//
// Four legitimate evaluation trees were tested as one bounded matrix and
// crossed independently with clean plus 50 declaration-forest TU states:
//   x - entry->x - (w = entry->w)
//   x - (w = entry->w) - entry->x
//   (w = entry->w, x - entry->x - w)
//   x - ((w = entry->w) + entry->x)
// Artifacts:
//   build/iconf2bc-typed-assignment-width-axes.json
//   build/match-variants/iconf2bc-typed-assignment-width-order-clean-20260727/
//   build/match-variants/iconf2bc-typed-assignment-width-order-states-20260727/
//
// All four trees were byte-identical within each TU state.  Forest states 2,
// 18, and 28 produced a new best local island:
//   fuzzy: 87.201510%
//   size: 1338 (retail 1357)
//   relocations: 84/83
//   blocks: 76/76, 66 exact, 10 size-only, no flow mismatch
//   text hash: 883dc578af433e2b
//   normalized relocation hash: 36fd2278f07ddadb
//
// The island matches the typed direct source advance and owns x0 in EBP.  Its
// first byte divergence is still offset 0x1a: width and entry.x occupy EAX and
// ECX respectively, opposite retail.  It therefore does not explain retail's
// following `mov [esp+0x14], ecx` width spill.
//
// Disposition: preserve as a new branch seed and advance the observed local
// score, but do not change reconstructed source or any retained historical
// MAX.  The results JSON retains the paired text bytes, ordered relocation
// stream, byte differences, and block metrics for this disposable state.

#if 0
IconEntry* entries = srcIcon->Entries();
u8* src = reinterpret_cast<u8*>(entries);
IconEntry* entry = &entries[frame];
i32 w;
i32 x0 = x - entry->x - (w = entry->w);
i32 pitch;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);

if (x0 < clipX || clipW + clipX < x0 + w) {
}
#endif
