/*
 * Gold/Buka guard-ownership check for mouseManager::Close, PoL RVA 0x000c93f0.
 *
 * The exact later source uses an early return when the manager is inactive;
 * the prior PoL reconstruction nested the entire cleanup body. VC4.2 emits
 * the same retail-compatible 17-block CFG for both, so the exact donor's
 * shallower structure is retained.
 *
 * A clean plus 50 forest/top census of the retained source emitted one state
 * only at 99.692310%, size 237, exact 17/17 topology, and complete ordered
 * 18/18 relocations:
 *
 *   build/mouse-close-cross-version-states.json
 *   build/tu-state-noise/mouse-close-cross-version
 *
 * The unchanged two-byte residual is the DestroyIcon/DeleteObject register
 * assignment already exhausted by the earlier pointer, wrapper, history, and
 * loop matrices in this directory. Disposition: retain the exact donor guard
 * structure; no new source-shape descendant is justified.
 */

void mouseManager::Close(void) {
    if (m_active != 1)
        return;
    // Cleanup body is unchanged in the retained source.
}
