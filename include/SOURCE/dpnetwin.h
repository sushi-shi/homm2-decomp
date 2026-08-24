#ifndef HOMM2_DPNETWIN_H
#define HOMM2_DPNETWIN_H

#include <Ints.h>
#include <windows.h>
#include <dplay.h>
#include <SOURCE/REMOTE_TYPES.h>

typedef enum DirectPlayTransportConstant {
    DP_TRANSPORT_BUFFER_COUNT          = 200,
    DP_TRANSPORT_RECEIVE_SIZE          = 0x100,
    DP_TRANSPORT_BROADCAST_POSITION    = 0x7f,
    DP_TRANSPORT_STARTUP_MAPPING_COUNT = 6,
    DP_TRANSPORT_SESSION_COUNT         = 10,
    DP_TRANSPORT_TERM_DRAIN_READ_SIZE  = 100,
    DP_TRANSPORT_SESSION_DESC_SIZE     = 0x7c,
    DP_TRANSPORT_MAX_PLAYERS           = 6,
    DP_TRANSPORT_ENUM_SHORT_TIMEOUT    = 2000,
    DP_TRANSPORT_ENUM_LONG_TIMEOUT     = 5000,
    DP_TRANSPORT_ACCEPT_TIMEOUT        = 5000,
    DP_TRANSPORT_ENUM_LONG_INTERVAL    = 8,
    DP_TRANSPORT_ENUM_LONG_PHASE       = 4,
    DP_TRANSPORT_RETRY_WAIT_COUNT      = 30,
    DP_TRANSPORT_RETRY_DELAY           = 100
} DirectPlayTransportConstant;

#pragma pack(push, 1)
struct DirectPlayStartupMessage {
    char playerCount;
    char netPosition;
    i32 playerIds[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
};

enum class DirectPlayHostAcceptStatus : i32 {
    HOST_ACCEPT_PENDING  = 0,
    HOST_ACCEPT_ACCEPTED = 1,
    HOST_ACCEPT_REJECTED = 2
};
using enum DirectPlayHostAcceptStatus;

enum class DirectPlayFirstGuestState : i32 {
    FIRST_GUEST_CREATE_SESSION      = 0,
    FIRST_GUEST_DISABLE_COMPRESSION = 1,
    FIRST_GUEST_CREATE_PLAYER       = 2,
    FIRST_GUEST_WAIT_FOR_PLAYER     = 3
};
using enum DirectPlayFirstGuestState;
ENABLE_ENUM_STEPS(DirectPlayFirstGuestState)

enum class DirectPlayHostState : i32 {
    HOST_ENUMERATE_SESSIONS = 0,
    HOST_JOIN_SESSION       = 1,
    HOST_CREATE_PLAYER      = 2,
    HOST_ANNOUNCE_PLAYER    = 3,
    HOST_WAIT_FOR_ACCEPT    = 4,
    HOST_WAIT_FOR_STARTUP   = 5
};
using enum DirectPlayHostState;
ENABLE_ENUM_STEPS(DirectPlayHostState)
#pragma pack(pop)

BOOL WINAPI dpEnumServiceProvider(struct _GUID*, char*, DWORD, DWORD, void*);
BOOL WINAPI dpEnumSession(DPSESSIONDESC*, void*, LPDWORD, DWORD);
i16 dpnet_init(void);
void CleanupDPVars(void);
void dpnet_term(void);
void dpSendMessage(i32, NetworkPacketType, u16, void*);
i32 dpnet_snd(i32, i32, void*);
i16 dpnet_rcv(i16, u16, void*);
u8 dpnet_stat(i16, u16);
i16 __cdecl dpnet_sess(i32, i32, ...);
void dpProcessMessages(void);
void dpEvaluateMessage(u32l, i32);
i32 dpWaitForFirstGuest(void);
i32 dpWaitForExtraGuests(void);
i32 dpWaitForHost(void);
void DPSD(i32, char*, i32);

extern struct IDirectPlay* lpIDC;
extern DPID dcoID;
extern struct _GUID* IPXGuid;
extern struct _GUID* TCPGuid;
extern HANDLE dphEvent;
extern i32 iDPRcvBufferHead;
extern i32 iDPRcvBufferTail;
extern u8** ppDPRcvBuffer;
extern i32* piDPRcvBufferSize;
extern i32 bStartUpInfoReceived;
extern HMODULE hinstDplayx;
extern H2SteppedEnumStorage<DirectPlayFirstGuestState, i32> iDPWaitForFirstGuestStatus;
extern H2SteppedEnumStorage<DirectPlayHostState, i32> iDPWaitForHostStatus;
extern i32 iWaitForHostWaitCount;
extern i32 iEnumCount;
extern i32 iLastHereIAmTickCount;
extern i32 bInDPSD;
extern i32 iGUIDCount;
extern i32 iLastMsgNumHumanPlayers;
extern i32 iMaxSession;
extern DirectPlayHostAcceptStatus giHostAcceptStatus;
extern struct _GUID* g_lpGuid;
extern i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
extern i32 iSessionToTry;
extern i32l lSessions[DP_TRANSPORT_SESSION_COUNT];

#endif
