#ifndef HOMM2_REMOTE_H
#define HOMM2_REMOTE_H
#include <va.h>
#include <_types.h>
// Declarations of the free functions DEFINED in REMOTE.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

void RemoteCleanup(void);
void RemoteMain(int);
void UnloadRemoteDriver(short int);
int calc_crc_long(unsigned char *, int);
void calc_crc(unsigned short int *, unsigned char *, int);
int EncodePacket(unsigned char *, char, char, int);
int DecodePacket(unsigned char *, int);
int SendRemoteData(unsigned char *, unsigned char *, int, int);
int ReceiveRemoteData(unsigned char *, unsigned char *, int);
int TransmitRemoteData(char *, int, int, signed char, signed char, signed char, signed char);
char * GetRemoteData(signed char);
void PollRemote(void);
int TransmitAndWait(char *, int, int, signed char, signed char, char * *);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x0052a4fc) extern char gbUseDiffCompression;
DATA(0x0052a500) extern char gbUseRegularCompression;
DATA(0x0052a988) extern SNetPlayerInfo gsNetPlayerInfo[];

// ---- globals (declarations, RVA order) ----
DATA(0x00516f60) extern int iInOrderCtr;
DATA(0x00516f64) extern int iCurLastID;
DATA(0x00516f68) extern int giLastConfirm;
DATA(0x00516f6c) extern unsigned char GameMode;
DATA(0x00516f70) extern long lLastHeartbeatSend;
DATA(0x00516f74) extern int gbInRemoteMain;
DATA(0x00516f78) extern int gbInRemoteCleanup;
DATA(0x00516f7c) extern int iIDCtr;
DATA(0x00516f80) extern int iTimesDropped;
DATA(0x00516f84) extern signed char gbInNetSetup;
DATA(0x00516f88) extern int bUseDirectPlay;
DATA(0x00516f8c) extern int bUseWinsock;
DATA(0x00516f90) extern signed char bInTimeoutFail;
DATA(0x00516f98) extern int *iBaud;
DATA(0x00516fb8) extern int *iIRQ;
DATA(0x0052a268) extern char *rcvBufOut;
DATA(0x0052a378) extern int *iLastIds;
DATA(0x0052a3f0) extern char *PacketSend;
DATA(0x0052a508) extern int *iInOrder;
DATA(0x0052a730) extern char *sndBuf;
DATA(0x0052a840) extern char *gcThisNetName;
DATA(0x0052a860) extern long *lLastHeartbeatReceive;
DATA(0x0052a878) extern char *packet;
DATA(0x0052aa58) extern char *rcvBufIn;
DATA(0x0052ab68) extern char *rcvBuf[138];
DATA(0x0052ad90) extern int bGotGameType;

#endif // HOMM2_REMOTE_H
