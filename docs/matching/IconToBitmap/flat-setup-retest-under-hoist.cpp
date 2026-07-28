/*
 * IconToBitmap, RVA 0xd0570: flat setup retest under the hoist parent.
 *
 * The retail entry block reads entry->x and srcOffset through flat
 * [entries + 13*frame + disp] before materializing the pointer, so the
 * flat axes were retested after the m_data-hoist structural parent changed
 * the orbit (the pre-hoist matrix had erased them). Four shapes crossed
 * with clean plus 40 top-of-TU forest states, 164-arm product:
 *
 *   build/icon2b-flatsetup-axes-20260728.json
 *   build/icon2b-flatsetup-manifest-20260728.json
 *   build/source-variant-batch/icon2b-flatsetup/
 *
 * Negative: every flat variant stays below the recorded 79.6813% MAX
 * (best flat 78.2720 with complete 83/83 relocations; best pointer 78.5137
 * with over-published 87/83). The existing pointer-read spelling is
 * retained. The flat B0 lowering evidently needs yet another parent
 * (Icon2b's savedDst/loop ownership differs from the color-table sibling
 * where the equivalent axis won).
 */

void IconToBitmap_flat_setup_retest_under_hoist_attempt() {
}
