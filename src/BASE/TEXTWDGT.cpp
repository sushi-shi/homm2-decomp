// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TEXTWDGT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textWidget.h>
#include <BASE/TEXTWDGT_TYPES.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <SOURCE/KB.h>
#include <string.h>

DATA(0x0051fa70) static STextWidgetSourceFiles gTextWidgetSourceFiles = {
    "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP",
    "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP",
    "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP",
    "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP"
};

// @data-layout-note NB09 assigns TEXTWDGT one 0xb0 initialized-data
// contribution at 0x11fa70. Retail stores four identical source paths in 0x2c
// slots at owner addends 0, 0x2c, 0x58, and 0x84. Read and destruction use the
// first two owners; both resize frees use the third, and both resize allocations
// use the fourth. This typed aggregate reproduces the complete contribution and
// all six relocation addends. The existing 0xc textWidget
// vtable is the only rdata contribution, and this TU has no loader-zero data.
// Do not pool the paths or split them with padding symbols or section pragmas.
// @semantic
// /O2 register-allocation checkpoint: base and retail are both 0x3e bytes and both
// relocation targets agree. Instruction selection, ordering, and immediates are
// identical; only the retained this register differs (ESI in base, EDI in retail),
// producing raw differences at +0x00,+0x04,+0x1a,+0x21,+0x24,+0x27,+0x2e,
// +0x36,+0x3b,+0x3c. Direct stores, a cached self pointer,
// and chained versus separate initialization of the two value-1 fields were tried.
// Four sibling-pinned AST variants and 60 guarded TU-state trials found no exact
// closure. Revisit only after a real predecessor/header state change.
VA(0x004d1060, 0x3e)
textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_color = 1;
    m_alignment = 1;
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

VA(0x004d10f0, 0x64)
textWidget::textWidget(short x, short y, short width, short height, char *text, char *fontName,
                       short color, short id, short kind, short alignment)
    : widget(x, y, width, height, id, kind)
{
    font *loadedFont = gpResourceManager->GetFont(fontName);
    m_color = color;
    m_font = loadedFont;
    m_text = text;
    m_alignment = static_cast<char>(alignment);
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d1160, 0xef)
void textWidget::Read(void)
{
    char resourceName[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    short len = gpResourceManager->ReadWord();
    m_text = static_cast<char *>(
        H2_ALLOC(len, gTextWidgetSourceFiles.read, 0x39));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_text), len);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(resourceName));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(resourceName);
    gpResourceManager->RestorePosition();
    m_color = gpResourceManager->ReadWord() & 0xff;
    m_alignment = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d1250, 0x30)
textWidget::~textWidget()
{
    gpResourceManager->Dispose(m_font);
    H2_FREE(m_text, gTextWidgetSourceFiles.destruction, 0x55);
}

VA(0x004d1280, 0x210)
int textWidget::Main(tag_message &msg)
{
    unsigned short flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }

    switch (msg.type) {
    case MESSAGE_LEFT_BUTTON_DOWN:
    case MESSAGE_RIGHT_BUTTON_DOWN: {
        short relativeX = static_cast<short>(msg.payload.mouse.x) -
                          static_cast<short>(m_owner->m_posX);
        short relativeY = static_cast<short>(msg.payload.mouse.y) -
                          static_cast<short>(m_owner->m_posY);
        if (relativeX < m_x || relativeY < m_y ||
            relativeX >= m_x + m_width || relativeY >= m_y + m_height)
            return 0;
        m_flags = flags | WIDGET_FLAG_SELECTED;
        if (msg.type == MESSAGE_RIGHT_BUTTON_DOWN)
            msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_SELECT;
        msg.payload.widget.id = m_id;
        return 2;
    }

    case MESSAGE_LEFT_BUTTON_UP:
    case MESSAGE_RIGHT_BUTTON_UP:
        if ((flags & WIDGET_FLAG_SELECTED) != 0) {
            m_flags = flags & ~WIDGET_FLAG_SELECTED;
            if (msg.type == MESSAGE_RIGHT_BUTTON_UP)
                msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;

    case MESSAGE_WIDGET:
        switch (msg.payload.widget.command) {
        case WIDGET_COMMAND_SET_TEXT: {
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
                H2_FREE(m_text, gTextWidgetSourceFiles.resizeFree, 0xd3);
                m_text = static_cast<char *>(H2_ALLOC(
                    newLen + 5, gTextWidgetSourceFiles.resizeAlloc, 0xd4));
            }
            strcpy(m_text, newText);
            return 1;
        }

        case WIDGET_COMMAND_SET_FILL_COLOR:
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
    if ((m_flags & WIDGET_FLAG_DIMMED) == 0)
        color = m_color;
    m_font->DrawBoundedString(m_text, m_x + m_owner->m_posX,
                                  m_y + m_owner->m_posY, m_width, m_height,
                                  color, m_alignment);
}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(short int color)
{
    m_color = color;
}

VA(0x004d14f0, 0xa2)
void textWidget::SetText(char *text)
{
    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT &&
        DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT_ENTRY) {
        m_text = text;
        return;
    }
    unsigned short newLen = strlen(text);
    if (strlen(m_text) < newLen) {
        H2_FREE(m_text, gTextWidgetSourceFiles.resizeFree, 0xd3);
        m_text = static_cast<char *>(H2_ALLOC(
            newLen + 5, gTextWidgetSourceFiles.resizeAlloc, 0xd4));
    }
    strcpy(m_text, text);
}


// ===== vtable textWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d1490, 0x49)  void textWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d10a0, 0x45)  void * textWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d1280, 0x210)  int textWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textWidget, 0x004eba50);
