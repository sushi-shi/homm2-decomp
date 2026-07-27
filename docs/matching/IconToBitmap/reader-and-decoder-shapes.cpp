/*
 * IconToBitmap command-reader and decoder structure census.
 *
 * Representative byte-reader spellings:
 *
 *     command = ReadIconRleByte(gIcSrc);
 *     command = *gIcSrc++;
 *     gIcSrc = gIcSrc + 1;
 *     command = gIcSrc[-1];
 *
 * The matrix independently crossed those spellings at the command, long-solid
 * count, solid color, dim flags, and long-dim count reads.  It also crossed
 * read-before-increment versus increment-before-read for dim pixels and direct,
 * split-offset, and typed-level dim-palette expressions.
 *
 * Completed matrices (2026-07-26):
 *
 *   icon2b-reader-mechanism-clean-20260726
 *     32/32 source shapes; best 74.096150%, size 1252, 83/83 relocations.
 *
 *   icon2b-decoder-structure-clean-20260726
 *     27/27 source shapes; best 75.079670%, size 1256, 83/83 relocations.
 *
 *   icon2b-66-block-islands-20260726
 *     121/121 states for direct reads before pointer advance.  Trial 26
 *     reached 78.414830%, size 1241, 83/83 relocation count.
 *
 *   icon2b-read-helper-120-20260726
 *     A complete 120-trial inline-reader census reached 78.760990%.
 *     Replaying trial 73 through the repository script recorded the current
 *     78.788460% historical MAX for the unchanged effective source.
 *
 *   icon2b-inline-helper-{clean,islands}-20260726
 *     8 clean helper structures and 968/968 source/state variants, including
 *     direct decoding and extracted dim-loop helpers.  Best 76.107140%.
 *
 * No reader/decoder spelling produced exact bytes and relocations.
 */

void IconToBitmap_reader_and_decoder_attempts() {
}
