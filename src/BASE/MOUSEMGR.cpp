// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\MOUSEMGR.OBJ   from: .\basewin.lib
// functions: 17   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/MOUSEMGR_TYPES.h>
#include <BASE/mouseManager.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <BASE/INPUTMGR.h>

// ---- zero-fill storage (retail RVA order) ----
DATA(0x00533238) static i32 gOldMouseRight;
DATA(0x00533240) BITMAP bmpAndMask[MOUSE_CURSOR_COUNT];
DATA(0x00533b40) static POINT gMouseScreenPt; // GetCursorPos scratch (mouseManager::MouseCoords)
DATA(0x00533b48) static POINT
    gMouseCheckPt; // GetCursorPos scratch (mouseManager::CheckUpdateMousePos/ShowColorPointer)
DATA(0x00533b50) static i32 gOldMouseTop;
DATA(0x00533b58) HICON hMouseCursor[MOUSE_CURSOR_COUNT];
DATA(0x00533cd8) void* cAndBits[MOUSE_CURSOR_COUNT];
DATA(0x00533e58) void* cColorBits[MOUSE_CURSOR_COUNT];
DATA(0x00533fd8) static i32 gOldMouseBottom;
DATA(0x00533fdc) static i32 gOldMouseLeft;
DATA(0x00533fe0) ICONINFO IconInfo[MOUSE_CURSOR_COUNT];
DATA(0x00534760) HBITMAP hbmpAndMask[MOUSE_CURSOR_COUNT];

// @data-layout-note Retail loader-zero storage spans 0x133238..0x1348e0
// (0x16a8). All twelve owners above have proved addresses, types, and extents,
// and all code references use their exact retail owner-relative addends. VC 4.2
// emits the same 0x16a0 bytes of owned storage in a 0x16a4 BSS contribution but
// orders the symbols differently; moving the real definitions into retail order
// is byte-neutral. This is a final-link/common-allocation packing residual. Do not
// add padding symbols, aliases, cursor placement, or section pragmas to force it.

// ---- initialized storage (retail RVA order) ----
DATA(0x0051ebc8) i32 iMouseOffset[4] = {0, 41, 57, 0};
// Per-cursor bitmap {width, height} pairs.
DATA(0x0051ebd8) i8 iMouseSize[MOUSE_CURSOR_COUNT][2] = {
    {15, 21}, {22, 21}, {24, 20}, {24, 24}, {30, 25}, {24, 24}, {24, 24}, {19, 23}, {15, 20},
    {30, 30}, {32, 26}, {31, 25}, {30, 25}, {24, 24}, {22, 21}, {31, 31}, {32, 26}, {31, 25},
    {30, 25}, {24, 24}, {22, 21}, {31, 31}, {32, 26}, {32, 25}, {32, 25}, {27, 24}, {25, 22},
    {31, 31}, {24, 24}, {30, 25}, {30, 25}, {32, 25}, {8, 32},  {24, 24}, {32, 8},  {24, 24},
    {8, 32},  {24, 24}, {32, 8},  {24, 24}, {15, 21}, {20, 20}, {18, 23}, {21, 24}, {24, 24},
    {18, 22}, {11, 18}, {13, 13}, {22, 22}, {31, 14}, {22, 22}, {22, 22}, {31, 14}, {22, 22},
    {14, 31}, {14, 31}, {28, 16}, {20, 20}, {28, 33}, {25, 40}, {37, 41}, {25, 29}, {52, 29},
    {24, 39}, {29, 35}, {45, 28}, {52, 40}, {33, 42}, {34, 31}, {32, 35}, {32, 35}, {43, 33},
    {34, 38}, {37, 35}, {27, 30}, {29, 27}, {32, 23}, {23, 36}, {30, 36}, {21, 33}, {27, 38},
    {28, 34}, {36, 34}, {66, 41}, {38, 35}, {59, 41}, {54, 39}, {35, 42}, {30, 34}, {45, 41},
    {39, 34}, {29, 43}, {25, 35}, {39, 35}, {47, 37}, {51, 13}
};
// Per-cursor {x, y} click-point offsets inside the bitmap; fed to ICONINFO
// xHotspot/yHotspot and the software draw position. -1 = default to center
// (patched to iMouseSize/2 in Open).
DATA(0x0051ec98) i8 iHotSpot[MOUSE_CURSOR_COUNT][2] = {
    {2, 3},   {2, 3},   {12, 11}, {12, 13}, {15, 11}, {10, 10}, {12, 13}, {9, 12},  {7, 9},
    {15, 15}, {15, 11}, {10, 10}, {12, 13}, {9, 12},  {7, 9},   {15, 15}, {15, 11}, {10, 10},
    {12, 13}, {9, 12},  {7, 9},   {15, 15}, {15, 11}, {10, 10}, {12, 13}, {9, 12},  {7, 9},
    {15, 15}, {12, 12}, {12, 12}, {12, 12}, {12, 12}, {3, 0},   {23, 0},  {31, 4},  {23, 23},
    {3, 31},  {0, 24},  {0, 5},   {0, 0},   {2, 3},   {10, 9},  {9, 11},  {10, 11}, {12, 12},
    {10, 12}, {5, 8},   {1, 1},   {21, 1},  {30, 7},  {21, 21}, {1, 21},  {1, 7},   {1, 1},
    {7, 1},   {7, 30},  {14, 8},  {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}
};
DATA(0x0051ed58) i32 gbInSetPointer = 0;
DATA(0x0051ed5c) i32 bInNewMouseUpdate = 0;

DATA(0x0051ed60) static SMouseManagerStrings gMouseManagerStrings = {
    {MOUSE_MANAGER_NAME},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_ADVENTURE_ICON},
    {MOUSE_MANAGER_SPELL_ICON},
    {MOUSE_MANAGER_COMBAT_ICON},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_ADVENTURE_BITMAP},
    {MOUSE_MANAGER_SPELL_BITMAP},
    {MOUSE_MANAGER_COMBAT_BITMAP},
    {MOUSE_MANAGER_SOURCE_FILE},
    {MOUSE_MANAGER_SOURCE_FILE},
    ""
};

// @data-layout-note Retail initialized storage spans 0x11ebc8..0x11ef28
// (0x360). The public cursor tables occupy the first 0x198 bytes. Retail then
// stores sixteen independently referenced strings at 4-byte boundaries; the
// typed owner above reproduces their complete payload and addends 0x0, 0x10,
// 0x3c, 0x68, 0x74, 0x80, 0x8c, 0xb8, 0xe4, 0x110, 0x13c, 0x14c, 0x15c,
// 0x16c, 0x198, and 0x1c4. The complete candidate contribution is byte-exact.

VA(0x004c9270, 0xd9)
mouseManager::mouseManager(void) : baseManager() {
    i32 i;
    m_savedUnderlying = 0;
    m_active = 0;
    m_cursorType = MOUSE_INVALID_CURSOR_TYPE;
    strcpy(m_name, gMouseManagerStrings.managerName.text);
    m_cursorFrame = 0;
    m_cursorReady = 1;
    m_cursorIcon = 0;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++)
        hbmpAndMask[i] = 0;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++)
        hMouseCursor[i] = 0;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++)
        cColorBits[i] = 0;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++)
        cAndBits[i] = 0;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++) {
        if (iHotSpot[i][0] == -1)
            iHotSpot[i][0] = iMouseSize[i][1] / 2;
        if (iHotSpot[i][1] == -1)
            iHotSpot[i][1] = iMouseSize[i][1] / 2;
    }
    m_hideCount = 1;
}

// @semantic coverage checkpoint: semantics, member layout, and no-frame CFG are
// complete. The relocation counts are 3/3; the delinked target embeds ShowCursor's
// IAT address, but manual -dr review confirms the same external access at the same
// call site. Base .text is 0x91 versus retail 0x94. The first code divergence is
// base +0x52: MSVC keeps 0xf0 in ECX and
// shares it across the two stores, while retail emits two immediate stores at
// +0x5b..+0x68; the suffix is identical after the three-byte shift. Direct stores
// and spelling the second value as 0xef+1 still select the shared-load form.
// Revisit after a predecessor/shared-header TU-state change, not with predicates.
VA(0x004c9350, 0x94)
i32 mouseManager::Open(i32 priority) {
    m_forcePointerUpdate = 0;
    m_savedUnderlying =
        new bitmap(BITMAP_TYPE_MEMORY, MOUSE_SAVED_BITMAP_WIDTH, MOUSE_SAVED_BITMAP_HEIGHT);
    m_savedLeft = MOUSE_SCREEN_CENTER_X - 1;
    m_savedTop = MOUSE_SCREEN_CENTER_Y - 1;
    m_cursorRight = MOUSE_SCREEN_CENTER_X;
    m_cursorLeft = MOUSE_SCREEN_CENTER_X - 1;
    m_cursorTop = MOUSE_SCREEN_CENTER_Y - 1;
    m_mouseX = MOUSE_SCREEN_CENTER_X;
    m_cursorSizeIndex = 0;
    m_drawnCursorSizeIndex = 0;
    m_cursorBottom = MOUSE_SCREEN_CENTER_Y;
    m_mouseY = MOUSE_SCREEN_CENTER_Y;
    if (gbColorMice != 0)
        ShowCursor(0);
    m_messageMask = MOUSE_MANAGER_MESSAGE_MASK;
    m_active = 1;
    m_priority = priority;
    return 0;
}

// @semantic coverage checkpoint: semantics and the zero-frame loop CFG are
// complete; both bodies are 0xed bytes. Relocation counts are 14/14, and manual -dr
// review confirms the same ordered arrays, allocator calls, resource manager, and
// Dispose target; the delinked target embeds the four Win32 IAT addresses. The first
// divergence is the loop setup at +0x2b: base uses EBX as the byte offset and EBP
// for DeleteObject, while retail uses EBP as the offset and EBX for DeleteObject.
// The later differences are only that register exchange. for-scope, hoisted, and
// register-qualified counters retain the base allocation. A bounded libclang AST
// pass tested eight single mutations in 30 walks and retained none. Revisit after
// TU-state changes or during the AST last-mile phase.
VA(0x004c93f0, 0xed)
void mouseManager::Close(void) {
    i32 cursorIndex;
    if (m_active == 1) {
        m_active = 0;
        if (m_savedUnderlying != 0)
            delete m_savedUnderlying;
        m_savedUnderlying = 0;
        SetCursor(LoadCursorA(0, IDC_ARROW));
        for (cursorIndex = 0; cursorIndex < MOUSE_CURSOR_COUNT; cursorIndex++) {
            if (hMouseCursor[cursorIndex] != 0)
                DestroyIcon(hMouseCursor[cursorIndex]);
            hMouseCursor[cursorIndex] = 0;
            if (cAndBits[cursorIndex] != 0)
                H2_FREE_AT(
                    cAndBits[cursorIndex],
                    gMouseManagerStrings.andMaskDestruction.text,
                    0x14a
                );
            cAndBits[cursorIndex] = 0;
            if (cColorBits[cursorIndex] != 0)
                H2_FREE_AT(
                    cColorBits[cursorIndex],
                    gMouseManagerStrings.colorBitsDestruction.text,
                    0x14e
                );
            cColorBits[cursorIndex] = 0;
            if (hbmpAndMask[cursorIndex] != 0)
                DeleteObject(hbmpAndMask[cursorIndex]);
            hbmpAndMask[cursorIndex] = 0;
        }
        if (m_cursorIcon != 0)
            gpResourceManager->Dispose(m_cursorIcon);
        m_cursorIcon = 0;
    }
}

VA(0x004c94e0, 0x5)
i32 mouseManager::Main(struct tag_message&) {
    return 0;
}

VA(0x004c94f0, 0x135)
void mouseManager::SetPointer(char* name, i32 param_2, i32 param_3) {
    if (m_forcePointerUpdate == 0) {
        gbPutzingWithMouseCtr++;
        gpResourceManager->SavePosition();
        if (param_3 == MOUSE_AUTO_CURSOR_TYPE) {
            if (giCurExe == 1 || *name == 'a' || *name == 'A')
                param_3 = MOUSE_CURSOR_ADVENTURE;
            else if (*name == 's' || *name == 'S')
                param_3 = MOUSE_CURSOR_SPELL;
            else
                param_3 = MOUSE_CURSOR_COMBAT;
        }
        if (m_cursorType != param_3 && (m_cursorType = param_3, gbColorMice != 0)) {
            i32 saved82 = m_cursorReady;
            m_cursorReady = 0;
            if (m_cursorIcon != 0)
                gpResourceManager->Dispose(m_cursorIcon);
            char local_10[16];
            if (m_cursorType == MOUSE_CURSOR_ADVENTURE)
                sprintf(local_10, gMouseManagerStrings.adventureIcon.text);
            else if (m_cursorType == MOUSE_CURSOR_SPELL)
                sprintf(local_10, gMouseManagerStrings.spellIcon.text);
            else
                sprintf(local_10, gMouseManagerStrings.combatIcon.text);
            m_cursorIcon = gpResourceManager->GetIcon(local_10);
            H2_ASSERT(
                param_2 != MOUSE_KEEP_CURRENT_FRAME,
                gMouseManagerStrings.cursorFrameAssertion.text,
                0x19a
            );
            m_cursorFrame = MOUSE_INVALID_CURSOR_FRAME;
            m_cursorReady = saved82;
        }
        SetPointer(param_2);
        gpResourceManager->RestorePosition();
        gbPutzingWithMouseCtr--;
    }
}

// @semantic coverage checkpoint: semantics, 0x14-byte local area, CFG, cursor
// table types, and external relocations are reconstructed (70/70, no base-only
// target). This is still not an early stop. Base code is 0x3d9 versus retail
// 0x405. The first divergence is the
// prologue: retail spills this at [esp] before saving EBX/ESI/EDI/EBP; base keeps
// this in EBP and later spills the color-row offset. The retail mask loop uses
// independent color and mask byte offsets, which this source now models directly.
// A row/column multiplication form scores ~94% but is structurally false; direct
// offset forms score ~75%. A nested lifetime block and the hash-informed semantic
// name sourceOffset do not change allocation. Register-qualified counters also
// retain the base allocation. Revisit after TU-state changes or in last-mile AST work.
VA(0x004c9630, 0x405)
void mouseManager::SetPointer(i32 frame) {
    if (m_forcePointerUpdate != 0 || frame < 0 || m_active != 1 || m_cursorFrame == frame
        || gbInSetPointer != 0)
        return;

    gbInSetPointer = 1;
    gbPutzingWithMouseCtr++;
    gpResourceManager->SavePosition();
    if (giCurExe == 1)
        m_cursorType = MOUSE_CURSOR_ADVENTURE;
    if (frame == MOUSE_KEEP_CURRENT_FRAME)
        frame = m_cursorFrame;
    else
        m_cursorFrame = frame;
    m_cursorSizeIndex = iMouseOffset[m_cursorType] + frame;
    H2_ASSERT(
        m_cursorSizeIndex >= 0 && m_cursorSizeIndex < MOUSE_CURSOR_COUNT,
        gMouseManagerStrings.cursorSizeAssertion.text,
        0x1ca
    );

    if (gbColorMice != 0) {
        NewUpdate(1);
    } else {
        if (hMouseCursor[m_cursorSizeIndex] == 0) {
            cColorBits[m_cursorSizeIndex] = H2_ALLOC_AT(
                MOUSE_CURSOR_COLOR_BYTES,
                gMouseManagerStrings.colorBitsAllocation.text,
                480
            );
            cAndBits[m_cursorSizeIndex] = H2_ALLOC_AT(
                MOUSE_CURSOR_AND_BYTES,
                gMouseManagerStrings.andBitsAllocation.text,
                481
            );

            char filename[16];
            if (m_cursorType == MOUSE_CURSOR_ADVENTURE)
                sprintf(filename, gMouseManagerStrings.adventureBitmap.text, frame + 1);
            else if (m_cursorType == MOUSE_CURSOR_SPELL)
                sprintf(filename, gMouseManagerStrings.spellBitmap.text, frame);
            else
                sprintf(filename, gMouseManagerStrings.combatBitmap.text, frame + 1);

            gpResourceManager->PointToFile(gpResourceManager->MakeId(filename, 1));
            gpResourceManager->ReadBlock(
                reinterpret_cast<i8*>(cColorBits[m_cursorSizeIndex]),
                MOUSE_CURSOR_BITMAP_HEADER_BYTES
            );
            gpResourceManager->ReadBlock(
                reinterpret_cast<i8*>(cColorBits[m_cursorSizeIndex]),
                MOUSE_CURSOR_COLOR_BYTES
            );
            memset(cAndBits[m_cursorSizeIndex], 0, MOUSE_CURSOR_AND_BYTES);
            {
                i32 sourceOffset = 0;
                i32 maskOffset = 0;
                while (sourceOffset < MOUSE_CURSOR_COLOR_BYTES) {
                    for (i32 column = 0; column < MOUSE_CURSOR_BITMAP_WIDTH; column++) {
                        if (static_cast<u8*>(cColorBits[m_cursorSizeIndex])[sourceOffset + column]
                            == 0)
                            static_cast<u8*>(
                                cAndBits[m_cursorSizeIndex]
                            )[maskOffset + column / 8] |= 1
                                                          << (MOUSE_CURSOR_MASK_HIGH_BIT
                                                              - (column
                                                                 & MOUSE_CURSOR_MASK_HIGH_BIT));
                        else if (static_cast<u8*>(
                                     cColorBits[m_cursorSizeIndex]
                                 )[sourceOffset + column]
                                 == 1)
                            static_cast<u8*>(
                                cAndBits[m_cursorSizeIndex]
                            )[MOUSE_CURSOR_MASK_PLANE_BYTES + maskOffset + column / 8] |=
                                1
                                << (MOUSE_CURSOR_MASK_HIGH_BIT
                                    - (column & MOUSE_CURSOR_MASK_HIGH_BIT));
                    }
                    sourceOffset += MOUSE_CURSOR_BITMAP_WIDTH;
                    maskOffset += MOUSE_CURSOR_MASK_ROW_BYTES;
                }
            }

            bmpAndMask[m_cursorSizeIndex].bmType = 0;
            bmpAndMask[m_cursorSizeIndex].bmWidth = MOUSE_CURSOR_BITMAP_WIDTH;
            bmpAndMask[m_cursorSizeIndex].bmHeight = MOUSE_CURSOR_MASK_HEIGHT;
            bmpAndMask[m_cursorSizeIndex].bmWidthBytes = MOUSE_CURSOR_MASK_ROW_BYTES;
            bmpAndMask[m_cursorSizeIndex].bmPlanes = MOUSE_CURSOR_BITMAP_PLANES;
            bmpAndMask[m_cursorSizeIndex].bmBitsPixel = MOUSE_CURSOR_BITMAP_BITS_PER_PIXEL;
            bmpAndMask[m_cursorSizeIndex].bmWidthBytes = MOUSE_CURSOR_MASK_ROW_BYTES;
            bmpAndMask[m_cursorSizeIndex].bmBits = cAndBits[m_cursorSizeIndex];
            hbmpAndMask[m_cursorSizeIndex] = CreateBitmapIndirect(&bmpAndMask[m_cursorSizeIndex]);
            H2_ASSERT(
                reinterpret_cast<i32>(hbmpAndMask[m_cursorSizeIndex]),
                gMouseManagerStrings.bitmapAssertion.text,
                0x202
            );

            IconInfo[m_cursorSizeIndex].fIcon = 0;
            if (m_cursorType == MOUSE_CURSOR_SPELL) {
                IconInfo[m_cursorSizeIndex].xHotspot = MOUSE_SPELL_CURSOR_HOTSPOT;
                IconInfo[m_cursorSizeIndex].yHotspot = MOUSE_SPELL_CURSOR_HOTSPOT;
            } else {
                IconInfo[m_cursorSizeIndex].xHotspot = iHotSpot[m_cursorSizeIndex][0];
                IconInfo[m_cursorSizeIndex].yHotspot = iHotSpot[m_cursorSizeIndex][1];
            }
            IconInfo[m_cursorSizeIndex].hbmMask = hbmpAndMask[m_cursorSizeIndex];
            IconInfo[m_cursorSizeIndex].hbmColor = 0;
            hMouseCursor[m_cursorSizeIndex] = CreateIconIndirect(&IconInfo[m_cursorSizeIndex]);
            H2_ASSERT(
                reinterpret_cast<i32>(hMouseCursor[m_cursorSizeIndex]),
                gMouseManagerStrings.cursorAssertion.text,
                0x215
            );
        }
        SetCursor(hMouseCursor[m_cursorSizeIndex]);
    }
    gpResourceManager->RestorePosition();
    gbInSetPointer = 0;
    gbPutzingWithMouseCtr--;
}

// @semantic coverage checkpoint: semantics, member layout, zero-frame CFG, local
// lifetimes, and all 83 relocation occurrences are complete. Manual -dr review
// confirms the canonical private globals and arrays; the delinked target uses
// interior/addend aliases for their second columns and embeds two Win32 IAT addresses.
// Base .text is 0x46a versus retail 0x47a. Their non-branch instruction streams are
// otherwise aligned: retail selects the near form for two additional JG and two
// additional JL instructions, accounting exactly for the 16-byte delta (4 * 4).
// Direct/reversed comparisons were byte-neutral, while ternary/direct-field width
// forms regress. A libclang pass exposed 62 syntax-valid variants but produced no
// retained improvement before its nine-minute bound; interrupted candidates were
// reverted. Revisit after a predecessor/shared-layout TU-state change, or with a
// last-mile branch-width-aware pass rather than more local predicate spellings.
VA(0x004c9a40, 0x47a)
void mouseManager::NewUpdate(i32 force) {
    i32 width;
    i32 height;
    if (m_hideCount != 0 && force == 0)
        return;
    if (m_cursorReady == 0 || bInNewMouseUpdate != 0)
        return;

    bInNewMouseUpdate = 1;
    gbPutzingWithMouseCtr++;
    if (force == 0) {
        if (gbColorMice == 0)
            goto updateDone;
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        m_mouseX = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
        m_mouseY = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
        CheckChangeCursor(m_mouseX, m_mouseY, 0);
    }
    if (gbColorMice == 0)
        goto updateDone;

    if (force == 0 && m_mouseX - iHotSpot[m_cursorSizeIndex][0] == m_cursorLeft
        && m_mouseY - iHotSpot[m_cursorSizeIndex][1] == m_cursorTop)
        goto updateDone;

    gOldMouseLeft = m_savedLeft;
    gOldMouseTop = m_savedTop;
    gOldMouseRight = m_cursorRight;
    gOldMouseBottom = m_cursorBottom;

    m_cursorLeft = m_mouseX - iHotSpot[m_cursorSizeIndex][0];
    m_cursorTop = m_mouseY - iHotSpot[m_cursorSizeIndex][1];
    m_cursorRight = m_cursorLeft + iMouseSize[m_cursorSizeIndex][0] - 1;
    m_cursorBottom = m_cursorTop + iMouseSize[m_cursorSizeIndex][1] - 1;
    if (m_cursorRight > MOUSE_SCREEN_WIDTH - 1)
        m_cursorRight = MOUSE_SCREEN_WIDTH - 1;
    if (m_cursorBottom > MOUSE_SCREEN_HEIGHT - 1)
        m_cursorBottom = MOUSE_SCREEN_HEIGHT - 1;
    m_savedLeft = 0;
    if (m_cursorLeft >= 0)
        m_savedLeft = m_cursorLeft;
    m_savedTop = 0;
    if (m_cursorTop >= 0)
        m_savedTop = m_cursorTop;

    if (gOldMouseLeft <= MOUSE_SCREEN_WIDTH - 1 && gOldMouseTop <= MOUSE_SCREEN_HEIGHT - 1
        && gOldMouseRight >= 0 && gOldMouseBottom >= 0) {
        if (gOldMouseRight < m_savedLeft || gOldMouseLeft > m_cursorRight
            || gOldMouseBottom < m_savedTop || gOldMouseTop > m_cursorBottom) {
            if (gOldMouseRight > MOUSE_SCREEN_WIDTH - 1)
                gOldMouseRight = MOUSE_SCREEN_WIDTH - 1;
            if (gOldMouseBottom > MOUSE_SCREEN_HEIGHT - 1)
                gOldMouseBottom = MOUSE_SCREEN_HEIGHT - 1;
            BlitBitmapToScreenNoMouseCheck(
                gpWindowManager->m_screen,
                gOldMouseLeft,
                gOldMouseTop,
                gOldMouseRight - gOldMouseLeft + 1,
                gOldMouseBottom - gOldMouseTop + 1,
                gOldMouseLeft,
                gOldMouseTop
            );
        } else {
            if (gOldMouseLeft > m_savedLeft)
                gOldMouseLeft = m_savedLeft;
            if (gOldMouseTop > m_savedTop)
                gOldMouseTop = m_savedTop;
            i32 right = m_savedLeft + iMouseSize[m_cursorSizeIndex][0] - 1;
            if (gOldMouseRight < right)
                gOldMouseRight = right;
            i32 bottom = m_savedTop + iMouseSize[m_cursorSizeIndex][1] - 1;
            if (gOldMouseBottom < bottom)
                gOldMouseBottom = bottom;
            goto updateBoundsReady;
        }
    }

    gOldMouseLeft = m_savedLeft;
    gOldMouseTop = m_savedTop;
    gOldMouseRight = m_savedLeft + iMouseSize[m_drawnCursorSizeIndex][0] - 1;
    gOldMouseBottom = m_savedTop + iMouseSize[m_drawnCursorSizeIndex][1] - 1;
updateBoundsReady:
    if (gOldMouseLeft <= MOUSE_SCREEN_WIDTH - 1 && gOldMouseTop <= MOUSE_SCREEN_HEIGHT - 1
        && gOldMouseRight >= 0 && gOldMouseBottom >= 0) {
        if (gOldMouseRight > MOUSE_SCREEN_WIDTH - 1)
            gOldMouseRight = MOUSE_SCREEN_WIDTH - 1;
        if (gOldMouseBottom > MOUSE_SCREEN_HEIGHT - 1)
            gOldMouseBottom = MOUSE_SCREEN_HEIGHT - 1;
    }

    width = iMouseSize[m_cursorSizeIndex][0];
    if (m_savedLeft + width > MOUSE_SCREEN_WIDTH)
        m_savedWidth = MOUSE_SCREEN_WIDTH - m_savedLeft;
    else
        m_savedWidth = width;
    height = iMouseSize[m_cursorSizeIndex][1];
    if (m_savedTop + height > MOUSE_SCREEN_HEIGHT)
        m_savedHeight = MOUSE_SCREEN_HEIGHT - m_savedTop;
    else
        m_savedHeight = height;

    gpWindowManager->m_screen->CopyToCareful(
        m_savedUnderlying,
        0,
        0,
        m_savedLeft,
        m_savedTop,
        m_savedWidth,
        m_savedHeight
    );
    if (m_hideCount == 0)
        IconToBitmap(
            m_cursorIcon,
            gpWindowManager->m_screen,
            m_cursorLeft,
            m_cursorTop,
            m_cursorFrame,
            1,
            0,
            0,
            MOUSE_SCREEN_WIDTH,
            MOUSE_SCREEN_HEIGHT,
            0
        );
    BlitBitmapToScreenNoMouseCheck(
        gpWindowManager->m_screen,
        gOldMouseLeft,
        gOldMouseTop,
        gOldMouseRight - gOldMouseLeft + 1,
        gOldMouseBottom - gOldMouseTop + 1,
        gOldMouseLeft,
        gOldMouseTop
    );
    m_savedUnderlying->CopyToCareful(
        gpWindowManager->m_screen,
        m_savedLeft,
        m_savedTop,
        0,
        0,
        m_savedWidth,
        m_savedHeight
    );
    m_drawnCursorSizeIndex = m_cursorSizeIndex;

updateDone:
    bInNewMouseUpdate = 0;
    gbPutzingWithMouseCtr--;
}

VA(0x004c9ec0, 0x56)
void mouseManager::MouseCoords(i32& x, i32& y) {
    GetCursorPos(&gMouseScreenPt);
    ScreenToClient(hwndApp, &gMouseScreenPt);
    x = (gMouseScreenPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
    y = (gMouseScreenPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
}

VA(0x004c9f20, 0xa2)
void mouseManager::SaveAndDraw(void) {
    m_savedWidth = m_cursorLeft + iMouseSize[m_cursorSizeIndex][0] > MOUSE_SCREEN_WIDTH
                       ? MOUSE_SCREEN_WIDTH - m_cursorLeft
                       : iMouseSize[m_cursorSizeIndex][0];
    m_savedHeight = m_cursorTop + iMouseSize[m_cursorSizeIndex][1] > MOUSE_SCREEN_HEIGHT
                        ? MOUSE_SCREEN_HEIGHT - m_cursorTop
                        : iMouseSize[m_cursorSizeIndex][1];
    gpWindowManager->m_screen->CopyToCareful(
        m_savedUnderlying,
        0,
        0,
        m_savedLeft,
        m_savedTop,
        m_savedWidth,
        m_savedHeight
    );
    IconToBitmap(
        m_cursorIcon,
        gpWindowManager->m_screen,
        m_cursorLeft,
        m_cursorTop,
        m_cursorFrame,
        1,
        0,
        0,
        MOUSE_SCREEN_WIDTH,
        MOUSE_SCREEN_HEIGHT,
        0
    );
}

VA(0x004c9fd0, 0x29)
void mouseManager::RestoreUnderlying(void) {
    m_savedUnderlying->CopyToCareful(
        gpWindowManager->m_screen,
        m_savedLeft,
        m_savedTop,
        0,
        0,
        m_savedWidth,
        m_savedHeight
    );
}

VA(0x004ca000, 0x2c)
void mouseManager::ReallyHidePointer(void) {
    if (gbColorMice != 0) {
        m_hideCount++;
        if (m_hideCount == 1)
            NewUpdate(1);
    } else {
        ShowCursor(0);
    }
}

VA(0x004ca030, 0xb9)
void mouseManager::ReallyShowPointer(void) {
    if (gbColorMice != 0) {
        if (m_hideCount > 0 && --m_hideCount == 0) {
            gbPutzingWithMouseCtr++;
            if (gbColorMice != 0) {
                GetCursorPos(&gMouseCheckPt);
                ScreenToClient(hwndApp, &gMouseCheckPt);
                i32 x = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
                m_mouseX = x;
                i32 y = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
                m_mouseY = y;
                CheckChangeCursor(x, y, 0);
            }
            NewUpdate(1);
            gbPutzingWithMouseCtr = gbPutzingWithMouseCtr - 1;
        }
    } else {
        ShowCursor(1);
    }
}

VA(0x004ca0f0, 0x1a)
void mouseManager::HideColorPointer(void) {
    m_hideCount++;
    if (m_hideCount == 1)
        NewUpdate(1);
}

VA(0x004ca110, 0x98)
void mouseManager::ShowColorPointer(void) {
    if (m_hideCount > 0 && --m_hideCount == 0) {
        gbPutzingWithMouseCtr++;
        if (gbColorMice != 0) {
            GetCursorPos(&gMouseCheckPt);
            ScreenToClient(hwndApp, &gMouseCheckPt);
            i32 x = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
            m_mouseX = x;
            i32 y = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
            m_mouseY = y;
            CheckChangeCursor(x, y, 0);
        }
        NewUpdate(1);
        gbPutzingWithMouseCtr--;
    }
}

VA(0x004ca1b0, 0xc)
i32 mouseManager::IsVis(void) {
    return m_hideCount == 0;
}

VA(0x004ca1c0, 0x66)
void mouseManager::CheckUpdateMousePos(void) {
    if (gbColorMice != 0) {
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        i32 x = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
        m_mouseX = x;
        i32 y = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
        m_mouseY = y;
        CheckChangeCursor(x, y, 0);
    }
}

VA(0x004ca230, 0x191)
void mouseManager::SetColorMice(i32 param_1) {
    if (param_1 != gbColorMice) {
        i32 savedWM56 = gpWindowManager->m_updateFlags;
        gpWindowManager->m_updateFlags = 0;
        gbPutzingWithMouseCtr++;
        i32 savedInNew = bInNewMouseUpdate;
        bInNewMouseUpdate = 0;
        if (gbColorMice != 0) {
            m_hideCount++;
            if (m_hideCount == 1)
                NewUpdate(1);
        } else {
            ShowCursor(0);
        }
        i32 savedX = m_cursorFrame;
        i32 savedY = m_cursorType;
        i32 saved7e = m_forcePointerUpdate;
        m_cursorReady = 0;
        gbColorMice = param_1;
        m_cursorFrame = MOUSE_RELOAD_CURSOR_FRAME;
        m_cursorType = MOUSE_INVALID_CURSOR_TYPE;
        m_forcePointerUpdate = 0;
        SetPointer(gMouseManagerStrings.defaultCursorName, savedX, savedY);
        m_cursorReady = 1;
        m_forcePointerUpdate = saved7e;
        if (gbColorMice != 0) {
            if (m_hideCount > 0 && --m_hideCount == 0) {
                gbPutzingWithMouseCtr++;
                if (gbColorMice != 0) {
                    GetCursorPos(&gMouseCheckPt);
                    ScreenToClient(hwndApp, &gMouseCheckPt);
                    i32 x = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
                    m_mouseX = x;
                    i32 y = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
                    m_mouseY = y;
                    CheckChangeCursor(x, y, 0);
                }
                NewUpdate(1);
                gbPutzingWithMouseCtr--;
            }
        } else {
            ShowCursor(1);
        }
        gbPutzingWithMouseCtr--;
        bInNewMouseUpdate = savedInNew;
        gpWindowManager->m_updateFlags = savedWM56;
    }
}

// ===== vtable mouseManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c9350, 0x94)  int mouseManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c93f0, 0xed)  void mouseManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c94e0, 0x5)  int mouseManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(mouseManager, 0x004eba00);
