#ifndef HOMM2_COMWIN_H
#define HOMM2_COMWIN_H

#include <windows.h>
#include <va.h>

H2_ENUM_BEGIN(ComPortConstant)
    COM_PORT_FLEXIBLE_DATA_SIZE = 1,
    COM_PORT_PREFIX_GAP_SIZE    = 4,
    COM_PORT_RUNTIME_GAP_SIZE   = 0x18,
    COM_SERIAL_BYTE_SIZE        = 8
H2_ENUM_END(ComPortConstant)

H2_ENUM_CLASS_BEGIN(ComBaudRate)
    COM_BAUD_2400  = 1,
    COM_BAUD_4800  = 2,
    COM_BAUD_9600  = 3,
    COM_BAUD_19200 = 4,
    COM_BAUD_38400 = 5
H2_ENUM_CLASS_END(ComBaudRate)

// Intrusive-list anchors store the tail before the head.
struct tag_Node {
    struct tag_Node* prev;
    struct tag_Node* next;
    u16 len;
    union {
        u8 comData[COM_PORT_FLEXIBLE_DATA_SIZE];
        struct {
            u8 sessionIndex; // NetBIOS session table index
            u8 data[COM_PORT_FLEXIBLE_DATA_SIZE]; // NetBIOS payload (variable length)
        };
    };
};
struct tag_Anchor {
    struct tag_Node* head;
    struct tag_Node* tail;
};

struct ComPortState {
    HANDLE handle;
    u8 reserved04[COM_PORT_PREFIX_GAP_SIZE];
    DCB savedState;
    COMMTIMEOUTS savedTimeouts;
    u8 reserved38[COM_PORT_RUNTIME_GAP_SIZE];
    tag_Anchor normalQueue;
    tag_Anchor priorityQueue;
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

#endif
