// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\dpnetwin.obj   from: (directly linked into exe)
// functions: 16   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/dpnetwin.h>
#include <_all.h>
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

// ---- data / globals / vtables ----
DATA(0x004ef7c8)  // struct IDirectPlay * lpIDC
DATA(0x004ef7cc)  // unsigned long int dcoID
DATA(0x004ef7d0)  // struct _GUID * IPXGuid
DATA(0x004ef7d4)  // struct _GUID * TCPGuid
DATA(0x004ef7d8)  // void * dphEvent
DATA(0x004ef7dc)  // int iDPRcvBufferHead
DATA(0x004ef7e0)  // int iDPRcvBufferTail
DATA(0x004ef7e4)  // unsigned char * * ppDPRcvBuffer
DATA(0x004ef7e8)  // int * piDPRcvBufferSize
DATA(0x004ef7ec)  // int bStartUpInfoReceived
DATA(0x004ef7f0)  // void * hinstDplayx
DATA(0x004ef7f4)  // int iDPWaitForFirstGuestStatus
DATA(0x004ef7f8)  // int iDPWaitForHostStatus
DATA(0x004ef7fc)  // int iWaitForHostWaitCount
DATA(0x004ef800)  // int iEnumCount
DATA(0x004ef804)  // int iLastHereIAmTickCount
DATA(0x004ef808)  // int bInDPSD
DATA(0x004ef80c)  // int iGUIDCount
DATA(0x004ef810)  // int iLastMsgNumHumanPlayers
DATA(0x00525060)  // int iMaxSession
DATA(0x00525064)  // int giHostAcceptStatus
DATA(0x00525068)  // struct _GUID * g_lpGuid
DATA(0x00525070)  // int * giNetPosToDCOPos
DATA(0x00525088)  // int iSessionToTry
DATA(0x00525090)  // long int * lSessions
