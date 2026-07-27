/*
 * DoBlur inline-boundary matrix on the retained alternate sample partition
 * and red/blue/green component-table order.
 *
 * The six reviewed shapes were:
 *   - direct row-pointer construction;
 *   - one shared inline row-pixel helper;
 *   - an input-only inline row-pixel helper;
 *   - an inline north-one sample reader;
 *   - each row-pointer helper combined with the north-one reader.
 *
 * These helpers had previously been swept only on the retail-frame sample
 * partition. This matrix tests whether the stronger alternate partition opens
 * a different inlining/compiler-state orbit.
 *
 * Result (2026-07-27):
 *   complete matrix: 6 * (clean + 50 states) = 306/306
 *   direct best: 95.729960%, size 1703, relocs 43/43
 *   shared-row, input-row, and north-one helpers: 95.729960%, size 1703
 *   combined row+north helpers: 95.341774%, size 1703
 *
 * The three single-helper arms reach the same target text hash/state as the
 * retained direct arm under a different TU state. They do not expose a new
 * higher island.
 *
 * Artifacts:
 *   build/doblur-alt-partition-inline-boundary-manifest.json
 *   build/match-variants/doblur-alt-partition-inline-boundary
 *
 * Disposition: helpers rejected from the reconstructed source. Their complete
 * state censuses reproduce the retained maximum but do not advance it.
 */

void DoBlur_alternate_partition_inline_boundaries_record() {
}
