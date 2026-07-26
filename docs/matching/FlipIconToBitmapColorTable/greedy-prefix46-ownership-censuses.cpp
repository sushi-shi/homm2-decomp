// Ownership descendants of the 46-byte state-39 setup island.
//
// Retail-family evidence:
//   FlipIconToBitmapColorTable (0xd9790) and FlipIconToBitmap (0xd1ba0)
//   both consume the packed source offset with:
//       add esi, dword ptr [edi + 9]
//   Their surrounding load/subtract order differs, so the sibling supports a
//   shared direct-memory idiom but not a wholesale setup transplant.
//
// Full 50-state censuses crossed the assignment-expression setup with:
//   - four source-pointer ownership/lifetime structures (204 compiles);
//   - four entry-pointer constructions (204 compiles);
//   - three post-publication gFCEntry ownership forms (153 compiles);
//   - three source-offset ownership forms (153 compiles);
//   - IconEntry reference ownership (51 compiles);
//   - three clip-only width snapshot placements (153 compiles).
// Artifacts:
//   build/match-variants/iconf2bc-prefix46-source-owner-states-20260726/
//   build/match-variants/iconf2bc-prefix46-entry-owner-states-20260726/
//   build/match-variants/iconf2bc-prefix46-published-entry-owner-states-20260726/
//   build/match-variants/iconf2bc-prefix46-source-offset-owner-states-20260726/
//   build/match-variants/iconf2bc-prefix46-entry-reference-states-20260726/
//   build/match-variants/iconf2bc-prefix46-clip-width-snapshot-states-20260726/
//
// All retained 76/76 CFG forms had at most the existing 46 exact bytes.  The
// best 46-byte descendant remained state 39 at 86.153656%, size 1337 and
// relocations 84/83.  Publishing through gFCEntry, using an IconEntry
// reference, and the ordinary typed-table form collapsed to that same object.
// A distinct early sourceOffset snapshot reached 29 exact bytes and 86.408060%
// at state 42; preserve it as a lower-prefix structural clue, not source.
//
// Changing IconEntry::srcOffset from i32 to the ABI-equivalent retail-long
// alias i32l produced the identical state distribution and state-39 object:
//   build/match-variants/iconf2bc-prefix46-i32l-offset-states-20260726/
// The header was restored after the diagnostic.
//
// Disposition: no reconstructed source or MAX change.  The direct-memory add
// at offset 0x2e remains the first unsolved byte in the active 46-byte clue.

#if 0
// Source pointer ownership.
u8* iconData = srcIcon->m_data;
IconEntry* entries = reinterpret_cast<IconEntry*>(iconData);
IconEntry* entry = &entries[frame];
iconData += entry->srcOffset;
u8* src = iconData;

// Entry construction ownership.
i32 entryOffset = frame * sizeof(IconEntry);
IconEntry* entry = reinterpret_cast<IconEntry*>(src + entryOffset);

// Published global ownership.
gFCEntry = entry;
src += gFCEntry->srcOffset;

// Source-offset ownership.  Declaring this before coordinate construction
// created the distinct 29-byte orbit.
i32 sourceOffset = entry->srcOffset;
src += sourceOffset;

// Reference ownership collapsed to the ordinary pointer object.
IconEntry& selectedEntry = entries[frame];
gFCEntry = &selectedEntry;
src += selectedEntry.srcOffset;

// The retail spill suggested a width value owned only by clipping.  Three
// declaration points were tested while endpoint construction continued to use
// w; all three compiled identically in clean state and rejoined state 39.
src += entry->srcOffset;
x0++;
i32 clipWidth = w;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
#endif
