// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICON.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon.h>
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
    if (flip == 0) {
        IconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 0, 0, 0, 0x280, 0x1e0, 0);
        return;
    }
    FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 0, 0, 0, 0x280, 0x1e0, 0);
}

// @match-note
// /O2 residual begins at +0xb: base is 0x2b3 bytes and retail is 0x2bb. Both have the same
// CFG and ordered 37-relocation identity stream; the draw dispatcher is instruction-identical
// after aligning retail +0x146 with base +0x13e. Only extent construction/rejection differs:
// retail colors flip/index/limits as EAX/EBX/EDI while base uses EBX/EDI/EBX and factors the
// entry-y load after the flip join. Cached/repeated typed entries, shared offsets, flip branch
// orientations, direct/cached values, clipped-dimension locals, and reordered predicates were
// already tried. Branch-produced/local top variants regressed to 81.49%/87.73% by changing
// frame/liveness; an audited 80-attempt libclang AST pass retained nothing. Revisit after a
// material exact predecessor/header-state change; this is not a certified permanent wall.
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
            return 0;
    }

    if (gbLimitToExtent != 0 &&
        (gbCurrArmyDrawn == 0 || limits->left > giMaxExtentX ||
         limits->right < giMinExtentX || limits->top > giMaxExtentY ||
         limits->bottom < giMinExtentY))
        return 0;

    if (yModify != 0) {
        if (flip == 0)
            IconToBitmapYModify(this, gpWindowManager->m_screen, x, y, frame, 1,
                                0, 0, 0x280, 0x1bc, offset, yModify);
        else
            FlipIconToBitmapYModify(this, gpWindowManager->m_screen, x, y, frame, 1,
                                    0, 0, 0x280, 0x1bc, offset, yModify);
    } else if (colorTable != 0) {
        if (flip == 0)
            IconToBitmapColorTable(this, gpWindowManager->m_screen, x, y, frame, 1,
                                   0, 0, 0x280, 0x1bc, offset, colorTable, 1);
        else
            FlipIconToBitmapColorTable(this, gpWindowManager->m_screen, x, y, frame, 1,
                                       0, 0, 0x280, 0x1bc, offset, colorTable);
    } else if (gbLimitToExtent != 0) {
        if (flip == 0)
            IconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 1,
                         giMinExtentX, giMinExtentY,
                         giMaxExtentX - giMinExtentX + 1,
                         giMaxExtentY - giMinExtentY + 1, offset);
        else
            FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 1,
                             giMinExtentX, giMinExtentY,
                             giMaxExtentX - giMinExtentX + 1,
                             giMaxExtentY - giMinExtentY + 1, offset);
    } else if (flip == 0) {
        IconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 1,
                     0, 0, 0x280, 0x1bc, offset);
    } else {
        FlipIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, 1,
                         0, 0, 0x280, 0x1bc, offset);
    }
    return 1;
}

VA(0x004c7e10, 0x3d)
void icon::ClipFillToBuffer(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
{
    MonoIconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, p4, 1, p6, p7, p8, p9);
}

VA(0x004c7e50, 0x103)
void icon::FillToBuffer(int x, int y, int frame, int color, int flip,
                        struct SLimitData *limits)
{
    if (flip != 0) {
        FlipMonoIconToBitmap(this, gpWindowManager->m_screen, x, y, frame, color,
                             0, 0, 0, 0, 0);
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
                     0, 0, 0, 0, 0);
}

VA(0x004c7f60, 0x3e)
void icon::DimToBuffer(int p1, int p2, int p3, int p4)
{
    if (p4 == 0) {
        DimIconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, 0, 0, 0, 0, 0, 0);
        return;
    }
    FlipDimIconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, 0, 0, 0, 0, 0, 0);
}


// ===== vtable icon (root)  (1 slots) =====
//  [ 0] VA(0x004c7a90, 0x36)  void * icon::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(icon, 0x004eb9e8);
