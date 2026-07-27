/*
 * IconToBitmapScale, RVA 0xd2f90: address-owning geometry initialization.
 *
 * The earlier geometry record used direct field assignments and was promoted
 * to scalars.  This new matrix tested a reference-taking inline initializer
 * and a return-by-value factory so that sourceBase/sourceAdvance had a real
 * aggregate ownership boundary while step remained scalar:
 *
 *     SIconScaleGeometry geometry;
 *     InitializeIconScaleGeometry(geometry, scale, step);
 *
 *     SIconScaleGeometry geometry = MakeIconScaleGeometry(scale, step);
 *
 * Direct scalars and both new owners were crossed with clean plus 50 states:
 * 153/153 complete compilations.
 *
 * Results:
 *   reference initializer: 85.829790%, size 365/366
 *   return-by-value owner:  85.829790%, size 365
 *   direct scalar control: 85.829790%, size 365
 *   ordered relocations:   4/4
 *   best blocks: 18/18, 13 exact plus five size-only
 *
 * Both aggregate boundaries converge to the known text SHA
 * 5919a0a10f23defd.  Address-taking and hidden-return spelling do not keep
 * the geometry stack-resident at the best state and do not reproduce retail's
 * EBP step plus spilled source geometry.
 *
 * Artifacts:
 *   build/icon2bs-geometry-inline-owner-axes.json
 *   build/icon2bs-geometry-inline-owner-manifest.json
 *   build/match-variants/icon2bs-geometry-inline-owner
 *
 * Disposition: rejected from source; MAX remains 85.829790%.
 */

void IconToBitmapScale_geometry_inline_ownership_attempt() {
}
