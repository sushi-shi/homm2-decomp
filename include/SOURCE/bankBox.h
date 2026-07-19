#ifndef HOMM2_SOURCE_BANKBOX_H
#define HOMM2_SOURCE_BANKBOX_H
#include <va.h>
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

#pragma pack(push, 1)
class bankBox {
public:
    playerData* m_player;
    i16 m_x;
    i16 m_y;
    heroWindow* m_window;
    bankBox(i32 x, i32 y, class playerData* player);
    ~bankBox();
    void Update(i32);
};
#pragma pack(pop)
SIZE(bankBox, 0xc);
#endif
