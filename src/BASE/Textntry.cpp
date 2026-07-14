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

// Unresolved compiler folding artifact: keeping the standalone destructor exact makes VC4.2 emit a
// 0x1f ??_G wrapper that calls it, while each retail ??_E body is 0x36 and folds the same
// destructor operations inline. Marking the declaration inline reproduced the 0x36 body
// and 5/5 targets byte-for-byte but removed the separately mapped exact ??1 destructor,
// so the ABI-correct non-inline declaration is retained.
// VA(0x004d8770, 0x36) ??_E/??_G textEntryWidget deleting destructor aliases

// Active /O2 scheduling residual: base/retail sections are both 0x134 and all 6/6
// relocation targets agree. The instruction streams differ only by retail storing p10
// before the three constant flag stores, while VC4.2 delays that one store here.
VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(short p1, short p2, short p3, short p4, short p5, char *p6,
                                 char *p7, short p8, char *p9, short p10, short p11, short p12,
                                 short p13, int p14, int p15)
    : textWidget(p1, p2, p3, p4, p6, p7, p8, p11, p12, 1)
{
    field_0x31 = 0;
    m_maxLength = p5;
    icon *loadedIcon;
    loadedIcon = gpResourceManager->GetIcon(p9);
    short rectX;
    rectX = m_x;
    field_0x4b = 0;
    m_icon = loadedIcon;
    field_0x2f = p10;
    m_rectX = rectX;
    field_0x14 = 0x4000;
    m_rectY = m_y;
    m_rectW = m_width;
    m_maxLength = p5;
    m_color = 1;
    field_0x45 = 1;
    m_hasInset = 0;
    m_rectH = m_height;
#line 61 "I:\\Projects\\Heroes\\Prog\\BASE\\Textntry.cpp"
    m_text = static_cast<char *>(
        BaseAlloc(static_cast<unsigned short>(p5) + 5, __FILE__, __LINE__));
    strcpy(m_text, p6);
    if (p13 == 4) {
        m_innerX = p14 + m_x;
        m_hasInset = 1;
        m_innerY = p15 + m_y;
        m_innerW = m_width + -2 * p14;
        m_innerH = m_height;
    }
}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget()
{
    gpResourceManager->Dispose(m_icon);
}

// Active /O2 scheduling residual: all calls and 52/52 relocation targets agree.
// Base/retail are both 0x26c; the streams differ only by retail comparing type before
// loading m_height, while VC4.2 emits those two independent instructions in reverse order.
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
        int enabled;
        m_rectH = m_height;
        if (type == 3)
            enabled = 1;
        else
            enabled = 1;
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
// Delinked local-label identity only: base/retail sections are both 0x874 with the exact
// 0x9a8 frame, CFG, instruction stream, and jump tables at +0x7dc/+0x844. Manual raw-byte
// and relocation review finds the same ordered 35 external targets and 20 local table
// relocations; only delinked local-label identities differ.
VA(0x004d8b90, 0x874)
int textEntryWidget::Main(struct tag_message &message)
{
    if ((m_flags & 2) == 0) {
        if (message.type != 0x200)
            return 0;
    } else {
        switch (message.type) {
        default:
        defaultMessage:
            return widget::Main(message);
        case 8:
        case 0x20: {
            m_cursorBlink = 1;
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
                message.field8 = m_id;
                message.fieldC = 0x200;
                return 2;
            }
            if (mouseX >= m_x && mouseY >= m_y && mouseX < m_x + m_width &&
                mouseY < m_y + m_height) {
                mouseX = m_x + windowX;
                char original[600];
                char edit[600];
                char scratch[600];
                char backup[600];
                mouseY = m_y + windowY;
                strcpy(original, m_text);
                if ((m_hasInset & 1) != 0) {
                    field_0x31 = static_cast<unsigned short>(strlen(m_text));
                } else {
                    field_0x31 = 0;
                    m_text[0] = 0;
                }
                strcpy(edit, m_text);
                SetupDisplayString(edit, field_0x31);
                Draw();
                gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                short done = 0;
                glTimers[0] = KBTickCount() + 0x168;
                gpMouseManager->ReallyHidePointer();
                tag_message event;
                do {
                    if (KBTickCount() > glTimers[0]) {
                        SetupDisplayString(edit, field_0x31);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                    }
                    Process1WindowsMessage();
                    event = gpInputManager->GetEvent();
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
                                char typed = 0;
                                if (event.field4 >= 0x100) {
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
                                } else {
                                    typed = static_cast<char>(event.field4);
                                    if (typed == '{' || typed == '}')
                                        typed = 0;
                                }
                                if (typed != 0) {
                                    strcpy(scratch, m_text);
#line 388
                                    BaseFree(m_text, __FILE__, __LINE__);
#line 389
                                    m_text = static_cast<char *>(BaseAlloc(strlen(edit) + 6, __FILE__, __LINE__));
                                    strcpy(scratch, edit);
                                    scratch[field_0x31] = typed;
                                    scratch[field_0x31 + 1] = 0;
                                    strcat(scratch, edit + field_0x31);
                                    strcpy(edit, scratch);
                                    field_0x31++;
                                    SetupDisplayString(edit, field_0x31);
                                    if (field_0x49 != 3) {
                                        int lineLength = m_font->LineLength(m_text, m_innerW);
                                        if (field_0x45 >= lineLength) {
                                        } else {
                                            strcpy(edit, backup);
                                            field_0x31--;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        SetupDisplayString(edit, field_0x31);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                    }
                } while (done == 0);
                gpMouseManager->ReallyShowPointer();
                strcpy(m_text, edit);
                field_0x4b = 0;
                Draw();
                gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                message.field4 = 0xc;
                message.type = 0x200;
                message.field8 = m_id;
                return 2;
            }
            return 0;
        }
        case 0x200:
            switch (message.field4) {
            case 3:
                if (message.field8 == m_id) {
                    SetText(message.text);
                    return 1;
                }
                break;
            case 7:
                if (message.field8 == m_id) {
                    message.text = m_text;
                    return 1;
                }
                break;
            case 0x33:
                if (message.field8 == m_id) {
                    m_maxLength = static_cast<unsigned short>(message.field18);
                    return 1;
                }
                break;
            }
            break;
        }
    }
    goto defaultMessage;
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

// Active /O2 TU-cumulative register-allocation residual: all 8/8 relocation targets,
// the 0x130 frame, calls, and loop CFG agree. Base is 0x1bd versus retail 0x1be; VC4.2
// assigns this/cursor to ebx/ebp rather than retail ebp/ebx, and omits one redundant xor
// in the dead second-loop tail.
VA(0x004d9570, 0x1be)
void textEntryWidget::SetupDisplayString(char *source, unsigned short cursor)
{
    if (KBTickCount() > glTimers[0]) {
        m_cursorBlink = 1 - m_cursorBlink;
        glTimers[0] = KBTickCount() + 0x168;
    }
    if (cursor != 0)
        strncpy(m_text, source, cursor);
    if (m_cursorBlink != 0)
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
                display[cursor - field_0x4b + 1] = 0;
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
        } while (field_0x4b == 0 && fits != 0);
    }
}


// ===== vtable textEntryWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d9410, 0x160)  void textEntryWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d8770, 0x36)  void * textEntryWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d8b90, 0x874)  int textEntryWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textEntryWidget, 0x004ebaa0);
