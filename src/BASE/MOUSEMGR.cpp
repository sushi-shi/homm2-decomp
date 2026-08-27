#include <Ints.h>
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

#define MOUSE_CURSOR_MASK_SHIFT 3
#define MOUSE_MANAGER_SOURCE_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Base\\MOUSEMGR.CPP"
#define MOUSE_MANAGER_NAME "mouseManager"
#define MOUSE_MANAGER_ADVENTURE_ICON "ADVMCO.ICN"
#define MOUSE_MANAGER_SPELL_ICON "SPELCO.ICN"
#define MOUSE_MANAGER_COMBAT_ICON "CMSECO.ICN"
#define MOUSE_MANAGER_ADVENTURE_BITMAP "ADVMBW%02d.BMP"
#define MOUSE_MANAGER_SPELL_BITMAP "SPELBW%02d.BMP"
#define MOUSE_MANAGER_COMBAT_BITMAP "CMSEBW%02d.BMP"

typedef enum MouseManagerLocalConstant {
    HOTSPOT_CENTER_DIVISOR = 2,
    RESOURCE_NAME_CAPACITY = 16
} MouseManagerLocalConstant;

static i32 gOldMouseLeft = 0;
static i32 gOldMouseTop = 0;
BITMAP bmpAndMask[MOUSE_CURSOR_COUNT] = {};
static i32 gOldMouseRight = 0;
HICON hMouseCursor[MOUSE_CURSOR_COUNT] = {NULL};
HBITMAP hbmpAndMask[MOUSE_CURSOR_COUNT] = {NULL};
void* cColorBits[MOUSE_CURSOR_COUNT] = {NULL};
static i32 gOldMouseBottom = 0;
ICONINFO IconInfo[MOUSE_CURSOR_COUNT] = {};
static POINT gMouseScreenPt = {};
void* cAndBits[MOUSE_CURSOR_COUNT] = {NULL};
static POINT gMouseCheckPt = {};
b32 gbInSetPointer = false;
i32 bInNewMouseUpdate = 0;


i32 iMouseOffset[MOUSE_CURSOR_TYPE_SLOT_COUNT] = {0, 41, 57};
i8 iMouseSize[MOUSE_CURSOR_COUNT][MOUSE_CURSOR_AXIS_COUNT] = {
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
i8 iHotSpot[MOUSE_CURSOR_COUNT][MOUSE_CURSOR_AXIS_COUNT] = {
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
mouseManager::mouseManager(void) : baseManager() {
    i32 i;
    m_cursorType = MOUSE_INVALID_CURSOR_TYPE;
    m_savedUnderlying = NULL;
    m_active = false;
    strcpy(
        m_name,
        MOUSE_MANAGER_NAME
    );
    m_cursorFrame = 0;
    m_cursorIcon = NULL;
    m_cursorReady = 1;
    for (i = 0; i < MOUSE_CURSOR_COUNT; i++) {
        cAndBits[i] = NULL;
        cColorBits[i] = NULL;
        hMouseCursor[i] = NULL;
        hbmpAndMask[i] = NULL;
        if (iHotSpot[i][MOUSE_CURSOR_HORIZONTAL] == MOUSE_DEFAULT_HOTSPOT)
            iHotSpot[i][MOUSE_CURSOR_HORIZONTAL] =
                iMouseSize[i][MOUSE_CURSOR_VERTICAL] / HOTSPOT_CENTER_DIVISOR;
        if (iHotSpot[i][MOUSE_CURSOR_VERTICAL] == MOUSE_DEFAULT_HOTSPOT)
            iHotSpot[i][MOUSE_CURSOR_VERTICAL] =
                iMouseSize[i][MOUSE_CURSOR_VERTICAL] / HOTSPOT_CENTER_DIVISOR;
    }
    m_hideCount = 1;
}

i32 mouseManager::Open(i32 priority) {
    m_forcePointerUpdate = 0;
    m_savedUnderlying =
        new bitmap(BITMAP_TYPE_MEMORY, MOUSE_SAVED_BITMAP_WIDTH, MOUSE_SAVED_BITMAP_HEIGHT);
    m_savedLeft = MOUSE_SCREEN_CENTER_X - 1;
    m_savedTop = MOUSE_SCREEN_CENTER_Y - 1;
    m_cursorRight = MOUSE_SCREEN_CENTER_X;
    m_cursorBottom = MOUSE_SCREEN_CENTER_Y;
    m_cursorLeft = MOUSE_SCREEN_CENTER_X - 1;
    m_cursorTop = MOUSE_SCREEN_CENTER_Y - 1;
    m_mouseX = MOUSE_SCREEN_CENTER_X;
    m_mouseY = MOUSE_SCREEN_CENTER_Y;
    m_cursorSizeIndex = 0;
    m_drawnCursorSizeIndex = 0;
    if (gbColorMice != 0)
        ShowCursor(0);
    m_messageMask = BASE_MANAGER_ACCEPT_RIGHT_BUTTON_UP;
    m_priority = priority;
    m_active = true;
    return 0;
}

void mouseManager::Close(void) {
    i32 cursorIndex;
    if (m_active != 1)
        return;
    m_active = false;
    if (m_savedUnderlying != NULL)
        delete m_savedUnderlying;
    m_savedUnderlying = NULL;
    SetCursor(LoadCursorA(NULL, IDC_ARROW));
    for (cursorIndex = 0; cursorIndex < MOUSE_CURSOR_COUNT; cursorIndex++) {
        if (hMouseCursor[cursorIndex] != NULL)
            DestroyIcon(hMouseCursor[cursorIndex]);
        hMouseCursor[cursorIndex] = NULL;
        if (cAndBits[cursorIndex] != NULL)
            H2_FREE(cAndBits[cursorIndex]);
        cAndBits[cursorIndex] = NULL;
        if (cColorBits[cursorIndex] != NULL)
            H2_FREE(cColorBits[cursorIndex]);
        cColorBits[cursorIndex] = NULL;
        if (hbmpAndMask[cursorIndex] != NULL)
            DeleteObject(hbmpAndMask[cursorIndex]);
        hbmpAndMask[cursorIndex] = NULL;
    }
    if (m_cursorIcon != NULL)
        gpResourceManager->Dispose(m_cursorIcon);
    m_cursorIcon = NULL;
}

MessageDispatchResult mouseManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

void mouseManager::SetPointer(const char* name, i32 frame, MouseCursorType cursorType) {
    MouseCursorType type;
    if (m_forcePointerUpdate != 0)
        return;
    {
        gbPutzingWithMouseCtr++;
        gpResourceManager->SavePosition();
        if (cursorType == MOUSE_AUTO_CURSOR_TYPE) {
            if (giCurExe == CONFIG_EXECUTABLE_EDITOR || *name == 'a' || *name == 'A')
                type = MOUSE_CURSOR_ADVENTURE;
            else if (*name == 's' || *name == 'S')
                type = MOUSE_CURSOR_SPELL;
            else
                type = MOUSE_CURSOR_COMBAT;
        } else {
            type = cursorType;
        }
        if (type != m_cursorType && (m_cursorType = type, gbColorMice != 0)) {
            i32 saved82 = m_cursorReady;
            m_cursorReady = 0;
            if (m_cursorIcon != NULL)
                gpResourceManager->Dispose(m_cursorIcon);
            char local_10[RESOURCE_NAME_CAPACITY];
            if (m_cursorType == MOUSE_CURSOR_ADVENTURE)
                sprintf(
                    local_10,
                    MOUSE_MANAGER_ADVENTURE_ICON
                );
            else if (m_cursorType == MOUSE_CURSOR_SPELL)
                sprintf(
                    local_10,
                    MOUSE_MANAGER_SPELL_ICON
                );
            else
                sprintf(
                    local_10,
                    MOUSE_MANAGER_COMBAT_ICON
                );
            m_cursorIcon = gpResourceManager->GetIcon(local_10);
            H2_ASSERT(frame != MOUSE_KEEP_CURRENT_FRAME);
            m_cursorFrame = MOUSE_INVALID_CURSOR_FRAME;
            m_cursorReady = saved82;
        }
        SetPointer(frame);
        gpResourceManager->RestorePosition();
        gbPutzingWithMouseCtr--;
    }
}

void mouseManager::SetPointer(i32 frame) {
    if (m_forcePointerUpdate != 0)
        return;
    if (frame < 0)
        return;
    if (m_active != true)
        return;
    if (frame == m_cursorFrame)
        return;
    if (gbInSetPointer != 0)
        return;

    gbInSetPointer = true;
    gbPutzingWithMouseCtr++;
    gpResourceManager->SavePosition();
    if (giCurExe == CONFIG_EXECUTABLE_EDITOR)
        m_cursorType = MOUSE_CURSOR_ADVENTURE;
    if (frame == MOUSE_KEEP_CURRENT_FRAME)
        frame = m_cursorFrame;
    else
        m_cursorFrame = frame;
    m_cursorSizeIndex = frame + iMouseOffset[H2EnumIndex(m_cursorType)];
    H2_ASSERT(m_cursorSizeIndex >= 0 && m_cursorSizeIndex < MOUSE_CURSOR_COUNT);

    if (gbColorMice != 0) {
        NewUpdate(1);
        goto done;
    }
    if (hMouseCursor[m_cursorSizeIndex] == NULL) {
        cColorBits[m_cursorSizeIndex] = H2_ALLOC(MOUSE_CURSOR_COLOR_BYTES);
        cAndBits[m_cursorSizeIndex] = H2_ALLOC(MOUSE_CURSOR_AND_BYTES);

        char filename[RESOURCE_NAME_CAPACITY];
        if (m_cursorType == MOUSE_CURSOR_ADVENTURE)
            sprintf(
                filename,
                MOUSE_MANAGER_ADVENTURE_BITMAP,
                frame + 1
            );
        else if (m_cursorType == MOUSE_CURSOR_SPELL)
            sprintf(
                filename,
                MOUSE_MANAGER_SPELL_BITMAP,
                frame
            );
        else
            sprintf(
                filename,
                MOUSE_MANAGER_COMBAT_BITMAP,
                frame + 1
            );

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
            i32 x;
            i32 y;
            for (y = 0; y < MOUSE_CURSOR_BITMAP_WIDTH; y++) {
                for (x = 0; x < MOUSE_CURSOR_BITMAP_WIDTH; x++) {
                    if (*(static_cast<u8*>(cColorBits[m_cursorSizeIndex]) + x
                          + y * MOUSE_CURSOR_BITMAP_WIDTH)
                        == 0)
                        *(static_cast<u8*>(cAndBits[m_cursorSizeIndex])
                          + y * MOUSE_CURSOR_MASK_ROW_BYTES
                          + (x >> MOUSE_CURSOR_MASK_SHIFT)) |=
                            1
                            << (MOUSE_CURSOR_MASK_HIGH_BIT
                                - (x & MOUSE_CURSOR_MASK_HIGH_BIT));
                    else if (*(static_cast<u8*>(cColorBits[m_cursorSizeIndex]) + x
                               + y * MOUSE_CURSOR_BITMAP_WIDTH)
                             == 1)
                        *(static_cast<u8*>(cAndBits[m_cursorSizeIndex])
                          + MOUSE_CURSOR_MASK_PLANE_BYTES + y * MOUSE_CURSOR_MASK_ROW_BYTES
                          + (x >> MOUSE_CURSOR_MASK_SHIFT)) |=
                            1
                            << (MOUSE_CURSOR_MASK_HIGH_BIT
                                - (x & MOUSE_CURSOR_MASK_HIGH_BIT));
                }
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
        H2_ASSERT(reinterpret_cast<i32>(hbmpAndMask[m_cursorSizeIndex]));

        IconInfo[m_cursorSizeIndex].fIcon = 0;
        if (m_cursorType == MOUSE_CURSOR_SPELL) {
            IconInfo[m_cursorSizeIndex].xHotspot = MOUSE_SPELL_CURSOR_HOTSPOT;
            IconInfo[m_cursorSizeIndex].yHotspot = MOUSE_SPELL_CURSOR_HOTSPOT;
        } else {
            IconInfo[m_cursorSizeIndex].xHotspot =
                iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL];
            IconInfo[m_cursorSizeIndex].yHotspot =
                iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL];
        }
        IconInfo[m_cursorSizeIndex].hbmMask = hbmpAndMask[m_cursorSizeIndex];
        IconInfo[m_cursorSizeIndex].hbmColor = NULL;
        hMouseCursor[m_cursorSizeIndex] = CreateIconIndirect(&IconInfo[m_cursorSizeIndex]);
        H2_ASSERT(reinterpret_cast<i32>(hMouseCursor[m_cursorSizeIndex]));
    }
    SetCursor(hMouseCursor[m_cursorSizeIndex]);
done:
    gpResourceManager->RestorePosition();
    gbPutzingWithMouseCtr--;
    gbInSetPointer = false;
}

void mouseManager::NewUpdate(i32 force) {
    if (m_hideCount != 0 && force == 0)
        return;
    if (m_cursorReady == 0)
        return;
    if (bInNewMouseUpdate != 0)
        return;

    gbPutzingWithMouseCtr = gbPutzingWithMouseCtr + 1;
    bInNewMouseUpdate = 1;
    if (force == 0) {
        CheckUpdateMousePos();
    }
    if (gbColorMice != 0) {
        if (force != 0
            || m_cursorLeft != m_mouseX - iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL]
            || m_cursorTop != m_mouseY - iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL]) {
            gOldMouseLeft = m_savedLeft;
            gOldMouseTop = m_savedTop;
            gOldMouseRight = m_cursorRight;
            gOldMouseBottom = m_cursorBottom;

            m_cursorLeft = m_mouseX - iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL];
            m_cursorTop = m_mouseY - iHotSpot[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL];
            m_cursorRight =
                m_cursorLeft + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL] - 1;
            m_cursorBottom =
                m_cursorTop + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL] - 1;
            if (m_cursorRight > MOUSE_SCREEN_WIDTH - 1)
                m_cursorRight = MOUSE_SCREEN_WIDTH - 1;
            if (m_cursorBottom > MOUSE_SCREEN_HEIGHT - 1)
                m_cursorBottom = MOUSE_SCREEN_HEIGHT - 1;
            if (m_cursorLeft < 0)
                m_savedLeft = 0;
            else
                m_savedLeft = m_cursorLeft;
            if (m_cursorTop < 0)
                m_savedTop = 0;
            else
                m_savedTop = m_cursorTop;

            if (gOldMouseLeft > MOUSE_SCREEN_WIDTH - 1 || gOldMouseTop > MOUSE_SCREEN_HEIGHT - 1
                || gOldMouseRight < 0 || gOldMouseBottom < 0)
                goto resetBounds;

            if (m_savedLeft > gOldMouseRight || m_cursorRight < gOldMouseLeft
                || m_savedTop > gOldMouseBottom || m_cursorBottom < gOldMouseTop) {
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
            resetBounds:
                gOldMouseLeft = m_savedLeft;
                gOldMouseTop = m_savedTop;
                gOldMouseRight =
                    m_savedLeft + iMouseSize[m_drawnCursorSizeIndex][MOUSE_CURSOR_HORIZONTAL] - 1;
                gOldMouseBottom =
                    m_savedTop + iMouseSize[m_drawnCursorSizeIndex][MOUSE_CURSOR_VERTICAL] - 1;
            } else {
                if (m_savedLeft < gOldMouseLeft)
                    gOldMouseLeft = m_savedLeft;
                if (m_savedTop < gOldMouseTop)
                    gOldMouseTop = m_savedTop;
                if (m_savedLeft + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL] - 1
                    > gOldMouseRight)
                    gOldMouseRight =
                        m_savedLeft + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL] - 1;
                if (m_savedTop + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL] - 1
                    > gOldMouseBottom)
                    gOldMouseBottom =
                        m_savedTop + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL] - 1;
            }

            if (gOldMouseLeft > MOUSE_SCREEN_WIDTH - 1 || gOldMouseTop > MOUSE_SCREEN_HEIGHT - 1
                || gOldMouseRight < 0 || gOldMouseBottom < 0)
                goto finishUpdate;

            if (gOldMouseRight > MOUSE_SCREEN_WIDTH - 1)
                gOldMouseRight = MOUSE_SCREEN_WIDTH - 1;
            if (gOldMouseBottom > MOUSE_SCREEN_HEIGHT - 1)
                gOldMouseBottom = MOUSE_SCREEN_HEIGHT - 1;

            if (m_savedLeft + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL]
                > MOUSE_SCREEN_WIDTH)
                m_savedWidth = MOUSE_SCREEN_WIDTH - m_savedLeft;
            else
                m_savedWidth = iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL];
            if (m_savedTop + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL]
                > MOUSE_SCREEN_HEIGHT)
                m_savedHeight = MOUSE_SCREEN_HEIGHT - m_savedTop;
            else
                m_savedHeight = iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL];

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
                    ICON_DRAW_CLIP,
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
        finishUpdate:
            m_drawnCursorSizeIndex = m_cursorSizeIndex;
        }
    }
    bInNewMouseUpdate = 0;
    gbPutzingWithMouseCtr--;
}

void mouseManager::MouseCoords(i32& x, i32& y) {
    GetCursorPos(&gMouseScreenPt);
    ScreenToClient(hwndApp, &gMouseScreenPt);
    x = (gMouseScreenPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
    y = (gMouseScreenPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
}

void mouseManager::SaveAndDraw(void) {
    if (m_cursorLeft + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL]
        > MOUSE_SCREEN_WIDTH)
        m_savedWidth = MOUSE_SCREEN_WIDTH - m_cursorLeft;
    else
        m_savedWidth = iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_HORIZONTAL];
    if (m_cursorTop + iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL]
        > MOUSE_SCREEN_HEIGHT)
        m_savedHeight = MOUSE_SCREEN_HEIGHT - m_cursorTop;
    else
        m_savedHeight = iMouseSize[m_cursorSizeIndex][MOUSE_CURSOR_VERTICAL];
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
        ICON_DRAW_CLIP,
        0,
        0,
        MOUSE_SCREEN_WIDTH,
        MOUSE_SCREEN_HEIGHT,
        0
    );
}

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

void mouseManager::ReallyHidePointer(void) {
    if (gbColorMice != 0) {
        HideColorPointer();
    } else {
        ShowCursor(0);
    }
}

void mouseManager::ReallyShowPointer(void) {
    if (gbColorMice != 0) {
        ShowColorPointer();
    } else {
        ShowCursor(1);
    }
}

void mouseManager::HideColorPointer(void) {
    m_hideCount++;
    if (m_hideCount == 1)
        NewUpdate(1);
}

void mouseManager::ShowColorPointer(void) {
    if (m_hideCount > 0) {
        m_hideCount = m_hideCount - 1;
        if (m_hideCount == 0) {
            gbPutzingWithMouseCtr = gbPutzingWithMouseCtr + 1;
            CheckUpdateMousePos();
            NewUpdate(1);
            gbPutzingWithMouseCtr = gbPutzingWithMouseCtr - 1;
        }
    }
}

i32 mouseManager::IsVis(void) {
    return m_hideCount == 0;
}

void mouseManager::CheckUpdateMousePos(void) {
    if (gbColorMice != 0) {
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        m_mouseX = (gMouseCheckPt.x * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
        m_mouseY = (gMouseCheckPt.y * MOUSE_SCREEN_HEIGHT) / iMainWinScreenHeight;
        CheckChangeCursor(m_mouseX, m_mouseY, 0);
    }
}

void mouseManager::SetColorMice(i32 enabled) {
    if (enabled == gbColorMice)
        return;
    {
        i32 savedWM56 = gpWindowManager->m_updateFlags;
        gpWindowManager->m_updateFlags = 0;
        gbPutzingWithMouseCtr++;
        i32 wasInNew = bInNewMouseUpdate;
        bInNewMouseUpdate = 0;
        ReallyHidePointer();
        m_cursorReady = 0;
        i32 savedX = m_cursorFrame;
        MouseCursorType oldType = m_cursorType;
        i32 saved7e = m_forcePointerUpdate;
        gbColorMice = enabled;
        m_cursorFrame = MOUSE_RELOAD_CURSOR_FRAME;
        m_cursorType = MOUSE_INVALID_CURSOR_TYPE;
        m_forcePointerUpdate = 0;
        SetPointer(
            "",
            savedX,
            oldType
        );
        m_forcePointerUpdate = saved7e;
        m_cursorReady = 1;
        ReallyShowPointer();
        bInNewMouseUpdate = wasInNew;
        gbPutzingWithMouseCtr = gbPutzingWithMouseCtr - 1;
        gpWindowManager->m_updateFlags = savedWM56;
    }
}
