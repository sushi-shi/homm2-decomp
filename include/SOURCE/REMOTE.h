#ifndef HOMM2_REMOTE_H
#define HOMM2_REMOTE_H

#include <Ints.h>
#include <SOURCE/GAME.h>
#include <SOURCE/REMOTE_TYPES.h>

typedef enum RemoteConstant {
    REMOTE_PLAYER_COUNT                  = (GAME_PLAYER_COUNT),
    REMOTE_QUEUE_CAPACITY                = 128,
    REMOTE_QUEUE_STORAGE_COUNT           = 138,
    REMOTE_RECENT_ID_COUNT               = 30,
    REMOTE_NET_NAME_SIZE                 = 32,
    REMOTE_ENCODED_BUFFER_SIZE           = 268,
    REMOTE_RECEIVE_BUFFER_SIZE           = 266,
    REMOTE_TRANSPORT_BUFFER_SIZE         = 268,
    REMOTE_MESSAGE_SIZE                  = 256,
    REMOTE_MESSAGE_PAYLOAD_SIZE          = 247,
    REMOTE_PACKET_HEADER_SIZE            = 6,
    REMOTE_MESSAGE_HEADER_SIZE           = 9,
    REMOTE_BROADCAST_PLAYER              = 0x7f,
    REMOTE_RETRY_COUNT                   = 25,
    REMOTE_CONFIRM_POLL_COUNT            = 50,
    REMOTE_BAUD_RATE_COUNT               = 7,
    REMOTE_IRQ_COUNT                     = 7,
    REMOTE_CONFIRM_POLL_DELAY            = 20,
    REMOTE_SEND_RETRY_DELAY              = 1000,
    REMOTE_HEARTBEAT_INTERVAL            = 5000,
    REMOTE_HEARTBEAT_MESSAGE_SIZE        = 10,
    REMOTE_HEARTBEAT_CONTROL_FLAG        = 0x80,
    REMOTE_HEARTBEAT_PLAYER_SHIFT        = 4,
    REMOTE_HEARTBEAT_PHASE_MASK          = 0x0f,
    REMOTE_ERROR_TEXT_SIZE               = 200,
    REMOTE_HOST_TIMEOUT                  = 60000,
    REMOTE_CHAIN_GUEST_TIMEOUT_INCREMENT = 30000,
    REMOTE_GUEST_TIMEOUT                 = 60000,
    REMOTE_CHAIN_TIMEOUT                 = 90000,
    REMOTE_INITIAL_HEARTBEAT             = 1999999999,
    REMOTE_ORDER_SENTINEL                = 999999999
} RemoteConstant;

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
    i8 type;
    i8 command;
    i16 payloadSize;
    char payload[REMOTE_MESSAGE_PAYLOAD_SIZE];
};
#pragma pack(pop)

void RemoteCleanup(void);
void RemoteMain(RemoteGameMode);
void UnloadRemoteDriver(i16);
i32 calc_crc_long(u8*, i32);
void calc_crc(u16*, u8*, i32);
i32 EncodePacket(u8*, char, char, i32);
i32 DecodePacket(u8*, i32);
i32 SendRemoteData(u8*, u8*, i32, i32);
i32 ReceiveRemoteData(u8*, u8*, i32);
i32 TransmitRemoteData(
    char*,
    i32,
    i32,
    i8,
    i8,
    i8,
    RemoteMessageType
);
char* GetRemoteData(i8);
void PollRemote(void);
i32 TransmitAndWait(char*, i32, i32, i8, i8, char**);

extern bchar gbUseDiffCompression;
extern bchar gbUseRegularCompression;
extern SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];

extern i32 iInOrderCtr;
extern i32 iCurLastID;
extern i32 giLastConfirm;
extern u8 GameMode;
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
extern i32l lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
extern char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char* rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
extern b32 bGotGameType;
extern SNetPlayerInfo gsThisNetPlayerInfo;

#endif
