/*
 * IconToBitmapColorTable, RVA 0xd32a0: savedDst carry-in read site.
 *
 * The candidate read the s_dst carry-in static at function entry, but the
 * retail entry block loads m_entries first and touches no static there.
 * Three placements (entry-top, after publication, before the command loop)
 * crossed with clean plus 40 top-of-TU forest states, 123-arm product:
 *
 *   build/icon2bc-saveddst-axes-20260728.json
 *   build/icon2bc-saveddst-manifest-20260728.json
 *   build/source-variant-batch/icon2bc-saveddst/
 *
 * before_loop wins both objectives: best fuzzy 82.1127% (trial 17 state,
 * 90/91 relocs) and best topology 81.7864% with complete 91/91 relocations
 * and exact 86/86 blocks (trial 24 state). after_publication over-publishes
 * (92/91). The retained replay recorded 82.1127% for the new hash:
 *
 *   build/tu-state-noise/icon2bc-beforeloop-replay-20260728/
 *
 * The m_data hoist remains rejected for this TU (data-read-hoist.md).
 */

void IconToBitmapColorTable_saved_dst_read_site_attempt() {
}
