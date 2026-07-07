#ifndef HOMM2_WSNETWIN_H
#define HOMM2_WSNETWIN_H
// Declarations of the free functions DEFINED in Wsnetwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

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
DATA(0x004ed2ac) extern int bHostFound;
DATA(0x004ed2b0) extern unsigned int sd_dg;
DATA(0x004ed2b4) extern int iWSLastMsgNumHumanPlayers;
DATA(0x004ed2b8) extern int iWSAttempts;
DATA(0x004ed2bc) extern int iWSNextTickCount;
DATA(0x004ed2c0) extern int iWSWaitForHostStatus;
DATA(0x00522f88) extern int iRc;
DATA(0x00522f90) extern char *cWSTextBuffer;
DATA(0x005230a0) extern unsigned short wVer;
DATA(0x00523250) extern int iAddrLen;

#endif // HOMM2_WSNETWIN_H
