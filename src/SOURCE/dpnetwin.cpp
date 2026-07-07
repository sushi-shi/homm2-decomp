// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\dpnetwin.obj   from: (directly linked into exe)
// functions: 16   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/dpnetwin.h>
VA(0x0041eda0, 0x95)
int __stdcall dpEnumServiceProvider(struct _GUID *, char *, unsigned long int, unsigned long int, void *) { return 0; }

VA(0x0041ee35, 0x7a)
int __stdcall dpEnumSession(struct DPSESSIONDESC *, void *, unsigned long int *, unsigned long int) { return 0; }

VA(0x0041eeaf, 0x311)
short int dpnet_init(void) { return 0; }

VA(0x0041f1c0, 0xce)
void CleanupDPVars(void) {}

VA(0x0041f28e, 0x116)
void dpnet_term(void) {}

VA(0x0041f3a4, 0xee)
void dpSendMessage(int, unsigned char, unsigned short int, void *) {}

VA(0x0041f492, 0x5a)
int dpnet_snd(int, int, void *) { return 0; }

VA(0x0041f4ec, 0xa7)
short int dpnet_rcv(short int, unsigned short int, void *) { return 0; }

VA(0x0041f593, 0x1d)
unsigned char dpnet_stat(short int, unsigned short int) { return 0; }

VA(0x0041f5b0, 0x13)
short int __cdecl dpnet_sess(int, int, ...) { return 0; }

VA(0x0041f5c3, 0xbe)
void dpProcessMessages(void) {}

VA(0x0041f681, 0x274)
void dpEvaluateMessage(unsigned long int, int) {}

VA(0x0041f8f5, 0x182)
int dpWaitForFirstGuest(void) { return 0; }

VA(0x0041fa77, 0x84)
int dpWaitForExtraGuests(void) { return 0; }

VA(0x0041fafb, 0x3d2)
int dpWaitForHost(void) { return 0; }

VA(0x0041fecd, 0x5eb)
void DPSD(int, char *, int) {}

// ---- globals (definitions, RVA order) ----
struct IDirectPlay *lpIDC;
unsigned long dcoID;
struct _GUID *IPXGuid;
struct _GUID *TCPGuid;
void *dphEvent;
int iDPRcvBufferHead;
int iDPRcvBufferTail;
unsigned char **ppDPRcvBuffer;
int *piDPRcvBufferSize;
int bStartUpInfoReceived;
void *hinstDplayx;
int iDPWaitForFirstGuestStatus;
int iDPWaitForHostStatus;
int iWaitForHostWaitCount;
int iEnumCount;
int iLastHereIAmTickCount;
int bInDPSD;
int iGUIDCount;
int iLastMsgNumHumanPlayers;
int iMaxSession;
int giHostAcceptStatus;
struct _GUID *g_lpGuid;
int *giNetPosToDCOPos;
int iSessionToTry;
long *lSessions;
