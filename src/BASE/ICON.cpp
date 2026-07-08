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
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
VA(0x004c7a20, 0x67)
icon::icon(unsigned long int id) : resource(1, id, 1, 0)
{
    gpResourceManager->PointToFile(id);
    field_0x10 = gpResourceManager->ReadWord();
    unsigned int len = gpResourceManager->ReadLong();
    field_0x12 = static_cast<char *>(BaseAlloc(len, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(field_0x12), len);
}

VA(0x004c7ad0, 0x21)
icon::~icon()
{
    BaseFree(field_0x12, __FILE__, __LINE__);
}

VA(0x004c7b00, 0x44)
void icon::DrawToBuffer(int p1, int p2, int p3, int p4)
{
    if (p4 == 0) {
        IconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, 0, 0, 0, 0x280, 0x1e0, 0);
        return;
    }
    FlipIconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, 0, 0, 0, 0x280, 0x1e0, 0);
}

VA(0x004c7b50, 0x2bb)
int icon::CombatClipDrawToBuffer(int, int, int, struct SLimitData *, int, int, unsigned char *, signed char *) { return 0; }

VA(0x004c7e10, 0x3d)
void icon::ClipFillToBuffer(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
{
    MonoIconToBitmap(this, gpWindowManager->m_screen, p1, p2, p3, p4, 1, p6, p7, p8, p9);
}

VA(0x004c7e50, 0x103)
void icon::FillToBuffer(int, int, int, int, int, struct SLimitData *) {}

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
