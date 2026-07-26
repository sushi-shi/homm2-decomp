// Experiment: combine structures observed across the icon-function family.
// Artifacts:
//   build/match-variants/iconf2bc-family-fusion-clean-20260726/
//   build/match-variants/iconf2bc-family-fusion-islands-20260726/
// Axes: serialized entry setup, file/function scratch scope, split/postincrement
// decoder reads. Best across the reviewed family was 86.380356%.
// Disposition: rejected.

#if 0
u8* src = srcIcon->m_data;
i32 entryOffset = frame * sizeof(IconEntry);
i32 w = reinterpret_cast<IconEntry*>(src + entryOffset)->w;
i32 entryX = reinterpret_cast<IconEntry*>(src + entryOffset)->x;
IconEntry* entry = reinterpret_cast<IconEntry*>(src + entryOffset);
i32 x0 = x;
x0 = x0 - entryX;
x0 = x0 - w;
gFCEntry = entry;
src += entry->srcOffset;
x0++;
gFCX0 = x0;
i32 X = (gFCXEnd = w + x0 - 1);
i32 Y = y + entry->y;
gFCY = Y;

// The matrix also moved the existing gFC* scratch declarations between
// file scope and function-local static scope, and crossed both read forms:
i32 px = *dp++;
gFCDimDst = dp;
#endif
