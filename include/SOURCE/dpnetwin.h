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
extern struct IDirectPlay *lpIDC;
extern unsigned long dcoID;
extern struct _GUID *IPXGuid;
extern struct _GUID *TCPGuid;
extern void *dphEvent;
extern int iDPRcvBufferHead;
extern int iDPRcvBufferTail;
extern unsigned char **ppDPRcvBuffer;
extern int *piDPRcvBufferSize;
extern int bStartUpInfoReceived;
extern void *hinstDplayx;
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
extern int *giNetPosToDCOPos;
extern int iSessionToTry;
extern long *lSessions;

#endif // HOMM2_DPNETWIN_H
