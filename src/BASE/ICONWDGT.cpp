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

// @early-stop
// /O2 register-allocation wall: base and retail are both 0x6a bytes and all 4
// relocation targets agree. Bytes +0x00..+0x46 and +0x5e..+0x69 are identical;
// only +0x47..+0x5d schedules the icon store and colors frame/fill/flip as
// CX/AX/DL instead of retail CX/DX/AL. Direct assignments, reordered assignments,
// and placing the icon store before or after the parameter stores select the same
// base schedule.
VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       unsigned long int param_5, short int param_6, signed char param_7,
                       short int param_8, short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    m_iconId = param_5;
    m_icon = gpResourceManager->GetIcon(param_5);
    m_frame = param_6;
    m_flip = param_7;
    m_fillColor = param_10;
    field_0x14 = param_9;
}

// @early-stop
// /O2 register-allocation wall: base and retail are both 0x78 bytes and all 6
// relocation targets agree. The clean-slate MakeId/GetIcon path is identical through
// +0x54 and the epilogue +0x6c..+0x77 is identical; only +0x55..+0x6b has the same
// icon/frame/fill/flip store schedule described for the unsigned-id constructor.
// The same direct assignment-order variants retain this schedule after the exact
// MakeId/GetIcon sequence.
VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       char *param_5, short int param_6, signed char param_7,
                       short int param_8, short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    m_iconId = gpResourceManager->MakeId(param_5, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_frame = param_6;
    m_flip = param_7;
    m_fillColor = param_10;
    field_0x14 = param_9;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(local_10, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_frame = gpResourceManager->ReadWord();
    m_flip = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget()
{
    gpResourceManager->Dispose(m_icon);
}

// @early-stop
// Relocation-masked raw bytes are identical across the full 0x291-byte function.
// Retail delinks the two switch-dispatch references and five jump-table entries as
// containing-function relocations with addends, while VC4.2 emits $L local-label
// relocations; manual llvm-objdump review confirms all 17 sites and destinations.
VA(0x004d0cd0, 0x291)
int iconWidget::Main(tag_message &msg)
{
    unsigned short flags = m_flags;
    if ((flags & 2) == 0 && (msg.type != 0x200 || msg.payload.widget.command != 0x3c)) {
        if (msg.type == 0x200)
            return widget::Main(msg);
        return 0;
    }

    int eventType = msg.type;
    switch (eventType) {
    case 8:
    case 0x20: {
        short relativeX = static_cast<short>(msg.payload.mouse.x);
        heroWindow *window = m_owner;
        relativeX -= static_cast<short>(window->m_posX);
        short relativeY = static_cast<short>(msg.payload.mouse.y);
        relativeY -= static_cast<short>(window->m_posY);
        if (m_x <= relativeX && m_y <= relativeY &&
            relativeX < m_x + m_width && relativeY < m_y + m_height) {
            if (eventType == 0x20) {
                msg.payload.widget.parameter = 0x200;
                msg.payload.widget.command = 0xe;
            } else {
                m_flags = flags | 1;
                msg.payload.widget.command = 0xc;
            }
            msg.type = 0x200;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;
    }

    case 0x10:
    case 0x40:
        if ((flags & 1) != 0) {
            m_flags = flags & 0xfffe;
            msg.payload.widget.command = 0xd;
            msg.type = 0x200;
            msg.payload.widget.id = m_id;
            return 2;
        }
        return 0;

    case 0x200:
        switch (msg.payload.widget.command) {
        case 4:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            m_frame = msg.payload.widget.data.value;
            return 1;

        case 8:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            m_fillColor = msg.payload.widget.data.value & 0xff;
            return 1;

        case 9:
            if (m_id != msg.payload.widget.id)
                goto normalEvent;
            if (m_icon != 0) {
                gpResourceManager->Dispose(m_icon);
                m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.text);
            }
            return 1;

        case 0x3c:
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

// @early-stop
// /O2 two-instruction scheduling wall: base and retail are both 0xe5 bytes and all
// 4 relocation targets agree. All instructions are identical except +0x64..+0x6b:
// base loads m_width before subtracting entry->y, while retail performs those two
// independent instructions in the opposite order. Direct member expressions,
// cached width/icon-width locals, declaration/statement reordering, comma sequencing,
// and both x/y subtraction orders were tried.
VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void)
{
    heroWindow *window = m_owner;
    short y = static_cast<short>(window->m_posY);
    short x = static_cast<short>(window->m_posX);
    short kind = field_0x14;
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
