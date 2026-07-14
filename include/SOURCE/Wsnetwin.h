#ifndef HOMM2_WSNETWIN_H
#define HOMM2_WSNETWIN_H
// Declarations of the free functions DEFINED in Wsnetwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <va.h>
#include <winsock.h>

typedef enum WinsockTransportConstant {
    WS_TRANSPORT_BUFFER_COUNT = 200,
    WS_TRANSPORT_BUFFER_SIZE = 0x100,
    WS_TRANSPORT_BROADCAST_POSITION = 0x7f,
    WS_TRANSPORT_PLAYER_COUNT = 6,
    WS_TRANSPORT_PORT = 2000,
    WS_TRANSPORT_SEND_RETRY_DELAY = 300,
    WS_TRANSPORT_HOST_RETRY_DELAY = 3000,
    WS_TRANSPORT_HOST_RETRY_LIMIT = 15,
    WS_TRANSPORT_ADDRESS_SIZE = 16
} WinsockTransportConstant;

typedef enum WinsockMessageType {
    WS_MESSAGE_DATA = 1,
    WS_MESSAGE_GUEST_ARRIVED = 2,
    WS_MESSAGE_STARTUP = 3,
    WS_MESSAGE_GUEST_ACCEPTED = 4,
    WS_MESSAGE_GUEST_REJECTED = 5
} WinsockMessageType;

typedef enum WinsockWaitType {
    WS_WAIT_FIRST_GUEST = 11,
    WS_WAIT_EXTRA_GUESTS = 12,
    WS_WAIT_HOST = 13
} WinsockWaitType;

#pragma pack(push, 1)
struct WinsockStartupMessage {
    unsigned char playerCount;
    unsigned char netPosition;
    int playerAddresses[WS_TRANSPORT_PLAYER_COUNT];
};
#pragma pack(pop)
SIZE(WinsockStartupMessage, 0x1a);

short int wsnet_init(void);
void wsnet_term(void);
void wsSendMessage(int, unsigned char, unsigned short int, void *);
int wsnet_snd(int, int, void *);
short int wsnet_rcv(short int, unsigned short int, void *);
void wsProcessMessages(void);
void wsEvaluateMessage(unsigned long int, int);
int wsWaitForFirstGuest(void);
int wsWaitForExtraGuests(void);
int wsWaitForHost(void);

// ---- globals (declarations, RVA order) ----
extern int bHostFound;
extern unsigned int sd_dg;
extern int iWSLastMsgNumHumanPlayers;
extern int iWSAttempts;
extern int iWSNextTickCount;
extern int iWSWaitForHostStatus;
extern int iRc;
extern char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
extern struct sockaddr_in saddr_loc;
extern unsigned short wVer;
extern struct WSAData wsadata;
extern struct in_addr gIn_addrIP;
extern struct sockaddr_in saddr_remote;
extern int iAddrLen;

#endif // HOMM2_WSNETWIN_H
