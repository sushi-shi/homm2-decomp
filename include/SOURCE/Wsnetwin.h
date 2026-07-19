#ifndef HOMM2_WSNETWIN_H
#define HOMM2_WSNETWIN_H
#include <va.h>
#include <SOURCE/GAME.h>
#include <winsock.h>
#include <SOURCE/REMOTE_TYPES.h>

H2_ENUM_BEGIN(WinsockTransportConstant)
    WS_TRANSPORT_BUFFER_COUNT = 200,
    WS_TRANSPORT_BUFFER_SIZE = 0x100,
    WS_TRANSPORT_BROADCAST_POSITION = 0x7f,
    WS_TRANSPORT_PLAYER_COUNT = IDX(GAME_PLAYER_COUNT),
    WS_TRANSPORT_PORT = 2000,
    WS_TRANSPORT_SEND_RETRY_DELAY = 300,
    WS_TRANSPORT_HOST_RETRY_DELAY = 3000,
    WS_TRANSPORT_HOST_RETRY_LIMIT = 15,
    WS_TRANSPORT_ADDRESS_SIZE = 16
H2_ENUM_END(WinsockTransportConstant)

H2_ENUM_CLASS_BEGIN(WinsockWaitType)
    WS_WAIT_FIRST_GUEST = 11,
    WS_WAIT_EXTRA_GUESTS = 12,
    WS_WAIT_HOST = 13
H2_ENUM_CLASS_END(WinsockWaitType)

#pragma pack(push, 1)
struct WinsockStartupMessage {
    u8 playerCount;
    u8 netPosition;
    i32 playerAddresses[WS_TRANSPORT_PLAYER_COUNT];
};
#pragma pack(pop)
SIZE(WinsockStartupMessage, 0x1a);

i16 wsnet_init(void);
void wsnet_term(void);
void wsSendMessage(i32, u8, u16, void*);
i32 wsnet_snd(i32, i32, void*);
i16 wsnet_rcv(i16, u16, void*);
void wsProcessMessages(void);
void wsEvaluateMessage(u32l, i32);
i32 wsWaitForFirstGuest(void);
i32 wsWaitForExtraGuests(void);
i32 wsWaitForHost(void);

extern i32 bHostFound;
extern u32 sd_dg;
extern i32 iWSLastMsgNumHumanPlayers;
extern i32 iWSAttempts;
extern i32 iWSNextTickCount;
extern i32 iWSWaitForHostStatus;
extern i32 iRc;
extern char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
extern struct sockaddr_in saddr_loc;
extern u16 wVer;
extern struct WSAData wsadata;
extern struct in_addr gIn_addrIP;
extern struct sockaddr_in saddr_remote;
extern i32 iAddrLen;

#endif
