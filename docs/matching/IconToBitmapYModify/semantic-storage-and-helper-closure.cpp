/*
 * IconToBitmapYModify, RVA 0xda270: semantic storage/helper closure.
 *
 * Starting from the audited exact source, the scratch state was renamed from
 * gYM* spellings to semantic s_* names without changing scope or statements.
 * After rebuilding the raw candidate before regenerating data topology, the
 * renamed source reproduced exact closure at top-of-TU forest trial 39:
 *
 *   100.000000%, size 1416, 64/64 exact CFG blocks,
 *   zero retail-byte delta, 130/130 ordered relocations.
 *
 * The row-visibility helper was then made self-contained:
 *
 *   IconRowVisible(shear, clipTop, currentY, clipBottom)
 *
 * The same trial 39 again reached audited exact closure. The redundant local
 * entry-array pointer was then collapsed:
 *
 *   s_entry = &srcIcon->Entries()[frame];
 *
 * Trial 39 remained exact: size 1416, exact 64/64 CFG blocks, zero retail-byte
 * delta, and complete 130/130 ordered-relocation identity. The retained
 * effective-source hash is 4c56737a5064.30f5cab9fe5a with MAX 100.0000%.
 *
 * Exact retained evidence:
 *
 *   build/tu-state-noise/
 *       icon2by-direct-entries-recorded-trial39-v2-20260727/
 *
 * Function-local static ownership was tested separately. The literal scope
 * move kept the exact 64/64 graph but did not close in 200 forest states:
 *
 *   build/tu-state-noise/
 *       icon2by-semantic-local-statics-regenerated-200-20260727/
 *
 * Six reviewed local declaration orders (historical, address, ownership
 * groups, setup-use, macro-semantic groups, and reverse-use) were then crossed
 * with clean plus 50 forest states, 306/306 candidates:
 *
 *   build/icon2by-local-static-semantic-order-axes-20260727.json
 *   build/icon2by-local-static-semantic-order-matrix-20260727.json
 *   build/match-variants/
 *       icon2by-local-static-semantic-order-20260727/
 *
 * The best local-static descendant was the macro-semantic grouping at trial
 * 25: 98.020300%, size 1420, 130/130 relocations, exact 64/64 graph. No local
 * form reached exact closure, so function-local scope is rejected for this
 * decoder. File-scope storage is retained as retail structural evidence, not
 * merely as compiler steering.
 *
 * Ordering requirement exposed by this audit:
 *
 *   1. rebuild build/objdiff/base/BASE/icon2by.obj;
 *   2. run homm2 data-topology regenerate;
 *   3. rebuild both normalized base and target objects;
 *   4. run the island replay.
 *
 * Regenerating topology before compiling the changed storage scope consumes a
 * stale candidate COFF and produces misleading relocation-identity failures.
 */

void IconToBitmapYModify_semantic_storage_and_helper_closure_attempt() {
}
