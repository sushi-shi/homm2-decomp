/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Single-store inline-accessor ownership, 2026-07-27.

Seven structures moved exactly one 240 store behind a reference-return
accessor, reference setter, or pointer setter. Each was crossed with the clean
state plus all 50 forest states: 357/357 builds completed in 286.58 seconds.

  build/mousemgr-open-y-setter-axes-20260727.json
  build/mousemgr-open-y-setter-states-20260727.json
  build/match-variants/mousemgr-open-y-setter-states-20260727/results.json

Every source/state pair emitted the same 93.325584%, 145-byte, 4/4-relocation
object as the open stores. A one-store inline boundary is fully optimized away
and does not separate the two equal constants.

Disposition: retain the open member stores. No helper or generated state is
retained.
*/

#if 0
static inline i32& MouseCursorBottom(mouseManager& manager) {
    return manager.m_cursorBottom;
}

static inline void SetMouseCenterY(i32& output, i32 value) {
    output = value;
}

MouseCursorBottom(*this) = MOUSE_SCREEN_CENTER_Y;
SetMouseCenterY(m_mouseY, MOUSE_SCREEN_CENTER_Y);
#endif
