#include <va.h>
#include <BASE/icon.h>
#include <BASE/IconDraw.h>
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

H2_ENUM_CLASS_BEGIN(IconColorTableMode)
    COLOR_TABLE_SKIP_DIM  = 0,
    COLOR_TABLE_APPLY_DIM = 1
H2_ENUM_CLASS_END(IconColorTableMode)

H2_ENUM_BEGIN(IconDrawExtentConstant)
    DRAW_SCREEN_WIDTH  = 640,
    DRAW_SCREEN_HEIGHT = 480,
    DRAW_COMBAT_HEIGHT = 444
H2_ENUM_END(IconDrawExtentConstant)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\ICON.CPP"
VA(0x004c26f0, 0xb7)
icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    static char allocationSourceFile[] = RETAIL_FILE;
    gpResourceManager->PointToFile(id);
    m_frameCount = gpResourceManager->ReadWord();
    u32 len = gpResourceManager->ReadLong();
    m_data = static_cast<u8*>(H2_ALLOC_AT(len, allocationSourceFile, 18));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), len);
}

VA(0x004c27e0, 0x33)
inline icon::~icon() {
    static char destructionSourceFile[] = RETAIL_FILE;
    H2_FREE_AT(m_data, destructionSourceFile, 0x1a);
}

VA(0x004c2820, 0x72)
void icon::DrawToBuffer(
    i32 x, i32 y, i32 frame, H2_ENUM_PARAM(IconDrawOrientation, i32) orientation
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

VA(0x004c28a0, 0x3fc)
IconDrawResult icon::CombatClipDrawToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    struct SLimitData* limits,
    H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
    i32 offset,
    u8* colorTable,
    i8* yModify
) {
    if (gbComputeExtent != 0) {
        IconDrawOrientation mirror = orientation;
        i32 entryOffset = frame * sizeof(IconEntry);
        if (mirror != ICON_DRAW_NORMAL) {
            limits->right =
                x - reinterpret_cast<IconEntry*>(m_data + entryOffset)->x;
            limits->left =
                limits->right - reinterpret_cast<IconEntry*>(m_data + entryOffset)->w + 1;
            limits->top = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y + y;
            limits->bottom =
                reinterpret_cast<IconEntry*>(m_data + entryOffset)->h + limits->top - 1;
        } else {
            limits->left = reinterpret_cast<IconEntry*>(m_data + entryOffset)->x + x;
            limits->right =
                reinterpret_cast<IconEntry*>(m_data + entryOffset)->w + limits->left - 1;
            limits->top = reinterpret_cast<IconEntry*>(m_data + entryOffset)->y + y;
            limits->bottom =
                reinterpret_cast<IconEntry*>(m_data + entryOffset)->h + limits->top - 1;
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
                yModify
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
                yModify
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
                IDX(COLOR_TABLE_APPLY_DIM)
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

VA(0x004c2ca0, 0x40)
void icon::ClipFillToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
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

VA(0x004c2ce0, 0x13d)
void icon::FillToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    H2_ENUM_PARAM(IconDrawOrientation, i32) orientation,
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
        limits->left = Entries()[frame].x + x;
        limits->right = Entries()[frame].w + limits->left - 1;
        limits->top = Entries()[frame].y + y;
        limits->bottom = Entries()[frame].h + limits->top - 1;
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

VA(0x004c2e20, 0x66)
void icon::DimToBuffer(
    i32 x, i32 y, i32 frame, H2_ENUM_PARAM(IconDrawOrientation, i32) orientation
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



#undef RETAIL_FILE
