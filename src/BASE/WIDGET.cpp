// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WIDGET.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/widget.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>
VA(0x004dde00, 0x5a)
widget::widget(short int x, short int y, short int w, short int h, short int p5, short int kind)
{
    field_0x18 = x;
    field_0x4 = 0;
    field_0x1a = y;
    field_0x8 = 0;
    field_0xc = 0;
    field_0x16 = 6;
    field_0x12 = -1;
    field_0x1c = w;
    field_0x1e = h;
    field_0x10 = p5;
    field_0x14 = kind;
}

VA(0x004dde60, 0x3f)
widget::widget(void)
{
    field_0x10 = 0;
    field_0x4 = 0;
    field_0x8 = 0;
    field_0xc = 0;
    field_0x16 = 6;
    field_0x12 = -1;
    field_0x14 = 2;
    field_0x1a = 0;
    field_0x18 = 0;
    field_0x1c = 0x10;
    field_0x1e = 0x10;
}

VA(0x004ddea0, 0x7)
widget::~widget() {}

VA(0x004ddeb0, 0x14)
int widget::Open(int id, class heroWindow *win)
{
    field_0x12 = id;
    field_0x4 = win;
    return 0;
}

VA(0x004dded0, 0x1)
void widget::Close(void) {}

VA(0x004ddee0, 0x2f4)
int widget::Main(struct tag_message &) { return 0; }

VA(0x004de1e0, 0x47)
void widget::Dim(void)
{
    if (field_0x14 != 8 && field_0x14 != 0x200) {
        DimBitmapArea(gpWindowManager->field_0x46,
                      static_cast<short>(field_0x4->field_0x28 + field_0x18),
                      static_cast<short>(field_0x1a + field_0x4->field_0x2c),
                      field_0x1c, field_0x1e, 0);
    }
}


// ===== vtable widget (root)  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(widget, 0x004ebb00);
