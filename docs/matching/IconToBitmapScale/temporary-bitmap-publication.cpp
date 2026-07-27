/*
 * IconToBitmapScale, RVA 0xd2f90: temporary bitmap publication lifetime.
 *
 * Retail spills the temporary pixel pointer during the clear loop.  Four new
 * allocation/publication structures were crossed with clean plus 50 states:
 * combined declaration/new, split declaration/assignment, an allocation-only
 * inline factory, and a separate created pointer published to tmp.  The
 * complete matrix contains 204/204 compilations.
 *
 * Representative alternate forms:
 *
 *     bitmap* tmp;
 *     tmp = new bitmap(BITMAP_TYPE_NONE, 64, 64);
 *
 *     bitmap* created = new bitmap(BITMAP_TYPE_NONE, 64, 64);
 *     bitmap* tmp = created;
 *
 *     static inline bitmap* CreateIconScaleWorkBitmap() {
 *         return new bitmap(BITMAP_TYPE_NONE, 64, 64);
 *     }
 *
 * Results:
 *   combined and split forms are byte-identical within each state; best in
 *   this forest is 81.879430%, size 367
 *   allocation-only helper reaches 85.829790%, size 365
 *   created-then-published reaches 85.829790%, size 365/366 and opens
 *   distinct best text SHA e8f446cdf76e988c at size 366
 *   all arms retain 18/18 flow and 4/4 ordered relocations
 *
 * Neither explicit publication nor the factory retains the retail pixel
 * pointer spill at its best state; both remain in the known 13-exact,
 * five-size-only structural class.
 *
 * Artifacts:
 *   build/icon2bs-temporary-publication-axes.json
 *   build/icon2bs-temporary-publication-manifest.json
 *   build/match-variants/icon2bs-temporary-publication
 *
 * Disposition: lower/alternate islands only. Source and MAX are unchanged.
 */

void IconToBitmapScale_temporary_bitmap_publication_attempt() {
}
