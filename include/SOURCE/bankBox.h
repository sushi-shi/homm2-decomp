#ifndef HOMM2_SOURCE_BANKBOX_H
#define HOMM2_SOURCE_BANKBOX_H

#include <va.h>

class heroWindow;
class playerData;

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
