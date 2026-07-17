// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0) {
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
iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    u32l iconId,
    i16 frame,
    i8 flip,
    i16 id,
    i16 kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0b40, 0x78)
iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    char* iconName,
    i16 frame,
    i8 flip,
    i16 id,
    i16 kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void) {
    char iconName[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<i8*>(iconName));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_frame = gpResourceManager->ReadWord();
    m_flip = static_cast<i8>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & 0xFF;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget() {
    gpResourceManager->Dispose(m_icon);
}

// @semantic
// Candidate and retail are both 0x291 with the same frame and CFG. In the required
// WidgetKind enum TU state, the only non-relocation spans are +0x9e..+0x9f and
// +0xa1..+0xa2: candidate compares m_x to relativeX and branches JG, while retail
// compares relativeX to m_x and branches JL. All 17 relocation offsets/types align;
// the 10 external targets agree, while two switch-dispatch references and five
// jump-table entries are the proven retail-containing-function versus $L identity
// artifact. m_x<=relativeX, relativeX>=m_x, and !(relativeX<m_x) lower identically
// in this typed TU state. The latter polarity was raw exact before introducing the
// canonical enum header, but that untyped state is not retained. Revisit after a real
// predecessor/header TU-state change; this is not a byte-proven early stop.
VA(0x004d0cd0, 0x291)
i32 iconWidget::Main(tag_message& msg) {
    u16 flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0
        && (msg.type != MESSAGE_WIDGET
            || msg.payload.widget.command != WIDGET_COMMAND_REPLACE_ICON)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }

    i32 eventType = msg.type;
    switch (eventType) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 relativeX = static_cast<i16>(msg.payload.mouse.x);
            heroWindow* window = m_owner;
            relativeX -= static_cast<i16>(window->m_posX);
            i16 relativeY = static_cast<i16>(msg.payload.mouse.y);
            relativeY -= static_cast<i16>(window->m_posY);
            if (m_x <= relativeX && m_y <= relativeY && relativeX < m_x + m_width
                && relativeY < m_y + m_height) {
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
                    m_fillColor = msg.payload.widget.data.value & 0xFF;
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
                        m_icon = gpResourceManager->GetIcon(
                            static_cast<u32l>(msg.payload.widget.data.value)
                        );
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

// @semantic
// Candidate and retail are both 0xe5 with the same frame, switch CFG, and 4 ordered
// relocation offsets/types/targets. In the required WidgetKind enum TU state the
// non-relocation spans are +0x62..+0x63, +0x65..+0x68, +0x69..+0x6c, +0x71..+0x72,
// +0x76..+0x77, +0x79..+0x7a, +0x7b..+0x7c, +0x7d..+0x7e, and +0x80..+0x81.
// Candidate assigns iconWidth/widgetWidth to CX/DX and centers through EDX; retail
// uses DX/CX and EBP. Direct/cached widths, declaration and subtraction order,
// comma sequencing, all three libclang AST variants, normal enum-include positions,
// and a separate semantic top/left-local form were tried. Twenty-four guarded
// typed-TU probes produced no exact closure; several recovered the earlier six-byte
// schedule residual, but those sub-100 probe scores were correctly discarded.
// Revisit after a real predecessor/header TU-state change; this is not a
// byte-proven early stop.
VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void) {
    heroWindow* window = m_owner;
    i16 y = static_cast<i16>(window->m_posY);
    i16 x = static_cast<i16>(window->m_posX);
    i16 kind = m_kind;
    x += m_x;
    y += m_y;

    switch (DecodeWidgetKind(kind)) {
        case WIDGET_KIND_ICON_DIRECT:
            m_icon->DrawToBuffer(x, y, m_frame, m_flip);
            return;

        case WIDGET_KIND_ICON_CENTERED: {
            IconEntry* entry = GetIconEntry(m_icon, m_frame);
            i16 widgetWidth;
            i16 iconWidth;
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

        case WIDGET_KIND_ICON_FILL:
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
