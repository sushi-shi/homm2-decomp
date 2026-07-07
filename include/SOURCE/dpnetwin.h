#ifndef HOMM2_DPNETWIN_H
#define HOMM2_DPNETWIN_H
// Declarations of the free functions DEFINED in dpnetwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

int __stdcall dpEnumServiceProvider(struct _GUID *, char *, unsigned long int, unsigned long int, void *);
int __stdcall dpEnumSession(struct DPSESSIONDESC *, void *, unsigned long int *, unsigned long int);
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
DATA(0x004ef7c8) extern struct IDirectPlay *lpIDC;
DATA(0x004ef7cc) extern unsigned long dcoID;
DATA(0x004ef7d0) extern struct _GUID *IPXGuid;
DATA(0x004ef7d4) extern struct _GUID *TCPGuid;
DATA(0x004ef7d8) extern void *dphEvent;
DATA(0x004ef7dc) extern int iDPRcvBufferHead;
DATA(0x004ef7e0) extern int iDPRcvBufferTail;
DATA(0x004ef7e4) extern unsigned char **ppDPRcvBuffer;
DATA(0x004ef7e8) extern int *piDPRcvBufferSize;
DATA(0x004ef7ec) extern int bStartUpInfoReceived;
DATA(0x004ef7f0) extern void *hinstDplayx;
DATA(0x004ef7f4) extern int iDPWaitForFirstGuestStatus;
DATA(0x004ef7f8) extern int iDPWaitForHostStatus;
DATA(0x004ef7fc) extern int iWaitForHostWaitCount;
DATA(0x004ef800) extern int iEnumCount;
DATA(0x004ef804) extern int iLastHereIAmTickCount;
DATA(0x004ef808) extern int bInDPSD;
DATA(0x004ef80c) extern int iGUIDCount;
DATA(0x004ef810) extern int iLastMsgNumHumanPlayers;
DATA(0x00525060) extern int iMaxSession;
DATA(0x00525064) extern int giHostAcceptStatus;
DATA(0x00525068) extern struct _GUID *g_lpGuid;
DATA(0x00525070) extern int *giNetPosToDCOPos;
DATA(0x00525088) extern int iSessionToTry;
DATA(0x00525090) extern long *lSessions;

#endif // HOMM2_DPNETWIN_H
