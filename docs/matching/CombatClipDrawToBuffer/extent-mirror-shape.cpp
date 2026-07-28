/*
 * icon::CombatClipDrawToBuffer, RVA 0xc7b50: extent-block register binding.
 *
 * The whole residual is one register-binding orbit rooted in the
 * gbComputeExtent block. Retail evaluates the orientation test through a
 * dead caller-saved eax, binds the 13-byte entry offset to ebx, and the
 * limits pointer to edi; the candidate binds the orientation copy to ebx and
 * the entry offset to edi, and every downstream block inherits the swap
 * (candidate 690 bytes vs retail 687; exact 39/39 blocks; 37/37 relocs).
 *
 * Measured and rejected:
 *
 *   - mirror-then-offset / offset-then-mirror / direct-orientation matrix,
 *     each against clean plus 50 top-of-TU forest states (complete 153-arm
 *     product): all three shapes byte-identical in every state.
 *       build/icon-combatclip-mirror-axes-20260728.json
 *       build/icon-combatclip-mirror-manifest-20260728.json
 *       build/source-variant-batch/icon-combatclip-mirror/
 *   - unchanged-source censuses: 50 forest/top (retained best), 100
 *     forest/top seed 777, 50 forest/target; sparse orbits (3-6 unique
 *     states), no exact closure, baseline remains best at 97.266396%.
 *       build/tu-state-noise/icon-combatclip-50-20260728/
 *       build/tu-state-noise/icon-combatclip-100b-20260728/
 *       build/tu-state-noise/icon-combatclip-target50-20260728/
 *
 * The byte-identical arms prove only that this parent orbit erases the
 * mirror-local distinction; a future evidence-backed structural parent (for
 * example a different extent-store ownership) may make the axis observable.
 * Live residual: MAX 97.2664 for the current hash.
 */

void CombatClipDrawToBuffer_extent_mirror_shape_attempt() {
}
