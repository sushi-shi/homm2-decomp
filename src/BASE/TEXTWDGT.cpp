// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TEXTWDGT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <SOURCE/KB.h>
#include <string.h>
// @early-stop
// /O2 register-allocation wall: base and retail are both 0x3e bytes and both
// relocation targets agree. Instruction selection, ordering, and immediates are
// identical; only the retained this register differs (ESI in base, EDI in retail),
// at +0x00, +0x03..+0x04, +0x19..+0x3c. Direct stores, a cached self pointer,
// and chained versus separate initialization of the two value-1 fields were tried.
VA(0x004d1060, 0x3e)
textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_color = 1;
    field_0x2a = 1;
    m_font = 0;
    m_text = 0;
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

// @early-stop
// Compiler COMDAT-folding artifact: retail carries two strong, byte-identical 0x45
// ??_E/??_G deleting-destructor sections with 14 relocations apiece. Preserving the
// exact standalone 0x30 destructor below makes VC4.2 emit a 0x1d ??_G wrapper with
// only 2 relocations and a weak ??_E alias with no section; inlining the destructor
// would trade away the separately mapped exact ??1 body.
// VA(0x004d10a0, 0x45) ??_E/??_G textWidget deleting-destructor aliases

// @early-stop
// /O2 register-allocation wall: base and retail are both 0x64 bytes and all 4
// relocation targets agree. Bytes +0x00..+0x41 and +0x5d..+0x63 are identical;
// only +0x42..+0x5c colors the text/alignment parameters as EDX/AL instead of
// retail EAX/DL and schedules their same stores differently. A direct GetFont
// assignment, a cached result, cached alignment, and assignment-order variants
// were tried; the cached-result form shown is best.
VA(0x004d10f0, 0x64)
textWidget::textWidget(short p1, short p2, short p3, short p4, char *p5, char *p6, short p7,
                       short p8, short p9, short p10)
    : widget(p1, p2, p3, p4, p8, p9)
{
    font *newFont = gpResourceManager->GetFont(p6);
    m_color = p7;
    m_font = newFont;
    field_0x2a = static_cast<char>(p10);
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
    m_text = p5;
}

VA(0x004d1160, 0xef)
void textWidget::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    short len = gpResourceManager->ReadWord();
    m_text = static_cast<char *>(
        H2_ALLOC(len, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0x39));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_text), len);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    m_color = gpResourceManager->ReadWord() & 0xff;
    field_0x2a = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d1250, 0x30)
textWidget::~textWidget()
{
    gpResourceManager->Dispose(m_font);
    H2_FREE(m_text, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0x55);
}

// @early-stop
// /O2 compare-orientation wall: base and retail are both 0x210 bytes and all 6
// relocation targets agree. Every instruction is identical except +0x83..+0x87:
// base emits `cmp ax,di; jg` and retail emits the equivalent `cmp di,ax; jl` for
// relativeX < m_x. Positive and rejection-form hit tests, swapped relational
// spelling, an explicit cached left edge, and the `value | 0` spelling were tried.
VA(0x004d1280, 0x210)
int textWidget::Main(tag_message &msg)
{
    unsigned short flags = m_flags;
    if ((flags & 2) == 0) {
        if (msg.type == 0x200)
            return widget::Main(msg);
        return 0;
    }

    switch (msg.type) {
    case 8:
    case 0x20: {
        short relativeX = static_cast<short>(msg.payload.mouse.x) -
                          static_cast<short>(m_owner->m_posX);
        short relativeY = static_cast<short>(msg.payload.mouse.y) -
                          static_cast<short>(m_owner->m_posY);
        if (relativeX < m_x || relativeY < m_y ||
            relativeX >= m_x + m_width || relativeY >= m_y + m_height)
            return 0;
        m_flags = flags | 1;
        if (msg.type == 0x20)
            msg.payload.widget.parameter = 0x200;
        msg.type = 0x200;
        msg.payload.widget.command = 0xc;
        msg.payload.widget.id = m_id;
        return 2;
    }

    case 0x10:
    case 0x40:
        if ((flags & 1) != 0) {
            m_flags = flags & 0xfffe;
            if (msg.type == 0x40)
                msg.payload.widget.parameter = 0x200;
            msg.type = 0x200;
            msg.payload.widget.command = 0xd;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;

    case 0x200:
        switch (msg.payload.widget.command) {
        case 3: {
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            char *newText = msg.payload.widget.data.text;
            if (DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT &&
                DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT_ENTRY) {
                m_text = newText;
                return 1;
            }
            unsigned short newLen = strlen(newText);
            if (strlen(m_text) < newLen) {
                H2_FREE(m_text, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0xd3);
                m_text = static_cast<char *>(H2_ALLOC(
                    newLen + 5, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0xd4));
            }
            strcpy(m_text, newText);
            return 1;
        }

        case 8:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            m_color = msg.payload.widget.data.value;
            return 1;

        default:
            goto normalEvent;
        }

    default:
        goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

VA(0x004d1490, 0x49)
void textWidget::Draw(void)
{
    int color = 3;
    if ((m_flags & 8) == 0)
        color = m_color;
    m_font->DrawBoundedString(m_text, m_x + m_owner->m_posX,
                                  m_y + m_owner->m_posY, m_width, m_height,
                                  color, field_0x2a);
}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(short int param_1)
{
    m_color = param_1;
}

VA(0x004d14f0, 0xa2)
void textWidget::SetText(char *param_1)
{
    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT &&
        DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT_ENTRY) {
        m_text = param_1;
        return;
    }
    unsigned short newLen = strlen(param_1);
    if (strlen(m_text) < newLen) {
        H2_FREE(m_text, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0xd3);
        m_text = static_cast<char *>(H2_ALLOC(
            newLen + 5, "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP", 0xd4));
    }
    strcpy(m_text, param_1);
}


// ===== vtable textWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d1490, 0x49)  void textWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d10a0, 0x45)  void * textWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d1280, 0x210)  int textWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textWidget, 0x004eba50);
