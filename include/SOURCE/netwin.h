#ifndef HOMM2_NETWIN_H
#define HOMM2_NETWIN_H
#include <va.h>
// Declarations of the free functions DEFINED in netwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

HOMM2_ENUM_VALUES_BEGIN(NetbiosConstant)
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
    NETBIOS_INVALID_ID = 0xFF,
    NETBIOS_CALL_RETRY_LIMIT = 0x14,
    NETBIOS_CALL_RETRY_DELAY = 100,
    NETBIOS_RECEIVE_RETRY_DELAY = 0x32
HOMM2_ENUM_VALUES_END(NetbiosConstant)

HOMM2_ENUM_BEGIN(NetbiosCommand)
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
HOMM2_ENUM_END(NetbiosCommand)

HOMM2_ENUM_BEGIN(NetbiosResult)
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
    NETBIOS_RESULT_PENDING = 0xFF
HOMM2_ENUM_END(NetbiosResult)

HOMM2_ENUM_BEGIN(NetbiosSessionOperation)
    NETBIOS_SESSION_REGISTER = 0,
    NETBIOS_SESSION_RECEIVE_ANY = 1,
    NETBIOS_SESSION_CALL = 2,
    NETBIOS_SESSION_LISTEN_ANY = 3,
    NETBIOS_SESSION_LISTEN = 4,
    NETBIOS_SESSION_MOVE = 5,
    NETBIOS_SESSION_CLOSE = 6,
    NETBIOS_SESSION_CLEAR_CONNECTED = 7,
    NETBIOS_SESSION_GET_NAME = 9
HOMM2_ENUM_END(NetbiosSessionOperation)

HOMM2_ENUM_BEGIN(NetbiosSessionStatus)
    NETBIOS_SESSION_ACTIVE = 1,
    NETBIOS_SESSION_NAME_REGISTERED = 2,
    NETBIOS_SESSION_CONNECTED = 8,
    NETBIOS_SESSION_ERROR = 0x80
HOMM2_ENUM_END(NetbiosSessionStatus)

#pragma pack(push, 1)
struct NetbiosControlBlock;
typedef void(__stdcall* NetbiosPostRoutine)(NetbiosControlBlock* controlBlock);

struct NetbiosControlBlock {
    u8 command;
    u8 returnCode;
    u8 sessionNumber;
    u8 nameNumber;
    void* buffer;
    u16 length;
    u8 callName[NETBIOS_NAME_SIZE];
    u8 name[NETBIOS_NAME_SIZE];
    u8 receiveTimeout;
    u8 sendTimeout;
    NetbiosPostRoutine postRoutine;
    u8 adapterNumber;
    u8 commandComplete;
    u8 reserved[10];
    void* event;
};
#pragma pack(pop)

struct NetbiosName {
    u8 bytes[NETBIOS_NAME_SIZE];
};

union NetbiosPayload {
    u8 bytes[NETBIOS_PAYLOAD_SIZE];
    u32l words[NETBIOS_PAYLOAD_SIZE / sizeof(u32l)];
};

struct NetbiosSessionBuffer {
    char bytes[NETBIOS_PAYLOAD_SIZE];
};

struct NetbiosThreadEvents {
    void* handles[NETBIOS_THREAD_EVENT_STORAGE_COUNT];
};
SIZE(NetbiosControlBlock, NETBIOS_CONTROL_BLOCK_SIZE);
SIZE(NetbiosName, NETBIOS_NAME_SIZE);
SIZE(NetbiosPayload, NETBIOS_PAYLOAD_SIZE);
SIZE(NetbiosSessionBuffer, NETBIOS_PAYLOAD_SIZE);
SIZE(NetbiosThreadEvents, NETBIOS_THREAD_EVENT_STORAGE_COUNT * sizeof(void*));

i32 is_netbios_avail(void);
extern "C" u16 __fastcall nb_init(u16, u16);
extern "C" void __fastcall nb_term(void);
extern "C" u16 __fastcall nb_rcv(i16, void*);
extern "C" u16 __fastcall nb_snd(i16, i16, void*);
extern "C" u16 __cdecl nb_sess(i16, ...);
extern "C" char __fastcall nb_stat(i16);
void nb_thr_ctl(void);

#endif // HOMM2_NETWIN_H
