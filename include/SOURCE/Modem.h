#ifndef HOMM2_MODEM_H
#define HOMM2_MODEM_H
#include <va.h>
// Declarations of the free functions DEFINED in Modem.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

void ModemSetup(i32);
i32l Dial(void);
i32l Wait(void);
void GUIModemCommand(char *, char *);
i8 GUIModemCommandExec(void);
void ModemCommand(char *);
i8 GUIModemResponse(char *, char *);
i8 GUIModemResponseExec(void);
i32 write_buffer(char *, i32);
i32 read_byte(void);
void write_byte(i32);
void Connect(void);
i32 WaitForDirectConnect(void);
char ReadPacket(void);
void WriteModemPacket(char *, i32);

typedef enum ModemConstant {
    MODEM_MODE_DIAL = 3,
    MODEM_MODE_WAIT = 4,
    MODEM_WAIT_COMMAND = 5,
    MODEM_WAIT_RESPONSE = 6,
    MODEM_WAIT_DIRECT_CONNECT = 7,
    MODEM_CONNECTION_INIT_STAGE = 0,
    MODEM_CONNECTION_HANDSHAKE_STAGE = 1,
    MODEM_CONNECTION_READY_STAGE = 2,
    MODEM_BAUD_CLOCK = 115200,
    MODEM_QUEUE_GUARD = 16,
    MODEM_OUT_QUEUE_SIZE = 0x800,
    MODEM_COMMAND_INTERVAL = 250,
    MODEM_COMMAND_DELAY = 100,
    MODEM_RESET_DELAY = 200,
    MODEM_ID_MODULUS = 1000000,
    MODEM_ID_SIZE = 8,
    MODEM_NUMBER_BUFFER_SIZE = 36,
    MODEM_COMMAND_BUFFER_SIZE = 40,
    MODEM_WORK_TEXT_SIZE = 200,
    MODEM_RESPONSE_SIZE = 80,
    MODEM_RESPONSE_TRUNCATE_INDEX = 17,
    MODEM_PACKET_PAYLOAD_SIZE = 256,
    MODEM_PACKET_BUFFER_SIZE = 0x110,
    MODEM_ENCODED_PACKET_SIZE = 544,
    MODEM_QUEUE_INPUT_SIZE = 0x1000,
    MODEM_QUEUE_OUTPUT_SIZE = 0x800,
    MODEM_ESCAPE_BYTE = 0x70,
    MODEM_PACKET_END = 1
} ModemConstant;

struct inque_t {
    i32 readPosition;
    i32 writePosition;
    char data[MODEM_QUEUE_INPUT_SIZE];
};

struct outque_t {
    i32 readPosition;
    i32 writePosition;
    char data[MODEM_QUEUE_OUTPUT_SIZE];
};
SIZE(inque_t, 0x1008);
SIZE(outque_t, 0x808);

// ---- globals (declarations, RVA order) ----
extern i32 iBaudBits;
extern i32 inescape;
extern i32 newpacket;
extern i32 packetlen;
extern char idstr[MODEM_ID_SIZE];
extern i32 GUIMRc;
extern i32 iModemCommandPos;
extern i32 GUIMRrespptr;
extern i32 localstage;
extern char numbuf[MODEM_NUMBER_BUFFER_SIZE];
extern i32 WFDCStage;
extern char remoteidstr[MODEM_ID_SIZE];
extern i32 stime;
extern char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
extern i32 iLastDialPos;
extern i32 remotestage;
extern char GUIMRresp[MODEM_RESPONSE_SIZE];
extern i32 oldsec;
extern inque_t inque;
extern i32 iLastActionTime;
extern char GUIMRresponse[MODEM_RESPONSE_SIZE];
extern outque_t outque;

inline void TruncateModemResponse(void)
{
    GUIMRresponse[MODEM_RESPONSE_TRUNCATE_INDEX] = 0;
}

#endif // HOMM2_MODEM_H
