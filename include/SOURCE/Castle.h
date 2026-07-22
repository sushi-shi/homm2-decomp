#ifndef HOMM2_CASTLE_H
#define HOMM2_CASTLE_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/KB_TYPES.h>

class heroWindow;
struct tag_message;

H2_ENUM_BEGIN(CastleBuildingConstant)
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = 5
H2_ENUM_END(CastleBuildingConstant)

WidgetDispatchResult CastleHandler(struct tag_message&);

extern H2_ENUM_STORAGE(BuildingSlotType, u8) castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern H2_ENUM_STORAGE(BuildingSlotType, u8) castleSlotsUse[CASTLE_SLOT_COUNT];

#endif
