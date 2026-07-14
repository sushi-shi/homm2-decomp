#ifndef HOMM2_NETWIN_PRIVATE_H
#define HOMM2_NETWIN_PRIVATE_H

#include <SOURCE/netwin.h>

typedef enum NetwinSourceLine {
    NETWIN_SOURCE_LINE_INIT_BASE = 105,
    NETWIN_SOURCE_LINE_INIT_ALLOC = 145,
    NETWIN_SOURCE_LINE_INIT_FREE = 159,
    NETWIN_SOURCE_LINE_TERM_BASE = 169,
    NETWIN_SOURCE_LINE_TERM_SEND_FREE = 200,
    NETWIN_SOURCE_LINE_TERM_POOL_FREE = 204,
    NETWIN_SOURCE_LINE_TERM_RECEIVE_FREE = 219,
    NETWIN_SOURCE_LINE_RECEIVE_BASE = 226,
    NETWIN_SOURCE_LINE_RECEIVE_FREE = 237,
    NETWIN_SOURCE_LINE_SEND_BASE = 245,
    NETWIN_SOURCE_LINE_SEND_ALLOC = 260,
    NETWIN_SOURCE_LINE_THREAD_BASE = 412,
    NETWIN_SOURCE_LINE_THREAD_PENDING_ASSERT = 495,
    NETWIN_SOURCE_LINE_THREAD_FREE = 508,
    NETWIN_SOURCE_LINE_ADD_NAME_BASE = 534,
    NETWIN_SOURCE_LINE_ADD_NAME_ASSERT = 537,
    NETWIN_SOURCE_LINE_ARM_RECEIVE_BASE = 710,
    NETWIN_SOURCE_LINE_ARM_RECEIVE_ASSERT = 715,
    NETWIN_SOURCE_LINE_RECEIVE_COMPLETE_BASE = 780,
    NETWIN_SOURCE_LINE_RECEIVE_COMPLETE_ALLOC = 796
} NetwinSourceLine;

static void nb_add_name(void);
static void __stdcall nb_add_name_done(NetbiosControlBlock *controlBlock);
static unsigned short __fastcall nb_recv_any(int session);
static void __stdcall nb_recv_any_done(NetbiosControlBlock *controlBlock);
static unsigned short __fastcall nb_call(int session, void *name);
static unsigned short __fastcall nb_listen(int session, void *name);
static void __stdcall nb_call_done(NetbiosControlBlock *controlBlock);
static void __fastcall nb_arm_recv(int session);
static void __fastcall nb_close_session(int session);
static void __fastcall nb_recv_complete(int session);
static void __fastcall nb_format_name(char *source, unsigned char *destination);

extern "C" unsigned char __stdcall Netbios(NetbiosControlBlock *controlBlock);

#endif // HOMM2_NETWIN_PRIVATE_H
