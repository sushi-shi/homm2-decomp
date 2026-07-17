#ifndef HOMM2_REMOTE_TYPES_H
#define HOMM2_REMOTE_TYPES_H

#include <va.h>

typedef enum NetPlayerInfoConstant {
    NET_PLAYER_INFO_SYSTEM_ID_SIZE = 4,
    NET_PLAYER_INFO_NAME_SIZE = 21,
    NET_PLAYER_INFO_RESERVED_SIZE = 6
} NetPlayerInfoConstant;

HOMM2_ENUM_BEGIN(NetworkPacketType)
NETWORK_PACKET_DATA = 1, NETWORK_PACKET_GUEST_ARRIVED = 2, NETWORK_PACKET_STARTUP = 3,
                         NETWORK_PACKET_GUEST_ACCEPTED = 4,
                         NETWORK_PACKET_GUEST_REJECTED = 5 HOMM2_ENUM_END(NetworkPacketType)

                             HOMM2_ENUM_BEGIN(MultiplayerBaseType) MULTIPLAYER_BASE_MODEM = 0,
                         MULTIPLAYER_BASE_NETWORK = 1, MULTIPLAYER_BASE_HOT_SEAT = 2,
                         MULTIPLAYER_BASE_UNINITIALIZED = 10 HOMM2_ENUM_END(MultiplayerBaseType)

#pragma pack(push, 1)
                             struct SNetPlayerInfo {
    char uniqueSystemID[NET_PLAYER_INFO_SYSTEM_ID_SIZE];
    char name[NET_PLAYER_INFO_NAME_SIZE];
    i8 connectionType;
    i8 useRegularCompression;
    i8 useDiffCompression;
    char reserved[NET_PLAYER_INFO_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(SNetPlayerInfo, 34);

#endif // HOMM2_REMOTE_TYPES_H
