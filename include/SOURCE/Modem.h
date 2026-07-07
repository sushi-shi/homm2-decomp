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
DATA(0x004ede50) extern int iBaudBits;
DATA(0x004ede54) extern int inescape;
DATA(0x004ede58) extern int newpacket;
DATA(0x004ede5c) extern int packetlen;
DATA(0x00523280) extern char *idstr;
DATA(0x00523288) extern int GUIMRc;
DATA(0x0052328c) extern int iModemCommandPos;
DATA(0x00523290) extern int GUIMRrespptr;
DATA(0x00523294) extern int localstage;
DATA(0x00523298) extern char *numbuf;
DATA(0x005232bc) extern int WFDCStage;
DATA(0x005232c0) extern char *remoteidstr;
DATA(0x005232c8) extern int stime;
DATA(0x005232d0) extern char *cModemCommand;
DATA(0x005232f8) extern int iLastDialPos;
DATA(0x005232fc) extern int remotestage;
DATA(0x00523300) extern char *GUIMRresp;
DATA(0x00523350) extern int oldsec;
DATA(0x00524360) extern int iLastActionTime;
DATA(0x00524368) extern char *GUIMRresponse;

#endif // HOMM2_MODEM_H
