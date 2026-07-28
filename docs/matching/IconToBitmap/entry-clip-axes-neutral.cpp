/*
 * IconToBitmap, RVA 0xd0570: entry publication and clip-term mirrors.
 *
 * Three entry-publication shapes (materialized static-pointer reads, an
 * entry-local alias, flat entries[frame] indexing) crossed with four
 * clip-condition sum/mirror spellings, against clean plus 40 top-of-TU
 * forest states — complete 492-arm product:
 *
 *   build/icon2b-entry-clip-axes-20260728.json
 *   build/icon2b-entry-clip-manifest-20260728.json
 *   build/source-variant-batch/icon2b-entry-clip/
 *
 * All twelve source shapes are byte-identical in every reached state: MSVC
 * CSEs the entry-address computation and canonicalizes the sum mirrors in
 * this parent orbit, so neither axis is observable here. The retail B0
 * difference (no second m_entries reload, no separate materialize-add) must
 * come from a different structural parent, not from these spellings.
 *
 * The census also proved the unchanged source reaches 76.4313% (1258 bytes,
 * 83/83 relocations, exact 80/80 blocks) at forest trial 11; that
 * observation was recorded to the hash-scoped MAX by tu_state_noise
 * --only-trial 11 --record-max:
 *
 *   build/tu-state-noise/icon2b-trial11-recorded-20260728/
 */

void IconToBitmap_entry_clip_axes_neutral_attempt() {
}
