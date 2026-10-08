#ifndef HOMM2_SOURCE_CASTLE_H
#define HOMM2_SOURCE_CASTLE_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/kbTypes.h>

class heroWindow;
struct tag_message;

H2_ENUM_BEGIN(CastleBuildingConstant)
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = IDX(BUILDING_SLOT_UPGRADE_FIRST) - IDX(BUILDING_SLOT_DWELLING_SECOND)
H2_ENUM_END(CastleBuildingConstant)

MessageDispatchResult CastleHandler(struct tag_message& message);

extern H2_ENUM_STORAGE(BuildingSlotType, u8) castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern H2_ENUM_STORAGE(BuildingSlotType, u8) castleSlotsUse[CASTLE_SLOT_COUNT];

#endif // HOMM2_SOURCE_CASTLE_H
