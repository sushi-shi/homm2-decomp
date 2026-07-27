/*
 * FlipMonoIconToBitmap structural experiment: move all ten DATA-annotated
 * scratch objects from file scope into function-local static scope.
 *
 * Reviewed source shape:
 *
 *     void FlipMonoIconToBitmap(...) {
 *         DATA(0x00538190) static u32 gFMRun;
 *         DATA(0x00538194) static u8* gFMSrc;
 *         // ...the remaining gFM* scratch declarations...
 *         DATA(0x005381b4) static i32 gFMXEnd;
 *         // existing body
 *     }
 *
 * This tested the icon-family provenance hypothesis directly rather than
 * inferring it from sibling source. It produced a distinct compiler orbit,
 * so declaration scope is codegen-relevant in this optimized TU.
 *
 * Complete census (2026-07-27):
 *   clean candidate: 75.227550%, size 515, 26/27 relocations
 *   clean live report before normalization: 75.7365%
 *   clean + 50 forest trials: 51/51 complete, 22 unique states
 *   best: trial 14, 78.568860%, size 514
 *   retained historical file-scope MAX: 80.275450%
 *
 * The local-static object names also changed to C++ function-local manglings,
 * which the fixed relocation model correctly reported as stale rather than
 * silently treating as the established retail identities. No generated
 * manifest or baseline row is retained from this discarded source hash.
 *
 * Artifacts:
 *   build/tu-state-noise/iconmf2b-function-local-static-20260727
 *   build/iconmf2b-function-local-static-states-20260727.json
 *
 * Disposition: preserved as a tested lower structural island, but not retained
 * in live source. Its complete state census stayed below the file-scope MAX
 * and did not recover retail size or the missing ordered relocation.
 */
