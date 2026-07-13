// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Textntry.obj   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textEntryWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <string.h>
VA(0x004d8740, 0x2d)
textEntryWidget::textEntryWidget(void) : textWidget()
{
    field_0x31 = 0;
    m_icon = 0;
    field_0x14 = 0x4000;
    m_maxLength = 0;
    field_0x2f = 0;
    field_0x4b = 0;
}

// @early-stop
// Compiler folding artifact: keeping the standalone destructor exact makes VC4.2 emit a
// 0x1f ??_G wrapper that calls it, while each retail ??_E body is 0x36 and folds the same
// destructor operations inline. Marking the declaration inline reproduced the 0x36 body
// and 5/5 targets byte-for-byte but removed the separately mapped exact ??1 destructor,
// so the ABI-correct non-inline declaration is retained and the weak alias is parked.
// VA(0x004d8770, 0x36) ??_E/??_G textEntryWidget deleting destructor aliases

// @early-stop
// /O2 scheduling wall, base/retail sections both 0x134 with the same 6/6 relocation
// targets. Only function-relative 0x62..0xbd differs: retail reloads m_x before storing
// m_icon and schedules the constant field stores after m_rectX/m_rectY/m_rectW, while
// ours retains p1 in cx and advances the constants. Exhausted assignment reordering,
// early/late m_rectX, chained/separate stores, short/int temporaries, and unsigned p5.
VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(short p1, short p2, short p3, short p4, short p5, char *p6,
                                 char *p7, short p8, char *p9, short p10, short p11, short p12,
                                 short p13, int p14, int p15)
    : textWidget(p1, p2, p3, p4, p6, p7, p8, p11, p12, 1)
{
    m_maxLength = p5;
    field_0x31 = 0;
    m_icon = gpResourceManager->GetIcon(p9);
    m_rectX = m_x;
    field_0x4b = 0;
    field_0x14 = 0x4000;
    field_0x2f = p10;
    field_0x45 = 1;
    m_hasInset = 0;
    m_color = 1;
    m_rectY = m_y;
    m_rectW = m_width;
    m_maxLength = p5;
    m_rectH = m_height;
#line 62
    m_text = static_cast<char *>(
        BaseAlloc(static_cast<unsigned short>(p5) + 5, __FILE__, __LINE__));
    strcpy(m_text, p6);
    if (p13 == 4) {
        m_hasInset = 1;
        m_innerX = m_x + p14;
        m_innerY = m_y + p15;
        m_innerW = m_width + p14 * -2;
        m_innerH = m_height;
    }
}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget()
{
    gpResourceManager->Dispose(m_icon);
}

// @early-stop
// /O2 scheduling wall: all calls and 52/52 relocation targets agree. Base is 0x26b
// versus retail 0x26c (one trailing pad byte); only base 0x17b..0x1b2 versus retail
// 0x17b..0x1b3 differs, where retail delays the two default-one stores until after the
// four rectangle copies. Exhausted chained/separate stores, short/int flag temporaries,
// branch inversion, and moving the constants before/after the rectangle assignments.
VA(0x004d8920, 0x26c)
void textEntryWidget::Read(int type)
{
    char name[13];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_maxLength = gpResourceManager->ReadWord();
#line 99
    m_text = static_cast<char *>(BaseAlloc(m_maxLength + 5, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_text), m_maxLength);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(name);
    gpResourceManager->RestorePosition();
    m_color = gpResourceManager->ReadWord() & 0xff;
    field_0x2a = static_cast<char>(gpResourceManager->ReadWord());
    gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(name);
    gpResourceManager->RestorePosition();
    field_0x49 = static_cast<short>(type);
    if (type == 2) {
        m_rectX = gpResourceManager->ReadWord();
        m_rectY = gpResourceManager->ReadWord();
        m_rectW = gpResourceManager->ReadWord();
        m_rectH = gpResourceManager->ReadWord();
        field_0x45 = gpResourceManager->ReadWord();
        m_hasInset = gpResourceManager->ReadWord();
    } else {
        m_rectX = m_x;
        m_rectY = m_y;
        m_rectW = m_width;
        m_rectH = m_height;
        short enabled = 1;
        field_0x45 = enabled;
        m_hasInset = enabled;
        if (type != 3)
            m_hasInset = 0;
    }
    if (type == 4) {
        m_innerX = m_x + 7;
        m_innerY = m_y + 5;
        m_innerW = m_width - 14;
        m_innerH = m_height;
    } else if (type == 5) {
        m_innerX = m_x + 7;
        m_innerY = m_y + 4;
        m_innerW = m_width - 14;
        m_innerH = m_height;
    } else {
        m_innerX = m_x;
        m_innerY = m_y;
        m_innerW = m_width;
        m_innerH = m_height;
    }
    field_0x2f = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    field_0x14 = 0x4000;
}

// @early-stop
// /O2 control-flow/regalloc wall: base is 0x82c versus retail 0x874. Both use the exact
// 0x9a8 frame (four independent 600-byte buffers plus tag_message) and have 55 relocs:
// the same ordered 35 external callees/globals and 20 local switch/jump-table entries.
// Differing regions are the default-dispatch/hit-test schedule at 0x2d..0x134, short
// screen-coordinate evaluation at 0x1aa..0x214, and switch layout from 0x281 through the
// local tables at 0x794..0x82b (retail 0x296..0x873). Exhausted nested if, else-if,
// guard-return and switch dispatches; positive/rejection hit tests; int versus short
// screen locals; three/four-buffer layouts; case reorderings; and redundant pre-guards.
VA(0x004d8b90, 0x874)
int textEntryWidget::Main(struct tag_message &message)
{
    if ((m_flags & 2) == 0) {
        if (message.type != 0x200)
            return 0;
    } else {
        switch (message.type) {
        case 8:
        case 0x20: {
            field_0x4d = 1;
            short windowX = static_cast<short>(m_owner->m_posX);
            short mouseX = static_cast<short>(message.field4 - windowX);
            short windowY = static_cast<short>(m_owner->m_posY);
            short mouseY = static_cast<short>(message.field8 - windowY);
            if (message.type == 0x20) {
                if (mouseX < m_x || mouseY < m_y || mouseX >= m_x + m_width ||
                    mouseY >= m_y + m_height)
                    return 0;
                message.field4 = 0xe;
                message.type = 0x200;
                message.fieldC = 0x200;
                message.field8 = m_id;
                return 2;
            }
            if (mouseX >= m_x && mouseY >= m_y && mouseX < m_x + m_width &&
                mouseY < m_y + m_height) {
                char original[600];
                char edit[600];
                char scratch[600];
                char backup[600];
                strcpy(original, m_text);
                if ((m_hasInset & 1) != 0) {
                    field_0x31 = static_cast<short>(strlen(m_text));
                } else {
                    field_0x31 = 0;
                    m_text[0] = 0;
                }
                strcpy(edit, m_text);
                SetupDisplayString(edit, field_0x31);
                Draw();
                short screenX = m_x + windowX;
                short screenY = m_y + windowY;
                gpWindowManager->UpdateScreenRegion(screenX, screenY, m_width, m_height);
                short done = 0;
                glTimers[0] = KBTickCount() + 0x168;
                gpMouseManager->ReallyHidePointer();
                do {
                    if (KBTickCount() > glTimers[0]) {
                        SetupDisplayString(edit, field_0x31);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(screenX, screenY, m_width, m_height);
                    }
                    Process1WindowsMessage();
                    tag_message event = gpInputManager->GetEvent();
                    if (event.type == 1) {
                        switch (event.field4) {
                        case 1:
                            if (gbAllowTextEntryEscape != 0) {
                                strcpy(edit, original);
                                done++;
                                gbTextEntryEscaped = 1;
                            }
                            break;
                        case 0x4b:
                            if (field_0x31 != 0) {
                                field_0x31--;
                                if (field_0x31 < field_0x4b)
                                    field_0x4b = field_0x31;
                            }
                            break;
                        case 0x4d:
                            if (field_0x31 < strlen(edit))
                                field_0x31++;
                            break;
                        case 0x53:
                            if (field_0x31 < strlen(edit)) {
                                strcpy(scratch, edit + field_0x31 + 1);
                                strcpy(edit + field_0x31, scratch);
                            }
                            break;
                        default:
                            gpInputManager->AsciiConvert(event);
                            if (event.field4 == 10) {
                                gbTextEntryEscaped = 0;
                                done++;
                            } else if (event.field4 == 0x7f) {
                                if (field_0x31 != 0) {
                                    strcpy(scratch, edit + field_0x31);
                                    strcpy(edit + field_0x31 - 1, scratch);
                                    field_0x31--;
                                    if (field_0x31 < field_0x4b)
                                        field_0x4b = field_0x31;
                                }
                            } else if (strlen(edit) + 1 < m_maxLength && event.field4 != 0) {
                                strcpy(backup, edit);
                                char typed;
                                if (event.field4 < 0x100) {
                                    typed = static_cast<char>(event.field4);
                                    if (typed == '{' || typed == '}')
                                        typed = 0;
                                } else {
                                    typed = 0;
                                    switch ((event.field4 >> 8) & 0xff) {
                                    case 0x47: typed = '7'; break;
                                    case 0x48: typed = '8'; break;
                                    case 0x49: typed = '9'; break;
                                    case 0x4b: typed = '4'; break;
                                    case 0x4c: typed = '5'; break;
                                    case 0x4d: typed = '6'; break;
                                    case 0x4f: typed = '1'; break;
                                    case 0x50: typed = '2'; break;
                                    case 0x51: typed = '3'; break;
                                    case 0x52: typed = '0'; break;
                                    }
                                }
                                if (typed != 0) {
                                    strcpy(scratch, m_text);
#line 388
                                    BaseFree(m_text, __FILE__, __LINE__);
                                    m_text = static_cast<char *>(
                                        BaseAlloc(strlen(edit) + 5, __FILE__, __LINE__));
                                    strcpy(scratch, edit);
                                    scratch[field_0x31] = typed;
                                    scratch[field_0x31 + 1] = 0;
                                    strcat(scratch, edit + field_0x31);
                                    strcpy(edit, scratch);
                                    field_0x31++;
                                    SetupDisplayString(edit, field_0x31);
                                    if (field_0x49 != 3 &&
                                        m_font->LineLength(m_text, m_innerW) > field_0x45) {
                                        strcpy(edit, backup);
                                        field_0x31--;
                                    }
                                }
                            }
                            break;
                        }
                        SetupDisplayString(edit, field_0x31);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(screenX, screenY, m_width, m_height);
                    }
                } while (done == 0);
                gpMouseManager->ReallyShowPointer();
                strcpy(m_text, edit);
                field_0x4b = 0;
                Draw();
                gpWindowManager->UpdateScreenRegion(screenX, screenY, m_width, m_height);
                message.field4 = 0xc;
                message.type = 0x200;
                message.field8 = m_id;
                return 2;
            }
            break;
        }
        case 0x200:
            if (message.field4 == 3) {
                if (message.field8 == m_id) {
                    SetText(message.text);
                    return 1;
                }
            } else if (message.field4 == 7) {
                if (message.field8 == m_id) {
                    message.text = m_text;
                    return 1;
                }
            } else if (message.field4 == 0x33 && message.field8 == m_id) {
                m_maxLength = static_cast<unsigned short>(message.field18);
                return 1;
            }
            break;
        default:
            return widget::Main(message);
        }
    }
    return widget::Main(message);
}

// @early-stop
// Delinked local-label identity only: base/retail sections are 0x160, all 138 masked
// instructions are identical, and all 6/6 relocation targets agree.
VA(0x004d9410, 0x160)
void textEntryWidget::Draw(void)
{
    if (field_0x49 == 3) {
        char display[600];
        strcpy(display, m_text + field_0x4b);
        unsigned int length = strlen(display);
        if (m_font->LineWidth(display) > m_innerW) {
            do {
                display[length - 2] = 0;
                length--;
            } while (m_font->LineWidth(display) > m_innerW);
        }
        m_icon->DrawToBuffer(m_rectX + m_owner->m_posX, m_rectY + m_owner->m_posY,
                             field_0x2f, 0);
        m_font->DrawBoundedString(display, m_innerX + m_owner->m_posX,
                                  m_innerY + m_owner->m_posY, m_innerW, m_innerH,
                                  m_color, field_0x2a);
    } else {
        m_icon->DrawToBuffer(m_rectX + m_owner->m_posX, m_rectY + m_owner->m_posY,
                             field_0x2f, 0);
        int color = 3;
        if ((m_flags & 8) == 0)
            color = m_color;
        m_font->DrawBoundedString(m_text, m_innerX + m_owner->m_posX,
                                  m_innerY + m_owner->m_posY, m_innerW, m_innerH,
                                  color, field_0x2a);
    }
}

// @early-stop
// /O2 CSE/branch wall, with all 8/8 relocation targets equal. Base is 0x1bd versus
// retail 0x1be; only base 0x152..0x163 vs retail 0x152..0x15d (cached word load versus
// cmp+reload) and base 0x1a5..0x1b0 vs retail 0x1a4..0x1b1 (equivalent loop-test
// inversion plus xor) differ. Exhausted outer-if and do/while forms, shared/separate
// fit flags, condition inversion, and moving flag initialization through both branches.
VA(0x004d9570, 0x1be)
void textEntryWidget::SetupDisplayString(char *source, unsigned short cursor)
{
    if (KBTickCount() > glTimers[0]) {
        field_0x4d = 1 - field_0x4d;
        glTimers[0] = KBTickCount() + 0x168;
    }
    if (cursor != 0)
        strncpy(m_text, source, cursor);
    if (field_0x4d != 0)
        m_text[cursor] = 0x1f;
    else
        m_text[cursor] = '_';
    if (cursor < strlen(source))
        strcpy(m_text + cursor + 1, source + cursor);
    else
        m_text[cursor + 1] = 0;

    if (field_0x49 == 3) {
        int shifted;
        char display[300];
        do {
            shifted = 0;
            strcpy(display, m_text + field_0x4b);
            if (m_font->LineWidth(display) > m_innerW) {
                display[cursor - field_0x4b] = 0;
                if (m_font->LineWidth(display) > m_innerW) {
                    shifted = 1;
                    field_0x4b++;
                }
            }
        } while (shifted);
        int fits;
        do {
            if (field_0x4b > 0) {
                fits = 0;
                strcpy(display, m_text + field_0x4b - 1);
                if (m_font->LineWidth(display) <= m_innerW)
                    field_0x4b--;
                else
                    fits = 0;
            } else
                fits = 0;
        } while (field_0x4b != 0 && fits != 0);
    }
}


// ===== vtable textEntryWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d9410, 0x160)  void textEntryWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d8770, 0x36)  void * textEntryWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d8b90, 0x874)  int textEntryWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textEntryWidget, 0x004ebaa0);
