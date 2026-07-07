// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REMOTE.OBJ   from: (directly linked into exe)
// functions: 13   data: 30
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/REMOTE.h>
#include <_all.h>
VA(0x004a3080, 0x188)
void RemoteCleanup(void) {}

VA(0x004a3208, 0x6da)
void RemoteMain(int) {}

VA(0x004a38e2, 0x85)
void UnloadRemoteDriver(short int) {}

VA(0x004a3967, 0x8d)
int calc_crc_long(unsigned char *, int) { return 0; }

VA(0x004a39f4, 0x2c)
void calc_crc(unsigned short int *, unsigned char *, int) {}

VA(0x004a3a20, 0x87)
int EncodePacket(unsigned char *, char, char, int) { return 0; }

VA(0x004a3aa7, 0x13a)
int DecodePacket(unsigned char *, int) { return 0; }

VA(0x004a3be1, 0x18e)
int SendRemoteData(unsigned char *, unsigned char *, int, int) { return 0; }

VA(0x004a3d6f, 0x158)
int ReceiveRemoteData(unsigned char *, unsigned char *, int) { return 0; }

VA(0x004a3ec7, 0x21a)
int TransmitRemoteData(char *, int, int, signed char, signed char, signed char, signed char) { return 0; }

VA(0x004a40e1, 0x10b)
char * GetRemoteData(signed char) { return 0; }

VA(0x004a41ec, 0x6f4)
void PollRemote(void) {}

VA(0x004a48e0, 0x163)
int TransmitAndWait(char *, int, int, signed char, signed char, char * *) { return 0; }

// ---- data / globals / vtables ----
DATA(0x00516f60)  // int iInOrderCtr
DATA(0x00516f64)  // int iCurLastID
DATA(0x00516f68)  // int giLastConfirm
DATA(0x00516f6c)  // unsigned char GameMode
DATA(0x00516f70)  // long int lLastHeartbeatSend
DATA(0x00516f74)  // int gbInRemoteMain
DATA(0x00516f78)  // int gbInRemoteCleanup
DATA(0x00516f7c)  // int iIDCtr
DATA(0x00516f80)  // int iTimesDropped
DATA(0x00516f84)  // signed char gbInNetSetup
DATA(0x00516f88)  // int bUseDirectPlay
DATA(0x00516f8c)  // int bUseWinsock
DATA(0x00516f90)  // signed char bInTimeoutFail
DATA(0x00516f98)  // int * iBaud
DATA(0x00516fb8)  // int * iIRQ
DATA(0x0052a268)  // char * rcvBufOut
DATA(0x0052a378)  // int * iLastIds
DATA(0x0052a3f0)  // char * PacketSend
DATA(0x0052a4fc)  // char gbUseDiffCompression
DATA(0x0052a500)  // char gbUseRegularCompression
DATA(0x0052a508)  // int * iInOrder
DATA(0x0052a730)  // char * sndBuf
DATA(0x0052a840)  // char * gcThisNetName
DATA(0x0052a860)  // long int * lLastHeartbeatReceive
DATA(0x0052a878)  // char * packet
DATA(0x0052a988)  // struct SNetPlayerInfo * gsNetPlayerInfo
DATA(0x0052aa58)  // char * rcvBufIn
DATA(0x0052ab68)  // char * * rcvBuf
DATA(0x0052ad90)  // int bGotGameType
DATA(0x0052ad98)  // struct SNetPlayerInfo gsThisNetPlayerInfo
