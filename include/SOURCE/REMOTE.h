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
extern char gbUseDiffCompression;
extern char gbUseRegularCompression;
extern SNetPlayerInfo gsNetPlayerInfo[];

// ---- globals (declarations, RVA order) ----
extern int iInOrderCtr;
extern int iCurLastID;
extern int giLastConfirm;
extern unsigned char GameMode;
extern long lLastHeartbeatSend;
extern int gbInRemoteMain;
extern int gbInRemoteCleanup;
extern int iIDCtr;
extern int iTimesDropped;
extern signed char gbInNetSetup;
extern int bUseDirectPlay;
extern int bUseWinsock;
extern signed char bInTimeoutFail;
extern int *iBaud;
extern int *iIRQ;
extern char *rcvBufOut;
extern int *iLastIds;
extern char *PacketSend;
extern int *iInOrder;
extern char *sndBuf;
extern char *gcThisNetName;
extern long *lLastHeartbeatReceive;
extern char *packet;
extern char *rcvBufIn;
extern char *rcvBuf[138];
extern int bGotGameType;

#endif // HOMM2_REMOTE_H
