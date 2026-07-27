/*
 * DoBlur audit and missing Cartesian matrix: sample partition, lookup
 * allocation materialization, and palette-cursor update order with the
 * retained red/blue/green component-table order.
 *
 * The historical matrices did not cross these independent axes:
 *
 *   build/match-variants/doblur-blue-red-green-prefix-ownership-20260726
 *     base SHA-256: 256775e49c9d38d3bea6bfb0c8b64ce8f74336b7b5baa00f514792473a5f0b80
 *     retail-frame partition, blue/red/green tables
 *     lookup initializer/split x two palette update orders
 *     best 94.626580%
 *
 *   build/match-variants/doblur-alt-partition-rgb-palette-order-20260726
 *     base SHA-256: c4963c7fb3d0cdd1d61a59444a5875c0e2b05d14e4e4ba2e741bc9dea411c6bb
 *     alternate partition, all six table orders
 *     two palette update orders, but no lookup-materialization axis
 *     best 95.729960%
 *
 * The new matrix held the retained red/blue/green order fixed and crossed:
 *   - alternate north-four scalar / retail-frame west-four scalar;
 *   - lookup initializer / split declaration and assignment;
 *   - component-then-palette / palette-then-component cursor updates.
 *
 * Result (2026-07-27):
 *   complete matrix: 2 * 2 * 2 * (clean + 50 states) = 408/408
 *   alternate partition best: 95.729960%, size 1703, relocs 43/43
 *   retail-frame partition best: 94.523210%, size 1705, relocs 43/43
 *   lookup initializer and split forms are byte-identical in every paired arm
 *   block topology: 29/29, 28 exact and one size-only block
 *
 * Artifacts:
 *   build/doblur-partition-prefix-current-rbg-axes.json
 *   build/doblur-partition-prefix-current-rbg-manifest.json
 *   build/match-variants/doblur-partition-prefix-current-rbg
 *
 * Disposition: retain the alternate partition and red/blue/green table order.
 * The previously missing Cartesian arms are real compiler islands, but none
 * raises the 95.729960% historical MAX.
 */

void DoBlur_partition_prefix_current_table_cartesian_record() {
}
