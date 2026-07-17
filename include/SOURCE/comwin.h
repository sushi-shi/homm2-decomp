#ifndef HOMM2_COMWIN_H
#define HOMM2_COMWIN_H
#include <windows.h>
#include <va.h>
// Declarations of the free functions DEFINED in comwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

// The intrusive linked-list node + its anchor. init_anchor zeroes anchor[+0] and anchor[+4]
// (reversed from its disassembly), so the anchor is two node pointers (head/tail). The node's
// link pointers occupy +0/+4; nb_rcv reads a payload length at +8 and the payload bytes at +0xb.
struct tag_Node {
    struct tag_Node* prev; // +0x00
    struct tag_Node* next; // +0x04
    u16 len;               // +0x08  payload length
    union {
        u8 comData[1]; // +0x0a COM payload (variable length)
        struct {
            u8 sessionIndex; // NetBIOS session table index
            u8 data[1];      // NetBIOS payload (variable length)
        };
    };
};
struct tag_Anchor {
    struct tag_Node* head;
    struct tag_Node* tail;
};

typedef enum ComConstant {
    COM_PORT_COUNT = 2,
    COM_PORT_NAME_SIZE = 12,
    COM_ERROR_NAME_SIZE = 100,
    COM_ERROR_MESSAGE_SIZE = 500,
    COM_RECEIVE_BUFFER_SIZE = 0x2000,
    COM_TRANSMIT_BUFFER_SIZE = 0x1000,
    COM_BREAK_DELAY = 500,
    COM_NODE_HEADER_SIZE = 10
} ComConstant;

typedef enum ComBaudRate {
    COM_BAUD_2400 = 1,
    COM_BAUD_4800 = 2,
    COM_BAUD_9600 = 3,
    COM_BAUD_19200 = 4,
    COM_BAUD_38400 = 5
} ComBaudRate;

struct ComPortState {
    HANDLE handle;              // +0x00
    u8 reserved04[4];           // +0x04
    DCB savedState;             // +0x08
    COMMTIMEOUTS savedTimeouts; // +0x24
    u8 reserved38[0x18];        // +0x38
    tag_Anchor normalQueue;     // +0x50
    tag_Anchor priorityQueue;   // +0x58
};
SIZE(ComPortState, 0x60);

void add_node(struct tag_Anchor*, struct tag_Node*);
struct tag_Node* pop_node(struct tag_Anchor*);
void init_anchor(struct tag_Anchor*, i32, i32);
void ShutdownComError(char*);
i16 com_init(u8, i32, i32);
void com_term(i16);
i16 com_rcv(i16, u16, void*);
i16 com_snd(i16, u16, u16, void*, i32);
i16 __cdecl com_sess(i32, i32, ...);
u8 com_stat(i16, u16);
void comm_wrt_task(void);

#endif // HOMM2_COMWIN_H
