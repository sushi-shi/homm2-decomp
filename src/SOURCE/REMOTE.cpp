// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REMOTE.OBJ   from: (directly linked into exe)
// functions: 13   data: 30
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/REMOTE.h>
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

// ---- globals (definitions, RVA order) ----
int iInOrderCtr;
int iCurLastID;
int giLastConfirm;
unsigned char GameMode;
long lLastHeartbeatSend;
int gbInRemoteMain;
int gbInRemoteCleanup;
int iIDCtr;
int iTimesDropped;
signed char gbInNetSetup;
int bUseDirectPlay;
int bUseWinsock;
signed char bInTimeoutFail;
int *iBaud;
int *iIRQ;
char *rcvBufOut;
int *iLastIds;
char *PacketSend;
char gbUseDiffCompression;
char gbUseRegularCompression;
int *iInOrder;
char *sndBuf;
char *gcThisNetName;
long *lLastHeartbeatReceive;
char *packet;
SNetPlayerInfo gsNetPlayerInfo[1];
char *rcvBufIn;
char *rcvBuf[138];
int bGotGameType;
