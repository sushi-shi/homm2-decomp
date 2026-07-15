// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\netwin.obj   from: (directly linked into exe)
// functions: 8   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#define NCB_INCLUDED
#include <windows.h>
#include <stdarg.h>
#include <string.h>
#include <SOURCE/comwin.h>
#include <SOURCE/netwin.h>
#include <SOURCE/netwin_private.h>
#include <SOURCE/KB.h>
#include <BASE/Misc.h>
#include <stdio.h>



// ---- module-private globals (retail xref: single-module) ----
DATA(0x0051739c) static unsigned char gNbCallRetries = 0;      // nb_call_done retry counter
DATA(0x005173a0) static unsigned char gNetbiosAvail = 0;
DATA(0x005173a4) static unsigned char gNetbiosLana = 0;
DATA(0x005173a8) static unsigned char gNbShutdown = 0;         // shutdown flag, cleared by nb_init
DATA(0x005173ac) static unsigned char gNbMaxSess = NETBIOS_INVALID_ID;
DATA(0x005173b0) static unsigned char gNbLocalNum = 0;         // local netbios name number
DATA(0x005173b4) static unsigned char gNetStatus[NETBIOS_SESSION_COUNT];
DATA(0x005173c0) static char         *gNbGroupName = "Empire Too ";
DATA(0x005173c4) static char         *gNbListenName = "*";
DATA(0x005173d8) static short gNbInitSourceLineBase = NETWIN_SOURCE_LINE_INIT_BASE;
DATA(0x00517434) static short gNbTermSourceLineBase = NETWIN_SOURCE_LINE_TERM_BASE;
DATA(0x005174bc) static short gNbReceiveSourceLineBase = NETWIN_SOURCE_LINE_RECEIVE_BASE;
DATA(0x005174ec) static short gNbSendSourceLineBase = NETWIN_SOURCE_LINE_SEND_BASE;
DATA(0x0051751c) static short gNbThreadSourceLineBase = NETWIN_SOURCE_LINE_THREAD_BASE;
DATA(0x00517578) static short gNbAddNameSourceLineBase = NETWIN_SOURCE_LINE_ADD_NAME_BASE;
DATA(0x005175c0) static short gNbArmReceiveSourceLineBase = NETWIN_SOURCE_LINE_ARM_RECEIVE_BASE;
DATA(0x005175f0) static short gNbReceiveCompleteSourceLineBase = NETWIN_SOURCE_LINE_RECEIVE_COMPLETE_BASE;
DATA(0x0052ae68) static tag_Anchor    gNbFreeQueue;        // queue anchor (init_anchor)
DATA(0x0052ae70) static unsigned char gNbSessLsn[NETBIOS_SESSION_COUNT];
DATA(0x0052ae78) static NetbiosPayload gNbRcvData[NETBIOS_SESSION_COUNT];
DATA(0x00531e78) static NetbiosName    gNbNameBuf[NETBIOS_SESSION_COUNT];
DATA(0x00531ee8) static NetbiosSessionBuffer gNbSessBuf;
DATA(0x00532ee8) static NetbiosControlBlock gNbSessNcb[NETBIOS_SESSION_COUNT];
DATA(0x005330a8) static NetbiosControlBlock gNbCtlNcb;
DATA(0x005330e8) static tag_Anchor    gNbRcvQueue;         // receive queue (nb_rcv pops it)
DATA(0x005330f0) static tag_Anchor    gNbSndQueue;         // send queue
DATA(0x005330f8) static CRITICAL_SECTION gNbRcvLock;       // guards gNbRcvQueue
DATA(0x00533110) static NetbiosThreadEvents gNbEvents;
DATA(0x00533138) static CRITICAL_SECTION gNbSndLock;       // guards gNbSndQueue

VA(0x004a6be0, 0xa8)
int is_netbios_avail(void)
{
    NetbiosControlBlock ncb;
    memset(&ncb, 0, sizeof(ncb));
    for (gNetbiosLana = 0; gNetbiosLana < NETBIOS_MAX_LANA; gNetbiosLana++) {
        memset(&ncb, 0, sizeof(ncb));
        ncb.command = NETBIOS_COMMAND_PROBE;
        ncb.adapterNumber = gNetbiosLana;
        if (Netbios(&ncb) == NETBIOS_RESULT_ILLEGAL_COMMAND)
            break;
    }
    if (gNetbiosLana < NETBIOS_MAX_LANA) {
        gNetbiosAvail = 1;
        return 1;
    }
    return 0;
}

VA(0x004a6c88, 0x244)
extern "C" unsigned short __fastcall nb_init(unsigned short param1, unsigned short param2)
{
    NetbiosControlBlock localNcb;
    int i;
    unsigned char *statusBuffer;
    int returnCode;

    memset(gNbSessLsn, 0, sizeof(gNbSessLsn));
    memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
    memset(gNbSessNcb, 0, sizeof(gNbSessNcb));
    memset(&gNbSessBuf, -1, sizeof(gNbSessBuf));
    memset(gNbRcvData, -1, sizeof(gNbRcvData));
    memset(&gNbEvents, 0, sizeof(gNbEvents));
    if (is_netbios_avail() == 0)
        return 1;
    if (gNetbiosAvail != 0) {
        gNbMaxSess = param2;
        for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
            gNetStatus[i] = 0;
            gNbSessLsn[i] = NETBIOS_INVALID_ID;
            memset(&gNbSessNcb[i], 0, sizeof(gNbSessNcb[i]));
        }
        memset(gNbNameBuf, 0, sizeof(gNbNameBuf));
        InitializeCriticalSection(&gNbRcvLock);
        InitializeCriticalSection(&gNbSndLock);
        init_anchor(&gNbRcvQueue, 1, 0);
        init_anchor(&gNbSndQueue, 1, 0);
        init_anchor(&gNbFreeQueue, 1, 0);
        for (i = 0; i < NETBIOS_THREAD_EVENT_COUNT; i++)
            gNbEvents.handles[i] = CreateEventA(0, 1, 0, 0);
        memset(&localNcb, 0, sizeof(localNcb));
        statusBuffer = static_cast<unsigned char *>(BaseAlloc(
            NETBIOS_ADAPTER_STATUS_SIZE,
            "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
            gNbInitSourceLineBase +
                (NETWIN_SOURCE_LINE_INIT_ALLOC - NETWIN_SOURCE_LINE_INIT_BASE)));
        localNcb.command = NETBIOS_COMMAND_ADAPTER_STATUS;
        localNcb.length = NETBIOS_ADAPTER_STATUS_SIZE;
        localNcb.buffer = statusBuffer;
        localNcb.adapterNumber = gNetbiosLana;
        if (Netbios(&localNcb) == NETBIOS_RESULT_ENVIRONMENT_UNDEFINED) {
            memset(&localNcb, 0, sizeof(localNcb));
            localNcb.command = NETBIOS_COMMAND_RESET;
            localNcb.adapterNumber = gNetbiosLana;
            localNcb.callName[0] = NETBIOS_CALL_RETRY_LIMIT;
            localNcb.callName[2] = NETBIOS_RESULT_SESSION_CLOSED;
            Netbios(&localNcb);
        }
        BaseFree(statusBuffer, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                 gNbInitSourceLineBase +
                     (NETWIN_SOURCE_LINE_INIT_FREE - NETWIN_SOURCE_LINE_INIT_BASE));
        gNbShutdown = 0;
        return 0;
    }
    return 1;
}

VA(0x004a6ecc, 0x207)
extern "C" void __fastcall nb_term(void)
{
    tag_Node *node;
    NetbiosControlBlock localNcb;
    int i;

    for (i = 0; i < NETBIOS_SESSION_COUNT; i++)
        nb_close_session(i);
    if (gNbCtlNcb.commandComplete == NETBIOS_RESULT_PENDING) {
        memset(&localNcb, 0, sizeof(localNcb));
        localNcb.command = NETBIOS_COMMAND_CANCEL;
        localNcb.adapterNumber = gNetbiosLana;
        localNcb.buffer = &gNbCtlNcb;
        Netbios(&localNcb);
    }
    if ((gNetStatus[gNbMaxSess] & NETBIOS_SESSION_NAME_REGISTERED) != 0) {
        memset(&localNcb, 0, sizeof(localNcb));
        memcpy(localNcb.name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
        localNcb.command = NETBIOS_COMMAND_DELETE_NAME;
        localNcb.adapterNumber = gNetbiosLana;
        Netbios(&localNcb);
    }
    EnterCriticalSection(&gNbSndLock);
    while ((node = pop_node(&gNbSndQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                 gNbTermSourceLineBase +
                     (NETWIN_SOURCE_LINE_TERM_SEND_FREE - NETWIN_SOURCE_LINE_TERM_BASE));
    while ((node = pop_node(&gNbFreeQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                 gNbTermSourceLineBase +
                     (NETWIN_SOURCE_LINE_TERM_POOL_FREE - NETWIN_SOURCE_LINE_TERM_BASE));
    LeaveCriticalSection(&gNbSndLock);
    DeleteCriticalSection(&gNbSndLock);
    for (i = 0; i < NETBIOS_THREAD_EVENT_COUNT; i++) {
        CloseHandle(gNbEvents.handles[i]);
        gNbEvents.handles[i] = 0;
    }
    gNbShutdown |= 1;
    SetEvent(gNbEvents.handles[0]);
    EnterCriticalSection(&gNbRcvLock);
    while ((node = pop_node(&gNbRcvQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                 gNbTermSourceLineBase +
                     (NETWIN_SOURCE_LINE_TERM_RECEIVE_FREE - NETWIN_SOURCE_LINE_TERM_BASE));
    LeaveCriticalSection(&gNbRcvLock);
    DeleteCriticalSection(&gNbRcvLock);
}

VA(0x004a70d3, 0xb3)
extern "C" unsigned short __fastcall nb_rcv(short session, void *buf)
{
    tag_Node *node;
    int len;

    EnterCriticalSection(&gNbRcvLock);
    node = pop_node(&gNbRcvQueue);
    LeaveCriticalSection(&gNbRcvLock);
    if (node) {
        if (node->len >= session)
            len = static_cast<unsigned short>(session);
        else
            len = node->len;
        memcpy(buf, node->data, len);
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                 gNbReceiveSourceLineBase +
                     (NETWIN_SOURCE_LINE_RECEIVE_FREE - NETWIN_SOURCE_LINE_RECEIVE_BASE));
        return len;
    }
    return 0;
}

VA(0x004a7186, 0xe4)
extern "C" unsigned short __fastcall nb_snd(short session, short len, void *data)
{
    tag_Node *node;

    if (gNbMaxSess == session && len == 0) {
        nb_add_name();
        return 0;
    }
    if ((gNetStatus[session] & NETBIOS_SESSION_ACTIVE) == 0)
        return NETBIOS_RESULT_SESSION_OUT_OF_RANGE;
    node = static_cast<tag_Node *>(BaseAlloc(
        len + NETBIOS_PACKET_HEADER_SIZE,
        "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
        gNbSendSourceLineBase +
            (NETWIN_SOURCE_LINE_SEND_ALLOC - NETWIN_SOURCE_LINE_SEND_BASE)));
    node->len = len;
    node->sessionIndex = static_cast<unsigned char>(session);
    memcpy(node->data, data, len);
    EnterCriticalSection(&gNbSndLock);
    add_node(&gNbSndQueue, node);
    LeaveCriticalSection(&gNbSndLock);
    SetEvent(gNbEvents.handles[0]);
    return 0;
}

// @early-stop
// reloc-masked: all 0x4cd bytes match; 99.84% is delinked jump-table
// local-label identity only; external relocations agree.
VA(0x004a726a, 0x4cd)
extern "C" unsigned short __cdecl nb_sess(short operation, ...)
{
    int oldSession;
    int destinationSession;
    int detachFlag;
    NetbiosControlBlock controlBlock;
    char *callName;
    short returnCode;
    va_list argList;

    va_start(argList, operation);
    switch (operation) {
    case NETBIOS_SESSION_REGISTER:
        callName = va_arg(argList, char *);
        gNetStatus[gNbMaxSess] &= ~NETBIOS_SESSION_ERROR;
        nb_format_name(callName, gNbNameBuf[gNbMaxSess].bytes);
        memset(&gNbSessNcb[gNbMaxSess], 0, sizeof(NetbiosControlBlock));
        memcpy(gNbSessNcb[gNbMaxSess].name,
               gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
        gNbSessNcb[gNbMaxSess].command = NETBIOS_COMMAND_ADD_NAME |
                                         NETBIOS_COMMAND_ASYNC;
        gNbSessNcb[gNbMaxSess].postRoutine = nb_add_name_done;
        gNbSessNcb[gNbMaxSess].commandComplete = NETBIOS_RESULT_PENDING;
        gNbSessNcb[gNbMaxSess].adapterNumber = gNetbiosLana;
        returnCode = Netbios(&gNbSessNcb[gNbMaxSess]);
        break;

    case NETBIOS_SESSION_RECEIVE_ANY: {
        destinationSession = va_arg(argList, int);
        if (gNbSessNcb[destinationSession].commandComplete == NETBIOS_RESULT_PENDING) {
            switch (gNbSessNcb[destinationSession].command & ~NETBIOS_COMMAND_ASYNC) {
            case NETBIOS_COMMAND_CALL:
            case NETBIOS_COMMAND_RECEIVE_BROADCAST:
                return 0;
            default:
                break;
            }
            memset(&controlBlock, 0, sizeof(controlBlock));
            controlBlock.command = NETBIOS_COMMAND_CANCEL;
            controlBlock.adapterNumber = gNetbiosLana;
            controlBlock.buffer = &gNbSessNcb[destinationSession];
            Netbios(&controlBlock);
        }
        returnCode = nb_recv_any(destinationSession);
        break;
    }

    case NETBIOS_SESSION_CALL:
        destinationSession = va_arg(argList, int);
        callName = va_arg(argList, char *);
        nb_format_name(callName, gNbNameBuf[destinationSession].bytes);
        returnCode = nb_call(destinationSession, gNbNameBuf[destinationSession].bytes);
        break;

    case NETBIOS_SESSION_LISTEN_ANY:
        destinationSession = va_arg(argList, int);
        nb_snd(gNbMaxSess, 0, 0);
        returnCode = nb_listen(destinationSession, gNbListenName);
        break;

    case NETBIOS_SESSION_LISTEN:
        destinationSession = va_arg(argList, int);
        callName = va_arg(argList, char *);
        nb_format_name(callName, gNbNameBuf[destinationSession].bytes);
        returnCode = nb_listen(destinationSession, gNbNameBuf[destinationSession].bytes);
        break;

    case NETBIOS_SESSION_MOVE:
        oldSession = va_arg(argList, int);
        destinationSession = va_arg(argList, int);
        detachFlag = va_arg(argList, int);
        if (gNbMaxSess == oldSession)
            gNbMaxSess = static_cast<unsigned char>(destinationSession);
        if (gNbSessLsn[oldSession] == NETBIOS_INVALID_ID)
            return 0;
        gNbSessLsn[destinationSession] = gNbSessLsn[oldSession];
        gNetStatus[destinationSession] = gNetStatus[oldSession];
        memcpy(gNbNameBuf[destinationSession].bytes,
               gNbNameBuf[oldSession].bytes, NETBIOS_NAME_SIZE);
        nb_arm_recv(destinationSession);
        if (detachFlag != 0) {
            gNbSessLsn[oldSession] = NETBIOS_INVALID_ID;
            gNetStatus[oldSession] = 0;
            memset(gNbNameBuf[oldSession].bytes, 0, NETBIOS_NAME_SIZE);
        }
        returnCode = 0;
        break;

    case NETBIOS_SESSION_CLOSE:
        destinationSession = va_arg(argList, int);
        if (gNbSessNcb[destinationSession].commandComplete == NETBIOS_RESULT_PENDING) {
            memset(&controlBlock, 0, sizeof(controlBlock));
            controlBlock.command = NETBIOS_COMMAND_CANCEL;
            controlBlock.adapterNumber = gNetbiosLana;
            controlBlock.buffer = &gNbSessNcb[destinationSession];
            Netbios(&controlBlock);
        }
        nb_close_session(destinationSession);
        returnCode = 0;
        break;

    case NETBIOS_SESSION_CLEAR_CONNECTED:
        destinationSession = va_arg(argList, int);
        gNetStatus[destinationSession] &= ~NETBIOS_SESSION_CONNECTED;
        returnCode = 0;
        break;

    case NETBIOS_SESSION_GET_NAME:
        destinationSession = va_arg(argList, int);
        callName = va_arg(argList, char *);
        memcpy(callName, gNbNameBuf[destinationSession].bytes, NETBIOS_NAME_SIZE);
        returnCode = 0;
        break;

    default:
        return 1;
    }
    if (returnCode == NETBIOS_RESULT_PENDING)
        returnCode = 0;
    return returnCode;
}

VA(0x004a7737, 0x21)
extern "C" char __fastcall nb_stat(short session) { return gNetStatus[session]; }

// nb_thr_ctl — the receiver-thread pump: drain completed per-session receives into the rcv queue,
// then drain the send/free queues, issuing a synchronous NCBSEND for each queued packet.
// @early-stop
// Byte-proven delinker wall: the explicit 0xdd2-byte CodeView span, including
// 11 folded static helpers, has 124 raw differences and all are inside the
// union of 242 base and 226 retail relocation fields; zero bytes remain.
// The 49 shared external relocations have identical targets. The 16 base-only
// sites are 11 IAT calls and five calls to folded helpers whose retail rel32
// bytes resolve to the same entries. The 6.73% score is local identity only.
VA(0x004a7758, 0xdd2)
void nb_thr_ctl(void)
{
    int keepRunning;
    int i;
    tag_Node *node;
    NetbiosControlBlock localNcb;
    unsigned char rc;
    int sendComplete;

    keepRunning = 1;
    if (WaitForMultipleObjects(NETBIOS_THREAD_EVENT_COUNT,
                               gNbEvents.handles, 0, 0) == WAIT_TIMEOUT)
        return;
    {
        if (WaitForSingleObject(gNbEvents.handles[0], 0) == WAIT_OBJECT_0)
            ResetEvent(gNbEvents.handles[0]);
        for (i = 0; i < NETBIOS_RECEIVE_EVENT_COUNT; i++) {
            if (WaitForSingleObject(
                    gNbEvents.handles[i + NETBIOS_RECEIVE_EVENT_FIRST], 0) ==
                WAIT_OBJECT_0) {
                ResetEvent(
                    gNbEvents.handles[i + NETBIOS_RECEIVE_EVENT_FIRST]);
                nb_recv_complete(i);
            }
        }
        while (keepRunning) {
            EnterCriticalSection(&gNbSndLock);
            node = pop_node(&gNbFreeQueue);
            if (node == 0)
                node = pop_node(&gNbSndQueue);
            LeaveCriticalSection(&gNbSndLock);
            if (node == 0) {
                keepRunning = 0;
            } else {
                memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
                gNbCtlNcb.sessionNumber = gNbSessLsn[node->sessionIndex];
                if (gNbCtlNcb.sessionNumber != NETBIOS_INVALID_ID) {
                    memcpy(gNbSessBuf.bytes, node->data, node->len);
                    gNbCtlNcb.buffer = gNbSessBuf.bytes;
                    gNbCtlNcb.length = node->len;
                    gNbCtlNcb.command = NETBIOS_COMMAND_SEND;
                    gNbCtlNcb.adapterNumber = gNetbiosLana;
                    sendComplete = 0;
                    while (!sendComplete) {
                        rc = Netbios(&gNbCtlNcb);
                        switch (rc) {
                        case NETBIOS_RESULT_SUCCESS:
                            sendComplete = 1;
                            break;
                        case NETBIOS_RESULT_PENDING:
                            ProcessAssert(
                                0,
                                "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                                gNbThreadSourceLineBase +
                                    (NETWIN_SOURCE_LINE_THREAD_PENDING_ASSERT -
                                     NETWIN_SOURCE_LINE_THREAD_BASE));
                            break;
                        case NETBIOS_RESULT_SESSION_OUT_OF_RANGE:
                        case NETBIOS_RESULT_SESSION_CLOSED:
                        case NETBIOS_RESULT_SESSION_ENDED:
                            gNetStatus[node->sessionIndex] &= ~NETBIOS_SESSION_ACTIVE;
                            break;
                        default:
                            break;
                        }
                    }
                }
                BaseFree(node,
                         "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                         gNbThreadSourceLineBase +
                             (NETWIN_SOURCE_LINE_THREAD_FREE -
                              NETWIN_SOURCE_LINE_THREAD_BASE));
            }
        }
    }
}

// The remaining file-static routines occupy the rest of nb_thr_ctl's CodeView
// span. Their source order is fixed by the retail entry addresses.

VA(0x004a79c6, 0xbb)
static void nb_add_name(void)
{
    if (gNbCtlNcb.commandComplete != NETBIOS_RESULT_PENDING) {
        strcpy(gNbSessBuf.bytes, gNbGroupName);
        memcpy(gNbSessBuf.bytes + strlen(gNbGroupName),
               gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
        memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
        gNbCtlNcb.command = NETBIOS_COMMAND_RECEIVE_DATAGRAM |
                            NETBIOS_COMMAND_ASYNC;
        gNbCtlNcb.nameNumber = gNbLocalNum;
        gNbCtlNcb.length = strlen(gNbGroupName) + NETBIOS_NAME_SIZE;
        gNbCtlNcb.buffer = gNbSessBuf.bytes;
        gNbCtlNcb.adapterNumber = gNetbiosLana;
        Netbios(&gNbCtlNcb);
    }
}

VA(0x004a7a81, 0x174)
static void __stdcall nb_add_name_done(NetbiosControlBlock *ncb)
{
    int j;
    ProcessAssert(&gNbSessNcb[gNbMaxSess] == ncb,
                  "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                  gNbAddNameSourceLineBase +
                      (NETWIN_SOURCE_LINE_ADD_NAME_ASSERT -
                       NETWIN_SOURCE_LINE_ADD_NAME_BASE));
    switch (ncb->returnCode) {
    case NETBIOS_RESULT_SUCCESS:
    case NETBIOS_RESULT_CANCEL_COMPLETED:
        gNbLocalNum = ncb->nameNumber;
        memcpy(gNbNameBuf[gNbMaxSess].bytes, ncb->name,
               NETBIOS_NAME_SIZE);
        gNetStatus[gNbMaxSess] |= NETBIOS_SESSION_NAME_REGISTERED;
        break;
    case NETBIOS_RESULT_DUPLICATE_NAME:
    case NETBIOS_RESULT_NAME_IN_USE:
    case NETBIOS_RESULT_NAME_CONFLICT:
    case NETBIOS_RESULT_DUPLICATE_ENVIRONMENT:
        for (j = NETBIOS_NAME_SIZE - 1; j >= 0; j--) {
            ncb->name[j]++;
            if (gNbNameBuf[gNbMaxSess].bytes[j] != ncb->name[j])
                break;
        }
        Netbios(ncb);
        break;
    case NETBIOS_RESULT_CANCELLED:
        break;
    default:
        sprintf(gText, "Add Name Error: %02x", ncb->returnCode);
        ShutDown(gText);
        gNetStatus[gNbMaxSess] |= NETBIOS_SESSION_ERROR;
        break;
    }
}

VA(0x004a7c4b, 0xbe)
static unsigned short __fastcall nb_recv_any(int session)
{
    if (gNbSessNcb[session].commandComplete != NETBIOS_RESULT_PENDING) {
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command = NETBIOS_COMMAND_RECEIVE_BROADCAST |
                                      NETBIOS_COMMAND_ASYNC;
        gNbSessNcb[session].nameNumber = gNbLocalNum;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].postRoutine = nb_recv_any_done;
        Netbios(&gNbSessNcb[session]);
    }
    return gNbSessNcb[session].commandComplete;
}

VA(0x004a7d09, 0x142)
static void __stdcall nb_recv_any_done(NetbiosControlBlock *ncb)
{
    int i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (&gNbSessNcb[i] == ncb)
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    {
        if (gNbSessNcb[i].returnCode == NETBIOS_RESULT_SUCCESS) {
            if (memcmp(gNbRcvData[i].bytes, gNbGroupName,
                       strlen(gNbGroupName)) == 0) {
                memcpy(gNbNameBuf[i].bytes,
                       gNbRcvData[i].bytes + strlen(gNbGroupName),
                       NETBIOS_NAME_SIZE);
                nb_call(i, gNbNameBuf[i].bytes);
            } else {
                Netbios(&gNbSessNcb[i]);
            }
        } else if (gNbSessNcb[i].returnCode != NETBIOS_RESULT_CANCELLED &&
                   gNbSessNcb[i].returnCode != NETBIOS_RESULT_CANCEL_COMPLETED) {
            Netbios(&gNbSessNcb[i]);
        }
    }
}

VA(0x004a7e4b, 0xcf)
static unsigned short __fastcall nb_call(int session, void *name)
{
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes,
           NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_CALL | NETBIOS_COMMAND_ASYNC;
    gNbSessNcb[session].commandComplete = NETBIOS_RESULT_PENDING;
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x004a7f1a, 0xcf)
static unsigned short __fastcall nb_listen(int session, void *name)
{
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes,
           NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_LISTEN |
                                  NETBIOS_COMMAND_ASYNC;
    gNbSessNcb[session].commandComplete = NETBIOS_RESULT_PENDING;
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x004a7fe9, 0x126)
static void __stdcall nb_call_done(NetbiosControlBlock *ncb)
{
    int i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (&gNbSessNcb[i] == ncb)
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    switch (gNbSessNcb[i].returnCode) {
    case NETBIOS_RESULT_SUCCESS:
        gNbSessLsn[i] = gNbSessNcb[i].sessionNumber;
        memcpy(gNbNameBuf[i].bytes, gNbSessNcb[i].callName,
               NETBIOS_NAME_SIZE);
        gNetStatus[i] |= NETBIOS_SESSION_ACTIVE |
                         NETBIOS_SESSION_CONNECTED;
        nb_arm_recv(i);
        break;
    case NETBIOS_RESULT_CANCELLED:
    case NETBIOS_RESULT_CANCEL_COMPLETED:
        break;
    default:
        gNbCallRetries++;
        if (gNbCallRetries < NETBIOS_CALL_RETRY_LIMIT) {
            Sleep(NETBIOS_CALL_RETRY_DELAY);
            Netbios(&gNbSessNcb[i]);
        }
        break;
    }
}

VA(0x004a8119, 0x13b)
static void __fastcall nb_arm_recv(int session)
{
    unsigned char result;
    for (;;) {
        ProcessAssert(gNbSessNcb[session].returnCode != NETBIOS_RESULT_PENDING,
                      "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                      gNbArmReceiveSourceLineBase +
                          (NETWIN_SOURCE_LINE_ARM_RECEIVE_ASSERT -
                           NETWIN_SOURCE_LINE_ARM_RECEIVE_BASE));
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command = NETBIOS_COMMAND_RECEIVE |
                                      NETBIOS_COMMAND_ASYNC;
        gNbSessNcb[session].sessionNumber = gNbSessLsn[session];
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].adapterNumber = gNetbiosLana;
        gNbSessNcb[session].event =
            gNbEvents.handles[session + NETBIOS_RECEIVE_EVENT_FIRST];
        result = Netbios(&gNbSessNcb[session]);
        switch (result) {
        case NETBIOS_RESULT_SUCCESS:
        case NETBIOS_RESULT_SESSION_OUT_OF_RANGE:
        case NETBIOS_RESULT_SESSION_CLOSED:
        case NETBIOS_RESULT_SESSION_ENDED:
        case NETBIOS_RESULT_PENDING:
            return;
        default:
            Sleep(NETBIOS_RECEIVE_RETRY_DELAY);
            continue;
        }
    }
}

VA(0x004a8268, 0xc2)
static void __fastcall nb_close_session(int session)
{
    NetbiosControlBlock controlBlock;
    if (gNbSessNcb[session].commandComplete == NETBIOS_RESULT_PENDING) {
        memset(&controlBlock, 0, sizeof(controlBlock));
        controlBlock.command = NETBIOS_COMMAND_CANCEL;
        controlBlock.adapterNumber = gNetbiosLana;
        controlBlock.buffer = &gNbSessNcb[session];
        Netbios(&controlBlock);
    }
    if (gNbSessLsn[session] != NETBIOS_INVALID_ID) {
        memset(&controlBlock, 0, sizeof(controlBlock));
        controlBlock.sessionNumber = gNbSessLsn[session];
        controlBlock.command = NETBIOS_COMMAND_HANGUP;
        controlBlock.adapterNumber = gNetbiosLana;
        Netbios(&controlBlock);
        gNetStatus[session] &= ~NETBIOS_SESSION_ACTIVE;
    }
}

VA(0x004a832a, 0x147)
static void __fastcall nb_recv_complete(int session)
{
    tag_Node *node;
    switch (gNbSessNcb[session].command & ~NETBIOS_COMMAND_ASYNC) {
    case NETBIOS_COMMAND_RECEIVE:
        switch (gNbSessNcb[session].returnCode) {
        case NETBIOS_RESULT_SUCCESS:
            node = static_cast<tag_Node *>(BaseAlloc(
                gNbSessNcb[session].length + NETBIOS_PACKET_HEADER_SIZE,
                "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp",
                gNbReceiveCompleteSourceLineBase +
                    (NETWIN_SOURCE_LINE_RECEIVE_COMPLETE_ALLOC -
                     NETWIN_SOURCE_LINE_RECEIVE_COMPLETE_BASE)));
            if (node != 0) {
                node->len = gNbSessNcb[session].length;
                node->sessionIndex = static_cast<unsigned char>(session);
                memcpy(node->data, gNbRcvData[session].bytes, node->len);
                EnterCriticalSection(&gNbRcvLock);
                add_node(&gNbRcvQueue, node);
                LeaveCriticalSection(&gNbRcvLock);
            }
            nb_arm_recv(session);
            break;
        case NETBIOS_RESULT_SESSION_OUT_OF_RANGE:
        case NETBIOS_RESULT_SESSION_CLOSED:
        case NETBIOS_RESULT_SESSION_ENDED:
            gNetStatus[session] &= ~NETBIOS_SESSION_ACTIVE;
            break;
        default:
            nb_arm_recv(session);
            break;
        }
    }
}

VA(0x004a84a3, 0x87)
static void __fastcall nb_format_name(char *src, unsigned char *dst)
{
    unsigned int i;
    memset(dst, 0, NETBIOS_NAME_SIZE);
    for (i = 0; i < NETBIOS_NAME_SIZE - 1 && *src != '\0'; i++, src++)
        dst[i] = *src;
    for (; i < NETBIOS_NAME_SIZE - 1; i++)
        dst[i] = ' ';
}
