/*
 * DoBlur reviewed declaration-order matrices.
 *
 * The three component lookup tables are independent semantic objects, so all six
 * declaration orders were crossed with the clean state and 50 forest TU states.
 *
 * Retail-shaped 13+3 sample partition:
 *   artifact: build/match-variants/doblur-rgb-array-declaration-order-20260726
 *   best: blue, red, green; trial 45; 94.601265%; size 1705; 43/43 relocs
 *
 * Retained alternate sample partition, crossed with both palette-loop update
 * orders:
 *   artifact: build/match-variants/doblur-alt-partition-rgb-palette-order-20260726
 *   complete matrix: 6 * 2 * (clean + 50 states) = 612 candidates
 *   best: red, blue, green; componentOffset then paletteColor; trial 10
 *         (also trial 24); 95.729960%; size 1703; 43/43 relocs
 *
 * The latter is a legitimate MAX-raising source shape and is retained in
 * src/BASE/Blur.cpp. Generated forest declarations are not retained.
 */

void DoBlur_component_table_declaration_order_attempt() {
    BlurComponentTable redComponents;
    BlurComponentTable blueComponents;
    BlurComponentTable greenComponents;
}
