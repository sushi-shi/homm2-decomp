/*
 * IconToBitmapColorTable structural experiment: transfer the entry-selection
 * and byte-reader ownership used by the related IconToBitmap, flipped, and
 * Y-modified blitters.
 *
 * The entry axis tested the retained byte-offset setup plus four typed family
 * shapes:
 *
 *     IconEntry* entry = reinterpret_cast<IconEntry*>(data) + frame;
 *
 *     IconEntry* entries = reinterpret_cast<IconEntry*>(srcIcon->m_data);
 *     IconEntry* entry = &entries[frame];
 *
 *     IconEntry* entries = srcIcon->Entries();
 *     IconEntry* entry = &entries[frame];
 *
 * The typed alternatives either retained the entryX/sourceOffset snapshots or
 * read entry->x and entry->srcOffset directly.  A second axis kept the five
 * manual advance/read pairs or routed all five through:
 *
 *     ReadIconRleByte(gCTSrc)
 *
 * Complete matrix (2026-07-26):
 *   five entry shapes x two reader shapes x (clean + 50 forest states)
 *   510/510 compiled in 393.44 seconds; source restored
 *   retail: size 1455, 91 relocations, 86 blocks
 *
 * Best score remained the retained setup/manual reader at forest trial 11:
 *   82.776990%, size 1428, 89/91 relocations,
 *   69 exact blocks + 17 size-only, no flow mismatch.
 *
 * The strongest alternate entry island was typed-pointer-add/manual at the
 * same state:
 *   82.765260%, size 1429, 89/91 relocations,
 *   69 exact blocks + 17 size-only.
 *
 * The inline reader did move toward retail size and relocation count, but away
 * from the aligned block bodies.  Its best typed-pointer state was:
 *   82.392020%, size 1438, 90/91 relocations,
 *   67 exact blocks + 19 size-only, no flow mismatch.
 *
 * The raw entries view and Entries() accessor emitted identical objects for
 * corresponding states, so no accessor boundary survived this /O2 lowering.
 *
 * Artifacts:
 *   build/match-variants/icon2bc-family-setup-reader-axes-20260726.json
 *   build/match-variants/icon2bc-family-setup-reader-matrix-20260726.json
 *   build/match-variants/icon2bc-family-setup-reader-islands-20260726/
 *
 * Disposition: do not land an entry or reader change.  The reader orbit remains
 * useful evidence because it recovers one relocation and ten bytes, but this
 * matrix eliminates it as a descendant of these entry-selection shapes.
 */
