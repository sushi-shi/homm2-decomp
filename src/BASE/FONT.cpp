// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\FONT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <SOURCE/KB.h>

VA(0x004c6fd0, 0xc8)
font::font(unsigned long id) : resource(5, id, 1, 0)
{
    gpResourceManager->PointToFile(id);
    field_0x10 = gpResourceManager->ReadWord();
    int h = gpResourceManager->ReadWord();
    if (field_0x10 >= 14)
        field_0x14 = 1;
    else
        field_0x14 = 0;
    char fname[13];
    // Read13 takes signed char*, GetIcon char* — one filename buffer feeds both, so bridge the
    // API signedness once (codegen-identical: both are byte pointers).
    gpResourceManager->Read13((signed char *)fname);
    gbLoadingMonoIcon = 1;
    field_0x1c = gpResourceManager->GetIcon(fname);
    gbLoadingMonoIcon = 0;
}

VA(0x004c70e0, 0x39)
font::~font()
{
    gpResourceManager->Dispose(field_0x1c);
}

VA(0x004c7120, 0x24a)
void font::DrawStringExecute(char *, int, int, int, int, int, int, int) {}

VA(0x004c7370, 0x48)
void font::DrawString(char *s, int x, int y, int c)
{
    field_0x18 = 0;
    DrawStringExecute(s, x, y, c, 0, 0, 0x280, 0x1e0);
}

VA(0x004c73c0, 0xaf)
int font::GetCharacterWidth(unsigned char c)
{
    if (c == '{' || c == '}') {
        return 0;
    } else {
        if (c == ' ')
            c = 'i';
        if (c == 0x1f)
            c = '_';
        c -= ' ';
        if (c < 0 || c > 0x5f)
            c = 0x5f;
        // stride-13 char-metrics table in the glyph icon; width is the short at +4.
        return *(short *)(field_0x1c->field_0x12 + c * 13 + 4) + field_0x14;
    }
}

VA(0x004c7470, 0x313)
void font::DrawBoundedString(char *, int, int, int, int, int, int) {}

VA(0x004c7790, 0x1b3)
int font::LineLength(char *, int) { return 0; }

VA(0x004c7950, 0xc4)
int font::LineWidth(char *s)
{
    int len = strlen(s);
    int a = 0, b = 0, cc = 0, d = 0;   // zeroed alongside the counters (vestigial, cf. LineLength)
    int i = 0;
    int width = 0;
    char *p = s;
    while (i < len && p[i] != 0) {
        while (p[i] != 0 && p[i] != '\n') {
            width += GetCharacterWidth(p[i]);
            i++;
        }
    }
    return width;
}


// ===== vtable font (root)  (1 slots) =====
//  [ 0] VA(0x004c70a0, 0x39)  void * font::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(font, 0x004eb9e4);
