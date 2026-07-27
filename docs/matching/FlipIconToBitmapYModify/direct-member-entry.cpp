/*
 * FlipIconToBitmapYModify, RVA 0xd9ce0: direct member-entry publication.
 *
 * The retained setup owns the table before publishing the selected entry:
 *
 *   IconEntry* entries = srcIcon->Entries();
 *   gFYEntry = &entries[frame];
 *
 * This attempt used:
 *
 *   gFYEntry = &srcIcon->Entries()[frame];
 *
 * Complete clean + 50 top-of-TU forest-state sweep:
 *   build/iconf2by-direct-member-entry-manifest.json
 *   build/match-variants/iconf2by-direct-member-entry/
 *
 * The direct and table-owner forms reproduce the same significant states,
 * including 93.592490%, size 1420, 144/144 relocations at trial 10.
 * With no codegen advantage, the explicit table owner remains retained.
 */

void FlipIconToBitmapYModify_direct_member_entry_attempt() {
}
