#ifndef HOMM2_DPNETWIN_H
#define HOMM2_DPNETWIN_H

#include <Ints.h>
// Declarations of the free functions DEFINED in dpnetwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <windows.h>
#include <dplay.h>
#include <SOURCE/REMOTE_TYPES.h>
// forward declarations (was <_all.h>):

HOMM2_ENUM_VALUES_BEGIN(DirectPlayTransportConstant)
    DP_TRANSPORT_BUFFER_COUNT = 200,
    DP_TRANSPORT_RECEIVE_SIZE = 0x100,
    DP_TRANSPORT_BROADCAST_POSITION = 0x7f,
    DP_TRANSPORT_STARTUP_MAPPING_COUNT = 6,
    DP_TRANSPORT_TERM_DRAIN_READ_SIZE = 100,
    DP_TRANSPORT_SESSION_DESC_SIZE = 0x7c,
    DP_TRANSPORT_MAX_PLAYERS = 6,
    DP_TRANSPORT_ENUM_SHORT_TIMEOUT = 2000,
    DP_TRANSPORT_ENUM_LONG_TIMEOUT = 5000,
    DP_TRANSPORT_ACCEPT_TIMEOUT = 5000,
    DP_TRANSPORT_ENUM_LONG_INTERVAL = 8,
    DP_TRANSPORT_ENUM_LONG_PHASE = 4,
    DP_TRANSPORT_RETRY_WAIT_COUNT = 30,
    DP_TRANSPORT_RETRY_DELAY = 100
HOMM2_ENUM_VALUES_END(DirectPlayTransportConstant)

HOMM2_ENUM_BEGIN(DirectPlayProtocol)
    DP_PROTOCOL_IPX = 1,
    DP_PROTOCOL_TCP = 2
HOMM2_ENUM_END(DirectPlayProtocol)

HOMM2_ENUM_BEGIN(DirectPlaySessionOpenFlag)
    DP_SESSION_OPEN_JOIN = 1,
    DP_SESSION_OPEN_CREATE = 2
HOMM2_ENUM_END(DirectPlaySessionOpenFlag)

HOMM2_ENUM_BEGIN(DirectPlayWaitType)
    DP_WAIT_FIRST_GUEST = 8,
    DP_WAIT_EXTRA_GUESTS = 9,
    DP_WAIT_HOST = 10
HOMM2_ENUM_END(DirectPlayWaitType)

HOMM2_ENUM_BEGIN(DirectPlayResult)
    DP_RESULT_OK = 0,
    DP_RESULT_INVALID_ARGUMENT = static_cast<i32>(0x80070057),
    DP_RESULT_INVALID_PLAYER = static_cast<i32>(0x88770096),
    DP_RESULT_NO_MESSAGES = static_cast<i32>(0x887700be),
    DP_RESULT_NO_SESSIONS = static_cast<i32>(0x887700dc)
HOMM2_ENUM_END(DirectPlayResult)

#pragma pack(push, 1)
struct DirectPlayStartupMessage {
    u8 playerCount;
    u8 netPosition;
    i32 playerIds[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
};
#pragma pack(pop)
SIZE(DirectPlayStartupMessage, 0x1a);

BOOL WINAPI dpEnumServiceProvider(struct _GUID*, char*, DWORD, DWORD, void*);
BOOL WINAPI dpEnumSession(DPSESSIONDESC*, void*, LPDWORD, DWORD);
i16 dpnet_init(void);
void CleanupDPVars(void);
void dpnet_term(void);
void dpSendMessage(i32, u8, u16, void*);
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

// ---- globals (declarations, RVA order) ----
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
extern i32 iDPWaitForFirstGuestStatus;
extern i32 iDPWaitForHostStatus;
extern i32 iWaitForHostWaitCount;
extern i32 iEnumCount;
extern i32 iLastHereIAmTickCount;
extern i32 bInDPSD;
extern i32 iGUIDCount;
extern i32 iLastMsgNumHumanPlayers;
extern i32 iMaxSession;
extern i32 giHostAcceptStatus;
extern struct _GUID* g_lpGuid;
extern i32 giNetPosToDCOPos[6];
extern i32 iSessionToTry;
extern i32l lSessions[10];

#endif // HOMM2_DPNETWIN_H
