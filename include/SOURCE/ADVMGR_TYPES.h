#ifndef HOMM2_ADVMGR_TYPES_H
#define HOMM2_ADVMGR_TYPES_H

#include <va.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE_TYPES.h>

#pragma pack(push, 1)
union AdventureRemotePayload {
    char bytes[REMOTE_MESSAGE_PAYLOAD_SIZE];
    RemoteSaveInitialization save;
    SPlayerExit playerExit;
};
#pragma pack(pop)

SIZE(AdventureRemotePayload, REMOTE_MESSAGE_PAYLOAD_SIZE);

#endif
