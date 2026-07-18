#ifndef HOMM2_SOURCE_BANKBOX_H
#define HOMM2_SOURCE_BANKBOX_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class heroWindow;
class playerData;

H2_ENUM_BEGIN(BankBoxConstant)
    BANK_BOX_TEXT_SIZE = 12,
    BANK_BOX_NON_GOLD_RESOURCE_COUNT = 6,
    BANK_BOX_FIRST_RESOURCE_WIDGET = 0x7ee,
    BANK_BOX_GOLD_WIDGET = 0x7f4,
    BANK_BOX_WINDOW_Z_ORDER = -1,
    BANK_BOX_WINDOW_ACTIVE = 1
H2_ENUM_END(BankBoxConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class bankBox {
public:
    playerData* m_player; // +0x00
    i16 m_x;              // +0x04
    i16 m_y;              // +0x06
    heroWindow* m_window; // +0x08
    // --- constructors ---
    bankBox(i32 x, i32 y, class playerData* player);
    ~bankBox();
    // --- methods ---
    void Update(i32);
};
#pragma pack(pop)
SIZE(bankBox, 0xc);
#endif // HOMM2_SOURCE_BANKBOX_H
