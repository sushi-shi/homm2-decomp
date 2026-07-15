#ifndef HOMM2_DPNETWIN_H
#define HOMM2_DPNETWIN_H
// Declarations of the free functions DEFINED in dpnetwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <windows.h>
#include <dplay.h>
// forward declarations (was <_all.h>):

typedef enum DirectPlayTransportConstant {
    DP_TRANSPORT_BUFFER_COUNT = 200,
    DP_TRANSPORT_RECEIVE_SIZE = 0x100,
    DP_TRANSPORT_BROADCAST_POSITION = 0x7f,
    DP_TRANSPORT_STARTUP_MAPPING_COUNT = 6,
    DP_TRANSPORT_TERM_DRAIN_SIZE = 104,
    DP_TRANSPORT_SESSION_DESC_SIZE = 0x7c,
    DP_TRANSPORT_MAX_PLAYERS = 6,
    DP_TRANSPORT_ENUM_SHORT_TIMEOUT = 2000,
    DP_TRANSPORT_ENUM_LONG_TIMEOUT = 5000,
    DP_TRANSPORT_ACCEPT_TIMEOUT = 5000,
    DP_TRANSPORT_ENUM_LONG_INTERVAL = 8,
    DP_TRANSPORT_ENUM_LONG_PHASE = 4,
    DP_TRANSPORT_RETRY_WAIT_COUNT = 30,
    DP_TRANSPORT_RETRY_DELAY = 100
} DirectPlayTransportConstant;

typedef enum DirectPlayProtocol {
    DP_PROTOCOL_IPX = 1,
    DP_PROTOCOL_TCP = 2
} DirectPlayProtocol;

typedef enum DirectPlaySessionOpenFlag {
    DP_SESSION_OPEN_JOIN = 1,
    DP_SESSION_OPEN_CREATE = 2
} DirectPlaySessionOpenFlag;

typedef enum DirectPlayWaitType {
    DP_WAIT_FIRST_GUEST = 8,
    DP_WAIT_EXTRA_GUESTS = 9,
    DP_WAIT_HOST = 10
} DirectPlayWaitType;

typedef enum DirectPlayMessageType {
    DP_MESSAGE_DATA = 1,
    DP_MESSAGE_GUEST_ARRIVED = 2,
    DP_MESSAGE_STARTUP = 3,
    DP_MESSAGE_GUEST_ACCEPTED = 4,
    DP_MESSAGE_GUEST_REJECTED = 5
} DirectPlayMessageType;

typedef enum DirectPlayResult {
    DP_RESULT_OK = 0,
    DP_RESULT_INVALID_ARGUMENT = static_cast<int>(0x80070057),
    DP_RESULT_INVALID_PLAYER = static_cast<int>(0x88770096),
    DP_RESULT_NO_MESSAGES = static_cast<int>(0x887700be),
    DP_RESULT_NO_SESSIONS = static_cast<int>(0x887700dc)
} DirectPlayResult;

#pragma pack(push, 1)
struct DirectPlayStartupMessage {
    unsigned char playerCount;
    unsigned char netPosition;
    int playerIds[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
};
#pragma pack(pop)
SIZE(DirectPlayStartupMessage, 0x1a);

int __stdcall dpEnumServiceProvider(struct _GUID *, char *, unsigned long int, unsigned long int, void *);
int __stdcall dpEnumSession(DPSESSIONDESC *, void *, unsigned long int *, unsigned long int);
short int dpnet_init(void);
void CleanupDPVars(void);
void dpnet_term(void);
void dpSendMessage(int, unsigned char, unsigned short int, void *);
int dpnet_snd(int, int, void *);
short int dpnet_rcv(short int, unsigned short int, void *);
unsigned char dpnet_stat(short int, unsigned short int);
short int __cdecl dpnet_sess(int, int, ...);
void dpProcessMessages(void);
void dpEvaluateMessage(unsigned long int, int);
int dpWaitForFirstGuest(void);
int dpWaitForExtraGuests(void);
int dpWaitForHost(void);
void DPSD(int, char *, int);

// ---- globals (declarations, RVA order) ----
extern struct IDirectPlay *lpIDC;
extern unsigned long dcoID;
extern struct _GUID *IPXGuid;
extern struct _GUID *TCPGuid;
extern HANDLE dphEvent;
extern int iDPRcvBufferHead;
extern int iDPRcvBufferTail;
extern unsigned char **ppDPRcvBuffer;
extern int *piDPRcvBufferSize;
extern int bStartUpInfoReceived;
extern HMODULE hinstDplayx;
extern int iDPWaitForFirstGuestStatus;
extern int iDPWaitForHostStatus;
extern int iWaitForHostWaitCount;
extern int iEnumCount;
extern int iLastHereIAmTickCount;
extern int bInDPSD;
extern int iGUIDCount;
extern int iLastMsgNumHumanPlayers;
extern int iMaxSession;
extern int giHostAcceptStatus;
extern struct _GUID *g_lpGuid;
extern int giNetPosToDCOPos[6];
extern int iSessionToTry;
extern long lSessions[10];

#endif // HOMM2_DPNETWIN_H
