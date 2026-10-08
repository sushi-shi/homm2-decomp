#ifndef HOMM2_SOURCE_CASTLE_H
#define HOMM2_SOURCE_CASTLE_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/kbTypes.h>

class heroWindow;
struct tag_message;

typedef enum CastleBuildingConstant {
    CASTLE_SLOT_COUNT     = 18,
    CASTLE_UPGRADE_OFFSET = (BUILDING_SLOT_UPGRADE_FIRST) - (BUILDING_SLOT_DWELLING_SECOND)
} CastleBuildingConstant;

MessageDispatchResult CastleHandler(struct tag_message& message);

extern u8 castleSlotsBase[CASTLE_SLOT_COUNT];
extern class heroWindow* casWin;
extern u8 castleSlotsUse[CASTLE_SLOT_COUNT];

#endif
