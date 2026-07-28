/*
 * FlipIconToBitmapColorTable, RVA 0xd9790: flat entry-field reads.
 *
 * The candidate read setup fields through the s_entry pointer, producing a
 * shifted-pointer add-4/[+5] srcOffset access; the retail entry block reads
 * w/x through flat [entries + 13*frame + disp] addressing and materializes
 * the pointer separately. Three shapes crossed with clean plus 40 top
 * forest states, 123-arm product:
 *
 *   build/iconf2bc-flat-reads-axes-20260728.json
 *   build/iconf2bc-flat-reads-manifest-20260728.json
 *   build/source-variant-batch/iconf2bc-flat-reads/
 *
 * flat entries[frame] reads in the existing statement order win both
 * objectives: 84.8186% at forest trial 30 with exact 76/76 topology
 * (recorded by replay). Reordering w/x before s_src over-publishes
 * relocations (87/83) and scores lower; rejected.
 *
 *   build/tu-state-noise/iconf2bc-flat-replay-20260728/
 */

void FlipIconToBitmapColorTable_flat_entry_reads_attempt() {
}
