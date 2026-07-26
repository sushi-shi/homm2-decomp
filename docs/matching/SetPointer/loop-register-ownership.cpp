// SetPointer mask-loop register-ownership matrix, 2026-07-26.
//
// Artifact:
//   build/match-variants/mousemgr-setpointer-loop-ownership-20260726/
//
// Complete matrix: five reviewed structural arms x clean state plus 50
// declaration-forest states = 255/255 compiled, not wall-time truncated.
// Retail has 36 blocks, size 1029, and 73 ordered relocations.
//
// Results:
//   for_block_declarations          clean 98.402250%, best 99.962400%
//   for_predeclared_counters        clean 98.402250%, EXACT at trials 22/35/38
//   while_predeclared_counters      clean 98.402250%, EXACT at trials 22/35/38
//   explicit_source_and_mask_offsets clean 78.981200%, no exact
//   explicit_source_offset_derived_mask clean 73.842100%, no exact
//
// Both exact arms produced retail size 1029, all 36 blocks exact, and all
// 73 ordered relocations. The predeclared for-loop is retained because it is
// the simplest mid-1990s C/C++ spelling in the exact structural family.

#if 0
// Previous source: declarations owned by the for-statements.
for (i32 row = 0; row < MOUSE_CURSOR_BITMAP_WIDTH; row++) {
    for (i32 column = 0; column < MOUSE_CURSOR_BITMAP_WIDTH; column++) {
        ProcessMaskPixel(row, column);
    }
}

// Retained exact structural arm.
i32 row;
i32 column;
for (row = 0; row < MOUSE_CURSOR_BITMAP_WIDTH; row++) {
    for (column = 0; column < MOUSE_CURSOR_BITMAP_WIDTH; column++) {
        ProcessMaskPixel(row, column);
    }
}

// Byte-identical exact sibling arm.
i32 row = 0;
i32 column;
while (row < MOUSE_CURSOR_BITMAP_WIDTH) {
    column = 0;
    while (column < MOUSE_CURSOR_BITMAP_WIDTH) {
        ProcessMaskPixel(row, column);
        column++;
    }
    row++;
}

// Lower branch: two explicit induction offsets.
i32 sourceOffset = 0;
i32 maskOffset = 0;
while (sourceOffset < MOUSE_CURSOR_COLOR_BYTES) {
    for (i32 column = 0; column < MOUSE_CURSOR_BITMAP_WIDTH; column++) {
        ProcessMaskPixelByOffsets(sourceOffset, maskOffset, column);
    }
    sourceOffset += MOUSE_CURSOR_BITMAP_WIDTH;
    maskOffset += MOUSE_CURSOR_MASK_ROW_BYTES;
}

// Lower branch: mask address derived from the source offset.
for (i32 sourceOffset = 0; sourceOffset < MOUSE_CURSOR_COLOR_BYTES;
     sourceOffset += MOUSE_CURSOR_BITMAP_WIDTH) {
    for (i32 column = 0; column < MOUSE_CURSOR_BITMAP_WIDTH; column++) {
        ProcessMaskPixelByDerivedOffset(sourceOffset, column);
    }
}
#endif
