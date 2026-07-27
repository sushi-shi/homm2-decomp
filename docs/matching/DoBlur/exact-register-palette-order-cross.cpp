/*
 * DoBlur incremental prefix closure from the exact-frame/register island.
 *
 * The 94.080170% sample14/sample13/sample15 island has retail's 0xC5C frame,
 * exact input and three scalar-sample registers, and exact sample-load prefix.
 * Its first ordered-relocation displacement differs at gpBufferPalette:
 * candidate +0xB2 versus retail +0xB5.  This follow-up held that sample
 * structure fixed and permuted the remaining natural palette-loop prefix:
 *
 *   paletteColor/componentOffset declaration order
 *     x componentOffset/paletteColor update order
 *     x (clean + 50 target-adjacent forest states).
 *
 * All 4 * 51 = 204 cells compiled in 187.82 seconds without truncation.
 *
 * The original palette-first declaration and offset-first update remained
 * best at forest trial 38:
 *
 *   original/original: 94.080170%, size 1707, 43/43 relocations
 *   reversed update:   94.063290%, size 1707, 43/43 relocations
 *   offset-first declaration arms: at most 92.894516%
 *
 * The best arm retains the exact 0xC5C frame, 29-block CFG, and retail sample
 * registers, but gpBufferPalette remains at +0xB2.  Therefore the three-byte
 * prefix displacement is not controlled by either palette local/update order.
 * Future incremental work should change the lookup allocation/result owner or
 * the boundary immediately before palette acquisition, not replay these four
 * palette spellings.
 *
 * Artifacts:
 *   build/doblur-exact-register-palette-order-axes-20260727.json
 *   build/doblur-exact-register-palette-order-manifest-20260727.json
 *   build/match-variants/doblur-exact-register-palette-order-20260727/
 *
 * No disposable source or generated forest declaration is retained.
 */

void DoBlur_exact_register_palette_order_cross_record() {
}
