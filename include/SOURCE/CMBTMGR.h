#ifndef HOMM2_CMBTMGR_H
#define HOMM2_CMBTMGR_H

#include <Ints.h>

struct tag_message;

struct CombatTowerOrigin {
    u16 x;
    u16 y;
};

i32 ValidHex(i32);
void UpdateCombatSystemOptions(i32);
i32 CombatSystemOptionsHandler(struct tag_message&);

#endif
