/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * saved-destination/dim-owner orbit crossed with the icon-family RLE reader.
 *
 * The retained five manual advance/read sites and five
 * ReadIconRleByte(gCTSrc) sites were each crossed with clean plus 50 forest
 * states. The complete 102/102 matrix preserved exact 86-block flow.
 *
 * Manual reader:
 *   best 83.842720%, size 1430, 89/91 relocations.
 *
 * Family inline reader:
 *   best 83.237090%, size 1440, 90/91 relocations,
 *   65 exact blocks plus 21 size-only blocks.
 *
 * Artifacts:
 *   build/icon2bc-saved-dst-reader-manifest.json
 *   build/match-variants/icon2bc-saved-dst-reader/
 *
 * Disposition: do not land the reader yet. Preserve it as an active lower
 * structural island: it is only 15 bytes and one net relocation short of
 * retail, despite its lower fuzzy score.
 */
