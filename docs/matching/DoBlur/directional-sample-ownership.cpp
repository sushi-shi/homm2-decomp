/*
 * DoBlur directional sample-owner structures on the retained RBG parent.
 *
 * The proven 13+3 spill partition and acquisition/accumulation statement
 * orders were held fixed.  Only the physical owner of the thirteen spilled
 * indices changed.  Every authored structure received clean plus 50 forest
 * states.
 *
 * 1. One flat array reordered by compass ownership:
 *      horizontal: west4..west1, east1..east4
 *      vertical:   north3, north2, south1, south2, south3
 *
 *    Complete result: 51/51.  Trials 10 and 24 reach a new 95.738396%
 *    island, size 1703, relocs 43/43, text SHA d5b05b5590c10960.
 *    This is numerically above the retained source hash's 95.729960% MAX,
 *    but is evidence for a different effective source shape and therefore
 *    does not overwrite that hash-scoped maximum.
 *
 *    Direct retained-object audit rejects the apparent gain structurally:
 *    frame 0xC60 versus retail 0xC5C, table base 0x70 versus 0x6C, and B10
 *    217 versus 216 instructions.  Input remains ECX rather than retail EDX.
 *    West-four/north-one use the retail EBP/ESI, but south-four remains EDX
 *    rather than ECX: input and south-four retain the parent's wrong ownership.
 *
 * 2. A horizontal-first grouped owner:
 *
 *      struct SBlurKernelSamples {
 *          i32 horizontal[8];
 *          i32 vertical[5];
 *          i32& operator[](i32 index);
 *      };
 *
 *    Complete result: 51/51; best 91.253170%, size 1708.
 *
 * 3. The faithful vertical-first field order with constant-index routing:
 *    complete result 51/51; best 91.244730%, size 1708.
 *
 * The explicit grouped accessor boundary is therefore material rather than
 * byte-neutral.  The flat directional slot order is the valuable new island;
 * wrapping the same storage in grouped fields destroys it.
 *
 * The remaining bounded family used one generic inline sixteen-term channel
 * helper for red, green, and blue on the retained parent.  Its complete 51/51
 * census reached only 91.327000%, size 1708, at trial 2.  The whole-channel
 * argument/inlining boundary is therefore not the missing B10 structure.
 *
 * Artifacts:
 *   build/doblur-rbg-directional-flat-samples-manifest.json
 *   build/match-variants/doblur-rbg-directional-flat-samples
 *   build/tu-state-noise/doblur-rbg-directional-flat-trial10
 *   build/doblur-rbg-directional-grouped-hv-samples-manifest.json
 *   build/match-variants/doblur-rbg-directional-grouped-hv-samples
 *   build/doblur-rbg-directional-grouped-vh-samples-manifest.json
 *   build/match-variants/doblur-rbg-directional-grouped-vh-samples
 *   build/doblur-rbg-full-channel-inline-helper-manifest.json
 *   build/match-variants/doblur-rbg-full-channel-inline-helper
 *
 * Disposition: preserve the 95.738396% paired object as a distinct structural
 * clue.  No structure is exact, and no experimental source or generated
 * TU-state input is retained.
 */

void DoBlur_directional_sample_ownership_record() {
}
