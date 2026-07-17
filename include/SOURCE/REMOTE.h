#ifndef HOMM2_REMOTE_H
#define HOMM2_REMOTE_H
#include <va.h>
#include <SOURCE/REMOTE_TYPES.h>
// Declarations of the free functions DEFINED in REMOTE.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

typedef enum RemoteConstant {
    REMOTE_PLAYER_COUNT = 6,
    REMOTE_QUEUE_CAPACITY = 128,
    REMOTE_QUEUE_STORAGE_COUNT = 138,
    REMOTE_RECENT_ID_COUNT = 30,
    REMOTE_ENCODED_BUFFER_SIZE = 268,
    REMOTE_RECEIVE_BUFFER_SIZE = 266,
    REMOTE_TRANSPORT_BUFFER_SIZE = 272,
    REMOTE_MESSAGE_SIZE = 256,
    REMOTE_MESSAGE_PAYLOAD_SIZE = 247,
    REMOTE_PACKET_HEADER_SIZE = 6,
    REMOTE_MESSAGE_HEADER_SIZE = 9,
    REMOTE_BROADCAST_PLAYER = 0x7f,
    REMOTE_RETRY_COUNT = 25,
    REMOTE_CONFIRM_POLL_COUNT = 50,
    REMOTE_BAUD_RATE_COUNT = 8,
    REMOTE_IRQ_COUNT = 7,
    REMOTE_CONFIRM_POLL_DELAY = 20,
    REMOTE_SEND_RETRY_DELAY = 1000,
    REMOTE_HEARTBEAT_INTERVAL = 5000,
    REMOTE_HEARTBEAT_MESSAGE_SIZE = 10,
    REMOTE_HEARTBEAT_CONTROL_FLAG = 0x80,
    REMOTE_HEARTBEAT_PHASE_MASK = 0x0f,
    REMOTE_HOST_TIMEOUT = 15000,
    REMOTE_CHAIN_GUEST_TIMEOUT_INCREMENT = 30000,
    REMOTE_GUEST_TIMEOUT = 60000,
    REMOTE_CHAIN_TIMEOUT = 90000,
    REMOTE_INITIAL_HEARTBEAT = 1999999999,
    REMOTE_ORDER_SENTINEL = 999999999
} RemoteConstant;

typedef enum RemoteGameMode {
    REMOTE_GAME_NETWORK_HOST = 1,
    REMOTE_GAME_NETWORK_GUEST = 2,
    REMOTE_GAME_MODEM_HOST = 3,
    REMOTE_GAME_MODEM_GUEST = 4
} RemoteGameMode;

typedef enum RemoteNetworkProtocol {
    REMOTE_PROTOCOL_NETBIOS = 0,
    REMOTE_PROTOCOL_DIRECT_PLAY = 1,
    REMOTE_PROTOCOL_WINSOCK = 2
} RemoteNetworkProtocol;

typedef enum RemoteMessageType {
    REMOTE_MESSAGE_CONFIRM = 1,
    REMOTE_MESSAGE_RELIABLE = 2,
    REMOTE_MESSAGE_UNRELIABLE = 3,
    REMOTE_MESSAGE_HEARTBEAT = 4
} RemoteMessageType;

typedef enum RemoteSetupCommand {
    REMOTE_SETUP_PLAYER_INFO = 0x22,
    REMOTE_SETUP_STANDARD_GAME = 0x3d,
    REMOTE_SETUP_CAMPAIGN_GAME = 0x3e
} RemoteSetupCommand;

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
    u16 payloadSize;
    u8 payload[REMOTE_MESSAGE_PAYLOAD_SIZE];
};
#pragma pack(pop)
SIZE(RemotePacketHeader, REMOTE_PACKET_HEADER_SIZE);
SIZE(RemoteMessage, REMOTE_MESSAGE_SIZE);

void RemoteCleanup(void);
void RemoteMain(i32);
void UnloadRemoteDriver(i16);
i32 calc_crc_long(u8 *, i32);
void calc_crc(u16 *, u8 *, i32);
i32 EncodePacket(u8 *, char, char, i32);
i32 DecodePacket(u8 *, i32);
i32 SendRemoteData(u8 *, u8 *, i32, i32);
i32 ReceiveRemoteData(u8 *, u8 *, i32);
i32 TransmitRemoteData(char *, i32, i32, i8, i8, i8, i8);
char * GetRemoteData(i8);
void PollRemote(void);
i32 TransmitAndWait(char *, i32, i32, i8, i8, char * *);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern char gbUseDiffCompression;
extern char gbUseRegularCompression;
extern SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];

// ---- globals (declarations, RVA order) ----
extern i32 iInOrderCtr;
extern i32 iCurLastID;
extern i32 giLastConfirm;
extern u8 GameMode;
extern i32l lLastHeartbeatSend;
extern i32 gbInRemoteMain;
extern i32 gbInRemoteCleanup;
extern i32 iIDCtr;
extern i32 iTimesDropped;
extern i8 gbInNetSetup;
extern i32 bUseDirectPlay;
extern i32 bUseWinsock;
extern i8 bInTimeoutFail;
extern i32 iBaud[REMOTE_BAUD_RATE_COUNT];
extern i32 iIRQ[REMOTE_IRQ_COUNT];
extern char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
extern i32 iLastIds[REMOTE_RECENT_ID_COUNT];
extern char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
extern i32 iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
extern char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char gcThisNetName[32];
extern i32l lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
extern char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char *rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
extern i32 bGotGameType;
extern SNetPlayerInfo gsThisNetPlayerInfo;

#endif // HOMM2_REMOTE_H
