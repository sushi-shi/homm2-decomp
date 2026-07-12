// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Modem.obj   from: (directly linked into exe)
// functions: 15   data: 22
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/Modem.h>
VA(0x0040c8f0, 0x24e)
void ModemSetup(int) {}

VA(0x0040cb3e, 0x9e)
long int Dial(void) { return 0; }

VA(0x0040cbdc, 0x54)
long int Wait(void) { return 0; }

VA(0x0040cc30, 0x79)
void GUIModemCommand(char *, char *) {}

VA(0x0040cca9, 0x95)
signed char GUIModemCommandExec(void) { return 0; }

VA(0x0040cd3e, 0x8e)
void ModemCommand(char *) {}

VA(0x0040cdcc, 0x82)
signed char GUIModemResponse(char *, char *) { return 0; }

VA(0x0040ce4e, 0xe2)
signed char GUIModemResponseExec(void) { return 0; }

VA(0x0040cf30, 0x4e)
int write_buffer(char *, int) { return 0; }

VA(0x0040cf7e, 0x47)
int read_byte(void) { return 0; }

VA(0x0040cfc5, 0x27)
void write_byte(int) {}

VA(0x0040cfec, 0x1bb)
void Connect(void) {}

VA(0x0040d1a7, 0x211)
int WaitForDirectConnect(void) { return 0; }

VA(0x0040d3b8, 0x127)
char ReadPacket(void) { return 0; }

VA(0x0040d4df, 0xff)
void WriteModemPacket(char *, int) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004ede50) int iBaudBits;
DATA(0x004ede54) int inescape;
DATA(0x004ede58) int newpacket;
DATA(0x004ede5c) int packetlen;
DATA(0x00523280) char *idstr;
DATA(0x00523288) int GUIMRc;
DATA(0x0052328c) int iModemCommandPos;
DATA(0x00523290) int GUIMRrespptr;
DATA(0x00523294) int localstage;
DATA(0x00523298) char *numbuf;
DATA(0x005232bc) int WFDCStage;
DATA(0x005232c0) char *remoteidstr;
DATA(0x005232c8) int stime;
DATA(0x005232d0) char *cModemCommand;
DATA(0x005232f8) int iLastDialPos;
DATA(0x005232fc) int remotestage;
DATA(0x00523300) char *GUIMRresp;
DATA(0x00523350) int oldsec;
DATA(0x00524360) int iLastActionTime;
DATA(0x00524368) char *GUIMRresponse;
