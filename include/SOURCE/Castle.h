#ifndef HOMM2_CASTLE_H
#define HOMM2_CASTLE_H

#include <Ints.h>

class heroWindow;
struct tag_message;

H2_ENUM_BEGIN(CastleBuildingConstant)
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = 5
H2_ENUM_END(CastleBuildingConstant)

i32 CastleHandler(struct tag_message&);

extern u8 castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern u8 castleSlotsUse[CASTLE_SLOT_COUNT];

#endif
