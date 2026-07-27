/*
 * FlipIconToBitmap, RVA 0xd1ba0: shared icon::Entries() ownership.
 *
 * The live 75-block source previously recovered its entry from the raw byte
 * cursor. Two member-accessor structures were each tested against clean plus
 * 50 forest states:
 *
 *   IconEntry* entry = &srcIcon->Entries()[frame];
 *
 *   IconEntry* entries = srcIcon->Entries();
 *   IconEntry* entry = &entries[frame];
 *
 * Artifacts:
 *   build/iconf2b-member-entry-manifest.json
 *   build/match-variants/iconf2b-member-entry/
 *   build/iconf2b-owned-entries-manifest.json
 *   build/match-variants/iconf2b-owned-entries/
 *
 * The direct expression reaches 86.968170% at forest trial 5, size 1246,
 * 79/81 relocations. A separately owned table pointer is stronger: the matrix
 * clean compile reaches 87.007960%, size 1246, 79/81 relocations. The
 * canonical build records 87.0212%, improving the historical 86.8594% MAX
 * without disposable compiler state.
 *
 * This is the same typed inline mechanism now used by IconToBitmap and other
 * icon-family blitters. The separately owned Entries() table is retained as
 * legitimate reconstruction. It does not close the missing setup block or
 * the two relocation occurrences.
 */

void FlipIconToBitmap_member_entry_ownership_attempts() {
}
