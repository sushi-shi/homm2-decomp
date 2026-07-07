// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Wsnetwin.obj   from: (directly linked into exe)
// functions: 10   data: 14
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
VA(0x004068b0, 0x5b5)
short int wsnet_init(void) { return 0; }

VA(0x00406e65, 0xd2)
void wsnet_term(void) {}

VA(0x00406f37, 0x1f5)
void wsSendMessage(int, unsigned char, unsigned short int, void *) {}

VA(0x0040712c, 0x61)
int wsnet_snd(int, int, void *) { return 0; }

VA(0x0040718d, 0xa7)
short int wsnet_rcv(short int, unsigned short int, void *) { return 0; }

VA(0x00407234, 0xaf)
void wsProcessMessages(void) {}

VA(0x004072e3, 0x37d)
void wsEvaluateMessage(unsigned long int, int) {}

VA(0x00407660, 0x2e)
int wsWaitForFirstGuest(void) { return 0; }

VA(0x0040768e, 0xb8)
int wsWaitForExtraGuests(void) { return 0; }

VA(0x00407746, 0x126)
int wsWaitForHost(void) { return 0; }

// ---- data / globals / vtables ----
DATA(0x004ed2ac)  // int bHostFound
DATA(0x004ed2b0)  // unsigned int sd_dg
DATA(0x004ed2b4)  // int iWSLastMsgNumHumanPlayers
DATA(0x004ed2b8)  // int iWSAttempts
DATA(0x004ed2bc)  // int iWSNextTickCount
DATA(0x004ed2c0)  // int iWSWaitForHostStatus
DATA(0x00522f88)  // int iRc
DATA(0x00522f90)  // char * cWSTextBuffer
DATA(0x00523090)  // struct sockaddr_in saddr_loc
DATA(0x005230a0)  // unsigned short int wVer
DATA(0x005230a8)  // struct WSAData wsadata
DATA(0x00523238)  // struct in_addr gIn_addrIP
DATA(0x00523240)  // struct sockaddr_in saddr_remote
DATA(0x00523250)  // int iAddrLen
