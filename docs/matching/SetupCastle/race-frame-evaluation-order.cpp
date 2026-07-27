/*
 * SetupCastle final evaluation-order residual and exact unchanged-source state.
 *
 * The live object is structurally closed:
 *
 *   - 948/948 masked instructions are identical;
 *   - all 161 basic blocks are exact;
 *   - frame size is 316 on both sides, with EBX/ESI/EDI saved;
 *   - relocation identities/count are 139/139 with no base-only target;
 *   - candidate and retail comparison sizes are both 4283 bytes.
 *
 * Raw comparison localized the entire 99.997530% residual to four bytes at
 * function offsets 0xf7e, 0xf7f, 0xf84, and 0xf85.  They are the two local
 * displacements in the second background-tile frame calculation:
 *
 *     static_cast<i16>(raceIconFrame + backgroundFrame6)
 *
 * The live object loads backgroundFrame6 (-0x128) and then adds raceIconFrame
 * (-0xe8).  Retail loads raceIconFrame first and then adds backgroundFrame6.
 * No opcode, CFG, frame, size, or external relocation differs.
 *
 * The reviewed exact-span axis swapped the commutative spelling:
 *
 *     static_cast<i16>(backgroundFrame6 + raceIconFrame)
 *
 * It was compiled against clean plus all 50 forest states (51/51 complete,
 * source restored).  Several states scored a relocation-masked 100%, but the
 * exact gate rejected them because their ordered relocation stream differed
 * from retail.  The clean swapped spelling remained 99.997530%, so it is not
 * retained merely to perturb compiler state.
 *
 * The unchanged source was then swept directly.  Forest trial 2 produced
 * audited exact closure and stopped the search as required:
 *
 *   score:                 100.000000%
 *   comparison size:       4283 / 4283
 *   raw byte differences:  0
 *   ordered relocations:   139 / 139 exact
 *   text SHA:              53ec961ec4841ee2
 *   relocation detail SHA: f528522ddbf28c9d
 *   trial tag:             484f4d32-0002-e2d06adb
 *
 * Artifacts:
 *   build/setupcastle-race-frame-order-states-manifest-20260727.json
 *   build/match-variants/setupcastle-race-frame-order-states-20260727
 *   build/tu-state-noise/setupcastle-clean-20260727
 *
 * Disposition: retain the original semantic expression and no generated
 * declarations.  The unchanged effective-source hash d126c18c1200 already
 * carried an audited 100.0000% MAX; this sweep reproduces and revalidates it.
 */

void SetupCastle_race_frame_evaluation_order_record() {
}
