#ifndef HOMM2_COMWIN_H
#define HOMM2_COMWIN_H

#include <windows.h>
#include <Ints.h>
#include <BASE/Misc.h>

typedef enum ComPortConstant {
    COM_PORT_FLEXIBLE_DATA_SIZE = 1,
    COM_PORT_PREFIX_GAP_SIZE    = 4,
    COM_PORT_RUNTIME_GAP_SIZE   = 0x18,
    COM_SERIAL_BYTE_SIZE        = 8
} ComPortConstant;

enum {
    COM_BAUD_2400  = 1,
    COM_BAUD_4800  = 2,
    COM_BAUD_9600  = 3,
    COM_BAUD_19200 = 4,
    COM_BAUD_38400 = 5
};
typedef i32 ComBaudRate;
struct tag_Node {
    struct tag_Node* prev;
    struct tag_Node* next;
    u16 len;
    union {
        u8 comData[COM_PORT_FLEXIBLE_DATA_SIZE];
        struct {
            u8 sessionIndex;
            u8 data[COM_PORT_FLEXIBLE_DATA_SIZE];
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

void add_node(struct tag_Anchor* anchor, struct tag_Node* node);
struct tag_Node* pop_node(struct tag_Anchor* anchor);
void init_anchor(struct tag_Anchor* anchor, i32, i32);

#define FREE_NODE_QUEUE(node, anchor)                                                              \
    while (((node) = pop_node(anchor)) != NULL)                                                    \
    H2_FREE(node)
void ShutdownComError(const char* function);
i16 com_init(u8 portNumber, ComBaudRate baudRate, i32 useDtr);
void com_term(i16 portIndex);
i16 com_rcv(i16 portIndex, u16 requested, void* buffer);
i16 com_snd(i16 portIndex, u16, u16 length, const void* data, i32 priority);
i16 __cdecl com_sess(i32, i32, ...);
u8 com_stat(i16 portIndex, u16);
void comm_wrt_task(void);

#endif
