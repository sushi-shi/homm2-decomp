#ifndef HOMM2_REMOTE_TYPES_H
#define HOMM2_REMOTE_TYPES_H

#include <Ints.h>

typedef enum NetPlayerInfoConstant {
    NET_PLAYER_INFO_SYSTEM_ID_SIZE = 4,
    NET_PLAYER_INFO_NAME_SIZE      = 21,
    NET_PLAYER_INFO_RESERVED_SIZE  = 6,
    NET_PLAYER_CONNECTION_CURRENT  = 2
} NetPlayerInfoConstant;

enum {
    NETWORK_PACKET_DATA           = 1,
    NETWORK_PACKET_GUEST_ARRIVED  = 2,
    NETWORK_PACKET_STARTUP        = 3,
    NETWORK_PACKET_GUEST_ACCEPTED = 4,
    NETWORK_PACKET_GUEST_REJECTED = 5
};
typedef i32 NetworkPacketType;
enum {
    REMOTE_MESSAGE_DEFAULT    = -1,
    REMOTE_MESSAGE_CONFIRM    = 1,
    REMOTE_MESSAGE_RELIABLE   = 2,
    REMOTE_MESSAGE_UNRELIABLE = 3,
    REMOTE_MESSAGE_HEARTBEAT  = 4
};
typedef i32 RemoteMessageType;
enum {
    MULTIPLAYER_BASE_MODEM         = 0,
    MULTIPLAYER_BASE_NETWORK       = 1,
    MULTIPLAYER_BASE_HOT_SEAT      = 2,
    MULTIPLAYER_BASE_UNINITIALIZED = 10
};
typedef i32 MultiplayerBaseType;
enum {
    REMOTE_GAME_NONE          = 0,
    REMOTE_GAME_NETWORK_HOST  = 1,
    REMOTE_GAME_NETWORK_GUEST = 2,
    REMOTE_GAME_MODEM_HOST    = 3,
    REMOTE_GAME_MODEM_GUEST   = 4,
    REMOTE_GAME_UNINITIALIZED = 10
};
typedef i32 RemoteGameMode;
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

enum {
    REMOTE_PROTOCOL_NETBIOS        = 0,
    REMOTE_PROTOCOL_DIRECT_PLAY    = 1,
    DP_PROTOCOL_IPX                = REMOTE_PROTOCOL_DIRECT_PLAY,
    REMOTE_PROTOCOL_WINSOCK        = 2,
    DP_PROTOCOL_TCP                = REMOTE_PROTOCOL_WINSOCK,
    OLD_MAIN_NETWORK_PROTOCOL      = REMOTE_PROTOCOL_WINSOCK,
    REMOTE_PROTOCOL_MODEM          = 3,
    REMOTE_PROTOCOL_DIRECT_CONNECT = 4
};
typedef i32 RemoteNetworkProtocol;
#endif
