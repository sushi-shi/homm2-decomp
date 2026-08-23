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

#define RETAIL_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Game\\NETWIN.CPP"

H2_ENUM_BEGIN(NetbiosResetConstant)
    RESET_SESSION_LIMIT_INDEX = 0,
    RESET_NAME_LIMIT_INDEX    = 2,
    RESET_SESSION_LIMIT       = 20,
    RESET_NAME_LIMIT          = 10
H2_ENUM_END(NetbiosResetConstant)


DATA(0x00528650) static tag_Anchor gNbFreeQueueRuntime = {0};
DATA(0x00528658) static NetbiosThreadEvents gNbThreadEventsContext = {0};
DATA(0x00528680) static tag_Anchor gNbReceiveQueueEntry = {0};
DATA(0x00528688) static u8 gNbSessionNumbersEntry[NETBIOS_SESSION_COUNT] = {0};
DATA(0x00528690) static tag_Anchor gNbSendQueueHead = {0};
DATA(0x00528698) static NetbiosPayload gNbReceiveDataLocal[NETBIOS_SESSION_COUNT] = {0};
DATA(0x0052f698) static CRITICAL_SECTION gNbReceiveLockCriticalSection = {0};
DATA(0x0052f6b0) static NetbiosSessionBuffer gNbSessionBufferContext = {0};
DATA(0x005306b0) static NetbiosControlBlock gNbSessionControlBlocksArena[NETBIOS_SESSION_COUNT] = {0};
DATA(0x00530870) static NetbiosControlBlock gNbControlBlockArena = {0};
DATA(0x005308b0) static NetbiosName gNbNameBufferBacking[NETBIOS_SESSION_COUNT] = {0};
DATA(0x00530920) static CRITICAL_SECTION gNbSendLockBacking = {0};
DATA(0x00530938) static u8 gNbCallRetries = 0;
DATA(0x00530939) static u8 gNetbiosAvail = 0;
DATA(0x0053093a) static u8 gNetbiosLana = 0;
DATA(0x0053093b) static u8 gNbShutdown = 0;
DATA(0x0053093c) static u8 gNbLocalNum = 0;
DATA(0x00530940) static H2_ENUM_STORAGE(NetbiosSessionStatus, u8) gNetStatus[NETBIOS_STATUS_COUNT] = {0};

DATA(0x00515c44) static u8 gNbMaxSess = NETBIOS_INVALID_ID;
DATA(0x00515c48) static char* gNbGroupName =
    "Empire Too ";
DATA(0x00515c4c) static char* gNbListenName =
    "*";

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

VA(0x00473da0, 0x94)
i32 is_netbios_avail(void) {
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

VA(0x00473e34, 0x218)
extern "C" u16 __cdecl nb_init(u16 maxNames, u16 maxSessions) {
    NetbiosControlBlock blk;
    i32 idx;
    u8* statusBuffer;
    i32 result;

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
        for (idx = 0; idx < NETBIOS_SESSION_COUNT; idx++) {
            gNetStatus[idx] = 0;
            gNbSessLsn[idx] = NETBIOS_INVALID_ID;
            memset(&gNbSessNcb[idx], 0, sizeof(gNbSessNcb[idx]));
        }
        memset(gNbNameBuf, 0, sizeof(gNbNameBuf));
        InitializeCriticalSection(&gNbRcvLock);
        InitializeCriticalSection(&gNbSndLock);
        init_anchor(&gNbRcvQueue, 1, 0);
        init_anchor(&gNbSndQueue, 1, 0);
        init_anchor(&gNbFreeQueue, 1, 0);
        for (idx = 0; idx < NETBIOS_THREAD_EVENT_COUNT; idx++)
            gNbEvents.handles[idx] = CreateEventA(NULL, 1, 0, NULL);
        memset(&blk, 0, sizeof(blk));
        statusBuffer = static_cast<u8*>(H2_ALLOC(NETBIOS_ADAPTER_STATUS_SIZE));
        blk.command = NETBIOS_COMMAND_ADAPTER_STATUS;
        blk.length = NETBIOS_ADAPTER_STATUS_SIZE;
        blk.buffer = statusBuffer;
        blk.adapterNumber = gNetbiosLana;
        if (Netbios(&blk) == NETBIOS_RESULT_ENVIRONMENT_UNDEFINED) {
            memset(&blk, 0, sizeof(blk));
            blk.command = NETBIOS_COMMAND_RESET;
            blk.adapterNumber = gNetbiosLana;
            blk.callName[RESET_SESSION_LIMIT_INDEX] = RESET_SESSION_LIMIT;
            blk.callName[RESET_NAME_LIMIT_INDEX] = RESET_NAME_LIMIT;
            Netbios(&blk);
        }
        H2_FREE(statusBuffer);
        gNbShutdown = 0;
        return 0;
    }
    return 1;
}

VA(0x0047404c, 0x1d0)
extern "C" void __fastcall nb_term(void) {
    tag_Node* np;
    NetbiosControlBlock block;
    i32 i;

    for (i = 0; i < NETBIOS_SESSION_COUNT; i++)
        nb_close_session(i);
    if (gNbCtlNcb.commandComplete == NETBIOS_RESULT_PENDING) {
        memset(&block, 0, sizeof(block));
        block.command = NETBIOS_COMMAND_CANCEL;
        block.adapterNumber = gNetbiosLana;
        block.buffer = &gNbCtlNcb;
        Netbios(&block);
    }
    if (HAS(gNetStatus[gNbMaxSess], NETBIOS_SESSION_NAME_REGISTERED)) {
        memset(&block, 0, sizeof(block));
        memcpy(block.name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
        block.command = NETBIOS_COMMAND_DELETE_NAME;
        block.adapterNumber = gNetbiosLana;
        Netbios(&block);
    }
    EnterCriticalSection(&gNbSndLock);
    while ((np = pop_node(&gNbSndQueue)) != NULL)
        H2_FREE(np);
    while ((np = pop_node(&gNbFreeQueue)) != NULL)
        H2_FREE(np);
    LeaveCriticalSection(&gNbSndLock);
    DeleteCriticalSection(&gNbSndLock);
    for (i = 0; i < NETBIOS_THREAD_EVENT_COUNT; i++) {
        CloseHandle(gNbEvents.handles[i]);
        gNbEvents.handles[i] = NULL;
    }
    gNbShutdown |= 1;
    SetEvent(gNbEvents.handles[0]);
    EnterCriticalSection(&gNbRcvLock);
    while ((np = pop_node(&gNbRcvQueue)) != NULL)
        H2_FREE(np);
    LeaveCriticalSection(&gNbRcvLock);
    DeleteCriticalSection(&gNbRcvLock);
}

VA(0x0047421c, 0x96)
extern "C" u16 __cdecl nb_rcv(i16 session, void* buf) {
    tag_Node* node;
    i32 len;

    EnterCriticalSection(&gNbRcvLock);
    node = pop_node(&gNbRcvQueue);
    LeaveCriticalSection(&gNbRcvLock);
    if (node) {
        len = node->len < session ? node->len : static_cast<u16>(session);
        memcpy(buf, node->data, len);
        H2_FREE(node);
        return len;
    }
    return 0;
}

VA(0x004742b2, 0xc0)
extern "C" u16 __cdecl nb_snd(i16 session, i16 len, void* data) {
    tag_Node* node;

    if (session == gNbMaxSess && len == 0) {
        nb_add_name();
        return 0;
    }
    if (!HAS(gNetStatus[session], NETBIOS_SESSION_ACTIVE))
        return IDX(NETBIOS_RESULT_SESSION_OUT_OF_RANGE);
    node = static_cast<tag_Node*>(H2_ALLOC(len + NETBIOS_PACKET_HEADER_SIZE));
    node->len = len;
    node->sessionIndex = static_cast<u8>(session);
    memcpy(node->data, data, len);
    EnterCriticalSection(&gNbSndLock);
    add_node(&gNbSndQueue, node);
    LeaveCriticalSection(&gNbSndLock);
    SetEvent(gNbEvents.handles[0]);
    return 0;
}

VA(0x00474372, 0x4c7)
extern "C" u16 __cdecl
nb_sess(H2_ENUM_PARAM(NetbiosSessionOperation, i16) operation, ...) {
    i32 oldsess;
    i32 destinationSession;
    i32 detach;
    NetbiosControlBlock controlBlock;
    char* peer;
    va_list args;
    H2_ENUM_STORAGE(NetbiosResult, i16) rc;

    va_start(args, operation);
    switch (operation) {
        case NETBIOS_SESSION_REGISTER:
            peer = va_arg(args, char*);
            gNetStatus[gNbMaxSess] &= ~NETBIOS_SESSION_ERROR;
            nb_format_name(peer, gNbNameBuf[gNbMaxSess].bytes);
            memset(&gNbSessNcb[gNbMaxSess], 0, sizeof(NetbiosControlBlock));
            memcpy(gNbSessNcb[gNbMaxSess].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
            gNbSessNcb[gNbMaxSess].command =
                NETBIOS_COMMAND_ADD_NAME | NETBIOS_COMMAND_ASYNC;
            gNbSessNcb[gNbMaxSess].postRoutine = nb_add_name_done;
            gNbSessNcb[gNbMaxSess].commandComplete = NETBIOS_RESULT_PENDING;
            gNbSessNcb[gNbMaxSess].adapterNumber = gNetbiosLana;
            rc = Netbios(&gNbSessNcb[gNbMaxSess]);
            break;

        case NETBIOS_SESSION_RECEIVE_ANY: {
            destinationSession = va_arg(args, i32);
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
            rc = nb_recv_any(destinationSession);
            break;
        }

        case NETBIOS_SESSION_CALL:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            nb_format_name(peer, gNbNameBuf[destinationSession].bytes);
            rc = nb_call(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case NETBIOS_SESSION_LISTEN_ANY:
            destinationSession = va_arg(args, i32);
            nb_snd(gNbMaxSess, 0, NULL);
            rc = nb_listen(destinationSession, gNbListenName);
            break;

        case NETBIOS_SESSION_LISTEN:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            nb_format_name(peer, gNbNameBuf[destinationSession].bytes);
            rc = nb_listen(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case NETBIOS_SESSION_MOVE:
            oldsess = va_arg(args, i32);
            destinationSession = va_arg(args, i32);
            detach = va_arg(args, i32);
            if (oldsess == gNbMaxSess)
                gNbMaxSess = static_cast<u8>(destinationSession);
            if (gNbSessLsn[oldsess] == NETBIOS_INVALID_ID)
                return 0;
            gNbSessLsn[destinationSession] = gNbSessLsn[oldsess];
            gNetStatus[destinationSession] = gNetStatus[oldsess];
            memcpy(
                gNbNameBuf[destinationSession].bytes,
                gNbNameBuf[oldsess].bytes,
                NETBIOS_NAME_SIZE
            );
            nb_arm_recv(destinationSession);
            if (detach != 0) {
                gNbSessLsn[oldsess] = NETBIOS_INVALID_ID;
                gNetStatus[oldsess] = 0;
                memset(gNbNameBuf[oldsess].bytes, 0, NETBIOS_NAME_SIZE);
            }
            rc = 0;
            break;

        case NETBIOS_SESSION_CLOSE:
            destinationSession = va_arg(args, i32);
            if (gNbSessNcb[destinationSession].commandComplete == NETBIOS_RESULT_PENDING) {
                memset(&controlBlock, 0, sizeof(controlBlock));
                controlBlock.command = NETBIOS_COMMAND_CANCEL;
                controlBlock.adapterNumber = gNetbiosLana;
                controlBlock.buffer = &gNbSessNcb[destinationSession];
                Netbios(&controlBlock);
            }
            nb_close_session(destinationSession);
            rc = 0;
            break;

        case NETBIOS_SESSION_CLEAR_CONNECTED:
            destinationSession = va_arg(args, i32);
            gNetStatus[destinationSession] &= ~NETBIOS_SESSION_CONNECTED;
            rc = 0;
            break;

        case NETBIOS_SESSION_GET_NAME:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            memcpy(peer, gNbNameBuf[destinationSession].bytes, NETBIOS_NAME_SIZE);
            rc = 0;
            break;

        default:
            return 1;
    }
    if (rc == NETBIOS_RESULT_PENDING)
        rc = 0;
    return IDX(rc);
}

VA(0x00474839, 0xf)
extern "C" char __cdecl nb_stat(i16 session) {
    return static_cast<char>(gNetStatus[session]);
}

VA(0x00474848, 0x215)
void nb_thr_ctl(void) {
    i32 keepRunning;
    i32 idx;
    tag_Node* entry;
    NetbiosControlBlock block;
    H2_ENUM_STORAGE(NetbiosResult, u8) result;
    i32 sendComplete;

    keepRunning = 1;
    if (WaitForMultipleObjects(NETBIOS_THREAD_EVENT_COUNT, gNbEvents.handles, 0, 0) == WAIT_TIMEOUT)
        return;
    {
        if (WaitForSingleObject(gNbEvents.handles[0], 0) == WAIT_OBJECT_0)
            ResetEvent(gNbEvents.handles[0]);
        for (idx = 0; idx < NETBIOS_RECEIVE_EVENT_COUNT; idx++) {
            if (WaitForSingleObject(gNbEvents.handles[idx + NETBIOS_RECEIVE_EVENT_FIRST], 0)
                == WAIT_OBJECT_0) {
                ResetEvent(gNbEvents.handles[idx + NETBIOS_RECEIVE_EVENT_FIRST]);
                nb_recv_complete(idx);
            }
        }
        while (keepRunning) {
            EnterCriticalSection(&gNbSndLock);
            entry = pop_node(&gNbFreeQueue);
            if (entry == NULL)
                entry = pop_node(&gNbSndQueue);
            LeaveCriticalSection(&gNbSndLock);
            if (entry == NULL) {
                keepRunning = 0;
            } else {
                memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
                gNbCtlNcb.sessionNumber = gNbSessLsn[entry->sessionIndex];
                if (gNbCtlNcb.sessionNumber != NETBIOS_INVALID_ID) {
                    memcpy(gNbSessBuf.bytes, entry->data, entry->len);
                    gNbCtlNcb.buffer = gNbSessBuf.bytes;
                    gNbCtlNcb.length = entry->len;
                    gNbCtlNcb.command = NETBIOS_COMMAND_SEND;
                    gNbCtlNcb.adapterNumber = gNetbiosLana;
                    sendComplete = 0;
                    while (!sendComplete) {
                        result = Netbios(&gNbCtlNcb);
                        switch (result) {
                            case NETBIOS_RESULT_SUCCESS:
                                sendComplete = 1;
                                break;
                            case NETBIOS_RESULT_PENDING:
                                ProcessAssert(
                                    0,
                                    RETAIL_FILE,
                                    495
                                );
                                break;
                            case NETBIOS_RESULT_SESSION_OUT_OF_RANGE:
                            case NETBIOS_RESULT_SESSION_CLOSED:
                            case NETBIOS_RESULT_SESSION_ENDED:
                                gNetStatus[entry->sessionIndex] &= ~NETBIOS_SESSION_ACTIVE;
                                break;
                            default:
                                break;
                        }
                    }
                }
                H2_FREE(entry);
            }
        }
    }
}


VA(0x00474a5d, 0xb5)
static void nb_add_name(void) {
    if (gNbCtlNcb.commandComplete != NETBIOS_RESULT_PENDING) {
        strcpy(gNbSessBuf.bytes, gNbGroupName);
        memcpy(
            gNbSessBuf.bytes + strlen(gNbGroupName),
            gNbNameBuf[gNbMaxSess].bytes,
            NETBIOS_NAME_SIZE
        );
        memset(&gNbCtlNcb, 0, sizeof(gNbCtlNcb));
        gNbCtlNcb.command = NETBIOS_COMMAND_RECEIVE_DATAGRAM | NETBIOS_COMMAND_ASYNC;
        gNbCtlNcb.nameNumber = gNbLocalNum;
        gNbCtlNcb.length = strlen(gNbGroupName) + NETBIOS_NAME_SIZE;
        gNbCtlNcb.buffer = gNbSessBuf.bytes;
        gNbCtlNcb.adapterNumber = gNetbiosLana;
        Netbios(&gNbCtlNcb);
    }
}

VA(0x00474b12, 0x193)
static void __stdcall nb_add_name_done(NetbiosControlBlock* ncb) {
    i32 j;
    ProcessAssert(
        ncb == &gNbSessNcb[gNbMaxSess],
        RETAIL_FILE,
        537
    );
    switch (ncb->returnCode) {
        case NETBIOS_RESULT_SUCCESS:
        case NETBIOS_RESULT_CANCEL_COMPLETED:
            gNbLocalNum = ncb->nameNumber;
            memcpy(gNbNameBuf[gNbMaxSess].bytes, ncb->name, NETBIOS_NAME_SIZE);
            gNetStatus[gNbMaxSess] |= NETBIOS_SESSION_NAME_REGISTERED;
            break;
        case NETBIOS_RESULT_DUPLICATE_NAME:
        case NETBIOS_RESULT_NAME_IN_USE:
        case NETBIOS_RESULT_NAME_CONFLICT:
        case NETBIOS_RESULT_DUPLICATE_ENVIRONMENT:
            for (j = NETBIOS_NAME_SIZE - 1; j >= 0; j--) {
                ncb->name[j]++;
                if (ncb->name[j] != gNbNameBuf[gNbMaxSess].bytes[j])
                    break;
            }
            Netbios(ncb);
            break;
        case NETBIOS_RESULT_CANCELLED:
            break;
        default:
            sprintf(
                gText,
                "Add Name Error %02x\n",
                IDX(ncb->returnCode)
            );
            ShutDown(gText);
            gNetStatus[gNbMaxSess] |= NETBIOS_SESSION_ERROR;
            break;
    }
}

VA(0x00474ca5, 0xb4)
static H2_ENUM_PARAM(NetbiosResult, u16) __fastcall nb_recv_any(i32 session) {
    if (gNbSessNcb[session].commandComplete != NETBIOS_RESULT_PENDING) {
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command =
            NETBIOS_COMMAND_RECEIVE_BROADCAST | NETBIOS_COMMAND_ASYNC;
        gNbSessNcb[session].nameNumber = gNbLocalNum;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].postRoutine = nb_recv_any_done;
        Netbios(&gNbSessNcb[session]);
    }
    return gNbSessNcb[session].commandComplete;
}

VA(0x00474d59, 0x11d)
static void __stdcall nb_recv_any_done(NetbiosControlBlock* ncb) {
    i32 i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (ncb == &gNbSessNcb[i])
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    {
        if (gNbSessNcb[i].returnCode == NETBIOS_RESULT_SUCCESS) {
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
        } else if (gNbSessNcb[i].returnCode != NETBIOS_RESULT_CANCELLED
                   && gNbSessNcb[i].returnCode != NETBIOS_RESULT_CANCEL_COMPLETED) {
            Netbios(&gNbSessNcb[i]);
        }
    }
}

VA(0x00474e76, 0xc2)
static H2_ENUM_PARAM(NetbiosResult, u16) __fastcall nb_call(i32 session, void* name) {
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_CALL | NETBIOS_COMMAND_ASYNC;
    gNbSessNcb[session].commandComplete = NETBIOS_RESULT_PENDING;
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x00474f38, 0xc2)
static H2_ENUM_PARAM(NetbiosResult, u16) __fastcall nb_listen(i32 session, void* name) {
    memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
    memcpy(gNbSessNcb[session].callName, name, NETBIOS_NAME_SIZE);
    memcpy(gNbSessNcb[session].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
    gNbSessNcb[session].command = NETBIOS_COMMAND_LISTEN | NETBIOS_COMMAND_ASYNC;
    gNbSessNcb[session].commandComplete = NETBIOS_RESULT_PENDING;
    gNbSessNcb[session].postRoutine = nb_call_done;
    gNbSessNcb[session].adapterNumber = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

VA(0x00474ffa, 0xf4)
static void __stdcall nb_call_done(NetbiosControlBlock* ncb) {
    i32 i;
    for (i = 0; i < NETBIOS_SESSION_COUNT; i++) {
        if (ncb == &gNbSessNcb[i])
            break;
    }
    if (i >= NETBIOS_SESSION_COUNT)
        return;
    switch (gNbSessNcb[i].returnCode) {
        case NETBIOS_RESULT_SUCCESS:
            gNbSessLsn[i] = gNbSessNcb[i].sessionNumber;
            memcpy(gNbNameBuf[i].bytes, gNbSessNcb[i].callName, NETBIOS_NAME_SIZE);
            gNetStatus[i] |= NETBIOS_SESSION_ACTIVE | NETBIOS_SESSION_CONNECTED;
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

VA(0x004750ee, 0x124)
static void __fastcall nb_arm_recv(i32 session) {
    H2_ENUM_STORAGE(NetbiosResult, u8) result;
    while (1) {
        ProcessAssert(
            gNbSessNcb[session].returnCode != NETBIOS_RESULT_PENDING,
            RETAIL_FILE,
            715
        );
        memset(&gNbSessNcb[session], 0, sizeof(NetbiosControlBlock));
        gNbSessNcb[session].command = NETBIOS_COMMAND_RECEIVE | NETBIOS_COMMAND_ASYNC;
        gNbSessNcb[session].sessionNumber = gNbSessLsn[session];
        gNbSessNcb[session].buffer = gNbRcvData[session].bytes;
        gNbSessNcb[session].length = NETBIOS_PAYLOAD_SIZE;
        gNbSessNcb[session].adapterNumber = gNetbiosLana;
        gNbSessNcb[session].event = gNbEvents.handles[session + NETBIOS_RECEIVE_EVENT_FIRST];
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

VA(0x00475212, 0xb1)
static void __fastcall nb_close_session(i32 session) {
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

VA(0x004752c3, 0x137)
static void __fastcall nb_recv_complete(i32 session) {
    tag_Node* node;
    switch (gNbSessNcb[session].command & ~NETBIOS_COMMAND_ASYNC) {
        case NETBIOS_COMMAND_RECEIVE:
            switch (gNbSessNcb[session].returnCode) {
                case NETBIOS_RESULT_SUCCESS:
                    node = static_cast<tag_Node*>(
                        H2_ALLOC(gNbSessNcb[session].length + NETBIOS_PACKET_HEADER_SIZE)
                    );
                    if (node != NULL) {
                        node->len = gNbSessNcb[session].length;
                        node->sessionIndex = static_cast<u8>(session);
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

VA(0x004753fa, 0x76)
static void __fastcall nb_format_name(char* src, u8* dst) {
    u32 i;
    memset(dst, 0, NETBIOS_NAME_SIZE);
    for (i = 0; i < NETBIOS_NAME_SIZE - 1 && *src != '\0'; i++, src++)
        dst[i] = *src;
    for (; i < NETBIOS_NAME_SIZE - 1; i++)
        dst[i] = ' ';
}
