/*
 * heroWindowManager::FizzleForward, RVA 0xcb1e0, retail size 1026.
 * Local ownership matrices, 2026-07-27.
 *
 * Fresh clean evidence was 99.302635%, size 1028, all 33 ordered
 * relocations, and 34/34 block bodies and successor edges exact.  The only
 * live differences were:
 *
 *   - EBX/EDX ownership while combining saved and work pixels;
 *   - equivalent sourceY/end row-loop comparison polarity;
 *   - equivalent base/index ownership in three palette subscripts.
 *
 * The first complete matrix crossed:
 *
 *   lookup owner:
 *     low-byte-first expression;
 *     high-byte-first expression;
 *     inline pointer-owning FizzleLookup helper;
 *
 *   row loop:
 *     sourceY < y + height;
 *     y + height > sourceY;
 *
 *   palette subscripts:
 *     pointer[index];
 *     index[pointer].
 *
 * All 12 x 51 = 612 clean/top-forest cells completed in 527.78 seconds.
 * The relation and subscript axes were byte-neutral in every important
 * state.  The high-byte-first expression was a coherent exact-CFG lower
 * island at 99.457240%, size 1026, 33/33 relocations.  Moving lookup
 * ownership into an inline pointer helper was structurally contradictory:
 * 76.052635%, size 1002, only 9 exact block bodies, 5 shifted targets,
 * 11 flow-kind differences, and one missing block.
 *
 * Artifacts:
 *
 *   build/fizzle-forward-local-ownership-axes-20260727.json
 *   build/fizzle-forward-local-ownership-manifest-20260727.json
 *   build/match-variants/fizzle-forward-local-ownership-20260727/
 *
 * A focused descendant then tested explicit narrow byte lifetimes:
 */

#if 0
// Saved-byte lifetime first.
u16 saved = *savedPixel++;
u16 work = *workPixel++;
u16 lookup =
    static_cast<u16>((saved << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT) | work);

// Work-byte lifetime first.
u16 work = *workPixel++;
u16 saved = *savedPixel++;
u16 lookup =
    static_cast<u16>(work | (saved << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT));
#endif

/*
 * All 3 x 51 = 153 cells completed in 127.33 seconds.  Saved-then-work
 * peaked at 98.990135%, size 1028; work-then-saved peaked at 98.503290%,
 * size 1026.  Both retained 33/33 relocations and exact 34-block topology,
 * but neither approached the unchanged expression.
 *
 * The unchanged expression at forest trial 32 reproduced the durable
 * 99.720400% MAX with exact retail size, 33/33 relocations, and exact
 * 34-block flow.  Official replay retained paired clue objects here:
 *
 *   build/tu-state-noise/fizzle-forward-trial32-record-max-20260727/
 *
 * The replay's relocation-aware raw comparison has only 12 differing code
 * bytes.  Its remaining disassembly differences are precisely lookup
 * EBX/EDX ownership and palette SIB base/index ownership; the row-loop
 * comparison already agrees.  No reviewed source structure improved MAX,
 * so reconstructed source and generated configuration remain unchanged.
 *
 * Descendant artifacts:
 *
 *   build/fizzle-forward-staged-lookup-axes-20260727.json
 *   build/fizzle-forward-staged-lookup-manifest-20260727.json
 *   build/match-variants/fizzle-forward-staged-lookup-20260727/
 *
 * Current-hash follow-up, 2026-08-05:
 *
 * A fresh seed-20260805 forest/top census completed 50 trials and emitted 24
 * distinct states. Its best state was 99.490135%, retail size, exact 34-block
 * topology, and 33/33 relocations, below the durable historical MAX:
 *
 *   build/fizzle-forward-seed-20260805-state-summary.json
 *   build/tu-state-noise/fizzle-forward-seed-20260805/
 *
 * Re-generating the default-seed trial 32 named by the older dossier no longer
 * reproduced that historical island under the current TU state; it measured
 * 92.684210%. The 99.720400% MAX remains durable evidence, but the stale probe
 * cannot serve as a reproducible second-layer parent for the current hash:
 *
 *   build/fizzle-forward-default-trial32-state-summary.json
 *   build/tu-state-noise/fizzle-forward-default-trial32/
 *
 * Later-to-current structural subtraction, 2026-08-05:
 *
 * The exact Gold/Buka parent recomputes the work and screen row pointers from
 * `sourceY` on every row. PoL's retained parent instead carried `workOffset`
 * and `screenOffset` induction variables. Testing only this ownership change,
 * while preserving PoL's proven countdown inner loop, null cleanup, allocator
 * metadata, and surrounding CFG, improved clean output from 99.302635% at
 * 1028 bytes to 99.523026% at the exact retail size of 1026. Both arms were
 * crossed with clean plus 50 forest/top states (102/102 complete):
 *
 *   build/fizzle-forward-cross-version-row-ownership-axes.json
 *   build/fizzle-forward-cross-version-row-ownership-manifest.json
 *   build/source-variant-batch/fizzle-forward-cross-version-row-ownership/results.json
 *
 * The later-row arm reached 99.720400% at trial 37 with exact 34/34 topology
 * and 33/33 relocations. The ordinary source now retains the direct row
 * expressions and removes the obsolete running offsets entirely; dead offset
 * maintenance is not retained as compiler steering.
 *
 * A separate 102-cell product-order matrix proved Gold/Buka's frame-first
 * palette interpolation spelling byte-identical to PoL's delta-first spelling
 * in every clean/state cell:
 *
 *   build/fizzle-forward-cross-version-palette-product-manifest.json
 *   build/source-variant-batch/fizzle-forward-cross-version-palette-product/results.json
 *
 * Finally, the clean retained parent crossed lookup-byte order and palette
 * subscript ownership with all 51 states (204/204 complete). Pointer/index
 * subscript order remained erased; saved-byte-first lookup ownership was a
 * lower island. No exact closure appeared:
 *
 *   build/fizzle-forward-later-row-descendants-axes.json
 *   build/fizzle-forward-later-row-descendants-manifest.json
 *   build/source-variant-batch/fizzle-forward-later-row-descendants/results.json
 *
 * Disposition: retain later-style row recomputation as the stronger semantic
 * parent. Its clean body has retail size, exact CFG, and complete ordered
 * relocations; the remaining lookup-register and palette-SIB residual stays
 * live for a different evidence-backed structural descendant.
 */

void FizzleForward_local_ownership_attempts() {
}
