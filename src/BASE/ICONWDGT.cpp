// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_frame = 0;
    m_fillColor = 0;
    m_icon = 0;
    m_flip = 0;
    m_iconId = 0;
}

// @early-stop
// Compiler COMDAT-folding artifact: retail carries two strong, byte-identical 0x36
// ??_E/??_G deleting-destructor sections (10 relocations apiece), while retaining the
// exact standalone 0x21 ??1 destructor below makes VC4.2 emit a 0x1f ??_G wrapper and
// a weak ??_E alias with no section. Making the destructor inline produces the folded
// deleting body but removes the separately mapped exact ??1 symbol, so the ABI-correct
// standalone destructor is retained rather than traded for weak aliases.
// VA(0x004d0a90, 0x36) ??_E/??_G iconWidget deleting-destructor aliases

VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int x, short int y, short int width, short int height,
                       unsigned long int iconId, short int frame, signed char flip,
                       short int id, short int kind, short int fillColor)
    : widget(x, y, width, height, id, kind)
{
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int x, short int y, short int width, short int height,
                       char *iconName, short int frame, signed char flip,
                       short int id, short int kind, short int fillColor)
    : widget(x, y, width, height, id, kind)
{
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void)
{
    char iconName[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(iconName));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_frame = gpResourceManager->ReadWord();
    m_flip = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget()
{
    gpResourceManager->Dispose(m_icon);
}

// @early-stop
// All 0x291 code bytes are identical after masking 17 aligned relocations, and all
// external targets agree. Objdiff's residual is only the retail containing-function
// identity for two switch-dispatch references and five jump-table entries versus
// VC4.2's equivalent $L local-label relocations.
VA(0x004d0cd0, 0x291)
int iconWidget::Main(tag_message &msg)
{
    unsigned short flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0 &&
        (msg.type != MESSAGE_WIDGET ||
         msg.payload.widget.command != WIDGET_COMMAND_REPLACE_ICON)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }

    int eventType = msg.type;
    switch (eventType) {
    case MESSAGE_LEFT_BUTTON_DOWN:
    case MESSAGE_RIGHT_BUTTON_DOWN: {
        short relativeX = static_cast<short>(msg.payload.mouse.x);
        heroWindow *window = m_owner;
        relativeX -= static_cast<short>(window->m_posX);
        short relativeY = static_cast<short>(msg.payload.mouse.y);
        relativeY -= static_cast<short>(window->m_posY);
        if (m_x <= relativeX && m_y <= relativeY &&
            relativeX < m_x + m_width && relativeY < m_y + m_height) {
            if (eventType == MESSAGE_RIGHT_BUTTON_DOWN) {
                msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
            } else {
                m_flags = flags | WIDGET_FLAG_SELECTED;
                msg.payload.widget.command = WIDGET_COMMAND_SELECT;
            }
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;
    }

    case MESSAGE_LEFT_BUTTON_UP:
    case MESSAGE_RIGHT_BUTTON_UP:
        if ((flags & WIDGET_FLAG_SELECTED) != 0) {
            m_flags = flags & 0xfffe;
            msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;

    case MESSAGE_WIDGET:
        switch (msg.payload.widget.command) {
        case WIDGET_COMMAND_SET_FRAME:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            m_frame = msg.payload.widget.data.value;
            return 1;

        case WIDGET_COMMAND_SET_FILL_COLOR:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            m_fillColor = msg.payload.widget.data.value & 0xff;
            return 1;

        case WIDGET_COMMAND_SET_ICON:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            if (m_icon != 0) {
                gpResourceManager->Dispose(m_icon);
                m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.text);
            }
            return 1;

        case WIDGET_COMMAND_REPLACE_ICON:
            if (m_iconId == msg.payload.widget.id) {
                m_iconId = msg.payload.widget.data.value;
                gpResourceManager->Dispose(m_icon);
                m_icon = gpResourceManager->GetIcon(static_cast<unsigned long>(msg.payload.widget.data.value));
            }
            return 0;

        default:
            goto normalEvent;
        }

    default:
        goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

// @match-note
// /O2 two-instruction scheduling wall: base and retail are both 0xe5 bytes and all
// 4 relocation targets agree. All instructions are identical except +0x64..+0x6b:
// base loads m_width before subtracting entry->y, while retail performs those two
// independent instructions in the opposite order. Direct member expressions,
// cached width/icon-width locals, declaration/statement reordering, comma sequencing,
// both x/y subtraction orders, and all three syntax-aware AST variants were tried.
VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void)
{
    heroWindow *window = m_owner;
    short y = static_cast<short>(window->m_posY);
    short x = static_cast<short>(window->m_posX);
    short kind = m_kind;
    x += m_x;
    y += m_y;

    switch (kind) {
    case 0x10:
        m_icon->DrawToBuffer(x, y, m_frame, m_flip);
        return;

    case 0x11: {
        IconEntry *entry = GetIconEntry(m_icon, m_frame);
        short widgetWidth;
        short iconWidth;
        iconWidth = entry->w;
        y -= entry->y;
        widgetWidth = m_width;
        x -= entry->x;
        if (iconWidth < widgetWidth)
            x += (widgetWidth - iconWidth) >> 1;
        if (entry->h + 2 < m_height)
            y += m_height - entry->h - 2;
        m_icon->DrawToBuffer(x, y, m_frame, m_flip);
        return;
    }

    case 0x80:
        m_icon->FillToBuffer(x, y, m_frame, m_fillColor, m_flip, 0);
        return;
    }
}


// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(iconWidget, 0x004eba40);
