#ifndef HOMM2_ADVMGR_TYPES_H
#define HOMM2_ADVMGR_TYPES_H

#include <Ints.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/KB.h>

#pragma pack(push, 1)
struct AdventureRemoteSave {
    i32 saveDataSize;
    i32 saveCrc;
    i32 saveTransmitCrc;
    i32 savePlayerExited;
};
static_assert(sizeof(AdventureRemoteSave) == ADVMGR_REMOTE_PAYLOAD_VIEW_SIZE);
#pragma pack(pop)


#endif
