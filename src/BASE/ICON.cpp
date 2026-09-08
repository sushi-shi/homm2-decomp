#include <Ints.h>
#include <BASE/icon.h>
#include <BASE/IconDraw.h>
#include <BASE/ImageDecode.h>
#include <BASE/resource.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
#include <BASE/Icond2b.h>
#include <BASE/Icondf2b.h>
#include <BASE/Iconm2b.h>
#include <BASE/Iconmf2b.h>
#include <BASE/icon2bc.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon2by.h>
#include <BASE/iconf2by.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/SPELLS.h>

enum class IconColorTableMode : i32 {
    COLOR_TABLE_SKIP_DIM  = 0,
    COLOR_TABLE_APPLY_DIM = 1
};
using enum IconColorTableMode;

typedef enum IconDrawExtentConstant {
    DRAW_SCREEN_WIDTH  = 640,
    DRAW_SCREEN_HEIGHT = 480,
    DRAW_COMBAT_HEIGHT = 444
} IconDrawExtentConstant;

icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = nullptr;
    gpResourceManager->PointToFile(id);
    m_frameCount = gpResourceManager->ReadWord();
    u32 len = gpResourceManager->ReadLong();
    const u32 memberSize = gpResourceManager->GetFileSize(id);
    if (memberSize < 6 || len > memberSize - 6 || m_frameCount <= 0
        || static_cast<u32>(m_frameCount) > len / images::IconFrameBytes) {
        ShutDown("Invalid ICN frame count or payload length.");
        return;
    }
    m_data = static_cast<u8*>(H2_ALLOC(len));
    m_dataSize = len;
    gpResourceManager->ReadBlock(m_data, len);
    const char* error = nullptr;
    if (!images::ValidateIconPayload({m_data, m_dataSize}, m_frameCount, error)) {
        H2_FREE(m_data);
        m_data = nullptr;
        m_dataSize = 0;
        ShutDown(error);
    }
}

icon::~icon() {
    H2_FREE(m_data);
}

void icon::DrawToBuffer(
    i32 x, i32 y, i32 frame, IconDrawOrientation orientation
) {
    if (orientation == ICON_DRAW_NORMAL) {
        IconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            DRAW_SCREEN_WIDTH,
            DRAW_SCREEN_HEIGHT,
            0
        );
        return;
    }
    FlipIconToBitmap(
        this,
        gpWindowManager->m_screen,
        x,
        y,
        frame,
        ICON_DRAW_NO_CLIP,
        0,
        0,
        DRAW_SCREEN_WIDTH,
        DRAW_SCREEN_HEIGHT,
        0
    );
}

IconDrawResult icon::CombatClipDrawToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    struct SLimitData* limits,
    IconDrawOrientation orientation,
    i32 offset,
    u8* colorTable,
    i8* yModify
) {
    if (gbComputeExtent != 0) {
        if (orientation != ICON_DRAW_NORMAL) {
            limits->right = x - GetIconEntry(this, frame)->x;
            limits->left = limits->right - GetIconEntry(this, frame)->w + 1;
            limits->top = y + GetIconEntry(this, frame)->y;
            limits->bottom = limits->top + GetIconEntry(this, frame)->h - 1;
        } else {
            limits->left = x + GetIconEntry(this, frame)->x;
            limits->right = limits->left + GetIconEntry(this, frame)->w - 1;
            limits->top = y + GetIconEntry(this, frame)->y;
            limits->bottom = limits->top + GetIconEntry(this, frame)->h - 1;
        }
        if (gbSaveBiggestExtent != 0) {
            if (limits->left < giMinExtentX)
                giMinExtentX = limits->left;
            if (limits->top < giMinExtentY)
                giMinExtentY = limits->top;
            if (limits->right > giMaxExtentX)
                giMaxExtentX = limits->right;
            if (limits->bottom > giMaxExtentY)
                giMaxExtentY = limits->bottom;
        }
        if (gbReturnAfterComputeExtent != 0)
            return ICON_DRAW_SKIPPED;
    }

    if (gbLimitToExtent != 0
        && (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX || limits->right < giMinExtentX
            || limits->top > giMaxExtentY || limits->bottom < giMinExtentY))
        return ICON_DRAW_SKIPPED;

    if (yModify != NULL) {
        if (orientation == ICON_DRAW_NORMAL)
            IconToBitmapYModify(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                DRAW_SCREEN_WIDTH,
                DRAW_COMBAT_HEIGHT,
                offset,
                {yModify, SPELL_MODIFIER_ROW_COUNT}
            );
        else
            FlipIconToBitmapYModify(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                DRAW_SCREEN_WIDTH,
                DRAW_COMBAT_HEIGHT,
                offset,
                {yModify, SPELL_MODIFIER_ROW_COUNT}
            );
    } else if (colorTable != NULL) {
        if (orientation == ICON_DRAW_NORMAL)
            IconToBitmapColorTable(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                DRAW_SCREEN_WIDTH,
                DRAW_COMBAT_HEIGHT,
                offset,
                colorTable,
                H2EnumIndex(COLOR_TABLE_APPLY_DIM)
            );
        else
            FlipIconToBitmapColorTable(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                DRAW_SCREEN_WIDTH,
                DRAW_COMBAT_HEIGHT,
                offset,
                colorTable
            );
    } else if (gbLimitToExtent != 0) {
        if (orientation == ICON_DRAW_NORMAL)
            IconToBitmap(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1,
                offset
            );
        else
            FlipIconToBitmap(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1,
                offset
            );
    } else if (orientation == ICON_DRAW_NORMAL) {
        IconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            ICON_DRAW_CLIP,
            0,
            0,
            DRAW_SCREEN_WIDTH,
            DRAW_COMBAT_HEIGHT,
            offset
        );
    } else {
        FlipIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            ICON_DRAW_CLIP,
            0,
            0,
            DRAW_SCREEN_WIDTH,
            DRAW_COMBAT_HEIGHT,
            offset
        );
    }
    return ICON_DRAW_COMPLETED;
}

void icon::ClipFillToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    IconDrawOrientation,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    MonoIconToBitmap(
        this,
        gpWindowManager->m_screen,
        x,
        y,
        frame,
        color,
        ICON_DRAW_CLIP,
        clipX,
        clipY,
        clipW,
        clipH
    );
}

void icon::FillToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    IconDrawOrientation orientation,
    struct SLimitData* limits
) {
    if (orientation != ICON_DRAW_NORMAL) {
        FlipMonoIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            color,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            0,
            0
        );
        return;
    }
    if (gbLimitToExtent != 0 && limits != NULL) {
        limits->left = x + GetIconEntry(this, frame)->x;
        limits->right = limits->left + GetIconEntry(this, frame)->w - 1;
        limits->top = y + GetIconEntry(this, frame)->y;
        limits->bottom = limits->top + GetIconEntry(this, frame)->h - 1;
        if (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX || limits->right < giMinExtentX
            || limits->top > giMaxExtentY || limits->bottom < giMinExtentY)
            return;
    }
    MonoIconToBitmap(
        this,
        gpWindowManager->m_screen,
        x,
        y,
        frame,
        color,
        ICON_DRAW_NO_CLIP,
        0,
        0,
        0,
        0
    );
}

void icon::DimToBuffer(
    i32 x, i32 y, i32 frame, IconDrawOrientation orientation
) {
    if (orientation == ICON_DRAW_NORMAL) {
        DimIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            0,
            ICON_DRAW_NO_CLIP,
            0,
            0,
            0,
            0
        );
        return;
    }
    FlipDimIconToBitmap(
        this,
        gpWindowManager->m_screen,
        x,
        y,
        frame,
        0,
        ICON_DRAW_NO_CLIP,
        0,
        0,
        0,
        0
    );
}
