#ifndef HOMM2_SOURCE_ADVMANAGERTYPES_H
#define HOMM2_SOURCE_ADVMANAGERTYPES_H

#include <match.h>
#include <SOURCE/advManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/remoteTypes.h>

#pragma pack(push, 1)
union AdventureRemotePayload {
    char bytes[REMOTE_MESSAGE_PAYLOAD_SIZE];
    RemoteSaveInitialization save;
    SPlayerExit playerExit;
};
#pragma pack(pop)

SIZE(AdventureRemotePayload, REMOTE_MESSAGE_PAYLOAD_SIZE);

#endif // HOMM2_SOURCE_ADVMANAGERTYPES_H
