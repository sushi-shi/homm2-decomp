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

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\ICON.CPP"
VA(0x004c7a20, 0x67)
icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, 1, 0) {
    DATA(0x0051e94c) static char allocationSourceFile[] = RETAIL_FILE;
    gpResourceManager->PointToFile(id);
    m_frameCount = gpResourceManager->ReadWord();
    u32 len = gpResourceManager->ReadLong();
    m_data = static_cast<u8*>(H2_ALLOC_AT(len, allocationSourceFile, 18));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), len);
}

// @early-stop: inline continuation artifact.
VA(0x004c7ad0, 0x21)
icon::~icon() {
    DATA(0x0051e974) static char destructionSourceFile[] = RETAIL_FILE;
    H2_FREE_AT(m_data, destructionSourceFile, 0x1a);
}

VA(0x004c7b00, 0x44)
void icon::DrawToBuffer(i32 x, i32 y, i32 frame, i32 flip) {
    if (flip == IDX(ICON_DRAW_NORMAL)) {
        IconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            IDX(ICON_DRAW_NO_CLIP),
            0,
            0,
            ICON_DRAW_SCREEN_WIDTH,
            ICON_DRAW_SCREEN_HEIGHT,
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
        IDX(ICON_DRAW_NO_CLIP),
        0,
        0,
        ICON_DRAW_SCREEN_WIDTH,
        ICON_DRAW_SCREEN_HEIGHT,
        0
    );
}

// @semantic: residual begins at +0xb: base is 0x2b3 bytes and retail is 0x2bb.
VA(0x004c7b50, 0x2bb)
i32 icon::CombatClipDrawToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    struct SLimitData* limits,
    i32 flip,
    i32 offset,
    u8* colorTable,
    i8* yModify
) {
    if (gbComputeExtent != 0) {
        i32 mirror = flip;
        IconEntry* entry = &Entries()[frame];
        if (mirror != 0) {
            limits->right = x - entry->x;
            limits->left = limits->right - entry->w + 1;
        } else {
            limits->left = entry->x + x;
            limits->right = entry->w + limits->left - 1;
        }
        limits->top = entry->y + y;
        limits->bottom = entry->h + limits->top - 1;
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
            return IDX(ICON_DRAW_SKIPPED);
    }

    if (gbLimitToExtent != 0
        && (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX || limits->right < giMinExtentX
            || limits->top > giMaxExtentY || limits->bottom < giMinExtentY))
        return IDX(ICON_DRAW_SKIPPED);

    if (yModify != 0) {
        if (flip == IDX(ICON_DRAW_NORMAL))
            IconToBitmapYModify(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                IDX(ICON_DRAW_CLIP),
                0,
                0,
                ICON_DRAW_SCREEN_WIDTH,
                ICON_DRAW_COMBAT_HEIGHT,
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
                IDX(ICON_DRAW_CLIP),
                0,
                0,
                ICON_DRAW_SCREEN_WIDTH,
                ICON_DRAW_COMBAT_HEIGHT,
                offset,
                yModify
            );
    } else if (colorTable != 0) {
        if (flip == IDX(ICON_DRAW_NORMAL))
            IconToBitmapColorTable(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                IDX(ICON_DRAW_CLIP),
                0,
                0,
                ICON_DRAW_SCREEN_WIDTH,
                ICON_DRAW_COMBAT_HEIGHT,
                offset,
                colorTable,
                IDX(ICON_COLOR_TABLE_APPLY_DIM)
            );
        else
            FlipIconToBitmapColorTable(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                IDX(ICON_DRAW_CLIP),
                0,
                0,
                ICON_DRAW_SCREEN_WIDTH,
                ICON_DRAW_COMBAT_HEIGHT,
                offset,
                colorTable
            );
    } else if (gbLimitToExtent != 0) {
        if (flip == IDX(ICON_DRAW_NORMAL))
            IconToBitmap(
                this,
                gpWindowManager->m_screen,
                x,
                y,
                frame,
                IDX(ICON_DRAW_CLIP),
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
                IDX(ICON_DRAW_CLIP),
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1,
                offset
            );
    } else if (flip == IDX(ICON_DRAW_NORMAL)) {
        IconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            IDX(ICON_DRAW_CLIP),
            0,
            0,
            ICON_DRAW_SCREEN_WIDTH,
            ICON_DRAW_COMBAT_HEIGHT,
            offset
        );
    } else {
        FlipIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            IDX(ICON_DRAW_CLIP),
            0,
            0,
            ICON_DRAW_SCREEN_WIDTH,
            ICON_DRAW_COMBAT_HEIGHT,
            offset
        );
    }
    return IDX(ICON_DRAW_COMPLETED);
}

VA(0x004c7e10, 0x3d)
void icon::ClipFillToBuffer(
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    i32 flip,
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
        IDX(ICON_DRAW_CLIP),
        clipX,
        clipY,
        clipW,
        clipH
    );
}

VA(0x004c7e50, 0x103)
void icon::FillToBuffer(i32 x, i32 y, i32 frame, i32 color, i32 flip, struct SLimitData* limits) {
    if (flip != IDX(ICON_DRAW_NORMAL)) {
        FlipMonoIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            color,
            IDX(ICON_DRAW_NO_CLIP),
            0,
            0,
            0,
            0
        );
        return;
    }
    if (gbLimitToExtent != 0 && limits != 0) {
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
        IDX(ICON_DRAW_NO_CLIP),
        0,
        0,
        0,
        0
    );
}

VA(0x004c7f60, 0x3e)
void icon::DimToBuffer(i32 x, i32 y, i32 frame, i32 flip) {
    if (flip == IDX(ICON_DRAW_NORMAL)) {
        DimIconToBitmap(
            this,
            gpWindowManager->m_screen,
            x,
            y,
            frame,
            IDX(ICON_DRAW_NO_CLIP),
            0,
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
        IDX(ICON_DRAW_NO_CLIP),
        0,
        0,
        0,
        0,
        0
    );
}


VTBL(icon, 0x004eb9e8);

#undef RETAIL_FILE
