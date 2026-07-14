// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BORDER.OBJ   from: .\basewin.lib
// functions: 8   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/border.h>
#include <BASE/resourceManager.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
VA(0x004d20a0, 0x32)
border::border(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    m_fillColor = 0;
}

// @match-note
// Generated-alias checkpoint: candidate emits a 0x1f ??_G with 2 relocations that
// calls the exact standalone destructor; each retail ??_E is 0x4d with 7 relocations
// and inlines both resource disposals plus widget::~widget. Inlining border::~border
// recovers that alias body but removes the mapped standalone ??1. This is a real
// emission tradeoff, not a relocation artifact; preserve the exact standalone body.
// VA(0x004d20e0, 0x4d) ??_E/??_G border deleting-destructor aliases

VA(0x004d2130, 0x64)
border::border(short int x, short int y, short int w, short int h, short int e, short int f, short int fillColor, char *name)
    : widget(x, y, w, h, e, f)
{
    if (name != 0)
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
    else
        m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    m_fillColor = fillColor;
}

VA(0x004d21a0, 0x38)
border::~border()
{
    if (m_backgroundBitmap != 0)
        gpResourceManager->Dispose(m_backgroundBitmap);
    if (m_backgroundIcon != 0)
        gpResourceManager->Dispose(m_backgroundIcon);
}

VA(0x004d21e0, 0x10e)
void border::Read(void)
{
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    short kind = gpResourceManager->ReadWord();
    m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    field_0x14 = kind;
    char name[16];
    if (kind == 0x800) {
        gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
        gpResourceManager->SavePosition();
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
        gpResourceManager->RestorePosition();
        return;
    }
    if (kind == 0x801) {
        gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
        gpResourceManager->SavePosition();
        m_backgroundIcon = gpResourceManager->GetIcon(name);
        gpResourceManager->RestorePosition();
        return;
    }
    m_fillColor = gpResourceManager->ReadWord() & 0xff;
}

// @early-stop
// Proven delinker artifact in the current combined TU: explicit-range comparison
// over all 0x181 bytes, including the +0x134 jump table and trailing index bytes,
// is raw-exact after masking 9/9 relocation payloads. Both external widget::Main
// calls agree; objdiff differs only on local dispatch/table owner identities.
VA(0x004d22f0, 0x181)
int border::Main(struct tag_message &msg)
{
    short flags = m_flags;
    if ((flags & 2) == 0) {
        if (msg.type == 0x200)
            return widget::Main(msg);
        return 0;
    }
    int type = msg.type;
    switch (type) {
    default:
        goto normalEvent;
    case 8:
    case 0x20:
        goto hoverEvent;
    case 0x10:
    case 0x40:
        goto leaveEvent;
    }

normalEvent:
    return widget::Main(msg);

hoverEvent: {
    short mx = static_cast<short>(msg.payload.mouse.x);
    heroWindow *window = m_owner;
    mx -= static_cast<short>(window->m_posX);
    short my = static_cast<short>(msg.payload.mouse.y) - window->m_posY;
    if (m_x <= mx && m_y <= my && mx < m_width + m_x && my < m_height + m_y) {
        if (type == 0x20) {
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

leaveEvent:
    if ((flags & 1) != 0) {
        m_flags = flags & 0xfffe;
        msg.type = 0x200;
        msg.payload.widget.command = 0xd;
        msg.payload.widget.id = m_id;
        return 2;
    }
    return 0;
}

// @match-note
// Structurally complete /O2 checkpoint: declaring X before Y removes the spurious
// EBP save and raises this from 73.80% to 88.03%. The switch, three draw paths,
// calls and 7 relocations agree. Base is 0xaa bytes versus retail 0xab; remaining
// bytes are the equivalent owner/member load order for X/Y. Y-first and explicit
// owner/two-step forms regress or emit no gain; revisit after a new TU state.
VA(0x004d2480, 0xab)
void border::Draw(void)
{
    short x = m_x + static_cast<short>(m_owner->m_posX);
    short y = m_y + static_cast<short>(m_owner->m_posY);
    int kind = field_0x14;
    switch (kind) {
    case 0x400:
        FillBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, m_fillColor);
        return;
    case 0x800:
        PollSound();
        BlitBitmap(m_backgroundBitmap, 0, 0, m_width, m_height, gpWindowManager->m_screen, x, y);
        PollSound();
        return;
    case 0x801:
        m_backgroundIcon->DrawToBuffer(x, y, 0, 0);
        return;
    default:
        return;
    }
}


// ===== vtable border : public widget  (3 slots) =====
//  [ 0] VA(0x004d2480, 0xab)  void border::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d20e0, 0x4d)  void * border::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d22f0, 0x181)  int border::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(border, 0x004eba60);
