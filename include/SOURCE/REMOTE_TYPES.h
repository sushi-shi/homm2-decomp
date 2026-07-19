#ifndef HOMM2_REMOTE_TYPES_H
#define HOMM2_REMOTE_TYPES_H

#include <va.h>

H2_ENUM_BEGIN(NetPlayerInfoConstant)
    NET_PLAYER_INFO_SYSTEM_ID_SIZE = 4,
    NET_PLAYER_INFO_NAME_SIZE      = 21,
    NET_PLAYER_INFO_RESERVED_SIZE  = 6
H2_ENUM_END(NetPlayerInfoConstant)

H2_ENUM_CLASS_BEGIN(NetworkPacketType)
    NETWORK_PACKET_DATA           = 1,
    NETWORK_PACKET_GUEST_ARRIVED  = 2,
    NETWORK_PACKET_STARTUP        = 3,
    NETWORK_PACKET_GUEST_ACCEPTED = 4,
    NETWORK_PACKET_GUEST_REJECTED = 5
H2_ENUM_CLASS_END(NetworkPacketType)

H2_ENUM_CLASS_BEGIN(MultiplayerBaseType)
    MULTIPLAYER_BASE_MODEM         = 0,
    MULTIPLAYER_BASE_NETWORK       = 1,
    MULTIPLAYER_BASE_HOT_SEAT      = 2,
    MULTIPLAYER_BASE_UNINITIALIZED = 10
H2_ENUM_CLASS_END(MultiplayerBaseType)

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

// stpnet2.bin protocol picker: 1=IPX (rides DirectPlay), 2=TCP (rides Winsock),
// 3=modem, 4=direct connect; NT dims options 1 and 3.
H2_ENUM_CLASS_BEGIN(RemoteNetworkProtocol)
    REMOTE_PROTOCOL_NETBIOS        = 0,
    REMOTE_PROTOCOL_DIRECT_PLAY    = 1,
    DP_PROTOCOL_IPX                = REMOTE_PROTOCOL_DIRECT_PLAY,
    REMOTE_PROTOCOL_WINSOCK        = 2,
    DP_PROTOCOL_TCP                = REMOTE_PROTOCOL_WINSOCK,
    OLD_MAIN_NETWORK_PROTOCOL      = REMOTE_PROTOCOL_WINSOCK,
    REMOTE_PROTOCOL_MODEM          = 3,
    REMOTE_PROTOCOL_DIRECT_CONNECT = 4
H2_ENUM_CLASS_END(RemoteNetworkProtocol)

#endif
