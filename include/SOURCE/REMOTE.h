#ifndef HOMM2_SOURCE_REMOTE_H
#define HOMM2_SOURCE_REMOTE_H

#include <match.h>
#include <Domains.h>
#include <SOURCE/gameTypes.h>
#include <SOURCE/remoteTypes.h>

H2_ENUM_BEGIN(RemoteStorageConstant)
    REMOTE_QUEUE_CAPACITY        = 128,
    REMOTE_QUEUE_STORAGE_COUNT   = 138,
    REMOTE_RECENT_ID_COUNT       = 30,
    REMOTE_NET_NAME_SIZE         = 32,
    REMOTE_ENCODED_BUFFER_SIZE   = 268,
    REMOTE_RECEIVE_BUFFER_SIZE   = 266,
    REMOTE_TRANSPORT_BUFFER_SIZE = 268,
    REMOTE_BAUD_RATE_COUNT       = 7,
    REMOTE_IRQ_COUNT             = 7,
    REMOTE_ERROR_TEXT_SIZE       = 200,
H2_ENUM_END(RemoteStorageConstant)

H2_ENUM_BEGIN(RemotePacketEncodingConstant)
    REMOTE_PACKET_HEADER_SIZE     = 6,
    REMOTE_BROADCAST_PLAYER       = 0x7f,
    REMOTE_HEARTBEAT_MESSAGE_SIZE = 10,
    REMOTE_HEARTBEAT_CONTROL_FLAG = 0x80,
    REMOTE_HEARTBEAT_PLAYER_SHIFT = 4,
    REMOTE_HEARTBEAT_PHASE_MASK   = 0x0f,
H2_ENUM_END(RemotePacketEncodingConstant)

H2_ENUM_BEGIN(RemoteTransportTimingConstant)
    REMOTE_RETRY_COUNT                   = 25,
    REMOTE_CONFIRM_POLL_COUNT            = 50,
    REMOTE_CONFIRM_POLL_DELAY            = 20,
    REMOTE_SEND_RETRY_DELAY              = 1000,
    REMOTE_HEARTBEAT_INTERVAL            = 5000,
    REMOTE_HOST_TIMEOUT                  = 60000,
    REMOTE_CHAIN_GUEST_TIMEOUT_INCREMENT = 30000,
    REMOTE_GUEST_TIMEOUT                 = 60000,
    // How long a peer may stay silent before the player is asked
    // whether to keep waiting.
    REMOTE_WAIT_TIMEOUT                  = 90000,
    REMOTE_INITIAL_HEARTBEAT             = 1999999999,
H2_ENUM_END(RemoteTransportTimingConstant)

// A RemoteMessage's command: what the payload carries, for TransmitRemoteData,
// TransmitAndWait and every receiver's switch.
H2_ENUM_BEGIN(RemoteCommand)
    REMOTE_COMMAND_SAVE_GAME          = 1,
    REMOTE_COMMAND_SAVE_INIT_RESPONSE = 2,
    REMOTE_COMMAND_SAVE_DATA          = 3,
    REMOTE_COMMAND_SAVE_ACK_REQUEST   = 4,
    REMOTE_COMMAND_SAVE_ACK_RESPONSE  = 5,
    REMOTE_COMMAND_SAVE_FINISH        = 6,
    REMOTE_COMMAND_POP_NET_BOX        = 11,
    REMOTE_COMMAND_COMBAT             = 21,
    REMOTE_COMMAND_COMBAT_CONFIRM     = 22,
    REMOTE_COMMAND_COMBAT_ACTION      = 23,
    REMOTE_COMMAND_PLAYER_EXIT        = 31,
    REMOTE_COMMAND_NET_SETUP          = 32,
    REMOTE_COMMAND_HOST_PLAYER_EXIT   = 33,
    REMOTE_COMMAND_SETUP_PLAYER_INFO  = 34,
    REMOTE_COMMAND_GROUP_MAP_CHANGE   = 41,
    REMOTE_COMMAND_GAME_SETUP         = 51,
    REMOTE_COMMAND_MAP_HEADER         = 52,
    REMOTE_COMMAND_GAME_START         = 53,
    REMOTE_COMMAND_GAME_CANCEL        = 54,
    REMOTE_COMMAND_PLAYER_INFO        = 55,
    REMOTE_COMMAND_NEW_GAME           = 61,
    REMOTE_COMMAND_LOAD_GAME          = 62
H2_ENUM_END(RemoteCommand)

H2_ENUM_BEGIN(RemoteQueueSentinel)
    REMOTE_ORDER_SENTINEL = 999999999,
H2_ENUM_END(RemoteQueueSentinel)

#pragma pack(push, 1)
struct RemotePacketHeader {
    char source;
    char destination;
    char reserved;
    char payloadSize;
    u16 crc;
};

struct RemoteMessage {
    i8 sender;
    i32 id;
    H2_ENUM_STORAGE(RemoteMessageType, i8) type;
    i8 command;
    i16 payloadSize;
    char payload[REMOTE_MESSAGE_PAYLOAD_SIZE];
};
#pragma pack(pop)
SIZE(RemotePacketHeader, REMOTE_PACKET_HEADER_SIZE);
SIZE(RemoteMessage, REMOTE_MESSAGE_SIZE);

void RemoteCleanup(void);
void RemoteMain(RemoteGameMode gameMode);
void UnloadRemoteDriver(i16 networkDriver);
i32 calc_crc_long(u8* data, i32 length);
void calc_crc(u16* crc, u8* data, i32 length);
i32 EncodePacket(u8* data, char source, char destination, i32 length);
i32 DecodePacket(u8* data, i32);
i32 SendRemoteData(u8* dataToSend, u8*, i32 destination, i32 length);
i32 ReceiveRemoteData(u8*, u8* data, i32 decodeType);
i32 TransmitRemoteData(
    char* data,
    i32 destination,
    i32 length,
    i8 command,
    i8 reliable,
    i8 allowRetryDialog = 1,
    H2_ENUM_PARAM(RemoteMessageType, i8) messageType = REMOTE_MESSAGE_DEFAULT
);
char* GetRemoteData(i8 remove);
void PollRemote(void);
i32 TransmitAndWait(char* bytes, i32 destination, i32 length, i8 command, i8 responseCommand, char** response);

extern bchar gbUseDiffCompression;
extern bchar gbUseRegularCompression;
extern SNetPlayerInfo gsNetPlayerInfo[GAME_PLAYER_COUNT];

extern i32 iInOrderCtr;
extern i32 iCurLastID;
extern i32 giLastConfirm;
extern H2_ENUM_STORAGE(RemoteGameMode, u8) GameMode;
extern i32l lLastHeartbeatSend;
extern b32 gbInRemoteMain;
extern b32 gbInRemoteCleanup;
extern i32 iIDCtr;
extern i32 iTimesDropped;
extern b8 gbInNetSetup;
extern b32 bUseDirectPlay;
extern b32 bUseWinsock;
extern b8 bInTimeoutFail;
extern i32 iBaud[REMOTE_BAUD_RATE_COUNT];
extern i32 iIRQ[REMOTE_IRQ_COUNT];
extern char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
extern i32 iLastIds[REMOTE_RECENT_ID_COUNT];
extern char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
extern i32 iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
extern char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char gcThisNetName[REMOTE_NET_NAME_SIZE];
extern i32l lLastHeartbeatReceive[GAME_PLAYER_COUNT];
extern char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char* rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
extern b32 bGotGameType;
extern SNetPlayerInfo gsThisNetPlayerInfo;

#endif // HOMM2_SOURCE_REMOTE_H
