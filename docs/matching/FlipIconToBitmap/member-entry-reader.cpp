/*
 * FlipIconToBitmap, RVA 0xd1ba0: Entries() owner plus shared RLE reader.
 *
 * The five command-stream reads were routed through ReadIconRleByte(src);
 * literal-copy loop reads remained direct, matching the non-flipped family.
 *
 * Complete clean + 50 forest-state sweep:
 *   build/iconf2b-member-entry-reader-manifest.json
 *   build/match-variants/iconf2b-member-entry-reader/
 *
 * This inline boundary creates a radically different 1294-1308-byte island.
 * Best: 68.100800%, size 1295, 79/81 relocations. It expands past retail size
 * while losing aligned bodies, so the reader change was rejected and removed.
 */

void FlipIconToBitmap_member_entry_reader_attempt() {
}
