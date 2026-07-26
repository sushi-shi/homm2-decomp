/*
 * DoBlur structural experiments for the north-one sample14 lifetime.
 *
 * Arm A: rolling north-row pointer
 *
 *     u8* northInput = input - SCREEN_WIDTH;
 *     // per pixel
 *     sample14 = static_cast<u32>(*northInput) << COMPONENT_INDEX_SHIFT;
 *     northInput++;
 *     input++;
 *
 * Arm B: preloaded loop-carried value
 *
 *     sample14 = static_cast<u32>(input[-SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
 *     do {
 *         // consume sample14
 *         input++;
 *         sample14 =
 *             static_cast<u32>(input[-SCREEN_WIDTH]) << COMPONENT_INDEX_SHIFT;
 *     } while (--remaining != 0);
 *
 * Both structures try to reserve an independent lifetime for sample14 so that
 * VC4.2 can assign the retail ESI/EDX ownership without source-level steering.
 *
 * Result (2026-07-26):
 *   rolling pointer clean/best-of-50: 90.953580% / 92.995780%
 *   rolling pointer size: 1726 clean; same 29-block CFG and 43 relocations
 *   preloaded value clean/best-of-50: 87.774260% / 89.415610%
 *   preloaded value size: 1720 clean; same 29-block CFG and 43 relocations
 *
 * Artifacts:
 *   build/doblur-rolling-north-pointer-target-50.json
 *   build/tu-state-noise/doblur-rolling-north-pointer-target-50
 *   build/doblur-preloaded-sample14-target-50.json
 *   build/tu-state-noise/doblur-preloaded-sample14-target-50
 *
 * Disposition: rejected.  Both arms expand the target and return B10 to 221
 * instructions, losing the retained partition's 217-instruction island.
 */
