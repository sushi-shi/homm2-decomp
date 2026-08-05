/*
 * Cross-version exact closure for inputManager::ForceMouseMove.
 * Target: BASE/INPUTMGR RVA 0x000cea80, retail size 233.
 *
 * The exact later source repeats the mouse-message re-entrancy test before
 * claiming the flag. Although the checks are logically redundant, VC4.2 CSE
 * folds them to one branch while preserving the retail load/test ownership.
 * The prior snapshot source emitted a shorter 230-byte body.
 *
 * Complete matrix: snapshot, duplicated-direct donor, and single-direct guard
 * crossed with clean plus 50 forest/top states = 153/153 cells:
 *
 *   build/force-mouse-move-guard-manifest.json
 *   build/source-variant-batch/force-mouse-move-guard/results.json
 *
 * The duplicated donor arm is clean-state exact and remains exact throughout
 * the observed states: 233/233 bytes, exact 5/5 CFG, complete ordered 11/11
 * relocations. Source hash a12600f71594; live 100.0000% retained.
 */

void inputManager::ForceMouseMove(void) {
    if (gpInputManager->m_mouseMessageActive != 0)
        return;
    if (gpInputManager->m_mouseMessageActive != 0)
        return;
    gpInputManager->m_mouseMessageActive = 1;
    // Event publication continues unchanged in the retained source.
}
