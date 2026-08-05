/*
 * SetGameDefaults, PoL RVA 0x000c49a0.
 * Gold/Buka direct-graphics-loop subtraction, 2026-08-05.
 *
 * The exact later source iterates an integer executable index and writes
 * gConfig.gfx[index] fields directly. PoL's retained structure instead walks
 * an i32 pointer rooted at the game fullScreen field. To isolate the donor
 * ownership from later-only behavior, the matrix preserved PoL's field order,
 * random-ID construction, strings, and post-loop assignments and changed only
 * the graphics-loop owner.
 *
 * Both structures were crossed with clean plus 50 forest/top states. The full
 * 102/102 matrix completed without truncation:
 *
 *   build/set-game-defaults-cross-version-loop-axes.json
 *   build/set-game-defaults-cross-version-loop-manifest.json
 *   build/source-variant-batch/set-game-defaults-cross-version-loop/results.json
 *
 * The retained field-pointer arm stayed best at 96.780700%, size 435, exact
 * six-block flow, and 42/42 relocations. Every later direct-index arm fell to
 * 88.807014%-88.982450% and grew to 440-441 bytes. The direct index is a
 * later-only ownership change, so it is subtracted and no source is retained.
 */

void SetGameDefaults_cross_version_direct_graphics_loop() {
}
