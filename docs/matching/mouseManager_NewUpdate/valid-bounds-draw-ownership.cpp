/*
mouseManager::NewUpdate, retail RVA 0x000c9a40, size 0x47a.

This file records the accepted structural experiment. It is evidence, not
compiled source.

Initial evidence
----------------

The prior source emitted all 408 retail instructions in the same order and had
83/83 relocation identities, but remained at 99.2570%. The block comparison
exposed the real difference:

  - candidate: 61 blocks; the four invalid old-cursor-bounds branches entered
    the saved-width calculation and draw path;
  - retail: 62 blocks; those branches skipped directly to publication of
    m_drawnCursorSizeIndex.

The corresponding candidate branches were short jumps to the width calculation.
Retail used near jumps to the publication block at function offset 0x45d. This
was a semantic ownership difference, not compiler-state noise.

Accepted structure
------------------

The saved-width calculation, background save, conditional cursor draw, screen
blit, and background restore all belong inside the existing valid-bounds guard:

    if (gOldMouseLeft <= MOUSE_SCREEN_WIDTH - 1
        && gOldMouseTop <= MOUSE_SCREEN_HEIGHT - 1
        && gOldMouseRight >= 0
        && gOldMouseBottom >= 0) {
        // Clamp the old bounds.
        // Compute m_savedWidth/m_savedHeight.
        // Save, draw, blit, and restore.
    }
    m_drawnCursorSizeIndex = m_cursorSizeIndex;

Result
------

The clean build closed immediately at exact 100.0000%, size 1146, with 408
identical instructions, 62/62 exact blocks, and 83/83 ordered relocations.
The accepted object is:

    build/objdiff/base/BASE/MOUSEMGR.obj

No source-variant or TU-state matrix was run: structural recovery comes first,
and the first evidence-backed ownership correction produced audited exact
closure.
*/
