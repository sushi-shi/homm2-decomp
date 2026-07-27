/*
 * DoBlur exact-frame structural experiment: move only the final three red
 * component-table reads across an inline ownership boundary.  The parent was
 * the 0xc5c-frame inner-scalar shape; earlier samples and the green/blue tails
 * were unchanged.
 *
 * Three-term return helper:
 *
 *     static inline u32 BlurRedTail(BlurComponentTable& table,
 *                                   i32 westFour,
 *                                   i32 northOne,
 *                                   i32 southFour)
 *     {
 *         u32 tail = BLUR_COMPONENT(table, westFour);
 *         tail += BLUR_COMPONENT(table, northOne);
 *         tail += BLUR_COMPONENT(table, southFour);
 *         return tail;
 *     }
 *
 *     redSum += BlurRedTail(redComponents, sample13, sample14, sample15);
 *
 * This clean-plus-50 sweep opened a distinct lower island.  Clean was
 * 87.867090% at 1668 bytes; trial 18 was best at 93.381860%, 1708 bytes,
 * 43/43 ordered relocations, a 0xc5c frame, and block B10=221 versus retail
 * B10=216.
 *
 * One-entry accumulator helper:
 *
 *     static inline void BlurAddRedComponent(u32& sum,
 *                                            BlurComponentTable& table,
 *                                            i32 offset)
 *     {
 *         sum += BLUR_COMPONENT(table, offset);
 *     }
 *
 *     BlurAddRedComponent(redSum, redComponents, sample13);
 *     BlurAddRedComponent(redSum, redComponents, sample14);
 *     BlurAddRedComponent(redSum, redComponents, sample15);
 *
 * All six call orders were permuted and crossed independently with clean plus
 * 50 TU states: 6 x 51 = 306/306 complete compilations.  Call order was
 * byte-neutral within each state.  Clean was 90.704640% at 1711 bytes; trial
 * 46 was best at 93.985230%, 1705 bytes, 43/43 ordered relocations, 29/29 CFG
 * blocks with 28 exact and B10=218 versus retail 216.  The frame remained the
 * desired 0xc5c, but the final sample owners were input=EDX, west4=EBP,
 * north1=ECX, south4=ESI rather than retail EDX/EBP/ESI/ECX.  Thus the helper
 * boundary reproduces the known north1/south4-swapped family rather than the
 * register-perfect schedule.
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-exact-frame-red-tail-return-helper-20260727
 *   build/tu-state-noise/doblur-exact-frame-red-tail-single-add-helper-20260727
 *   build/doblur-exact-frame-red-tail-add-helper-order-axes.json
 *   build/doblur-exact-frame-red-tail-add-helper-order-manifest.json
 *   build/match-variants/doblur-exact-frame-red-tail-add-helper-order
 *   build/tu-state-noise/doblur-exact-frame-red-tail-single-add-helper-trial7-20260727
 *
 * Disposition: rejected.  Neither real inline boundary improved the retained
 * 95.729960% alternate-partition MAX, and no exact byte/size closure appeared.
 * The reconstructed source is restored; generated probe code was not retained.
 */
