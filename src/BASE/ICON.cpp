// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICON.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
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

// @early-stop
// Compiler COMDAT-folding artifact: retail has two strong, byte-identical 0x36
// ??_E/??_G sections with 10 relocations each. Keeping the standalone 0x21 ??1
// destructor below exact makes VC4.2 emit a 0x1f ??_G wrapper and only a weak ??_E
// alias with no section. An inline destructor folds BaseFree into the 0x36 deleting
// body but removes the separately mapped retail ??1, so the exact standalone symbol
// is retained rather than traded for weak aliases.
// VA(0x004c7a90, 0x36) ??_E/??_G icon deleting-destructor aliases

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

// @semantic
// /O2 residual begins at +0xb: base is 0x2b3 bytes and retail is 0x2bb. Both
// have the same frame, CFG, and ordered 37-relocation identity stream; the draw
// dispatcher is instruction-identical after aligning retail +0x146 with base
// +0x13e. Only extent construction/rejection differs: retail colors
// flip/index/limits as EAX/EBX/EDI while base uses EBX/EDI/EBX and factors the
// entry-y load after the flip join. Cached/repeated typed entries, shared
// offsets, flip branch orientations, direct/cached values, clipped-dimension
// locals, and reordered predicates were already tried. Branch-produced/local
// top variants regressed to 81.49%/87.73% by changing frame/liveness. The old
// literal-source 80-variant AST pass and a fresh 37-variant pass over the named
// enum source retained nothing. A guarded 40-trial parser-visible TU-state pass
// (seed 0x49434f4e) found no exact closure and retained nothing. Revisit only
// after a material exact predecessor/header-state change; this is not a
// certified permanent wall.
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

// Retained exact-max checkpoint: retail/base are both 0x103 bytes with the same 0x4-byte frame,
// saved registers, CFG, and exact ordered 10-relocation offset/type/identity
// stream by manual COFF audit. Under the final named-orientation enum state,
// the sole relocation-masked raw residual is +0xbf: base emits
// `cmp ebp,edx; jg`, retail `cmp edx,ebp; jl` for the equivalent top/max-Y
// rejection. The same declaration changed the prior min-X compare residual
// into exact bytes without changing the score. Reversing both predicates in
// their respective TU states was byte-neutral; a bounded 13-variant libclang
// AST pass found no gain. Revisit after an exact predecessor or shared-header
// state change.
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

// ===== vtable icon (root)  (1 slots) =====
//  [ 0] VA(0x004c7a90, 0x36)  void * icon::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(icon, 0x004eb9e8);

#undef RETAIL_FILE
