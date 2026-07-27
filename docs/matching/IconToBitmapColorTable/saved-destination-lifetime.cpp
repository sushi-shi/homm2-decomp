/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * saved destination lifetime.
 *
 * Three structures were crossed with clean plus 50 forest states:
 *
 *   u8* savedDst;              // early declaration, late assignment
 *   u8* savedDst = gCTDst;     // declaration at the old assignment site
 *   u8* savedDst = gCTDst;     // early declaration and initialization
 *
 * The early initialization is semantically safe: no call or write to gCTDst
 * occurs before the old assignment point. It opened a new 83.737090%,
 * size-1430 island with 89/91 relocations and exact 86-block flow. The other
 * two forms canonicalized to the old 82.776990%, size-1428 orbit.
 *
 * Artifacts:
 *   build/icon2bc-saved-destination-lifetime-manifest.json
 *   build/match-variants/icon2bc-saved-destination-lifetime/
 *
 * Disposition: retain early initialization. It changes the long-lived
 * destination/X register ownership and exceeds the previous 82.8122% MAX.
 */
