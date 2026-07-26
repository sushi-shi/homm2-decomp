/*
 * DoBlur reviewed descendant matrices on the retail-shaped 13+3 partition.
 *
 * 1. red/green/blue pointer-local orders, 6 * (clean + 50 states):
 *    build/match-variants/doblur-blue-red-green-pointer-order-20260726
 *    No improvement over 94.601265%.
 *
 * 2. lookup initializer/split declaration crossed with both palette update
 *    orders, 2 * 2 * (clean + 50 states):
 *    build/match-variants/doblur-blue-red-green-prefix-ownership-20260726
 *    Best 94.626580%, palette componentOffset update before paletteColor.
 *    Splitting lookup declaration from assignment was byte-neutral.
 *
 * 3. Seven natural input-pointer identifier spellings, each crossed with the
 *    clean state and 50 states:
 *    build/match-variants/doblur-blue-red-green-input-names-20260726
 *    All names produced identical target states; best remained 94.626580%.
 *
 * 4. Palette pointer initializer versus split declaration/assignment:
 *    build/match-variants/doblur-blue-red-green-palette-pointer-materialization-20260726
 *    The two forms were byte-neutral; best remained 94.626580%.
 *
 * These shapes are not retained. They show that the prefix ownership and local
 * spelling axes alone do not explain the remaining block/register divergence.
 */

void DoBlur_prefix_and_pointer_matrix_record() {
}
