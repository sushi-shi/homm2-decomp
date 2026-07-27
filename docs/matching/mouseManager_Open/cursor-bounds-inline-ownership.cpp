/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Cursor-bound aggregate and vertical inline ownership, 2026-07-27.

Four structural versions crossed open member assignments, a parameterized
four-bound initializer, a constant four-bound initializer, and a two-bound
vertical initializer with the clean state plus all 50 forest states. The
complete 204-candidate matrix finished in 168.55 seconds.

  build/mousemgr-open-bounds-inline-axes-20260727.json
  build/mousemgr-open-bounds-inline-states-20260727.json
  build/match-variants/mousemgr-open-bounds-inline-states-20260727/results.json

The four-bound helpers formed a coherent lower 80.4186%, 143-byte island.
The vertical helper formed a higher 94.0000%, 145-byte island in every state.
It preserved exact flow and relocation count, but moved the 240-valued bottom
store ahead of mouseX and the zeroed indices. Retail keeps the bottom store
after those assignments and encodes both 240 stores as immediates. The higher
fuzzy score therefore does not justify retaining the helper.

A descendant matrix split the vertical helper into bottom-only, mouseY-only,
or two distinct zero-argument store-owning helpers. All four structures were
crossed with clean plus 50 forest states: 204/204 builds completed in 173.97
seconds.

  build/mousemgr-open-zeroarg-setter-axes-20260727.json
  build/mousemgr-open-zeroarg-setter-states-20260727.json
  build/match-variants/mousemgr-open-zeroarg-setter-states-20260727/results.json

Every descendant reproduced the same 94.0000% island and none restored retail
store scheduling, size 148, or exact bytes. The 94% object remains a structural
clue under build/, while authored source and the official 93.3256% MAX remain
unchanged.
*/

#if 0
static inline void InitializeMouseCursorVertical(
    mouseManager& manager, i32 top, i32 bottom
) {
    manager.m_cursorTop = top;
    manager.m_cursorBottom = bottom;
}

InitializeMouseCursorVertical(
    *this, MOUSE_SCREEN_CENTER_Y - 1, MOUSE_SCREEN_CENTER_Y
);

static inline void SetMouseCursorBottom(mouseManager& manager) {
    manager.m_cursorBottom = MOUSE_SCREEN_CENTER_Y;
}

static inline void SetMouseY(mouseManager& manager) {
    manager.m_mouseY = MOUSE_SCREEN_CENTER_Y;
}
#endif
