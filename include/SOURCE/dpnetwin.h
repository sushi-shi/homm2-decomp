#ifndef HOMM2_DPNETWIN_H
#define HOMM2_DPNETWIN_H

#include <Ints.h>
#include <windows.h>
#include <dplay.h>
#include <SOURCE/REMOTE_TYPES.h>
#include <BASE/Misc.h>
#include <string.h>

H2_ENUM_BEGIN(DirectPlayTransportConstant)
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
H2_ENUM_END(DirectPlayTransportConstant)

#pragma pack(push, 1)
struct DirectPlayStartupMessage {
    i8 playerCount;
    i8 netPosition;
    i32 playerIds[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
};

H2_ENUM_CLASS_BEGIN(DirectPlayHostAcceptStatus)
    HOST_ACCEPT_PENDING  = 0,
    HOST_ACCEPT_ACCEPTED = 1,
    HOST_ACCEPT_REJECTED = 2
H2_ENUM_CLASS_END(DirectPlayHostAcceptStatus)

H2_ENUM_CLASS_BEGIN(DirectPlayFirstGuestState)
    FIRST_GUEST_CREATE_SESSION      = 0,
    FIRST_GUEST_DISABLE_COMPRESSION = 1,
    FIRST_GUEST_CREATE_PLAYER       = 2,
    FIRST_GUEST_WAIT_FOR_PLAYER     = 3
H2_ENUM_CLASS_END(DirectPlayFirstGuestState)
H2_ENUM_STEPPED(DirectPlayFirstGuestState)

H2_ENUM_CLASS_BEGIN(DirectPlayHostState)
    HOST_ENUMERATE_SESSIONS = 0,
    HOST_JOIN_SESSION       = 1,
    HOST_CREATE_PLAYER      = 2,
    HOST_ANNOUNCE_PLAYER    = 3,
    HOST_WAIT_FOR_ACCEPT    = 4,
    HOST_WAIT_FOR_STARTUP   = 5
H2_ENUM_CLASS_END(DirectPlayHostState)
H2_ENUM_STEPPED(DirectPlayHostState)
#pragma pack(pop)
SIZE(DirectPlayStartupMessage, 0x1a);

BOOL WINAPI dpEnumServiceProvider(struct _GUID* guid, char* name, DWORD, DWORD, void*);
BOOL WINAPI dpEnumSession(DPSESSIONDESC* session, void*, LPDWORD, DWORD flags);
i16 dpnet_init(void);
void CleanupDPVars(void);
void dpnet_term(void);
void dpSendMessage(i32 destination, H2_ENUM_PARAM(NetworkPacketType, u8) type, u16 size, void* data);
i32 dpnet_snd(i32 position, i32 size, void* data);
i16 dpnet_rcv(i16, u16, void* data);
u8 dpnet_stat(i16, u16);
i16 __cdecl dpnet_sess(i32, i32, ...);
void dpProcessMessages(void);
void dpEvaluateMessage(u32l size, i32 sender);
i32 dpWaitForFirstGuest(void);
i32 dpWaitForExtraGuests(void);
i32 dpWaitForHost(void);
void DPSD(i32 result, H2_CONST char* file, i32 line);

extern struct IDirectPlay* lpIDC;
extern DPID dcoID;
extern struct _GUID* IPXGuid;
extern struct _GUID* TCPGuid;
extern HANDLE dphEvent;
extern i32 iDPRcvBufferHead;
extern i32 iDPRcvBufferTail;
extern u8** ppDPRcvBuffer;
extern i32* piDPRcvBufferSize;
extern b32 bStartUpInfoReceived;
extern HMODULE hinstDplayx;
extern H2_ENUM_STORAGE_STEPPED(DirectPlayFirstGuestState, i32) iDPWaitForFirstGuestStatus;
extern H2_ENUM_STORAGE_STEPPED(DirectPlayHostState, i32) iDPWaitForHostStatus;
extern i32 iWaitForHostWaitCount;
extern i32 iEnumCount;
extern i32 iLastHereIAmTickCount;
extern b32 bInDPSD;
extern i32 iGUIDCount;
extern i32 iLastMsgNumHumanPlayers;
extern i32 iMaxSession;
extern DirectPlayHostAcceptStatus giHostAcceptStatus;
extern struct _GUID* g_lpGuid;
extern i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
extern i32 iSessionToTry;
extern i32l lSessions[DP_TRANSPORT_SESSION_COUNT];

// Shared with Winsock. These do not reset indices or validate allocations/lengths.
#define INIT_TRANSPORT_RECEIVE_STORAGE()                                                           \
    (ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*))),         \
     piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(DP_TRANSPORT_BUFFER_COUNT * sizeof(i32))),     \
     memset(ppDPRcvBuffer, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*)),                            \
     memset(piDPRcvBufferSize, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(i32)))
// Stored size includes the omitted tag byte; preserve that legacy discrepancy.
#define ENQUEUE_TRANSPORT_PACKET(input, size)                                                      \
    (ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC((size) - 1)),                     \
     memcpy(ppDPRcvBuffer[iDPRcvBufferHead], (input) + 1, (size) - 1),                             \
     piDPRcvBufferSize[iDPRcvBufferHead] = (size),                                                 \
     iDPRcvBufferHead = (iDPRcvBufferHead + 1) % DP_TRANSPORT_BUFFER_COUNT)

// Arrays only: backend-specific packet draining and index reset stay outside.
inline void DisposeTransportReceiveStorage(void) {
    if (ppDPRcvBuffer != NULL)
        H2_FREE(ppDPRcvBuffer);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize);
    piDPRcvBufferSize = NULL;
}

#endif
