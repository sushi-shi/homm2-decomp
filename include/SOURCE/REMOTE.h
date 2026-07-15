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
    REMOTE_CONFIRM_POLL_DELAY = 20,
    REMOTE_SEND_RETRY_DELAY = 1000,
    REMOTE_HEARTBEAT_INTERVAL = 5000,
    REMOTE_HOST_TIMEOUT = 15000,
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

typedef enum RemoteBaseType {
    REMOTE_BASE_MODEM = 0,
    REMOTE_BASE_NETWORK = 1
} RemoteBaseType;

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
struct RemoteMessage {
    signed char sender;
    int id;
    signed char type;
    signed char command;
    unsigned short payloadSize;
    unsigned char payload[REMOTE_MESSAGE_PAYLOAD_SIZE];
};
#pragma pack(pop)
SIZE(RemoteMessage, REMOTE_MESSAGE_SIZE);

void RemoteCleanup(void);
void RemoteMain(int);
void UnloadRemoteDriver(short int);
int calc_crc_long(unsigned char *, int);
void calc_crc(unsigned short int *, unsigned char *, int);
int EncodePacket(unsigned char *, char, char, int);
int DecodePacket(unsigned char *, int);
int SendRemoteData(unsigned char *, unsigned char *, int, int);
int ReceiveRemoteData(unsigned char *, unsigned char *, int);
int TransmitRemoteData(char *, int, int, signed char, signed char, signed char, signed char);
char * GetRemoteData(signed char);
void PollRemote(void);
int TransmitAndWait(char *, int, int, signed char, signed char, char * *);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern char gbUseDiffCompression;
extern char gbUseRegularCompression;
extern SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];

// ---- globals (declarations, RVA order) ----
extern int iInOrderCtr;
extern int iCurLastID;
extern int giLastConfirm;
extern unsigned char GameMode;
extern long lLastHeartbeatSend;
extern int gbInRemoteMain;
extern int gbInRemoteCleanup;
extern int iIDCtr;
extern int iTimesDropped;
extern signed char gbInNetSetup;
extern int bUseDirectPlay;
extern int bUseWinsock;
extern signed char bInTimeoutFail;
extern int *iBaud;
extern int *iIRQ;
extern char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
extern int iLastIds[REMOTE_RECENT_ID_COUNT];
extern char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
extern int iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
extern char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char gcThisNetName[32];
extern long lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
extern char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
extern char *rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
extern int bGotGameType;
extern SNetPlayerInfo gsThisNetPlayerInfo;

#endif // HOMM2_REMOTE_H
