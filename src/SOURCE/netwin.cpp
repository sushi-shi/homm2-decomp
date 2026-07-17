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

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp"

// ---- module-private globals (retail xref: single-module) ----
DATA(0x0051739c) static u8 gNbCallRetries = 0; // nb_call_done retry counter
DATA(0x005173a0) static u8 gNetbiosAvail = 0;
DATA(0x005173a4) static u8 gNetbiosLana = 0;
DATA(0x005173a8) static u8 gNbShutdown = 0; // shutdown flag, cleared by nb_init
DATA(0x005173ac) static u8 gNbMaxSess = NETBIOS_INVALID_ID;
DATA(0x005173b0) static u8 gNbLocalNum = 0; // local netbios name number
DATA(0x005173b4) static u8 gNetStatus[NETBIOS_STATUS_COUNT] = {0};
DATA(0x005173c0) static char* gNbGroupName = "Empire Too ";
DATA(0x005173c4) static char* gNbListenName = "*";

// @data-layout-note Retail initialized storage is 0x11739c..0x117620.
// Source DATA owners recover the nine module globals and eight function-local
// line bases. Fifteen private literals cover the remaining payload. The two
// otherwise unreferenced source-path strings are byte-exact immediately after
// 534 and gNbReceiveCompleteSourceLineBase, at 0x11757c
// and 0x1175f4. All other private owners have singleton relocation or unique
// payload proof. The candidate payload ends at 0x11761e; retail's final two
// bytes are zero alignment. Loader-zero storage is the exact 0x82e8-byte BSS
// group below; do not add a padding owner for the initialized tail.

// Semantic suffixes retain the retail MSVC BSS allocation order; audit with section replay.
DATA(0x0052ae68) static tag_Anchor gNbFreeQueueRuntime;
DATA(0x0052ae70) static u8 gNbSessionNumbersEntry[NETBIOS_SESSION_COUNT];
DATA(0x0052ae78) static NetbiosPayload gNbReceiveDataLocal[NETBIOS_SESSION_COUNT];
DATA(0x00531e78) static NetbiosName gNbNameBufferBacking[NETBIOS_SESSION_COUNT];
DATA(0x00531ee8) static NetbiosSessionBuffer gNbSessionBufferContext;
DATA(0x00532ee8) static NetbiosControlBlock gNbSessionControlBlocksArena[NETBIOS_SESSION_COUNT];
DATA(0x005330a8) static NetbiosControlBlock gNbControlBlockArena;
DATA(0x005330e8) static tag_Anchor gNbReceiveQueueEntry;
DATA(0x005330f0) static tag_Anchor gNbSendQueueHead;
DATA(0x005330f8) static CRITICAL_SECTION gNbReceiveLockCriticalSection;
DATA(0x00533110) static NetbiosThreadEvents gNbThreadEventsContext;
DATA(0x00533138) static CRITICAL_SECTION gNbSendLockBacking;

#define gNbFreeQueue gNbFreeQueueRuntime
#define gNbSessLsn gNbSessionNumbersEntry
#define gNbRcvData gNbReceiveDataLocal
#define gNbNameBuf gNbNameBufferBacking
#define gNbSessBuf gNbSessionBufferContext
#define gNbSessNcb gNbSessionControlBlocksArena
#define gNbCtlNcb gNbControlBlockArena
#define gNbRcvQueue gNbReceiveQueueEntry
#define gNbSndQueue gNbSendQueueHead
#define gNbRcvLock gNbReceiveLockCriticalSection
#define gNbEvents gNbThreadEventsContext
#define gNbSndLock gNbSendLockBacking

VA(0x004a6be0, 0xa8)
i32 is_netbios_avail(void) {
    NetbiosControlBlock ncb;
    memset(&ncb, 0, sizeof(ncb));
    for (gNetbiosLana = 0; gNetbiosLana < NETBIOS_MAX_LANA; gNetbiosLana++) {
        memset(&ncb, 0, sizeof(ncb));
        ncb.command = IDX(NETBIOS_COMMAND_PROBE);
        ncb.adapterNumber = gNetbiosLana;
        if (Netbios(&ncb) == IDX(NETBIOS_RESULT_ILLEGAL_COMMAND))
            break;
    }
    if (gNetbiosLana < NETBIOS_MAX_LANA) {
        gNetbiosAvail = 1;
        return 1;
    }
    return 0;
}

VA(0x004a6c88, 0x244)
extern "C" u16 __fastcall nb_init(u16 maxNames, u16 maxSessions) {
    // Retail interleaves each line base with this function's literals; this compiler groups them.
    DATA(0x005173d8) static i16 gNbInitSourceLineBase = 105;
    NetbiosControlBlock localNcb;
    i32 i;
    u8* statusBuffer;
    i32 returnCode;

    memset(gNbSessLsn, 0, sizeof(gNbSessLsn));
    memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
    memset(gNbSessNcb, 0, sizeof(gNbSessNcb));
    memset(&gNbSessBuf, -1, sizeof(gNbSessBuf));
    memset(gNbRcvData, -1, sizeof(gNbRcvData));
    memset(&gNbEvents, 0, NETBIOS_THREAD_EVENTS_SIZE);
    if (is_netbios_avail() == 0)
        return 1;
    if (gNetbiosAvail != 0) {
        gNbMaxSess = maxSessions;
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
        statusBuffer = static_cast<u8*>(H2_ALLOC(NETBIOS_ADAPTER_STATUS_SIZE, 145));
        localNcb.command = IDX(NETBIOS_COMMAND_ADAPTER_STATUS);
        localNcb.length = NETBIOS_ADAPTER_STATUS_SIZE;
        localNcb.buffer = statusBuffer;
        localNcb.adapterNumber = gNetbiosLana;
        if (Netbios(&localNcb) == IDX(NETBIOS_RESULT_ENVIRONMENT_UNDEFINED)) {
            memset(&localNcb, 0, sizeof(localNcb));
            localNcb.command = IDX(NETBIOS_COMMAND_RESET);
            localNcb.adapterNumber = gNetbiosLana;
            localNcb.callName[0] = NETBIOS_CALL_RETRY_LIMIT;
            localNcb.callName[2] = IDX(NETBIOS_RESULT_SESSION_CLOSED);
            Netbios(&localNcb);
        }
        H2_FREE(statusBuffer, 159);
        gNbShutdown = 0;
        return 0;
    }
    return 1;
}

VA(0x004a6ecc, 0x207)
extern "C" void __fastcall nb_term(void) {
    DATA(0x00517434) static i16 gNbTermSourceLineBase = 169;
    tag_Node* node;
    NetbiosControlBlock localNcb;
    i32 i;

    for (i = 0; i < NETBIOS_SESSION_COUNT; i++)
        nb_close_session(i);
    if (gNbCtlNcb.commandComplete == IDX(NETBIOS_RESULT_PENDING)) {
        memset(&localNcb, 0, sizeof(localNcb));
        localNcb.command = IDX(NETBIOS_COMMAND_CANCEL);
        localNcb.adapterNumber = gNetbiosLana;
        localNcb.buffer = &gNbCtlNcb;
        Netbios(&localNcb);
    }
    if ((gNetStatus[gNbMaxSess] & IDX(NETBIOS_SESSION_NAME_REGISTERED)) != 0) {
        memset(&localNcb, 0, sizeof(localNcb));
        memcpy(localNcb.name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
        localNcb.command = IDX(NETBIOS_COMMAND_DELETE_NAME);
        localNcb.adapterNumber = gNetbiosLana;
        Netbios(&localNcb);
    }
    EnterCriticalSection(&gNbSndLock);
    while ((node = pop_node(&gNbSndQueue)) != 0)
        H2_FREE(node, 169 + (200 - 169));
    while ((node = pop_node(&gNbFreeQueue)) != 0)
        H2_FREE(node, 169 + (204 - 169));
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
        H2_FREE(node, 169 + (219 - 169));
    LeaveCriticalSection(&gNbRcvLock);
    DeleteCriticalSection(&gNbRcvLock);
}

VA(0x004a70d3, 0xb3)
extern "C" u16 __fastcall nb_rcv(i16 session, void* buf) {
    DATA(0x005174bc) static i16 gNbReceiveSourceLineBase = 226;
    tag_Node* node;
    i32 len;

    EnterCriticalSection(&gNbRcvLock);
    node = pop_node(&gNbRcvQueue);
    LeaveCriticalSection(&gNbRcvLock);
    if (node) {
        if (node->len >= session)
            len = static_cast<u16>(session);
        else
            len = node->len;
        memcpy(buf, node->data, len);
        H2_FREE(node, 226 + (237 - 226));
        return len;
    }
    return 0;
}

VA(0x004a7186, 0xe4)
extern "C" u16 __fastcall nb_snd(i16 session, i16 len, void* data) {
    DATA(0x005174ec) static i16 gNbSendSourceLineBase = 245;
    tag_Node* node;

    if (gNbMaxSess == session && len == 0) {
        nb_add_name();
        return 0;
    }
    if ((gNetStatus[session] & IDX(NETBIOS_SESSION_ACTIVE)) == 0)
        return NETBIOS_RESULT_SESSION_OUT_OF_RANGE;
    node = static_cast<tag_Node*>(H2_ALLOC(len + NETBIOS_PACKET_HEADER_SIZE, 260));
    node->len = len;
    node->sessionIndex = static_cast<u8>(session);
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
extern "C" u16 __cdecl nb_sess(i16 operation, ...) {
    i32 oldSession;
    i32 destinationSession;
    i32 detachFlag;
    NetbiosControlBlock controlBlock;
    char* callName;
    i16 returnCode;
    va_list argList;

    va_start(argList, operation);
    switch (operation) {
        case IDX(NETBIOS_SESSION_REGISTER):
            callName = va_arg(argList, char*);
            gNetStatus[gNbMaxSess] &= ~IDX(NETBIOS_SESSION_ERROR);
            nb_format_name(callName, gNbNameBuf[gNbMaxSess].bytes);
            memset(&gNbSessNcb[gNbMaxSess], 0, sizeof(NetbiosControlBlock));
            memcpy(gNbSessNcb[gNbMaxSess].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
            gNbSessNcb[gNbMaxSess].command = NETBIOS_COMMAND_ADD_NAME | IDX(NETBIOS_COMMAND_ASYNC);
            gNbSessNcb[gNbMaxSess].postRoutine = nb_add_name_done;
            gNbSessNcb[gNbMaxSess].commandComplete = IDX(NETBIOS_RESULT_PENDING);
            gNbSessNcb[gNbMaxSess].adapterNumber = gNetbiosLana;
            returnCode = Netbios(&gNbSessNcb[gNbMaxSess]);
            break;

        case IDX(NETBIOS_SESSION_RECEIVE_ANY): {
            destinationSession = va_arg(argList, i32);
            if (gNbSessNcb[destinationSession].commandComplete == IDX(NETBIOS_RESULT_PENDING)) {
                switch (gNbSessNcb[destinationSession].command & ~IDX(NETBIOS_COMMAND_ASYNC)) {
                    case IDX(NETBIOS_COMMAND_CALL):
                    case IDX(NETBIOS_COMMAND_RECEIVE_BROADCAST):
                        return 0;
                    default:
                        break;
                }
                memset(&controlBlock, 0, sizeof(controlBlock));
                controlBlock.command = IDX(NETBIOS_COMMAND_CANCEL);
                controlBlock.adapterNumber = gNetbiosLana;
                controlBlock.buffer = &gNbSessNcb[destinationSession];
                Netbios(&controlBlock);
            }
            returnCode = nb_recv_any(destinationSession);
            break;
        }

        case IDX(NETBIOS_SESSION_CALL):
            destinationSession = va_arg(argList, i32);
            callName = va_arg(argList, char*);
            nb_format_name(callName, gNbNameBuf[destinationSession].bytes);
            returnCode = nb_call(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case IDX(NETBIOS_SESSION_LISTEN_ANY):
            destinationSession = va_arg(argList, i32);
            nb_snd(gNbMaxSess, 0, 0);
            returnCode = nb_listen(destinationSession, gNbListenName);
            break;

        case IDX(NETBIOS_SESSION_LISTEN):
            destinationSession = va_arg(argList, i32);
            callName = va_arg(argList, char*);
            nb_format_name(callName, gNbNameBuf[destinationSession].bytes);
            returnCode = nb_listen(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case IDX(NETBIOS_SESSION_MOVE):
            oldSession = va_arg(argList, i32);
            destinationSession = va_arg(argList, i32);
            detachFlag = va_arg(argList, i32);
            if (gNbMaxSess == oldSession)
                gNbMaxSess = static_cast<u8>(destinationSession);
            if (gNbSessLsn[oldSession] == NETBIOS_INVALID_ID)
                return 0;
            gNbSessLsn[destinationSession] = gNbSessLsn[oldSession];
            gNetStatus[destinationSession] = gNetStatus[oldSession];
            memcpy(
                gNbNameBuf[destinationSession].bytes,
                gNbNameBuf[oldSession].bytes,
                NETBIOS_NAME_SIZE
            );
            nb_arm_recv(destinationSession);
            if (detachFlag != 0) {
                gNbSessLsn[oldSession] = NETBIOS_INVALID_ID;
                gNetStatus[oldSession] = 0;
                memset(gNbNameBuf[oldSession].bytes, 0, NETBIOS_NAME_SIZE);
            }
            returnCode = 0;
            break;

        case IDX(NETBIOS_SESSION_CLOSE):
            destinationSession = va_arg(argList, i32);
            if (gNbSessNcb[destinationSession].commandComplete == IDX(NETBIOS_RESULT_PENDING)) {
                memset(&controlBlock, 0, sizeof(controlBlock));
                controlBlock.command = IDX(NETBIOS_COMMAND_CANCEL);
                controlBlock.adapterNumber = gNetbiosLana;
                controlBlock.buffer = &gNbSessNcb[destinationSession];
                Netbios(&controlBlock);
            }
            nb_close_session(destinationSession);
            returnCode = 0;
            break;

        case IDX(NETBIOS_SESSION_CLEAR_CONNECTED):
            destinationSession = va_arg(argList, i32);
            gNetStatus[destinationSession] &= ~IDX(NETBIOS_SESSION_CONNECTED);
            returnCode = 0;
            break;

        case IDX(NETBIOS_SESSION_GET_NAME):
            destinationSession = va_arg(argList, i32);
            callName = va_arg(argList, char*);
            memcpy(callName, gNbNameBuf[destinationSession].bytes, NETBIOS_NAME_SIZE);
            returnCode = 0;
            break;

        default:
            return 1;
    }
    if (returnCode == IDX(NETBIOS_RESULT_PENDING))
        returnCode = 0;
    return returnCode;
}

VA(0x004a7737, 0x21)
extern "C" char __fastcall nb_stat(i16 session) {
    return gNetStatus[session];
}

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
void nb_thr_ctl(void) {
    DATA(0x0051751c) static i16 gNbThreadSourceLineBase = 412;
    i32 keepRunning;
    i32 i;
    tag_Node* node;
    NetbiosControlBlock localNcb;
    u8 rc;
    i32 sendComplete;

    keepRunning = 1;
    if (WaitForMultipleObjects(NETBIOS_THREAD_EVENT_COUNT, gNbEvents.handles, 0, 0) == WAIT_TIMEOUT)
        return;
    {
        if (WaitForSingleObject(gNbEvents.handles[0], 0) == WAIT_OBJECT_0)
            ResetEvent(gNbEvents.handles[0]);
        for (i = 0; i < NETBIOS_RECEIVE_EVENT_COUNT; i++) {
            if (WaitForSingleObject(gNbEvents.handles[i + NETBIOS_RECEIVE_EVENT_FIRST], 0)
                == WAIT_OBJECT_0) {
                ResetEvent(gNbEvents.handles[i + NETBIOS_RECEIVE_EVENT_FIRST]);
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
                    gNbCtlNcb.command = IDX(NETBIOS_COMMAND_SEND);
                    gNbCtlNcb.adapterNumber = gNetbiosLana;
                    sendComplete = 0;
                    while (!sendComplete) {
                        rc = Netbios(&gNbCtlNcb);
                        switch (rc) {
                            case IDX(NETBIOS_RESULT_SUCCESS):
                                sendComplete = 1;
                                break;
                            case IDX(NETBIOS_RESULT_PENDING):
                                ProcessAssert(0, RETAIL_FILE, 412 + (495 - 412));
                                break;
                            case IDX(NETBIOS_RESULT_SESSION_OUT_OF_RANGE):
                            case IDX(NETBIOS_RESULT_SESSION_CLOSED):
                            case IDX(NETBIOS_RESULT_SESSION_ENDED):
                                gNetStatus[node->sessionIndex] &= ~IDX(NETBIOS_SESSION_ACTIVE);
                                break;
                            default:
                                break;
                        }
                    }
                }
                H2_FREE(node, 412 + (508 - 412));
            }
        }
    }
}

// The remaining file-static routines occupy the rest of nb_thr_ctl's CodeView
// span. Their source order is fixed by the retail entry addresses.

VA(0x004a79c6, 0xbb)
static void nb_add_name(void) {
    if (gNbCtlNcb.commandComplete != IDX(NETBIOS_RESULT_PENDING)) {
        strcpy(gNbSessBuf.bytes, gNbGroupName);
        memcpy(
            gNbSessBuf.bytes + strlen(gNbGroupName),
            gNbNameBuf[gNbMaxSess].bytes,
            NETBIOS_NAME_SIZE
        );
        memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
        gNbCtlNcb.command = NETBIOS_COMMAND_RECEIVE_DATAGRAM | IDX(NETBIOS_COMMAND_ASYNC);
        gNbCtlNcb.nameNumber = gNbLocalNum;
        gNbCtlNcb.length = strlen(gNbGroupName) + NETBIOS_NAME_SIZE;
        gNbCtlNcb.buffer = gNbSessBuf.bytes;
        gNbCtlNcb.adapterNumber = gNetbiosLana;
        Netbios(&gNbCtlNcb);
    }
}

// @early-stop
// All 0x1ca bytes match after masking 35 aligned relocations.  The 0x08 frame,
// j at -0x04, switch value at -0x08, CFG, 0x20-byte case-pointer table, and
// adjacent 0x31-byte selector table agree.  The 99.70% residual is delinked
// private-global, string, interior-array, and local-label symbol identity only;
// every resolved external relocation target agrees.
VA(0x004a7a81, 0x1ca)
static void __stdcall nb_add_name_done(NetbiosControlBlock* ncb) {
    DATA(0x00517578) static i16 gNbAddNameSourceLineBase = 534;
    i32 j;
    ProcessAssert(&gNbSessNcb[gNbMaxSess] == ncb, RETAIL_FILE, 534 + (537 - 534));
    switch (ncb->returnCode) {
        case IDX(NETBIOS_RESULT_SUCCESS):
        case IDX(NETBIOS_RESULT_CANCEL_COMPLETED):
            gNbLocalNum = ncb->nameNumber;
            memcpy(gNbNameBuf[gNbMaxSess].bytes, ncb->name, NETBIOS_NAME_SIZE);
            gNetStatus[gNbMaxSess] |= IDX(NETBIOS_SESSION_NAME_REGISTERED);
            break;
        case IDX(NETBIOS_RESULT_DUPLICATE_NAME):
        case IDX(NETBIOS_RESULT_NAME_IN_USE):
        case IDX(NETBIOS_RESULT_NAME_CONFLICT):
        case IDX(NETBIOS_RESULT_DUPLICATE_ENVIRONMENT):
            for (j = NETBIOS_NAME_SIZE - 1; j >= 0; j--) {
                ncb->name[j]++;
                if (gNbNameBuf[gNbMaxSess].bytes[j] != ncb->name[j])
                    break;
            }
            Netbios(ncb);
            break;
        case IDX(NETBIOS_RESULT_CANCELLED):
            break;
        default:
            sprintf(gText, "Add Name Error %02x\n", ncb->returnCode);
            ShutDown(gText);
            gNetStatus[gNbMaxSess] |= IDX(NETBIOS_SESSION_ERROR);
            break;
    }
}

VA(0x004a7c4b, 0xbe)
static u16 __fastcall nb_recv_any(i32 session) {
    if (gNbSessNcb[session].commandComplete != IDX(NETBIOS_RESULT_PENDING)) {
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command = NETBIOS_COMMAND_RECEIVE_BROADCAST | IDX(NETBIOS_COMMAND_ASYNC);
        gNbSessNcb[session].nameNumber = gNbLocalNum;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].postRoutine = nb_recv_any_done;
        Netbios(&gNbSessNcb[session]);
    }
    return gNbSessNcb[session].commandComplete;
}

VA(0x004a7d09, 0x142)
static void __stdcall nb_recv_any_done(NetbiosControlBlock* ncb) {
    i32 i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (&gNbSessNcb[i] == ncb)
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    {
        if (gNbSessNcb[i].returnCode == IDX(NETBIOS_RESULT_SUCCESS)) {
            if (memcmp(gNbRcvData[i].bytes, gNbGroupName, strlen(gNbGroupName)) == 0) {
                memcpy(
                    gNbNameBuf[i].bytes,
                    gNbRcvData[i].bytes + strlen(gNbGroupName),
                    NETBIOS_NAME_SIZE
                );
                nb_call(i, gNbNameBuf[i].bytes);
            } else {
                Netbios(&gNbSessNcb[i]);
            }
        } else if (gNbSessNcb[i].returnCode != IDX(NETBIOS_RESULT_CANCELLED)
                   && gNbSessNcb[i].returnCode != IDX(NETBIOS_RESULT_CANCEL_COMPLETED)) {
            Netbios(&gNbSessNcb[i]);
        }
    }
}

VA(0x004a7e4b, 0xcf)
static u16 __fastcall nb_call(i32 session, void* name) {
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_CALL | IDX(NETBIOS_COMMAND_ASYNC);
    gNbSessNcb[session].commandComplete = IDX(NETBIOS_RESULT_PENDING);
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x004a7f1a, 0xcf)
static u16 __fastcall nb_listen(i32 session, void* name) {
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_LISTEN | IDX(NETBIOS_COMMAND_ASYNC);
    gNbSessNcb[session].commandComplete = IDX(NETBIOS_RESULT_PENDING);
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x004a7fe9, 0x126)
static void __stdcall nb_call_done(NetbiosControlBlock* ncb) {
    i32 i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (&gNbSessNcb[i] == ncb)
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    switch (gNbSessNcb[i].returnCode) {
        case IDX(NETBIOS_RESULT_SUCCESS):
            gNbSessLsn[i] = gNbSessNcb[i].sessionNumber;
            memcpy(gNbNameBuf[i].bytes, gNbSessNcb[i].callName, NETBIOS_NAME_SIZE);
            gNetStatus[i] |= IDX(NETBIOS_SESSION_ACTIVE) | IDX(NETBIOS_SESSION_CONNECTED);
            nb_arm_recv(i);
            break;
        case IDX(NETBIOS_RESULT_CANCELLED):
        case IDX(NETBIOS_RESULT_CANCEL_COMPLETED):
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
static void __fastcall nb_arm_recv(i32 session) {
    DATA(0x005175c0) static i16 gNbArmReceiveSourceLineBase = 710;
    u8 result;
    for (;;) {
        ProcessAssert(
            gNbSessNcb[session].returnCode != IDX(NETBIOS_RESULT_PENDING),
            RETAIL_FILE,
            710 + (715 - 710)
        );
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command = NETBIOS_COMMAND_RECEIVE | IDX(NETBIOS_COMMAND_ASYNC);
        gNbSessNcb[session].sessionNumber = gNbSessLsn[session];
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].adapterNumber = gNetbiosLana;
        gNbSessNcb[session].event = gNbEvents.handles[session + NETBIOS_RECEIVE_EVENT_FIRST];
        result = Netbios(&gNbSessNcb[session]);
        switch (result) {
            case IDX(NETBIOS_RESULT_SUCCESS):
            case IDX(NETBIOS_RESULT_SESSION_OUT_OF_RANGE):
            case IDX(NETBIOS_RESULT_SESSION_CLOSED):
            case IDX(NETBIOS_RESULT_SESSION_ENDED):
            case IDX(NETBIOS_RESULT_PENDING):
                return;
            default:
                Sleep(NETBIOS_RECEIVE_RETRY_DELAY);
                continue;
        }
    }
}

VA(0x004a8268, 0xc2)
static void __fastcall nb_close_session(i32 session) {
    NetbiosControlBlock controlBlock;
    if (gNbSessNcb[session].commandComplete == IDX(NETBIOS_RESULT_PENDING)) {
        memset(&controlBlock, 0, sizeof(controlBlock));
        controlBlock.command = IDX(NETBIOS_COMMAND_CANCEL);
        controlBlock.adapterNumber = gNetbiosLana;
        controlBlock.buffer = &gNbSessNcb[session];
        Netbios(&controlBlock);
    }
    if (gNbSessLsn[session] != NETBIOS_INVALID_ID) {
        memset(&controlBlock, 0, sizeof(controlBlock));
        controlBlock.sessionNumber = gNbSessLsn[session];
        controlBlock.command = IDX(NETBIOS_COMMAND_HANGUP);
        controlBlock.adapterNumber = gNetbiosLana;
        Netbios(&controlBlock);
        gNetStatus[session] &= ~IDX(NETBIOS_SESSION_ACTIVE);
    }
}

// @early-stop
// The complete 0x179-byte instruction stream, 0x10 frame (node -0x04,
// session -0x08, outer/inner switch values -0x0c/-0x10), CFG, 0x14-byte
// case-pointer table, and adjacent 0x19-byte selector table agree.  The 24/24
// resolved targets match.  Retail uses interior aliases for
// gNbSessNcb.returnCode/length and fixed IAT operands for Enter/LeaveCriticalSection;
// base retains the owner-relative field addends and two corresponding COFF relocs.
VA(0x004a832a, 0x179)
static void __fastcall nb_recv_complete(i32 session) {
    DATA(0x005175f0) static i16 gNbReceiveCompleteSourceLineBase = 780;
    tag_Node* node;
    switch (gNbSessNcb[session].command & ~IDX(NETBIOS_COMMAND_ASYNC)) {
        case IDX(NETBIOS_COMMAND_RECEIVE):
            switch (gNbSessNcb[session].returnCode) {
                case IDX(NETBIOS_RESULT_SUCCESS):
                    node = static_cast<tag_Node*>(
                        H2_ALLOC(gNbSessNcb[session].length + NETBIOS_PACKET_HEADER_SIZE, 796)
                    );
                    if (node != 0) {
                        node->len = gNbSessNcb[session].length;
                        node->sessionIndex = static_cast<u8>(session);
                        memcpy(node->data, gNbRcvData[session].bytes, node->len);
                        EnterCriticalSection(&gNbRcvLock);
                        add_node(&gNbRcvQueue, node);
                        LeaveCriticalSection(&gNbRcvLock);
                    }
                    nb_arm_recv(session);
                    break;
                case IDX(NETBIOS_RESULT_SESSION_OUT_OF_RANGE):
                case IDX(NETBIOS_RESULT_SESSION_CLOSED):
                case IDX(NETBIOS_RESULT_SESSION_ENDED):
                    gNetStatus[session] &= ~IDX(NETBIOS_SESSION_ACTIVE);
                    break;
                default:
                    nb_arm_recv(session);
                    break;
            }
    }
}

VA(0x004a84a3, 0x87)
static void __fastcall nb_format_name(char* src, u8* dst) {
    u32 i;
    memset(dst, 0, NETBIOS_NAME_SIZE);
    for (i = 0; i < NETBIOS_NAME_SIZE - 1 && *src != '\0'; i++, src++)
        dst[i] = *src;
    for (; i < NETBIOS_NAME_SIZE - 1; i++)
        dst[i] = ' ';
}

#undef RETAIL_FILE
