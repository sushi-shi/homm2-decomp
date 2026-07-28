/*
 * FlipIconToBitmap, RVA 0xd1ba0: semantic setup/static-order and cursor-owner
 * matrices.
 *
 * Setup matrix:
 *   build/iconf2b-setup-axes-20260727.json
 *   build/source-variant-batch/iconf2b-setup-clean-corrected-20260727/
 *
 * Direct setup removes the incidental entries/x0/w locals:
 *
 *   s_entry = &srcIcon->Entries()[frame];
 *   s_src = srcIcon->m_data + s_entry->srcOffset;
 *   u8* src = s_src;
 *   s_left = ((x - s_entry->w) - s_entry->x) + 1;
 *   i32 X = s_entry->w + s_left - 1;
 *   s_right = X;
 *   s_y = s_entry->y + y;
 *
 * Neither direct setup nor the FlipIconToBitmapYModify-relative static order
 * was independently better.  Their combination moved the clean match from
 * 79.713530% to 81.090190%, with the exact retail 76/76-block CFG retained.
 * That combined source was accepted.
 *
 * Cursor-owner matrix:
 *   build/iconf2b-cursor-owner-axes-20260727.json
 *   build/source-variant-batch/iconf2b-cursor-owners-clean-20260727/
 *
 * Keeping both hot locals (`u8* src` and `i32 X`) remained best at 81.090190%.
 * Publishing either cursor directly through its static owner introduced many
 * extra relocations:
 *
 *   static src, local X   67.740050%, 107/81 relocations
 *   local src, static X   67.175064%, 112/81 relocations
 *   static src, static X  59.981434%, 134/81 relocations
 *
 * The two cursor locals are therefore retained.  The family preference for
 * static-backed state does not override clear optimized register-owner
 * evidence.
 */

void FlipIconToBitmap_semantic_setup_and_cursor_owner_attempts() {
}
