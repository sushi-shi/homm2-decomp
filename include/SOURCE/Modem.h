#ifndef HOMM2_MODEM_H
#define HOMM2_MODEM_H
#include <va.h>
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

typedef enum ModemConstant {
    MODEM_MODE_DIAL = 3,
    MODEM_MODE_WAIT = 4,
    MODEM_WAIT_COMMAND = 5,
    MODEM_WAIT_RESPONSE = 6,
    MODEM_WAIT_DIRECT_CONNECT = 7,
    MODEM_BAUD_CLOCK = 115200,
    MODEM_QUEUE_GUARD = 16,
    MODEM_OUT_QUEUE_SIZE = 0x800,
    MODEM_COMMAND_INTERVAL = 250,
    MODEM_COMMAND_DELAY = 100,
    MODEM_RESET_DELAY = 200,
    MODEM_ID_SIZE = 8,
    MODEM_NUMBER_BUFFER_SIZE = 36,
    MODEM_COMMAND_BUFFER_SIZE = 40,
    MODEM_RESPONSE_SIZE = 80,
    MODEM_PACKET_PAYLOAD_SIZE = 256,
    MODEM_PACKET_BUFFER_SIZE = 0x110,
    MODEM_ENCODED_PACKET_SIZE = 544,
    MODEM_QUEUE_INPUT_SIZE = 0x1000,
    MODEM_QUEUE_OUTPUT_SIZE = 0x800,
    MODEM_ESCAPE_BYTE = 0x70,
    MODEM_PACKET_END = 1
} ModemConstant;

struct inque_t {
    int readPosition;
    int writePosition;
    char data[MODEM_QUEUE_INPUT_SIZE];
};

struct outque_t {
    int readPosition;
    int writePosition;
    char data[MODEM_QUEUE_OUTPUT_SIZE];
};
SIZE(inque_t, 0x1008);
SIZE(outque_t, 0x808);

// ---- globals (declarations, RVA order) ----
extern int iBaudBits;
extern int inescape;
extern int newpacket;
extern int packetlen;
extern char idstr[MODEM_ID_SIZE];
extern int GUIMRc;
extern int iModemCommandPos;
extern int GUIMRrespptr;
extern int localstage;
extern char numbuf[MODEM_NUMBER_BUFFER_SIZE];
extern int WFDCStage;
extern char remoteidstr[MODEM_ID_SIZE];
extern int stime;
extern char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
extern int iLastDialPos;
extern int remotestage;
extern char GUIMRresp[MODEM_RESPONSE_SIZE];
extern int oldsec;
extern inque_t inque;
extern int iLastActionTime;
extern char GUIMRresponse[MODEM_RESPONSE_SIZE];
extern outque_t outque;

#endif // HOMM2_MODEM_H
