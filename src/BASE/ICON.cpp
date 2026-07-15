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
VA(0x004c7a20, 0x67)
icon::icon(unsigned long int id) : resource(1, id, 1, 0)
{
    gpResourceManager->PointToFile(id);
    m_frameCount = gpResourceManager->ReadWord();
    unsigned int len = gpResourceManager->ReadLong();
    m_data = static_cast<char *>(
        H2_ALLOC(len, "I:\\Projects\\Heroes\\Prog\\BASE\\ICON.CPP", 0x12));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), len);
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
icon::~icon()
{
    H2_FREE(m_data, "I:\\Projects\\Heroes\\Prog\\BASE\\ICON.CPP", 0x1a);
}

VA(0x004c7b00, 0x44)
void icon::DrawToBuffer(int x, int y, int frame, int flip)
{
    if (flip == ICON_DRAW_NORMAL) {
        IconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                     ICON_DRAW_NO_CLIP, 0, 0, ICON_DRAW_SCREEN_WIDTH,
                     ICON_DRAW_SCREEN_HEIGHT, 0);
        return;
    }
    FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                     ICON_DRAW_NO_CLIP, 0, 0, ICON_DRAW_SCREEN_WIDTH,
                     ICON_DRAW_SCREEN_HEIGHT, 0);
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
int icon::CombatClipDrawToBuffer(int x, int y, int frame, struct SLimitData *limits,
                                 int flip, int offset, unsigned char *colorTable,
                                 signed char *yModify)
{
    if (gbComputeExtent != 0) {
        int mirror = flip;
        int entryOffset = frame * sizeof(IconEntry);
        if (mirror != 0) {
            limits->right = x -
                            reinterpret_cast<IconEntry *>(m_data + entryOffset)->x;
            limits->left = limits->right -
                           reinterpret_cast<IconEntry *>(m_data + entryOffset)->w + 1;
        } else {
            limits->left = reinterpret_cast<IconEntry *>(m_data + entryOffset)->x + x;
            limits->right = reinterpret_cast<IconEntry *>(m_data + entryOffset)->w +
                            limits->left - 1;
        }
        limits->top = reinterpret_cast<IconEntry *>(m_data + entryOffset)->y + y;
        limits->bottom = reinterpret_cast<IconEntry *>(m_data + entryOffset)->h +
                         limits->top - 1;
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

    if (gbLimitToExtent != 0 &&
        (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX ||
         limits->right < giMinExtentX || limits->top > giMaxExtentY ||
         limits->bottom < giMinExtentY))
        return ICON_DRAW_SKIPPED;

    if (yModify != 0) {
        if (flip == ICON_DRAW_NORMAL)
            IconToBitmapYModify(this, gpWindowManager->m_screen, x, y, frame,
                                ICON_DRAW_CLIP, 0, 0, ICON_DRAW_SCREEN_WIDTH,
                                ICON_DRAW_COMBAT_HEIGHT, offset, yModify);
        else
            FlipIconToBitmapYModify(this, gpWindowManager->m_screen, x, y,
                                    frame, ICON_DRAW_CLIP, 0, 0,
                                    ICON_DRAW_SCREEN_WIDTH,
                                    ICON_DRAW_COMBAT_HEIGHT, offset, yModify);
    } else if (colorTable != 0) {
        if (flip == ICON_DRAW_NORMAL)
            IconToBitmapColorTable(this, gpWindowManager->m_screen, x, y,
                                   frame, ICON_DRAW_CLIP, 0, 0,
                                   ICON_DRAW_SCREEN_WIDTH,
                                   ICON_DRAW_COMBAT_HEIGHT, offset, colorTable,
                                   ICON_COLOR_TABLE_APPLY_DIM);
        else
            FlipIconToBitmapColorTable(this, gpWindowManager->m_screen, x, y,
                                       frame, ICON_DRAW_CLIP, 0, 0,
                                       ICON_DRAW_SCREEN_WIDTH,
                                       ICON_DRAW_COMBAT_HEIGHT, offset,
                                       colorTable);
    } else if (gbLimitToExtent != 0) {
        if (flip == ICON_DRAW_NORMAL)
            IconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                         ICON_DRAW_CLIP,
                         giMinExtentX, giMinExtentY,
                         giMaxExtentX - giMinExtentX + 1,
                         giMaxExtentY - giMinExtentY + 1, offset);
        else
            FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                             ICON_DRAW_CLIP,
                             giMinExtentX, giMinExtentY,
                             giMaxExtentX - giMinExtentX + 1,
                             giMaxExtentY - giMinExtentY + 1, offset);
    } else if (flip == ICON_DRAW_NORMAL) {
        IconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                     ICON_DRAW_CLIP, 0, 0, ICON_DRAW_SCREEN_WIDTH,
                     ICON_DRAW_COMBAT_HEIGHT, offset);
    } else {
        FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                         ICON_DRAW_CLIP, 0, 0, ICON_DRAW_SCREEN_WIDTH,
                         ICON_DRAW_COMBAT_HEIGHT, offset);
    }
    return ICON_DRAW_COMPLETED;
}

VA(0x004c7e10, 0x3d)
void icon::ClipFillToBuffer(int x, int y, int frame, int color, int flip,
                            int clipX, int clipY, int clipW, int clipH)
{
    MonoIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, color,
                     ICON_DRAW_CLIP, clipX, clipY, clipW, clipH);
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
void icon::FillToBuffer(int x, int y, int frame, int color, int flip,
                        struct SLimitData *limits)
{
    if (flip != ICON_DRAW_NORMAL) {
        FlipMonoIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, color,
                             ICON_DRAW_NO_CLIP, 0, 0, 0, 0);
        return;
    }
    if (gbLimitToExtent != 0 && limits != 0) {
        limits->left = reinterpret_cast<IconEntry *>(m_data)[frame].x + x;
        limits->right = reinterpret_cast<IconEntry *>(m_data)[frame].w +
                        limits->left - 1;
        limits->top = reinterpret_cast<IconEntry *>(m_data)[frame].y + y;
        limits->bottom = reinterpret_cast<IconEntry *>(m_data)[frame].h +
                         limits->top - 1;
        if (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX ||
            limits->right < giMinExtentX || limits->top > giMaxExtentY ||
            limits->bottom < giMinExtentY)
            return;
    }
    MonoIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, color,
                     ICON_DRAW_NO_CLIP, 0, 0, 0, 0);
}

VA(0x004c7f60, 0x3e)
void icon::DimToBuffer(int x, int y, int frame, int flip)
{
    if (flip == ICON_DRAW_NORMAL) {
        DimIconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                        ICON_DRAW_NO_CLIP, 0, 0, 0, 0, 0);
        return;
    }
    FlipDimIconToBitmap(this, gpWindowManager->m_screen, x, y, frame,
                        ICON_DRAW_NO_CLIP, 0, 0, 0, 0, 0);
}


// ===== vtable icon (root)  (1 slots) =====
//  [ 0] VA(0x004c7a90, 0x36)  void * icon::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(icon, 0x004eb9e8);
