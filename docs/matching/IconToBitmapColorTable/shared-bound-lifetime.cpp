/*
 * IconToBitmapColorTable, RVA 0xd32a0, retail size 1455:
 * clipped-bound local lifetimes.
 *
 * Three versions were tested across clean plus 50 forest states:
 * scoped right/clipRight locals, one function-scope right, and shared
 * function-scope right plus clipRight.
 *
 * Complete result: 153/153 compilations. Only the scoped form reached the
 * 83.866196%, size-1430 island. Both shared forms topped out at 83.537560%
 * with the same size and relocation count.
 *
 * Artifacts:
 *   build/icon2bc-shared-bound-lifetime-manifest.json
 *   build/match-variants/icon2bc-shared-bound-lifetime/
 *
 * Disposition: retain the narrower lexical scopes.
 */
