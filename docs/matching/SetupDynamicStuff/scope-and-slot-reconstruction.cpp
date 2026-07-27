/*
 * SetupDynamicStuff scope, local-slot, and final relocation reconstruction.
 *
 * The starting source had the retail CFG already: all 236 basic blocks
 * matched structurally, but the clean object was only 97.7%.  A clean-source
 * TU-state census reached 99.97805%; its remaining 104 raw-byte differences
 * were all EBP-relative local displacements.  This made local ownership and
 * scope, rather than small expression spelling, the first real divergence.
 *
 * `scripts/od_slots.py` and the retail frame showed these structural owners:
 *
 *   - an otherwise unreferenced i32 in the scroll-knob else scope, reserving
 *     the retail -0x1c slot;
 *   - one town-scope displayed-troop counter reused by both troop and
 *     dwelling loops, rather than one counter owned by each nested scope;
 *   - the dwelling index and the town detail values owned by the town branch;
 *   - one hero-scope displayed-troop counter, with hero detail row, column,
 *     and skill index also owned by the hero branch.
 *
 * Reconstructing the town ownership reduced the best state to 39 differing
 * bytes.  Reconstructing the hero ownership reduced it to four.  The full
 * 50-state forest census then found trial 3 with zero differing instruction
 * bytes and the retail 8766-byte size.  Its only remaining mismatch was the
 * identity order of two otherwise equivalent global relocations.
 *
 * Those two relocations load `iLastDynamicType` and `giOverviewType` in the
 * early unchanged-state predicate.  Small source spellings, nested
 * comparisons, casts, and parenthesization were byte- and relocation-neutral.
 * An inline equality helper added 28 bytes, while an empty else-if spelling
 * added 15 bytes; both contradicted the retail size and were rejected.
 *
 * VC4.2's selected accumulator followed declaration registration order.
 * Placing the real `iLastDynamicType` declaration immediately before
 * `giOverviewType` in SOURCE/game.h repaired the final relocation identities
 * without changing semantics or adding generated steering.  Replaying the
 * unchanged source at forest trial 3 then produced audited exact closure:
 *
 *   score:                 100.000000%
 *   retail/candidate size: 8766 / 8766
 *   raw byte differences:  0
 *   ordered relocations:   340 / 340 exact
 *   source hash:           dda8d861a39b
 *   text SHA:              0f59d3059d332097
 *   relocation detail SHA: 4c067774a8d92d57
 *
 * Artifacts:
 *   build/tu-state-noise/setupdynamic-town-parent-trial2-20260727
 *   build/tu-state-noise/setupdynamic-all-slots-trial2-20260727
 *   build/setupdynamic-slot-exact-state-summary-20260727.json
 *   build/tu-state-noise/setupdynamic-header-order-trial3-20260727
 *
 * Disposition: retain the semantic portrait-field corrections, reconstructed
 * scope/local ownership, and adjacent declaration order.  The generated
 * forest probe remains disposable.  Its exact object is retained only as
 * reproducible evidence, and the generated MAX is recorded by the tooling.
 */

void SetupDynamicStuff_scope_and_slot_reconstruction_record() {
}
