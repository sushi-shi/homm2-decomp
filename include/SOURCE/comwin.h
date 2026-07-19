#ifndef HOMM2_COMWIN_H
#define HOMM2_COMWIN_H

#include <windows.h>
#include <va.h>

// Intrusive-list anchors store the tail before the head.
struct tag_Node {
    struct tag_Node* prev;
    struct tag_Node* next;
    u16 len;
    union {
        u8 comData[1];
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

struct ComPortState {
    HANDLE handle;
    u8 reserved04[4];
    DCB savedState;
    COMMTIMEOUTS savedTimeouts;
    u8 reserved38[0x18];
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
