/*
mouseManager::Open, retail RVA 0x004c9350, size 0x94.
Independent Y-center constant ownership, 2026-07-27.

The live function has exact 6/6 flow, five exact blocks, and one size-only
block. Allocation, bitmap construction, null-result handling, ShowCursor,
activation, priority, return shape, and all ordered relocations agree. The sole
byte difference is:

  candidate:
    mov ecx, 240
    mov [esi+0x72], ecx
    mov [esi+0x5a], ecx

  retail:
    mov dword ptr [esi+0x72], 240
    mov dword ptr [esi+0x5a], 240

The first complete matrix tested eight named-constant, height-division,
right-shift, and value-returning inline-helper owners against the clean state
and 50 forest states: 408/408 builds completed in 340.39 seconds.

  build/mousemgr-open-y-owner-axes-20260727.json
  build/mousemgr-open-y-owner-states-20260727.json
  build/match-variants/mousemgr-open-y-owner-states-20260727/results.json

The second complete matrix tested signed literal, unsigned literal, and named
enum ownership independently for either store: 306/306 builds completed in
254.10 seconds.

  build/mousemgr-open-y-literal-axes-20260727.json
  build/mousemgr-open-y-literal-states-20260727.json
  build/match-variants/mousemgr-open-y-literal-states-20260727/results.json

All 714 candidates were byte-identical: 93.325584%, size 145, and 4/4 objdiff
relocations. Parser-visible TU states also collapsed to the same function
object. Constant spelling and value-return ownership therefore do not explain
retail's two immediate stores.
*/

#if 0
m_cursorBottom = MOUSE_SCREEN_HEIGHT / 2;
m_mouseY = MOUSE_SCREEN_HEIGHT >> 1;

static inline i32 MouseScreenCenterY(i32 height) {
    return height / 2;
}

m_cursorBottom = MouseScreenCenterY(MOUSE_SCREEN_HEIGHT);
m_mouseY = MouseScreenCenterY(MOUSE_SCREEN_CENTER_Y * 2);

m_cursorBottom = 240;
m_mouseY = static_cast<i32>(240U);
#endif
