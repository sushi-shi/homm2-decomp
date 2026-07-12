#ifndef HOMM2_MODEM_H
#define HOMM2_MODEM_H
// Declarations of the free functions DEFINED in Modem.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

void ModemSetup(int);
long int Dial(void);
long int Wait(void);
void GUIModemCommand(char *, char *);
signed char GUIModemCommandExec(void);
void ModemCommand(char *);
signed char GUIModemResponse(char *, char *);
signed char GUIModemResponseExec(void);
int write_buffer(char *, int);
int read_byte(void);
void write_byte(int);
void Connect(void);
int WaitForDirectConnect(void);
char ReadPacket(void);
void WriteModemPacket(char *, int);

// ---- globals (declarations, RVA order) ----
extern int iBaudBits;
extern int inescape;
extern int newpacket;
extern int packetlen;
extern char *idstr;
extern int GUIMRc;
extern int iModemCommandPos;
extern int GUIMRrespptr;
extern int localstage;
extern char *numbuf;
extern int WFDCStage;
extern char *remoteidstr;
extern int stime;
extern char *cModemCommand;
extern int iLastDialPos;
extern int remotestage;
extern char *GUIMRresp;
extern int oldsec;
extern int iLastActionTime;
extern char *GUIMRresponse;

#endif // HOMM2_MODEM_H
