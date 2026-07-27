/*
 * FlipIconToBitmapColorTable, RVA 0xd9790, retail size 1357:
 * transfer of the forward color-table destination lifetime and dim owner.
 *
 * The first complete matrix crossed four dim-pixel ownership boundaries with
 * branch-local, shared-root, and early-initialized shared-root destination
 * lifetimes. All 12 structures received clean plus 50 forest states
 * (612/612 compilations).
 *
 * The previously tested broad reference helper remained a poor orbit. Two
 * narrower helpers that left counter and gFCDimDst publication in the caller
 * were compiler-equivalent and reached 88.357680%, size 1340, 84/83
 * relocations, with exact 76-block flow. A plain shared declaration and early
 * initialization were also byte-identical, proving that the transferable
 * mechanism is the shared destination lifetime rather than a redundant
 * gFCDimDst read.
 *
 * A descendant matrix then separated mapped-value ownership from destination
 * advance:
 *
 *   static inline u8 FlipColorDimValue(u8* dst, u8* palette) {
 *       return palette[*dst];
 *   }
 *
 * The caller retains dp++, count--, gFCDimDst publication, and the final
 * dp[-1] store. Complete clean plus 50-state results:
 *
 *   mapped-value helper:
 *     clean 87.042820%, best 88.458435%, size 1339
 *
 *   helper-owned published-palette load and pointer return:
 *     best 88.357680%, size 1340
 *
 * Both retain 84/83 relocations and 76/76 blocks with 67 exact and nine
 * size-only blocks. The published-palette arm is preserved as a meaningful
 * lower structural island because it is one byte closer to retail.
 *
 * Artifacts:
 *   build/iconf2bc-destination-owner-transfer-manifest.json
 *   build/match-variants/iconf2bc-destination-owner-transfer/
 *   build/iconf2bc-dim-value-descendants-manifest.json
 *   build/match-variants/iconf2bc-dim-value-descendants/
 *   build/tu-state-noise/iconf2bc-dim-value-trial32/
 *
 * Disposition: retain the shared root dp declaration and mapped-value helper.
 * Official trial 32 raised hash-scoped MAX from 87.0479% to 88.4584%; the
 * generated forest input was removed.
 */
