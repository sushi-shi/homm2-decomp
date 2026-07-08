// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BUTTON.OBJ   from: .\basewin.lib
// functions: 11   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/button.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/kbwin.h>
#include <BASE/mouseManager.h>
#include <BASE/inputManager.h>
#include <SOURCE/KB.h>
#include <_globals_model.h>
VA(0x004dd440, 0x34)
button::button(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x24 = 0;
    field_0x26 = 0;
    field_0x2c = 0;
    field_0x28 = 0;
    field_0x2a = -1;
    field_0x20 = 0;
}

VA(0x004dd4c0, 0x6e)
button::button(short int x, short int y, short int w, short int h, unsigned long int iconId, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    field_0x2c = iconId;
    field_0x20 = gpResourceManager->GetIcon(iconId);
    field_0x24 = p6;
    field_0x26 = p7;
    field_0x28 = p8;
    field_0x2a = p9;
}

VA(0x004dd530, 0x7c)
button::button(short int x, short int y, short int w, short int h, char *name, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    unsigned long id = gpResourceManager->MakeId(name, 1);
    field_0x2c = id;
    field_0x20 = gpResourceManager->GetIcon(id);
    field_0x24 = p6;
    field_0x26 = p7;
    field_0x28 = p8;
    field_0x2a = p9;
}

VA(0x004dd5b0, 0xeb)
void button::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x2c = gpResourceManager->MakeId(local_10, 1);
    field_0x20 = gpResourceManager->GetIcon(field_0x2c);
    gpResourceManager->RestorePosition();
    field_0x24 = gpResourceManager->ReadWord();
    field_0x26 = gpResourceManager->ReadWord();
    field_0x28 = gpResourceManager->ReadWord();
    field_0x2a = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
}

VA(0x004dd6a0, 0x21)
button::~button()
{
    gpResourceManager->Dispose(field_0x20);
}

VA(0x004dd6d0, 0x595)
int button::Main(tag_message &param_1)
{
    unsigned short uVar1;
    short sVar2, sVar7;
    long lVar3;
    int iVar4;
    if (field_0x14 == 0x1000 && (m_flags & 1) != 0 &&
        (lVar3 = KBTickCount(), gButtonRepeatTime < lVar3)) {
        if ((m_flags & 1) == 0)
            return 0;
        m_flags = m_flags & 0xfffe;
        Draw();
        gpWindowManager->UpdateScreenRegion(m_owner->m_posX + m_x,
                                            m_owner->m_posY + m_y, m_width, m_height);
        param_1.field4 = 0xd;
        param_1.type = 0x200;
        param_1.field8 = m_id;
        param_1.fieldC = iLeftRightSave;
        iLeftRightSave = 0;
        return 2;
    }
    uVar1 = m_flags;
    if ((uVar1 & 2) == 0) {
        if (param_1.type == 0x200)
            return widget::Main(param_1);
        return 0;
    }
    iVar4 = param_1.type;
    if (iVar4 < 9) {
        if (iVar4 != 8) {
            if (iVar4 == 1) {
                if ((uVar1 & 2) != 0 && (uVar1 & 4) != 0 && (uVar1 & 8) == 0) {
                    if (field_0x2a != -1 && field_0x2a == param_1.field4)
                        return Select(param_1);
                    return 0;
                }
            } else if (iVar4 == 2 && (uVar1 & 2) != 0 && (uVar1 & 4) != 0 && (uVar1 & 8) == 0) {
                if (field_0x2a != -1 && field_0x2a == param_1.field4) {
                    if ((uVar1 & 1) == 0)
                        return 0;
                    m_flags = uVar1 & 0xfffe;
                    Draw();
                    gpWindowManager->UpdateScreenRegion(m_owner->m_posX + m_x,
                                                        m_owner->m_posY + m_y,
                                                        m_width, m_height);
                    param_1.field4 = 0xd;
                    param_1.type = 0x200;
                    param_1.field8 = m_id;
                    param_1.fieldC = iLeftRightSave;
                    iLeftRightSave = 0;
                    return 2;
                }
                return 0;
            }
            goto LAB_004dd7d8;
        }
    } else {
        if (iVar4 == 0x10) {
            if ((uVar1 & 4) != 0 && (uVar1 & 1) != 0) {
                m_flags = uVar1 & 0xfffe;
                Draw();
                gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                    m_y + m_owner->m_posY, m_width,
                                                    m_height);
                param_1.field4 = 0xd;
                param_1.type = 0x200;
                param_1.field8 = m_id;
                param_1.fieldC = iLeftRightSave;
                iLeftRightSave = 0;
                return 2;
            }
            goto LAB_004dd7d8;
        }
        if (iVar4 != 0x20) {
            if (iVar4 == 0x200 && param_1.field4 == 0x3c) {
                if (param_1.field8 == field_0x2c) {
                    field_0x2c = reinterpret_cast<int>(param_1.text);
                    gpResourceManager->Dispose(field_0x20);
                    field_0x20 = gpResourceManager->GetIcon(reinterpret_cast<unsigned long>(param_1.text));
                }
                return 0;
            }
            goto LAB_004dd7d8;
        }
    }
    if ((uVar1 & 4) != 0) {
        sVar7 = static_cast<short>(param_1.field4) - m_owner->m_posX;
        sVar2 = static_cast<short>(param_1.field8) - m_owner->m_posY;
        if (iVar4 == 0x20) {
            if (m_x <= sVar7 && m_y <= sVar2 && sVar7 < m_width + m_x &&
                sVar2 < m_height + m_y) {
                param_1.field4 = 0xe;
                param_1.type = 0x200;
                param_1.fieldC = 0x200;
                param_1.field8 = m_id;
                return 2;
            }
            return 0;
        }
        if ((uVar1 & 8) == 0 && m_x <= sVar7 && m_y <= sVar2 &&
            sVar7 < m_width + m_x && sVar2 < m_height + m_y) {
            Select(param_1);
            iVar4 = param_1.type;
            while (iVar4 != 0x10 && param_1.type != 0x40) {
                PollSound();
                gpMouseManager->Main(param_1);
                if (param_1.type == 4) {
                    sVar2 = static_cast<short>(param_1.field4) - m_owner->m_posX;
                    sVar7 = static_cast<short>(param_1.field8) - m_owner->m_posY;
                    if (sVar2 < m_x || sVar7 < m_y ||
                        m_width + m_x <= sVar2 || m_height + m_y <= sVar7) {
                        if ((m_flags & 1) != 0) {
                            m_flags = m_flags & 0xfffe;
                            Draw();
                            gpWindowManager->UpdateScreenRegion(
                                m_owner->m_posX + m_x, m_owner->m_posY + m_y,
                                m_width, m_height);
                            param_1.field4 = 0xd;
                            param_1.type = 0x200;
                            param_1.field8 = m_id;
                            param_1.fieldC = iLeftRightSave;
                            iLeftRightSave = 0;
                        }
                    } else if ((m_flags & 1) == 0) {
                        Select(param_1);
                    }
                }
                Process1WindowsMessage();
                param_1 = gpInputManager->GetEvent();
                iVar4 = param_1.type;
            }
            if ((m_flags & 1) != 0) {
                m_flags = m_flags & 0xfffe;
                Draw();
                gpWindowManager->UpdateScreenRegion(m_owner->m_posX + m_x,
                                                    m_owner->m_posY + m_y, m_width,
                                                    m_height);
                param_1.field4 = 0xd;
                param_1.type = 0x200;
                param_1.field8 = m_id;
                param_1.fieldC = iLeftRightSave;
                iLeftRightSave = 0;
                return 2;
            }
            return 1;
        }
        return 0;
    }
LAB_004dd7d8:
    return widget::Main(param_1);
}

VA(0x004ddc70, 0x96)
short button::Select(struct tag_message &msg)
{
    short x = m_owner->m_posX + m_x;
    short y = m_y + m_owner->m_posY;
    field_0x20->DrawToBuffer(x, y, field_0x26, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= 1;
    msg.type = 0x200;
    msg.field8 = m_id;
    msg.field4 = 10;
    if (field_0x28 != 1)
        msg.field4 = 0xc;
    gButtonRepeatTime = KBTickCount() + 0x3c;
    iLeftRightSave = msg.fieldC & 0x300;
    return 2;
}

VA(0x004ddd10, 0x83)
short button::Deselect(struct tag_message &msg)
{
    if ((m_flags & 1) == 0)
        return 0;
    m_flags &= 0xfffe;
    Draw();
    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                        m_y + m_owner->m_posY, m_width, m_height);
    msg.field4 = 0xd;
    msg.type = 0x200;
    msg.field8 = m_id;
    msg.fieldC = iLeftRightSave;
    iLeftRightSave = 0;
    return 2;
}

VA(0x004ddda0, 0x55)
void button::Draw(void)
{
    heroWindow *win = m_owner;
    if ((m_flags & 1) != 0) {
        field_0x20->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY,
                                 field_0x26, 0);
        return;
    }
    field_0x20->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY,
                             field_0x24, 0);
}


// ===== vtable button : public widget  (3 slots) =====
//  [ 0] VA(0x004ddda0, 0x55)  void button::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd480, 0x36)  void * button::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd6d0, 0x595)  int button::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(button, 0x004ebaf0);

// ---- globals (definitions, RVA order) ----
int iLeftRightSave;
