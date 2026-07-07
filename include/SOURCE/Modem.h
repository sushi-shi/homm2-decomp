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

#endif // HOMM2_MODEM_H
