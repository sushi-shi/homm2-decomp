#ifndef HOMM2_PLATFORM_WIN32_WINSOCKTRANSPORT_H
#define HOMM2_PLATFORM_WIN32_WINSOCKTRANSPORT_H

#include <Ints.h>
#include <SOURCE/GAME.h>
#include <PLATFORM/Sockets.h>
#include <SOURCE/REMOTE_TYPES.h>

typedef enum WinsockTransportConstant {
    WS_TRANSPORT_BUFFER_COUNT       = 200,
    WS_TRANSPORT_BUFFER_SIZE        = 0x100,
    WS_TRANSPORT_BROADCAST_POSITION = 0x7f,
    WS_TRANSPORT_PLAYER_COUNT       = H2EnumIndex(GAME_PLAYER_COUNT),
    WS_TRANSPORT_PORT               = 2000,
    WS_TRANSPORT_SEND_RETRY_DELAY   = 300,
    WS_TRANSPORT_HOST_RETRY_DELAY   = 3000,
    WS_TRANSPORT_HOST_RETRY_LIMIT   = 15,
    WS_TRANSPORT_ADDRESS_SIZE       = 16
} WinsockTransportConstant;

#pragma pack(push, 1)
struct WinsockStartupMessage {
    u8 playerCount;
    u8 netPosition;
    i32 playerAddresses[WS_TRANSPORT_PLAYER_COUNT];
};
#pragma pack(pop)

i16 wsnet_init(void);
void wsnet_term(void);
void wsSendMessage(i32, NetworkPacketType, u16, void*);
i32 wsnet_snd(i32, i32, void*);
i16 wsnet_rcv(i16, u16, void*);
void wsProcessMessages(void);
void wsEvaluateMessage(u32l, i32);
i32 wsWaitForFirstGuest(void);
i32 wsWaitForExtraGuests(void);
i32 wsWaitForHost(void);

extern i32 bHostFound;
inline constexpr i32 WS_TRANSPORT_PLAYER_LIMIT = 6;

extern i32 giNetPosToDCOPos[WS_TRANSPORT_PLAYER_LIMIT];
extern platform::Socket sd_dg;
extern i32 iDPRcvBufferHead;
extern i32 iDPRcvBufferTail;
extern u8** ppDPRcvBuffer;
extern i32* piDPRcvBufferSize;
extern i32 bStartUpInfoReceived;
void CleanupDPVars(void);
extern i32 iWSLastMsgNumHumanPlayers;
extern i32 iWSAttempts;
extern i32 iWSNextTickCount;
extern i32 iWSWaitForHostStatus;
extern i32 iRc;
extern char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
extern u32l gIn_addrIP;
extern i32 iAddrLen;

#endif
