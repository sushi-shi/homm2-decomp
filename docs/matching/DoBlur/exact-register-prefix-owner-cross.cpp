/*
 * DoBlur exact-register prefix-owner cross, 2026-07-27.
 *
 * This matrix starts from the 94.080170% exact-frame/register island recorded
 * in exact-frame-early-brg-row-scalar-order-cross.cpp.  Its first divergence
 * is immediately after BaseAlloc: retail stores EAX to lookup before loading
 * gpBufferPalette, while the candidate loads gpBufferPalette first and stores
 * EAX second.  The following developer-plausible ownership choices were
 * crossed independently:
 *
 *   lookup result lifetime:
 *     BlurLookupRow* lookup = static_cast<BlurLookupRow*>(H2_ALLOC_AT(...));
 *     BlurLookupRow* lookup;
 *     lookup = static_cast<BlurLookupRow*>(H2_ALLOC_AT(...));
 *
 *   palette data owner:
 *     gpBufferPalette->m_data
 *     BlurPaletteData()                 // no-argument inline accessor
 *     BlurPaletteData(gpBufferPalette)  // parameterized inline accessor
 *
 * Complete matrix:
 *
 *   2 lookup lifetimes * 3 palette owners * (clean + 50 forest states)
 *     = 306/306 candidates
 *
 * Splitting the lookup declaration from its assignment was byte-neutral in
 * the best state.  Both direct arms reached 94.080170%, size 1707, with
 * 43/43 relocations and 28 exact blocks plus B10 size-only at forest trial
 * 38.
 *
 * Both inline palette accessors opened the same distinct lower island:
 * 93.687770%, size 1706, 43/43 relocations, and the same 28+1 block
 * classification (representative global-accessor forest trial 7).  This did
 * not repair the target prefix.  Its retained disassembly still loads
 * gpBufferPalette at candidate +0xB2, stores the allocation result at +0xB6,
 * and obtains m_data at +0xBA.  Retail stores the allocation result at +0xB0,
 * loads gpBufferPalette at relocation +0xB5, and obtains m_data at +0xB9.
 *
 * Therefore neither a split lookup lifetime nor either natural palette
 * accessor boundary owns the missing scheduling decision.  The accessor
 * island is useful evidence because it is a new size-1706 orbit, but it is
 * not prefix closure and is not retained in reconstructed source.
 *
 * Artifacts:
 *   build/doblur-exact-register-prefix-owner-axes-20260727.json
 *   build/doblur-exact-register-prefix-owner-manifest-20260727.json
 *   build/match-variants/doblur-exact-register-prefix-owner-20260727/
 *   build/tu-state-noise/
 *     doblur-exact-register-global-palette-trial7-20260727/
 *   build/doblur-exact-register-global-palette-trial7-state-20260727.json
 *
 * No disposable source or generated forest declaration is retained.
 */

void DoBlur_exact_register_prefix_owner_cross_record() {
}
