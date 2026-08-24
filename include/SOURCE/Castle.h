#ifndef HOMM2_CASTLE_H
#define HOMM2_CASTLE_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/KB_TYPES.h>

class heroWindow;
struct tag_message;

typedef enum CastleBuildingConstant {
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = 5
} CastleBuildingConstant;

MessageDispatchResult CastleHandler(struct tag_message&);

extern H2EnumStorage<BuildingSlotType, u8> castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern H2EnumStorage<BuildingSlotType, u8> castleSlotsUse[CASTLE_SLOT_COUNT];

#endif
