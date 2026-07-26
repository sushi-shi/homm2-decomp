/*
 * DoBlur structural experiment: turn the component-table byte-offset macro
 * body into a real inline accessor.
 *
 * Retained arm:
 *
 *     static inline i32& BlurComponentAt(
 *         BlurComponentTable& table, i32 offset
 *     ) {
 *         return *reinterpret_cast<i32*>(
 *             reinterpret_cast<u8*>(table) + offset
 *         );
 *     }
 *
 *     #define BLUR_COMPONENT(table, offset) \
 *         BlurComponentAt((table), (offset))
 *
 * Equivalent reviewed arms:
 *   - i32* table parameter with i32& return
 *   - BlurComponentTable& parameter with i32& return
 *   - BlurComponentTable& parameter with i32* return, dereferenced by macro
 *   - external `inline` linkage instead of private `static inline` linkage
 *
 * All three accessor signatures compile to the same target-local states.
 *
 * Result (2026-07-26):
 *   clean fuzzy: 92.000000%, size 1708
 *   best of 50 target-adjacent forest states: 93.411390% (trials 2 and 18)
 *   retail/candidate size at the best state: 1700/1707 bytes
 *   ordered relocations: 43/43, identical identities
 *   CFG: same 29-block family
 *
 * Artifacts:
 *   build/tu-state-noise/doblur-inline-component-access-clean
 *   build/tu-state-noise/doblur-inline-component-array-ref-clean
 *   build/tu-state-noise/doblur-inline-component-pointer-return-clean
 *   build/tu-state-noise/doblur-external-inline-component-clean
 *   build/tu-state-noise/doblur-inline-component-access-target-50
 *   build/doblur-inline-component-access-target-50-states.json
 *
 * Disposition: retained.  This is a legitimate type/inline-boundary recovery,
 * raises clean matching by 1.51055 points, and raises historical MAX from
 * 92.4346% to 93.41139%.  No disposable declaration forest is retained.
 */
