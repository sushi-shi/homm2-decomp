#ifndef HOMM2_REMOTE_TYPES_H
#define HOMM2_REMOTE_TYPES_H

#include <va.h>

typedef enum NetPlayerInfoConstant {
    NET_PLAYER_INFO_SYSTEM_ID_SIZE = 4,
    NET_PLAYER_INFO_NAME_SIZE = 21,
    NET_PLAYER_INFO_RESERVED_SIZE = 6
} NetPlayerInfoConstant;

#pragma pack(push, 1)
struct SNetPlayerInfo {
    char uniqueSystemID[NET_PLAYER_INFO_SYSTEM_ID_SIZE];
    char name[NET_PLAYER_INFO_NAME_SIZE];
    signed char connectionType;
    signed char useRegularCompression;
    signed char useDiffCompression;
    char reserved[NET_PLAYER_INFO_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(SNetPlayerInfo, 34);

#endif // HOMM2_REMOTE_TYPES_H
