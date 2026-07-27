/*
 * IconToBitmap entry/setup and scratch-storage structure census.
 *
 * The reviewed shapes included:
 *
 *     u8* data = srcIcon->m_data;
 *     IconEntry* entry =
 *         reinterpret_cast<IconEntry*>(data + frame * sizeof(IconEntry));
 *
 *     IconEntry* entries = reinterpret_cast<IconEntry*>(srcIcon->m_data);
 *     IconEntry* entry = &entries[frame];
 *
 * as well as typed accessors, direct DATA-root publication, file-scope versus
 * function-scope scratch storage, Y snapshots, and de-inlined byte reads.
 *
 * Completed matrices (2026-07-26):
 *
 *   icon2b-current-entry-family-{clean,blocks}-20260726
 *     10/10 variants; best 73.868130%, size 1232, 80/83 relocations.
 *
 *   icon2b-storage-entry-{clean,islands}-20260726
 *     6 clean shapes and 726 source/state variants.  File-scope storage with a
 *     serialized-byte-offset entry was best at 75.980770%, size 1225, 79/83.
 *
 *   icon2b-setup-deinline-{clean,islands}-20260726
 *     6 clean shapes and 726 source/state variants.  Direct DATA-root
 *     publication plus an inline reader reached 75.914830%, size 1225, 79/83.
 *
 *   icon2b-helper-family-setup-islands-20260726
 *     484/484 variants.  The current setup at TU-state trial 73 reached the
 *     retained historical MAX of 78.788460%, size 1245, 83/83 relocation
 *     count.  It is disposable compiler-state evidence, not retained source.
 *
 * A new first-divergence matrix (2026-07-27) tested five X/Y/pitch ownership
 * structures against the clean state and 50 forest states:
 *
 *   icon2b-setup-publication-islands-v2-20260727
 *     255/255 variants, 62 distinct normalized target states.  Staging the
 *     fully computed X and Y plus pitch before publishing gIcX0, gIcPitch, and
 *     gIcY reached 80.780220% at trials 8, 14, 36, 41, 43, and 45.  It has
 *     80/80 flow-exact blocks, size 1216, and 79/83 relocations.
 *
 * A descendant matrix kept Y global and staged only pitch or raw entry->y:
 *
 *   icon2b-pitch-entryy-islands-20260727
 *     255/255 variants, 58 states.  The raw-entry-Y/pitch version reached
 *     80.640110%, size 1239, 82/83 relocation count, but retained two gIcY
 *     references that retail does not emit.
 *
 * The stronger fully staged structure emits no relocation identity absent
 * from retail.  It is missing one each of gIcCnt2, gIcPitch, gIcX0, and gIcY.
 * Retail also computes X, Y, and pitch before their three ordered
 * publications, so this evidence-led source structure is retained as the next
 * branch seed.  It is not exact; the four missing reloads remain the targeted
 * descendants.
 *
 * The first reload-descendant Cartesian matrix crossed three setup-predicate
 * coordinate structures with three row-address structures and all 50 forest
 * states:
 *
 *   icon2b-published-reloads-islands-20260727
 *     459/459 variants, 99 states.  Materializing icon right/bottom endpoints
 *     adds one wrong block and destroys decoder alignment (81/80 blocks), so
 *     those six arms were rejected without fuzzy pruning.
 *
 *     Snapshotting gIcPitch and gIcY immediately before the row calculation
 *     preserves the exact 80-block flow family and reaches 80.807690% at
 *     trials 14, 41, 43, and 45.  Clean size/relocations/block partition are
 *     unchanged; the best state is size 1216 with 79/83 relocations and 62
 *     exact plus 18 size-only blocks.
 *
 * Retail performs both published-coordinate reloads at this row calculation,
 * making the row snapshot legitimate source evidence.  It is retained even
 * though the current compiler orbit still folds the two reads and leaves the
 * same four retail reloads outstanding.
 */

void IconToBitmap_entry_setup_and_storage_attempts() {
}
