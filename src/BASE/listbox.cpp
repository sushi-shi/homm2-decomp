// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\listbox.obj   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/listBoxWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <string.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004db060, 0x42)
listBoxWidget::listBoxWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_items = 0;
    m_scrollbar = 0;
    m_itemCount = 0;
    m_selectedIndex = -1;
    field_0x36 = -1;
}

VA(0x004db0d0, 0x86)
listBoxWidget::~listBoxWidget()
{
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_scrollbar != 0)
        delete m_scrollbar;
    for (int i = 0; i < m_itemCount; i++)
        BaseFree(m_items[i], __FILE__, __LINE__);
    BaseFree(m_items, __FILE__, __LINE__);
    gbSendMouseMoveMessages = 0;
}

// @early-stop
// /O2 regalloc wall: logic + field store-order byte-exact; icon-geometry value CSEs (entries[N].h/.w, m_x/m_y) color into different callee-saved regs than retail (di/ax/dx) and the h10 short spills to a 2-aligned frame slot (0x12) vs retail's dword slot (0x10) — not source-steerable.
VA(0x004db160, 0x26e)
void listBoxWidget::Read(void)
{
    short h10;
    char buf[13];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(buf));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(buf);
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(buf));
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(buf);
    gpResourceManager->RestorePosition();
    field_0x28 = gpResourceManager->ReadWord();
    field_0x2c = gpResourceManager->ReadWord();
    field_0x2e = gpResourceManager->ReadWord();
    field_0x30 = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    field_0x44 = 0;
    field_0x46 = 1;
    field_0x48 = 2;
    field_0x4a = 3;
    field_0x4c = 4;
    field_0x4e = 5;
    field_0x50 = 6;
    field_0x52 = 7;
    field_0x54 = 8;
    field_0x56 = 9;
    field_0x58 = 10;
    IconEntry *e10 = reinterpret_cast<IconEntry *>(m_icon->m_data) + 10;
    field_0x84 = e10->w;
    h10 = e10->h;
    field_0x86 = h10;
    short e0h = reinterpret_cast<IconEntry *>(m_icon->m_data)[0].h;
    field_0x5a = e0h;
    short e1h = reinterpret_cast<IconEntry *>(m_icon->m_data)[1].h;
    field_0x5c = e1h;
    short e2h = reinterpret_cast<IconEntry *>(m_icon->m_data)[2].h;
    field_0x5e = e2h;
    m_listX = m_x;
    m_listY = m_y;
    field_0x64 = reinterpret_cast<IconEntry *>(m_icon->m_data)[0].w;
    field_0x66 = (field_0x28 - 2) * e1h + e0h + e2h;
    short e3w = reinterpret_cast<IconEntry *>(m_icon->m_data)[3].w;
    field_0x6c = e3w;
    short e3h = reinterpret_cast<IconEntry *>(m_icon->m_data)[3].h;
    field_0x6e = e3h;
    field_0x7c = reinterpret_cast<IconEntry *>(m_icon->m_data)[5].w;
    field_0x68 = m_width - e3w + m_x;
    short e5h = reinterpret_cast<IconEntry *>(m_icon->m_data)[5].h;
    field_0x7e = e5h;
    field_0x6a = m_y;
    field_0x78 = field_0x68;
    field_0x7a = m_y + m_height - e5h;
    field_0x70 = field_0x68;
    field_0x8a = 0;
    field_0x8b = 0;
    field_0x8d = 0;
    field_0x8c = 0;
    m_topIndex = 0;
    field_0x42 = 0;
    field_0x2a = 0;
    field_0x72 = m_y + e3h;
    field_0x74 = field_0x7c;
    field_0x76 = field_0x7a - field_0x72;
    field_0x88 = field_0x76 - h10 - 7;
}

VA(0x004db3d0, 0x142)
void listBoxWidget::DeleteItem(int index)
{
    if (index >= m_itemCount)
        return;
    if (m_selectedIndex == index)
        m_selectedIndex = -1;
    if (m_topIndex == index && field_0x42 <= m_topIndex)
        m_topIndex--;
    if (--field_0x42 < 0)
        field_0x42 = 0;
    if (m_topIndex < 0)
        m_topIndex = 0;
    if (m_topIndex > field_0x42)
        m_topIndex = field_0x42;
    if (m_itemCount == 1) {
#line 156
        BaseFree(m_items[0], __FILE__, __LINE__);
        BaseFree(m_items, __FILE__, __LINE__);
        m_items = 0;
    } else {
        void **newItems = static_cast<void **>(BaseAlloc((m_itemCount - 1) * 4,
#line 162
                                                         __FILE__, __LINE__));
        memcpy(newItems, m_items, (m_itemCount - 1) * 4);
        if (m_itemCount - index - 1 > 0)
            memcpy(&newItems[index], &m_items[index + 1], (m_itemCount - index - 1) * 4);
        if (m_items != 0)
#line 169
            BaseFree(m_items, __FILE__, __LINE__);
        m_items = newItems;
    }
    m_itemCount--;
    if (field_0x42 > 0)
        field_0x2a = field_0x28;
    else
        field_0x2a = m_itemCount;
}

// @early-stop
// /O2 delinker artifact: the switch jump-table is emitted as a separate $L local symbol, so mine's Main spans 0x350 vs retail's 0x368 (delinker folds the table into Main and names its relocs Main-relative); table + jmp reloc unmatchable. Residual: 1 commutative hit-test compare (cmp m_x,mx;jg vs cmp mx,m_x;jl).
VA(0x004db520, 0x368)
int listBoxWidget::Main(tag_message &message)
{
    if (!(m_flags & 2)) {
        if (message.type == 0x200)
            return widget::Main(message);
        return 0;
    }
    switch (message.type) {
    case 4:
    case 0x10:
        if (m_flags & 4)
            return ProcessMouseMessage(message);
        break;
    case 8:
    case 0x20: {
        if (!(m_flags & 4))
            break;
        short mx = message.field4 - m_owner->m_posX;
        short my = message.field8 - m_owner->m_posY;
        if (m_x <= mx && m_y <= my && m_x + m_width > mx && m_y + m_height > my) {
            if (message.type == 0x20) {
                message.field4 = 0xe;
                message.type = 0x200;
                message.field8 = m_id;
                message.fieldC = 0x200;
                return 2;
            }
            return ProcessMouseMessage(message);
        }
        return 0;
    }
    case 0x200:
        switch (message.field4) {
        case 0x36:
            if (m_id == message.field8) {
                m_selectedIndex = reinterpret_cast<int>(message.text);
                return 1;
            }
            break;
        case 0x37:
            if (m_id == message.field8) {
                message.text = reinterpret_cast<char *>(static_cast<int>(m_selectedIndex));
                return 1;
            }
            break;
        case 0x39:
            if (m_id == message.field8 && m_itemCount > message.fieldC) {
#line 222
                BaseFree(m_items[message.fieldC], __FILE__, __LINE__);
                m_items[message.fieldC] = BaseAlloc(strlen(message.text) + 1, __FILE__, __LINE__);
                strcpy(static_cast<char *>(m_items[message.fieldC]), message.text);
            }
            break;
        case 0x38:
            if (m_id == message.field8) {
                char *text = message.text;
                void **newItems = static_cast<void **>(BaseAlloc((m_itemCount + 1) * 4,
#line 233
                                                                 __FILE__, __LINE__));
                if (m_itemCount != 0)
                    memcpy(newItems, m_items, m_itemCount * 4);
                newItems[m_itemCount] = BaseAlloc(strlen(text) + 1,
#line 236
                                                  __FILE__, __LINE__);
                strcpy(static_cast<char *>(newItems[m_itemCount]), text);
                m_itemCount++;
                if (m_items != 0)
#line 240
                    BaseFree(m_items, __FILE__, __LINE__);
                m_items = newItems;
                if (field_0x28 < m_itemCount) {
                    field_0x42 = m_itemCount - field_0x28;
                    m_topIndex = m_selectedIndex;
                    if (m_topIndex < 0)
                        m_topIndex = 0;
                    if (m_topIndex > field_0x42)
                        m_topIndex = field_0x42;
                }
                if (field_0x42 > 0)
                    field_0x2a = field_0x28;
                else
                    field_0x2a = m_itemCount;
            }
            break;
        case 0x3a:
            if (m_id == message.field8)
                DeleteItem(reinterpret_cast<int>(message.text));
            break;
        case 0x3b:
            if (m_id == message.field8)
                while (m_itemCount != 0)
                    DeleteItem(0);
            break;
        }
        break;
    }
    return widget::Main(message);
}

VA(0x004db890, 0x8)
void listBoxWidget::Draw(void)
{
    DrawLBStuff(0);
}

// @early-stop
// /O2 regalloc wall: logic + call sequence byte-exact; the baseX/baseY/m_posX/m_posY quad colors into ebx/edi/ecx/edx matching only 2-of-4 either way, plus one 16-bit thumb-X load the optimizer proves upper-zero (mov ax vs movsx) — not source-steerable.
VA(0x004db8a0, 0x334)
void listBoxWidget::DrawLBStuff(int doUpdate)
{
    int baseY = m_listY + m_owner->m_posY;
    int baseX = m_listX + m_owner->m_posX;
    for (int i = 0; i < field_0x28; i++) {
        if (i == 0) {
            m_icon->DrawToBuffer(baseX, baseY, field_0x44, 0);
            if (i < field_0x2a) {
                int color = m_selectedIndex == m_topIndex ? field_0x2e : field_0x2c;
                m_font->DrawBoundedString(static_cast<char *>(m_items[m_topIndex]), baseX + 5,
                                          baseY + 4, field_0x64 - 10, m_font->m_height + 1, color,
                                          field_0x30);
            }
            baseY += field_0x5a;
        } else if (field_0x28 - i == 1) {
            m_icon->DrawToBuffer(baseX, baseY, field_0x48, 0);
            if (i < field_0x2a) {
                int itemIndex = m_topIndex + i;
                int color = m_selectedIndex == itemIndex ? field_0x2e : field_0x2c;
                m_font->DrawBoundedString(static_cast<char *>(m_items[itemIndex]), baseX + 5,
                                          baseY + 2, field_0x64 - 10, m_font->m_height + 1, color,
                                          field_0x30);
            }
        } else {
            m_icon->DrawToBuffer(baseX, baseY, field_0x46, 0);
            if (i < field_0x2a) {
                int itemIndex = m_topIndex + i;
                int color = m_selectedIndex == itemIndex ? field_0x2e : field_0x2c;
                m_font->DrawBoundedString(static_cast<char *>(m_items[itemIndex]), baseX + 5,
                                          baseY + 2, field_0x64 - 10, m_font->m_height + 1, color,
                                          field_0x30);
            }
            baseY += field_0x5c;
        }
    }
    int upFrame = field_0x8a ? field_0x4c : field_0x4a;
    m_icon->DrawToBuffer(field_0x68 + m_owner->m_posX, field_0x6a + m_owner->m_posY, upFrame, 0);
    m_icon->DrawToBuffer(field_0x70 + m_owner->m_posX, field_0x72 + m_owner->m_posY, field_0x52, 0);
    int j;
    for (j = 2; j < field_0x28 - 2; j++)
        m_icon->DrawToBuffer(field_0x70 + m_owner->m_posX,
                             (j - 1) * field_0x5c + field_0x72 + m_owner->m_posY, field_0x54, 0);
    m_icon->DrawToBuffer(field_0x70 + m_owner->m_posX,
                         (j - 1) * field_0x5c + field_0x72 + m_owner->m_posY, field_0x56, 0);
    int downFrame = field_0x8b ? field_0x50 : field_0x4e;
    m_icon->DrawToBuffer(field_0x78 + m_owner->m_posX, field_0x7a + m_owner->m_posY, downFrame, 0);
    int thumbX = m_owner->m_posX + field_0x70 + 5;
    field_0x80 = thumbX;
    int offset;
    if (field_0x42 > 0)
        offset = m_topIndex * field_0x88 / field_0x42;
    else
        offset = field_0x88 / 2;
    int thumbY = m_owner->m_posY + field_0x72 + offset + 3;
    field_0x82 = thumbY;
    m_icon->DrawToBuffer(thumbX, thumbY, field_0x58, 0);
    if (doUpdate)
        gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width,
                                            m_height);
}

// @early-stop
// /O2 wall: logic + shared-exit (goto redraw/done) structure byte-exact; residual is commutative compare operand order (cmp A,B;jg vs cmp B,A;jl) in the hit-test + case-0x10 constant-store scheduling/block layout — not source-steerable.
VA(0x004dbbe0, 0x312)
int listBoxWidget::ProcessMouseMessage(tag_message &message)
{
    int mouseX = message.field10 - m_owner->m_posX;
    int mouseY = message.field14 - m_owner->m_posY;
    int adjY = mouseY - m_listY;
    switch (message.type) {
    case 4:
        if (field_0x8d) {
            int row;
            if (adjY > field_0x5a)
                row = (adjY - field_0x5a) / field_0x5c + 1;
            else
                row = 0;
            if (row < 0)
                row = 0;
            if (row >= field_0x2a)
                row = field_0x2a - 1;
            if (m_topIndex + row == m_selectedIndex)
                goto done;
            m_selectedIndex = row + m_topIndex;
        } else if (field_0x8c) {
            int newTop = (mouseY - field_0x86 / 2 - field_0x72 - 4) * (field_0x42 + 1) / field_0x88;
            if (newTop < 0)
                newTop = 0;
            if (newTop > field_0x42)
                newTop = field_0x42;
            if (m_topIndex == newTop)
                goto done;
            m_topIndex = newTop;
        } else {
            return 0;
        }
        goto redraw;
    case 8:
        if (m_itemCount == 0)
            goto done;
        if (m_listX <= mouseX && m_listY <= mouseY && mouseX < m_listX + field_0x64
            && mouseY < m_listY + field_0x66) {
            int clickedIndex;
            if (adjY > field_0x5a)
                clickedIndex = m_topIndex + (adjY - field_0x5a) / field_0x5c + 1;
            else
                clickedIndex = m_topIndex;
            if (m_itemCount <= clickedIndex)
                goto done;
            field_0x8d = 1;
            gbSendMouseMoveMessages = 1;
            if (m_selectedIndex == clickedIndex)
                goto done;
            m_selectedIndex = clickedIndex;
        } else if (mouseY < field_0x6a + field_0x6e) {
            if (m_topIndex > 0)
                m_topIndex--;
            field_0x8a = 1;
        } else if (mouseY >= field_0x7a) {
            if (m_topIndex < field_0x42)
                m_topIndex++;
            field_0x8b = 1;
        } else {
            if (field_0x82 <= mouseY && mouseY < field_0x82 + field_0x86) {
                field_0x8c = 1;
                gbSendMouseMoveMessages = 1;
            }
            m_topIndex = (mouseY - field_0x86 / 2 - field_0x72 - 4) * (field_0x42 + 1) / field_0x88;
            if (m_topIndex < 0)
                m_topIndex = 0;
            if (m_topIndex > field_0x42)
                m_topIndex = field_0x42;
        }
        goto redraw;
    case 0x10:
        gbSendMouseMoveMessages = 0;
        if (field_0x8a || field_0x8b || field_0x8c) {
            field_0x8c = 0;
            field_0x8b = 0;
            field_0x8a = 0;
            goto redraw;
        }
        if (!field_0x8d)
            return 0;
        field_0x8d = 0;
        message.field4 = 0xc;
        message.type = 0x200;
        message.field8 = m_id;
        message.fieldC = 1;
        message.text = reinterpret_cast<char *>(static_cast<int>(m_selectedIndex));
        if (field_0x36 == m_selectedIndex) {
            int lastTick = field_0x38;
            if (lastTick + 0x190 > KBTickCount())
                message.fieldC = 2;
        }
        field_0x36 = m_selectedIndex;
        field_0x38 = KBTickCount();
        return 2;
    }
    goto done;
redraw:
    DrawLBStuff(1);
done:
    return 1;
}


// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(listBoxWidget, 0x004ebac0);
