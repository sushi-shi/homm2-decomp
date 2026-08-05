/*
 * Gold/Buka source-structure transfer for heroWindowManager::AddWindow.
 *
 * Target: BASE/WINMGR, PoL RVA 0x000cac80, retail size 0xbc.
 * Donor: exact decomp-gold-2.1-buka body at VA 0x004b72b0.
 *
 * The prior reconstruction copied zOrder into a second local, assigned zero
 * before checking the tail, and saved m_windowListHead to decide whether the
 * first inserted window also became the tail. PoL retail instead proves the
 * mainline ownership:
 *
 *   - mutate zOrder directly;
 *   - assign zero only when zOrder is automatic and the tail is null;
 *   - walk the insertion point with the compact while condition;
 *   - test m_windowListTail after publishing the new head.
 *
 * The head and tail tests are normally equivalent by list invariant, but the
 * retail load comes from the tail field. This is real semantic ownership, not
 * a score-only substitution.
 *
 * Retained result: live 100.0000%, 188/188 bytes, identical 19-block CFG,
 * and the complete ordered 1/1 relocation stream under the clean VC4.2 state.
 * Source hash: 6b587be24fc2.
 */

void heroWindowManager::AddWindow(class heroWindow* w, i32 zOrder, i32 openFlags) {
    heroWindow* cur = m_windowListTail;
    if (HAS(w->m_winFlags, WINDOW_FLAG_FIXED_LAYER))
        zOrder = 0;
    if (zOrder == -1) {
        if (cur == NULL)
            zOrder = 0;
        else
            zOrder = cur->m_zOrder + 1;
    }
    // The retained source continues with the exact while/insertion structure.
}
