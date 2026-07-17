// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\FONT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>

VA(0x004c6fd0, 0xc8)
font::font(u32l id) : resource(5, id, 1, 0) {
    gpResourceManager->PointToFile(id);
    m_height = gpResourceManager->ReadWord();
    i32 h = gpResourceManager->ReadWord();
    if (m_height >= 14)
        m_isLarge = 1;
    else
        m_isLarge = 0;
    char fname[13];
    // Read13 takes signed char*, GetIcon char* — one filename buffer feeds both, so bridge the
    // API signedness once (codegen-identical: both are byte pointers).
    gpResourceManager->Read13(reinterpret_cast<i8*>(fname));
    gbLoadingMonoIcon = 1;
    m_glyphIcon = gpResourceManager->GetIcon(fname);
    gbLoadingMonoIcon = 0;
}

VA(0x004c70e0, 0x39)
font::~font() {
    gpResourceManager->Dispose(m_glyphIcon);
}
// The generated 0x39-byte ??_G COMDAT is raw-identical to both delinked retail ??_E copies;
// its two relocations target ~font and operator delete. MSVC emits ??_E as a weak ??_G alias,
// and the vtable relocation resolves through that alias, so the duplicate report rows are unscored.

VA(0x004c7120, 0x24a)
void font::DrawStringExecute(
    char* str,
    i32 x,
    i32 y,
    i32 mode,
    i32 clipL,
    i32 clipT,
    i32 clipR,
    i32 clipB
) {
    char c = 0;
    i32 pos = x;
    i32 i = 0;
    while (str[i] != 0) {
        c = str[i];
        if (c == FONT_SPACER_CHAR) {
            pos += GetCharacterWidth(static_cast<u8>(str[i]));
            goto next;
        }
        if (c == '{') {
            m_suppressDraw = 1;
            goto next;
        }
        if (c == '}') {
            m_suppressDraw = 0;
            goto next;
        }
        c -= 0x20;
        if (c < 0 || c > FONT_GLYPH_FALLBACK)
            c = FONT_GLYPH_FALLBACK;
        if (c != 0) {
            if (mode == 1 && m_suppressDraw == 0)
                IconToBitmap(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    c,
                    1,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0
                );
            else if (mode == 2 || (mode == 1 && m_suppressDraw != 0))
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    c,
                    1,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableYellow,
                    1
                );
            else if (mode == 4)
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    c,
                    1,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableScenWin,
                    0
                );
            else
                IconToBitmapColorTable(
                    m_glyphIcon,
                    gpWindowManager->m_screen,
                    pos,
                    y,
                    c,
                    1,
                    clipL,
                    clipT,
                    clipR,
                    clipB,
                    0,
                    gColorTableDarkGray,
                    1
                );
        }
        pos += GetCharacterWidth(static_cast<u8>(str[i]));
    next:
        i++;
    }
}

VA(0x004c7370, 0x48)
void font::DrawString(char* s, i32 x, i32 y, i32 mode) {
    m_suppressDraw = 0;
    DrawStringExecute(s, x, y, mode, 0, 0, 0x280, 0x1e0);
}

// @semantic
// /Od residual is +0x82..+0x98: retail loads m_glyphIcon->m_data into EAX before forming
// the c*13 index in ECX; ours forms the index first and loads the base into ECX. The 0xaf-byte
// function, EBP frame/this slot, CFG, and zero-relocation stream otherwise agree. This body
// reached 100% in 5491423 before the required icon.h SLimitData layout correction.
// Ordinary/commuted subscripts and raw byte-offset spelling were byte-neutral; exact predecessor
// cast cleanups and the DrawString mode rename did not move it. Revisit after material FONT/icon
// header state changes; current 86.93% is below the audited AST-permuter threshold.
VA(0x004c73c0, 0xaf)
i32 font::GetCharacterWidth(u8 c) {
    if (c == '{' || c == '}') {
        return 0;
    } else {
        if (c == ' ')
            c = 'i';
        if (c == FONT_SPACER_CHAR)
            c = '_';
        c -= ' ';
        if (c < 0 || c > FONT_GLYPH_FALLBACK)
            c = FONT_GLYPH_FALLBACK;
        return m_glyphIcon->Entries()[c].w + m_isLarge;
    }
}

VA(0x004c7470, 0x313)
void font::DrawBoundedString(char* str, i32 x, i32 y, i32 w, i32 h, i32 mode, i32 align) {
    // Semantic suffixes preserve the retail /Od slot buckets. glyphPos, space9, and v1 are
    // vestigial locals present in the retail frame.
    i32 size = strlen(str);
    u8* glyphPos = m_glyphIcon->m_data;
    char space9 = ' ';
    i32 xPosition = 0;
    i32 yOffC = 0;
    char savedChar;
    i32 lineStartD = 0;
    i32 lineEnd1 = 0;
    i32 v1;
    i32 idx = 0;
    i32 lineWidth3 = 0;
    i32 wordBreak0 = 0;
    char* text2 = str;
    i32 drawMode2 = mode;
    if (align & 4) {
        align -= 4;
        i32 lineCount = LineLength(str, w);
        i32 totalH = m_height * lineCount;
        if (totalH < h)
            yOffC = (h - totalH) / 2;
    }
    m_suppressDraw = 0;
    while (size > idx && text2[idx] != 0 && (m_height + yOffC <= h || yOffC == 0)) {
        while (text2[idx] != 0 && text2[idx] != '\n' && lineWidth3 <= w) {
            lineWidth3 += GetCharacterWidth(text2[idx]);
            idx++;
        }
        i32 savedWidth = lineWidth3;
        if (w < lineWidth3) {
            idx--;
            wordBreak0 = 0;
            // The SIB-equivalent lvalue makes VC4.2 load the other comparison operand first.
            while (text2[idx] != ' ' && OD_STEER(idx) >= lineStartD) {
                lineWidth3 -= GetCharacterWidth(text2[idx]);
                if (m_height * 2 + yOffC > h && lineWidth3 < w)
                    break;
                if (wordBreak0 == 0 && lineWidth3 < w)
                    wordBreak0 = idx;
                idx--;
            }
            if (OD_STEER(idx) <= lineStartD) {
                idx = wordBreak0;
                lineWidth3 = savedWidth;
            }
            if (text2[idx] == ' ')
                lineWidth3 -= GetCharacterWidth(text2[idx]);
        }
        lineEnd1 = idx;
        savedChar = text2[lineEnd1];
        text2[lineEnd1] = 0;
        switch (align) {
            case 0:
                xPosition = 0;
                break;
            case 1:
                xPosition = (w - lineWidth3) / 2;
                break;
            case 2:
                xPosition = w - lineWidth3;
                break;
        }
        DrawStringExecute(text2 + lineStartD, x + xPosition, y + yOffC, drawMode2, x, y, w, h);
        text2[lineEnd1] = savedChar;
        yOffC += m_height;
        lineStartD = lineEnd1 + 1;
        idx = lineStartD;
        lineWidth3 = 0;
    }
}

VA(0x004c7790, 0x1b3)
i32 font::LineLength(char* str, i32 maxW) {
    // Word-wrap: count how many lines `str` needs at width maxW, breaking at spaces.
    // 13 locals (frame 0x38); names carry the /Od slot hashes (od_slots). q/v declared-unused;
    // aa/t/u are set-or-zeroed-but-unused (vestigial). Init order matches the retail.
    i32 s = strlen(str); // len   @ -0x10
    char aa = ' ';       //       @ -0x30 (vestigial)
    i32 z = 0, t = 0, r = 0, y = 0, p = 0, u = 0, x = 0, gap = 0;
    // z=lineCount(-0x2c) t=vestigial(-0x14) r=wordStart(-0xc) y=(-0x28) p=i(-0x4)
    // u=vestigial(-0x18) x=width(-0x24) gap=breakPt(-0x34)
    char* w = str; // ptr   @ -0x20
    i32 q, v;      // unused @ -0x8, -0x1c
    while (p < s && w[p] != 0) {
        while (w[p] != 0 && w[p] != '\n' && OD_STEER(x) <= maxW) {
            x += GetCharacterWidth(w[p]);
            p++;
        }
        if (OD_STEER(x) > maxW) {
            p--;
            gap = 0;
            while (w[p] != ' ' && p >= r) {
                x -= GetCharacterWidth(w[p]);
                if (gap == 0 && OD_STEER(x) < maxW)
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
i32 font::LineWidth(char* str) {
    // 10 locals (frame 0x2c): local names carry the /Od slot hashes (od_slots); `q`/`u` are
    // declared-but-unused (reserved, not zeroed) — vestigial, as in the sibling string routines.
    i32 s = strlen(str); // len @ -0x10
    i32 q, u;            // unused @ -0x8, -0x18
    i32 y = 0, t = 0, r = 0, x = 0, p = 0,
        w = 0;     // zeroed in this order: -0x28,-0x14,-0xc,-0x24,-0x4(i),-0x20(width)
    char* v = str; // @ -0x1c
    while (OD_STEER(p) < s && v[p] != 0) {
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
