/*
 * executive::DoDialog, PoL RVA 0x000d1750, retail size 251.
 * Cross-version structure and restore-loop island census, 2026-08-05.
 *
 * The retained PoL body already has exact 15/15 block topology, retail size,
 * and the complete ordered 16/16 relocation stream. Its eight-byte residual
 * is confined to register ownership in the final manager-list restore loop:
 * candidate uses ECX as the array byte offset and EDX as the restored manager;
 * retail assigns those two roles in the opposite order.
 *
 * The exact Gold/Buka sibling is not a compatible donor for this function. It
 * constructs a real stack executive, saves with a while loop, and restores
 * with an increasing for loop; the sibling retail function is 368 bytes. PoL
 * instead uses raw executive storage and a decrementing do/while restore loop,
 * which explains its compact 251-byte CFG. The sibling structure was rejected
 * rather than imported merely to perturb compiler state.
 *
 * Two independent unchanged-source forest/top censuses completed 50 trials
 * each. Both emitted the same three 251-byte, exact-topology, 16-relocation
 * states at 99.437500%, 99.000000%, and 98.437500%. Neither seed produced the
 * retail register assignment or an audited exact closure:
 *
 *   build/executive-do-dialog-seed-20260805-state-summary.json
 *   build/tu-state-noise/executive-do-dialog-seed-20260805/
 *   build/executive-do-dialog-seed-20260806-state-summary.json
 *   build/tu-state-noise/executive-do-dialog-seed-20260806/
 *
 * The later sibling also supplies one narrower ownership clue: its restore
 * statements address saveMgr[idx] directly rather than publishing that entry
 * through the already-live traversal local. A reviewed two-arm matrix retained
 * PoL's proven decrementing loop while comparing those two ownership forms:
 *
 *   build/executive-do-dialog-restore-owner-axes.json
 *   build/executive-do-dialog-restore-owner-manifest.json
 *   build/source-variant-batch/executive-do-dialog-restore-owner/results.json
 *
 * The later direct-manager arm was exact in the clean compiler state: retail
 * size 251, exact target bytes, and the complete 16/16 relocation stream. It
 * reverses only the previously mismatched ECX/EDX ownership and is retained.
 * This is a genuine cross-version structural recovery, not compiler steering.
 */

void executive_DoDialog_cross_version_restore_loop_islands() {
}
