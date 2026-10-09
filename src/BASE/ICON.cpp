#include <H2/Ints.h>
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
#include <BASE/display.h>

enum class IconColorTableMode : i32 {
    COLOR_TABLE_SKIP_SHADOWS  = 0,
    COLOR_TABLE_DRAW_SHADOWS = 1
};
using enum IconColorTableMode;

typedef enum IconDrawExtentConstant {
    DRAW_COMBAT_HEIGHT = 444
} IconDrawExtentConstant;

icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = nullptr;
    gpResourceManager->PointToFile(id);
    m_frameCount = gpResourceManager->ReadWord();
    u32 length = gpResourceManager->ReadLong();
    const u32 memberSize = gpResourceManager->GetFileSize(id);
    if (memberSize < 6 || length > memberSize - 6 || m_frameCount <= 0
        || static_cast<u32>(m_frameCount) > length / images::IconFrameBytes) {
        ShutDown("Invalid ICN frame count or payload length.");
        return;
    }
    m_data = static_cast<u8*>(H2_ALLOC(length));
    m_dataSize = length;
    gpResourceManager->ReadBlock(m_data, length);
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
            LOGICAL_SCREEN_WIDTH,
            LOGICAL_SCREEN_HEIGHT,
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
        LOGICAL_SCREEN_WIDTH,
        LOGICAL_SCREEN_HEIGHT,
        0
    );
}

IconDrawResult icon::CombatClipDrawToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    struct SLimitData* limits,
    IconDrawOrientation orientation,
    i32 outlineColor,
    u8* colorTable,
    i8* shear
) {
    if (gbComputeExtent != false) {
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
        if (gbSaveBiggestExtent != false) {
            if (limits->left < giMinExtentX)
                giMinExtentX = limits->left;
            if (limits->top < giMinExtentY)
                giMinExtentY = limits->top;
            if (limits->right > giMaxExtentX)
                giMaxExtentX = limits->right;
            if (limits->bottom > giMaxExtentY)
                giMaxExtentY = limits->bottom;
        }
        if (gbReturnAfterComputeExtent != false)
            return ICON_DRAW_SKIPPED;
    }

    if (gbLimitToExtent != false
        && (gbCurrArmyDrawn == false || limits->left > giMaxExtentX || limits->right < giMinExtentX
            || limits->top > giMaxExtentY || limits->bottom < giMinExtentY))
        return ICON_DRAW_SKIPPED;

    // DrawFrame restores only this rectangle. Every variant must stay inside
    // it, or shadows outside the restored area are applied again each frame.
    const i32 clipX = gbLimitToExtent != false ? giMinExtentX : 0;
    const i32 clipY = gbLimitToExtent != false ? giMinExtentY : 0;
    const i32 clipW = gbLimitToExtent != false ? giMaxExtentX - giMinExtentX + 1 : LOGICAL_SCREEN_WIDTH;
    const i32 clipH = gbLimitToExtent != false ? giMaxExtentY - giMinExtentY + 1 : DRAW_COMBAT_HEIGHT;

    if (shear != NULL) {
        if (orientation == ICON_DRAW_NORMAL)
            IconToBitmapYModify(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                clipX,
                clipY,
                clipW,
                clipH,
                outlineColor,
                {shear, LOGICAL_SCREEN_HEIGHT}
            );
        else
            FlipIconToBitmapYModify(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                clipX,
                clipY,
                clipW,
                clipH,
                outlineColor,
                {shear, LOGICAL_SCREEN_HEIGHT}
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
                clipX,
                clipY,
                clipW,
                clipH,
                outlineColor,
                colorTable,
                H2EnumIndex(COLOR_TABLE_DRAW_SHADOWS)
            );
        else
            FlipIconToBitmapColorTable(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                ICON_DRAW_CLIP,
                clipX,
                clipY,
                clipW,
                clipH,
                outlineColor,
                colorTable
            );
    } else if (orientation == ICON_DRAW_NORMAL) {
        IconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            ICON_DRAW_CLIP,
            clipX,
            clipY,
            clipW,
            clipH,
            outlineColor
        );
    } else {
        FlipIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            ICON_DRAW_CLIP,
            clipX,
            clipY,
            clipW,
            clipH,
            outlineColor
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
    if (gbLimitToExtent != false && limits != NULL) {
        limits->left = x + GetIconEntry(this, frame)->x;
        limits->right = limits->left + GetIconEntry(this, frame)->w - 1;
        limits->top = y + GetIconEntry(this, frame)->y;
        limits->bottom = limits->top + GetIconEntry(this, frame)->h - 1;
        if (gbCurrArmyDrawn == false || limits->left > giMaxExtentX || limits->right < giMinExtentX
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
