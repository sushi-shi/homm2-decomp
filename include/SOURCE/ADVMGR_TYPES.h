#ifndef HOMM2_ADVMGR_TYPES_H
#define HOMM2_ADVMGR_TYPES_H

#include <va.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/KB.h>

#pragma pack(push, 1)
union AdventureRemotePayload {
    char bytes[ADVMGR_REMOTE_PAYLOAD_VIEW_SIZE];
    struct {
        i32 saveDataSize;
        i32 saveCrc;
        i32 saveTransmitCrc;
        i32 savePlayerExited;
    };
    SPlayerExit playerExit;
};
#pragma pack(pop)

SIZE(AdventureRemotePayload, ADVMGR_REMOTE_PAYLOAD_VIEW_SIZE);

#endif
