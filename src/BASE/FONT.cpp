// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\FONT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/heroWindowManager.h>
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
void font::DrawStringExecute(char *str, int x, int y, int mode,
                            int clipL, int clipT, int clipR, int clipB)
{
    char c = 0;
    int pos = x;
    int i = 0;
    while (str[i] != 0) {
        c = str[i];
        if (c == 0x1f) {
            pos += GetCharacterWidth(str[i]);
            goto next;
        } else if (c == '{') {
            field_0x18 = 1;
            goto next;
        } else if (c == '}') {
            field_0x18 = 0;
            goto next;
        } else {
            c -= 0x20;
            if (c < 0 || c > 0x5f)
                c = 0x5f;
            if (c != 0) {
                if (mode == 1 && field_0x18 == 0)
                    IconToBitmap(field_0x1c, gpWindowManager->field_0x46, pos, y, c, 1,
                                 clipL, clipT, clipR, clipB, 0);
                else if (mode == 2 || (mode == 1 && field_0x18 != 0))
                    IconToBitmapColorTable(field_0x1c, gpWindowManager->field_0x46, pos, y, c, 1,
                                           clipL, clipT, clipR, clipB, 0, gColorTableYellow, 1);
                else if (mode == 4)
                    IconToBitmapColorTable(field_0x1c, gpWindowManager->field_0x46, pos, y, c, 1,
                                           clipL, clipT, clipR, clipB, 0, gColorTableScenWin, 0);
                else
                    IconToBitmapColorTable(field_0x1c, gpWindowManager->field_0x46, pos, y, c, 1,
                                           clipL, clipT, clipR, clipB, 0, gColorTableDarkGray, 1);
            }
            pos += GetCharacterWidth(str[i]);
        }
    next:
        i++;
    }
}

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
void font::DrawBoundedString(char *str, int x, int y, int w, int h, int mode, int align)
{
    // Locals declared at function scope (period C89 style): the retail's /Od frame (0x4c) spills
    // all of them up-front, incl. two write-once vestigials (glyph pointer, a ' ') and local copies
    // of str/align — the same reserved-local pattern as LineWidth/LineLength.
    int len = strlen(str);
    char *glyph = field_0x1c->field_0x12;
    char sp = ' ';
    int xOff = 0;
    int yOff = 0;
    int lineStart = 0;
    int v1 = 0;
    int i = 0;
    int lineWidth = 0;
    int v2 = 0;
    char *s = str;
    int a = align;
    int breakPt;
    if (a & 4) {
        a -= 4;
        int lines = LineLength(s, w);
        if (field_0x10 * lines < h)
            yOff = (h - field_0x10 * lines) / 2;
    }
    field_0x18 = 0;
    while (1) {
        lineStart = i;
        lineWidth = 0;
        if (len <= i || s[i] == 0 || (h < field_0x10 + yOff && yOff != 0))
            return;
        while (s[i] != 0 && s[i] != '\n' && lineWidth <= w) {
            lineWidth += GetCharacterWidth(s[i]);
            i++;
        }
        if (w < lineWidth) {
            breakPt = 0;
            while (i--, s[i] != ' ' && lineStart <= i) {
                lineWidth -= GetCharacterWidth(s[i]);
                if (h < field_0x10 * 2 + yOff && lineWidth < w)
                    break;
                if (breakPt == 0 && lineWidth < w)
                    breakPt = i;
            }
            if (i <= lineStart)
                i = breakPt;
            if (s[i] == ' ')
                lineWidth -= GetCharacterWidth(s[i]);
        }
        char saved = s[i];
        s[i] = 0;
        if (a == 0)
            xOff = 0;
        else if (a == 1)
            xOff = (w - lineWidth) / 2;
        else if (a == 2)
            xOff = w - lineWidth;
        DrawStringExecute(s + lineStart, xOff + x, yOff + y, mode, x, y, w, h);
        s[i] = saved;
        yOff += field_0x10;
        i++;
    }
}

VA(0x004c7790, 0x1b3)
int font::LineLength(char *str, int maxW)
{
    // Word-wrap: count how many lines `str` needs at width maxW, breaking at spaces.
    // 13 locals (frame 0x38); names carry the /Od slot hashes (od_slots). q/v declared-unused;
    // aa/t/u are set-or-zeroed-but-unused (vestigial). Init order matches the retail.
    int s = strlen(str);                 // len   @ -0x10
    char aa = ' ';                       //       @ -0x30 (vestigial)
    int z = 0, t = 0, r = 0, y = 0, p = 0, u = 0, x = 0, gap = 0;
    // z=lineCount(-0x2c) t=vestigial(-0x14) r=wordStart(-0xc) y=(-0x28) p=i(-0x4)
    // u=vestigial(-0x18) x=width(-0x24) gap=breakPt(-0x34)
    char *w = str;                       // ptr   @ -0x20
    int q, v;                            // unused @ -0x8, -0x1c
    while (p < s && w[p] != 0) {
        while (w[p] != 0 && w[p] != '\n' && x <= maxW) {
            x += GetCharacterWidth(w[p]);
            p++;
        }
        if (x > maxW) {
            p--;
            gap = 0;
            while (w[p] != ' ' && p >= r) {
                x -= GetCharacterWidth(w[p]);
                if (gap == 0 && x < maxW)
                    gap = p;
                p--;
            }
            if (p <= r)
                p = gap;
            if (w[p] == ' ')
                x -= GetCharacterWidth(w[p]);
        }
        y = p;
        z++;
        r = y + 1;
        p = r;
        x = 0;
    }
    return z;
}

VA(0x004c7950, 0xc4)
int font::LineWidth(char *str)
{
    // 10 locals (frame 0x2c): local names carry the /Od slot hashes (od_slots); `q`/`u` are
    // declared-but-unused (reserved, not zeroed) — vestigial, as in the sibling string routines.
    int s = strlen(str);                        // len @ -0x10
    int q, u;                                   // unused @ -0x8, -0x18
    int y = 0, t = 0, r = 0, x = 0, p = 0, w = 0;  // zeroed in this order: -0x28,-0x14,-0xc,-0x24,-0x4(i),-0x20(width)
    char *v = str;                              // @ -0x1c
    while (p < s && v[p] != 0) {
        while (v[p] != 0 && v[p] != '\n') {
            w += GetCharacterWidth(v[p]);
            p++;
        }
    }
    return w;
}


// ===== vtable font (root)  (1 slots) =====
//  [ 0] VA(0x004c70a0, 0x39)  void * font::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(font, 0x004eb9e4);
