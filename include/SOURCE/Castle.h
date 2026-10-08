#ifndef HOMM2_SOURCE_CASTLE_H
#define HOMM2_SOURCE_CASTLE_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/kbTypes.h>

class heroWindow;
struct tag_message;

typedef enum CastleBuildingConstant {
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = H2EnumIndex(BUILDING_SLOT_UPGRADE_FIRST) - H2EnumIndex(BUILDING_SLOT_DWELLING_SECOND)
} CastleBuildingConstant;

MessageDispatchResult CastleHandler(struct tag_message& message);

extern H2EnumStorage<BuildingSlotType, u8> castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern H2EnumStorage<BuildingSlotType, u8> castleSlotsUse[CASTLE_SLOT_COUNT];

#endif
