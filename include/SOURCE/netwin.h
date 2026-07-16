#ifndef HOMM2_NETWIN_H
#define HOMM2_NETWIN_H
#include <va.h>
// Declarations of the free functions DEFINED in netwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

typedef enum NetbiosConstant {
    NETBIOS_SESSION_COUNT = 7,
    NETBIOS_STATUS_COUNT = 10,
    NETBIOS_THREAD_EVENT_COUNT = 9,
    NETBIOS_THREAD_EVENT_STORAGE_COUNT = 10,
    NETBIOS_RECEIVE_EVENT_FIRST = 2,
    NETBIOS_RECEIVE_EVENT_COUNT = 5,
    NETBIOS_NAME_SIZE = 0x10,
    NETBIOS_CONTROL_BLOCK_SIZE = 0x40,
    NETBIOS_THREAD_EVENTS_SIZE = 0x24,
    NETBIOS_PAYLOAD_SIZE = 0x1000,
    NETBIOS_ADAPTER_STATUS_SIZE = 0x400,
    NETBIOS_PACKET_HEADER_SIZE = 0x0b,
    NETBIOS_MAX_LANA = 0xfe,
    NETBIOS_INVALID_ID = 0xff,
    NETBIOS_CALL_RETRY_LIMIT = 0x14,
    NETBIOS_CALL_RETRY_DELAY = 100,
    NETBIOS_RECEIVE_RETRY_DELAY = 0x32
} NetbiosConstant;

typedef enum NetbiosCommand {
    NETBIOS_COMMAND_CALL = 0x10,
    NETBIOS_COMMAND_LISTEN = 0x11,
    NETBIOS_COMMAND_HANGUP = 0x12,
    NETBIOS_COMMAND_SEND = 0x14,
    NETBIOS_COMMAND_RECEIVE = 0x15,
    NETBIOS_COMMAND_RECEIVE_DATAGRAM = 0x22,
    NETBIOS_COMMAND_RECEIVE_BROADCAST = 0x23,
    NETBIOS_COMMAND_ADD_NAME = 0x30,
    NETBIOS_COMMAND_DELETE_NAME = 0x31,
    NETBIOS_COMMAND_RESET = 0x32,
    NETBIOS_COMMAND_ADAPTER_STATUS = 0x33,
    NETBIOS_COMMAND_CANCEL = 0x35,
    NETBIOS_COMMAND_PROBE = 0x7f,
    NETBIOS_COMMAND_ASYNC = 0x80
} NetbiosCommand;

typedef enum NetbiosResult {
    NETBIOS_RESULT_SUCCESS = 0,
    NETBIOS_RESULT_ILLEGAL_COMMAND = 3,
    NETBIOS_RESULT_SESSION_OUT_OF_RANGE = 8,
    NETBIOS_RESULT_NO_RESOURCES = 9,
    NETBIOS_RESULT_SESSION_CLOSED = 0x0a,
    NETBIOS_RESULT_CANCELLED = 0x0b,
    NETBIOS_RESULT_DUPLICATE_NAME = 0x0d,
    NETBIOS_RESULT_NAME_IN_USE = 0x16,
    NETBIOS_RESULT_SESSION_ENDED = 0x18,
    NETBIOS_RESULT_NAME_CONFLICT = 0x19,
    NETBIOS_RESULT_CANCEL_COMPLETED = 0x24,
    NETBIOS_RESULT_DUPLICATE_ENVIRONMENT = 0x30,
    NETBIOS_RESULT_ENVIRONMENT_UNDEFINED = 0x34,
    NETBIOS_RESULT_PENDING = 0xff
} NetbiosResult;

typedef enum NetbiosSessionOperation {
    NETBIOS_SESSION_REGISTER = 0,
    NETBIOS_SESSION_RECEIVE_ANY = 1,
    NETBIOS_SESSION_CALL = 2,
    NETBIOS_SESSION_LISTEN_ANY = 3,
    NETBIOS_SESSION_LISTEN = 4,
    NETBIOS_SESSION_MOVE = 5,
    NETBIOS_SESSION_CLOSE = 6,
    NETBIOS_SESSION_CLEAR_CONNECTED = 7,
    NETBIOS_SESSION_GET_NAME = 9
} NetbiosSessionOperation;

typedef enum NetbiosSessionStatus {
    NETBIOS_SESSION_ACTIVE = 1,
    NETBIOS_SESSION_NAME_REGISTERED = 2,
    NETBIOS_SESSION_CONNECTED = 8,
    NETBIOS_SESSION_ERROR = 0x80
} NetbiosSessionStatus;

#pragma pack(push, 1)
struct NetbiosControlBlock;
typedef void (__stdcall *NetbiosPostRoutine)(NetbiosControlBlock *controlBlock);

struct NetbiosControlBlock {
    unsigned char command;
    unsigned char returnCode;
    unsigned char sessionNumber;
    unsigned char nameNumber;
    void *buffer;
    unsigned short length;
    unsigned char callName[NETBIOS_NAME_SIZE];
    unsigned char name[NETBIOS_NAME_SIZE];
    unsigned char receiveTimeout;
    unsigned char sendTimeout;
    NetbiosPostRoutine postRoutine;
    unsigned char adapterNumber;
    unsigned char commandComplete;
    unsigned char reserved[10];
    void *event;
};
#pragma pack(pop)

struct NetbiosName {
    unsigned char bytes[NETBIOS_NAME_SIZE];
};

union NetbiosPayload {
    unsigned char bytes[NETBIOS_PAYLOAD_SIZE];
    unsigned long words[NETBIOS_PAYLOAD_SIZE / sizeof(unsigned long)];
};

struct NetbiosSessionBuffer {
    char bytes[NETBIOS_PAYLOAD_SIZE];
};

struct NetbiosThreadEvents {
    void *handles[NETBIOS_THREAD_EVENT_STORAGE_COUNT];
};
SIZE(NetbiosControlBlock, NETBIOS_CONTROL_BLOCK_SIZE);
SIZE(NetbiosName, NETBIOS_NAME_SIZE);
SIZE(NetbiosPayload, NETBIOS_PAYLOAD_SIZE);
SIZE(NetbiosSessionBuffer, NETBIOS_PAYLOAD_SIZE);
SIZE(NetbiosThreadEvents, NETBIOS_THREAD_EVENT_STORAGE_COUNT * sizeof(void *));

int is_netbios_avail(void);
extern "C" unsigned short __fastcall nb_init(unsigned short, unsigned short);
extern "C" void __fastcall nb_term(void);
extern "C" unsigned short __fastcall nb_rcv(short, void *);
extern "C" unsigned short __fastcall nb_snd(short, short, void *);
extern "C" unsigned short __cdecl nb_sess(short, ...);
extern "C" char __fastcall nb_stat(short);
void nb_thr_ctl(void);

#endif // HOMM2_NETWIN_H
