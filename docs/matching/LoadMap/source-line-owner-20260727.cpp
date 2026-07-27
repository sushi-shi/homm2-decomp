/*
 * LoadMap allocator source-line ownership, 2026-07-27.
 *
 * The initial function already had the retail 26-block CFG and aligned /Od
 * frame.  Its complete disassembly diff contained only three allocator line
 * arguments: reconstructed source pushed the folded literals 2893, 2894, and
 * 2902, while retail loaded the existing i16 gMapSourceLine data owner and
 * added 0x59, 0x5a, and 0x62.
 *
 * Restoring those owner-relative expressions produced identical assembly
 * across all 377 instructions, 26/26 exact blocks, retail size 1144, and the
 * complete ordered 51/51 relocation stream.  This was a deterministic
 * structural/data-ownership correction, so no spelling matrix or TU-state
 * census was justified.
 *
 * Disposition: retain the three gMapSourceLine-relative expressions in
 * src/SOURCE/GAME.cpp.
 */

void LoadMap_source_line_owner_record() {
}
