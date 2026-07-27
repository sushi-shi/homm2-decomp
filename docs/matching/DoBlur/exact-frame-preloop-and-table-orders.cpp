/*
 * DoBlur exact-frame matrices: preloop lifetimes, inline boundaries, palette
 * update order, and physical component-table declaration order.
 *
 * Reviewed source shapes included:
 *
 *     BlurLookupRow* lookup;                 // split allocation lifetime
 *     BlurComponentTable blueComponents;
 *     BlurComponentTable redComponents;
 *     BlurComponentTable greenComponents;
 *     lookup = static_cast<BlurLookupRow*>(H2_ALLOC_AT(...));
 *
 * and every permutation of the three table declarations.  Initialized lookup,
 * split lookup before/after the arrays, pointer/count/output declaration
 * orders, paletteColor/componentOffset update orders, direct table access,
 * and small inline helper boundaries were tested as independent axes.
 *
 * Complete matrices (2026-07-26):
 *   old-partition pointer lifetime/order: 306/306
 *   old-partition RGB declaration order: 306/306
 *   old-partition inline boundary: 306/306
 *   blue/red/green pointer lifetime/order: 306/306
 *   preloop materialization/update order: 408/408
 *   inner-scalar preloop cross: 408/408
 *   inner-scalar RGB declaration order: 306/306
 *
 * Blue/red/green reproduced retail's channel stack bases and channel
 * quantization schedule.  On the register-perfect inner-scalar branch,
 * trial 40 with blue/red/green remained 93.708860%.
 *
 * Trial 44 with blue/green/red or red/green/blue scored 93.985230%, but it is
 * structurally worse: north1 and south4 move to ECX/ESI instead of retail's
 * ESI/ECX.  Table-declaration green/red/blue (the same target state as
 * green/blue/red) preserves the exact sample registers, but its final red
 * table reads remain ECX -> ESI -> EBP instead of retail EBP -> ESI -> ECX,
 * and its green schedule is farther from retail.
 *
 * Artifacts:
 *   build/doblur-old-partition-pointer-lifetime-order-manifest.json
 *   build/doblur-old-partition-rgb-declaration-order-manifest.json
 *   build/doblur-old-partition-inline-boundary-manifest.json
 *   build/doblur-blue-red-green-pointer-lifetime-order-manifest.json
 *   build/doblur-blue-red-green-preloop-materialization-manifest.json
 *   build/doblur-exact-frame-inner-preloop-manifest.json
 *   build/doblur-exact-frame-inner-rgb-declaration-order-manifest.json
 *   build/match-variants/doblur-exact-frame-inner-rgb-declaration-order
 *   build/tu-state-noise/doblur-exact-frame-inner-blue-green-red-trial44
 *   build/tu-state-noise/doblur-exact-frame-inner-green-red-blue-trial40
 *
 * Disposition: all are preserved as lower structural islands; none advances
 * the retained MAX or fixes the exact-frame red tail.
 */
