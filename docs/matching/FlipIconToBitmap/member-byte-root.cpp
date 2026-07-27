/*
 * FlipIconToBitmap, RVA 0xd1ba0: shared typed table and byte-root ownership.
 *
 * The FlipMono family derives both the selected entry and byte stream from a
 * typed Entries() table. The analogous structure was:
 *
 *   IconEntry* entries = srcIcon->Entries();
 *   IconEntry* entry = &entries[frame];
 *   u8* src = reinterpret_cast<u8*>(entries);
 *
 * Complete clean + 50 forest-state sweep:
 *   build/iconf2b-family-entry-root-manifest.json
 *   build/match-variants/iconf2b-family-entry-root/
 *
 * Best: 86.533160%, size 1246, 79/81 relocations. This is a coherent lower
 * island but worse than keeping the byte cursor's direct m_data ownership.
 * The byte-root change was removed.
 */

void FlipIconToBitmap_member_byte_root_attempt() {
}
