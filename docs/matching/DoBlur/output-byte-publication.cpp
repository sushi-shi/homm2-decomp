/*
 * DoBlur output-byte publication and lookup-index composition, 2026-07-27.
 *
 * Retail B10 advances and publishes the output pointer after completing the
 * lookup index but before loading and storing the selected byte.  The live
 * source's post-increment store instead lets MSVC keep the output pointer
 * update until after the lookup load.  This matrix tested ownership of that
 * final byte rather than repeating the older input/output statement-order
 * spellings:
 *
 *   1. Direct post-increment:
 *
 *        *output++ = lookup[row][column];
 *
 *   2. A named outputColor followed by the post-increment store.
 *
 *   3. A named outputColor followed by:
 *
 *        output++;
 *        output[-1] = outputColor;
 *
 *   4. A pixel owner:
 *
 *        u8* outputPixel = output++;
 *        *outputPixel = lookup[row][column];
 *
 *   5. Named lookupRow, lookupColumn, and outputColor locals followed by the
 *      pre-increment publication above.
 *
 * Every arm was crossed with the clean state and the same 50 declaration-
 * forest states inserted at the top of the TU: 5 * 51 = 255/255 completed in
 * 240.36 seconds without truncation.
 *
 * Result:
 *   direct post-increment best: 95.729960%, size 1703
 *   named outputColor best:     95.729960%, size 1703
 *   pre-increment/color best:   93.702530%, size 1705
 *   staged lookup best:         93.702530%, size 1705
 *   outputPixel owner best:     92.383965%, size 1707
 *   ordered relocations:        43/43 in every successful arm
 *
 * The direct and named-color forms converge on the same retained text at
 * forest trials 10 and 24.  The two pre-increment arms are lower scoring, but
 * their clean states have all 29 block instruction counts equal to retail;
 * B10 grows from 215 to the retail count of 216.  They therefore expose a
 * genuine structural island rather than an inert spelling.
 *
 * A focused descendant then removed the named outputColor and expressed the
 * retail publication order directly:
 *
 *        output++;
 *        output[-1] = lookup[row][column];
 *
 * Its complete clean-plus-50 matrix reached 93.702530%, size 1705.  The clean
 * object is byte-identical to the staged-lookup clean object (text SHA
 * 693c900de71450c3) and retains the 29/29 block-count skeleton; at its best
 * forest state B10 returns to a one-instruction size residual.  Thus advancing
 * output before the byte read accounts for the missing clean instruction, but
 * does not repair the broader register and encoding schedule or the five-byte
 * total-size excess.
 *
 * Artifacts:
 *   build/doblur-output-publication-axes-20260727.json
 *   build/doblur-output-publication-manifest-20260727.json
 *   build/match-variants/doblur-output-publication-20260727
 *   build/doblur-direct-preincrement-axes-20260727.json
 *   build/doblur-direct-preincrement-manifest-20260727.json
 *   build/match-variants/doblur-direct-preincrement-20260727
 *   build/tu-state-noise/doblur-direct-preincrement-clean-20260727
 *
 * Disposition: preserve the clean direct-preincrement object as a lower
 * structural island, but retain the live source.  No exact closure appeared,
 * generated TU-state input is not retained, and durable MAX remains
 * 95.729960%.
 */

void DoBlur_output_byte_publication_record() {
}
