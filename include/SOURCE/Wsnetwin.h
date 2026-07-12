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
extern int bHostFound;
extern unsigned int sd_dg;
extern int iWSLastMsgNumHumanPlayers;
extern int iWSAttempts;
extern int iWSNextTickCount;
extern int iWSWaitForHostStatus;
extern int iRc;
extern char *cWSTextBuffer;
extern unsigned short wVer;
extern int iAddrLen;

#endif // HOMM2_WSNETWIN_H
