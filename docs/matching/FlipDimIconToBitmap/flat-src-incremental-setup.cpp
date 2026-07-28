/*
 * FlipDimIconToBitmap, RVA 0xdaa20: flat srcOffset and incremental setup.
 *
 * The retail entry block reads srcOffset through flat [entries + 13*frame +
 * 0x9] BEFORE materializing the entry pointer, then reads w/x/y through the
 * materialized pointer, and lowers the left edge as sub/sub/inc — the
 * incremental x0 -= width; x0++ statements, not the parenthesized
 * (x0 - width) + 1 form. Three setup shapes crossed with clean plus 40
 * top-of-TU forest states, 123-arm product:
 *
 *   build/icondf2b-setup-shape-axes-20260728.json
 *   build/icondf2b-setup-shape-manifest-20260728.json
 *   build/source-variant-batch/icondf2b-setup-shape/
 *
 * flat_src_incremental_entry_local dominates every retained row and wins
 * both objectives: 78.6867% at forest trial 3 with exact 31/31 topology
 * (recorded by replay; previous epoch 75.6446):
 *
 *   build/tu-state-noise/icondf2b-flatsrc-replay-20260728/
 *
 * The interleaved s_y publication between the x0 subtractions matches the
 * retail schedule; w is read before x and y.
 */

void FlipDimIconToBitmap_flat_src_incremental_setup_attempt() {
}
