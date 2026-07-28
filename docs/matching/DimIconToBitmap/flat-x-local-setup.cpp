/*
 * DimIconToBitmap, RVA 0xcfd50: flat entry-x local before publication.
 *
 * The retail entry block reads entry->x and srcOffset through flat
 * [entries + 13*frame + disp] before materializing the entry pointer. The
 * earlier flat matrix (all-flat reads, old parent) and the m_data hoist had
 * each lost separately; the combination matrix crossed four setup shapes
 * with clean plus 40 top-of-TU forest states, 164-arm product:
 *
 *   build/icond2b-hoist-flat-cross-axes-20260728.json
 *   build/icond2b-hoist-flat-cross-manifest-20260728.json
 *   build/source-variant-batch/icond2b-hoist-flat-cross/
 *
 * flat_x_local_then_src wins both objectives: 81.1724% at forest trial 18
 * with exact 36/36 topology and the first divergence pushed to B4
 * (recorded by replay; previous epoch 80.19). The m_data hoist stays
 * rejected for this TU even in combination.
 *
 *   build/tu-state-noise/icond2b-flatx-replay-20260728/
 */

void DimIconToBitmap_flat_x_local_setup_attempt() {
}
