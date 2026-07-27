/*
 * IconToBitmapScale, RVA 0xd2f90: native-size fast-path inline ownership.
 *
 * The prior large helper owned the non-native path.  This distinct first-
 * block experiment moved only the native IconToBitmap call behind a small
 * inline boundary, using both source-first and destination-first signatures.
 * Each of the three structures received clean plus 50 states: 153/153.
 *
 * Representative helper:
 *
 *     static inline void DrawNativeIconScale(
 *         icon* source, bitmap* destination, /* clip arguments */
 *     ) {
 *         IconToBitmap(source, destination, /* arguments */, 0);
 *     }
 *
 * Results:
 *   direct clean: 75.695040%, size 361
 *   either helper clean: 80.248230%, size 366
 *   best of every family: 85.829790%, size 365, text SHA 5919a0a10f23defd
 *   blocks: 18/18, 13 exact plus five size-only at the best state
 *   ordered relocations: 4/4
 *
 * Source/destination parameter order is byte-neutral after inlining.  The
 * helper opens a better clean island but does not select retail's entry
 * ownership or 0x18-byte frame.
 *
 * Artifacts:
 *   build/icon2bs-native-path-inline-axes.json
 *   build/icon2bs-native-path-inline-manifest.json
 *   build/match-variants/icon2bs-native-path-inline
 *
 * Disposition: rejected; no MAX advance and no generated input retained.
 */

void IconToBitmapScale_native_path_inline_ownership_attempt() {
}
