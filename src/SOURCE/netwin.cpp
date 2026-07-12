// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\netwin.obj   from: (directly linked into exe)
// functions: 8   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <windows.h>
#include <nb30.h>
#include <string.h>
#include <SOURCE/comwin.h>
#include <SOURCE/netwin.h>
#include <SOURCE/KB.h>
#include <BASE/Misc.h>
#include <stdio.h>
#include <_globals_model.h>



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x0051739c) static int           gNbCallRetries;      // FUN_004a7fe9 retry counter (cap 0x14)
DATA(0x005173a0) static unsigned char gNetbiosAvail;
DATA(0x005173a4) static unsigned char gNetbiosLana;
DATA(0x005173a8) static unsigned char gNbShutdown;         // shutdown flag, cleared by nb_init
DATA(0x005173ac) static unsigned char gNbMaxSess;          // max session count (nb_init param2)
DATA(0x005173b0) static unsigned char gNbLocalNum;         // local netbios name number
DATA(0x005173c0) static char         *gNbGroupName;        // netbios group name ("Empire Too")
DATA(0x005173c4) static char         *gNbListenName;       // netbios listen/any name (nb_sess case 3)
DATA(0x0052ae68) static tag_Anchor    gNbFreeQueue;        // queue anchor (init_anchor)
DATA(0x0052ae70) static unsigned char gNbSessLsn[7];       // per-session LSN, 0xff = free slot
DATA(0x0052ae78) static unsigned char gNbRcvData[0x7000];  // receive data pool, init 0xff
DATA(0x00531e78) static unsigned char gNbNameBuf[0x70];    // name scratch, init 0
DATA(0x00531ee8) static unsigned char gNbSessBuf[0x1000];  // session scratch, init 0xff
DATA(0x00532ee8) static NCB           gNbSessNcb[7];        // per-session NCB
DATA(0x005330a8) static NCB           gNbCtlNcb;            // control NCB
DATA(0x005330e8) static tag_Anchor    gNbRcvQueue;         // receive queue (nb_rcv pops it)
DATA(0x005330f0) static tag_Anchor    gNbSndQueue;         // send queue
DATA(0x005330f8) static CRITICAL_SECTION gNbRcvLock;       // guards gNbRcvQueue
DATA(0x00533110) static HANDLE        gNbEvents[9];         // manual-reset events
DATA(0x00533138) static CRITICAL_SECTION gNbSndLock;       // guards gNbSndQueue

VA(0x004a6be0, 0xa8)
int is_netbios_avail(void)
{
    NCB ncb;
    memset(&ncb, 0, sizeof(NCB));
    for (gNetbiosLana = 0; gNetbiosLana < 0xfe; gNetbiosLana++) {
        memset(&ncb, 0, sizeof(NCB));
        ncb.ncb_command = 0x7f;
        ncb.ncb_lana_num = gNetbiosLana;
        if (Netbios(&ncb) == 3)
            break;
    }
    if (gNetbiosLana < 0xfe) {
        gNetbiosAvail = 1;
        return 1;
    }
    return 0;
}

VA(0x004a6c88, 0x244)
extern "C" unsigned short __fastcall nb_init(unsigned short param1, unsigned short param2)
{
    NCB ncb;
    int i;
    unsigned char *buf;

    memset(gNbSessLsn, 0, 7);
    memset(&gNbCtlNcb, 0, 0x40);
    memset(gNbSessNcb, 0, 0x1c0);
    memset(gNbSessBuf, -1, 0x1000);
    memset(gNbRcvData, -1, 0x7000);
    memset(gNbEvents, 0, 0x24);
    if (is_netbios_avail() == 0)
        return 1;
    if (gNetbiosAvail == 0)
        return 1;
    gNbMaxSess = param2;
    for (i = 0; i < 7; i++) {
        gNetStatus[i] = 0;
        gNbSessLsn[i] = 0xff;
        memset(&gNbSessNcb[i], 0, 0x40);
    }
    memset(gNbNameBuf, 0, 0x70);
    InitializeCriticalSection(&gNbRcvLock);
    InitializeCriticalSection(&gNbSndLock);
    init_anchor(&gNbRcvQueue, 1, 0);
    init_anchor(&gNbSndQueue, 1, 0);
    init_anchor(&gNbFreeQueue, 1, 0);
    for (i = 0; i < 9; i++)
        gNbEvents[i] = CreateEventA(0, 1, 0, 0);
    memset(&ncb, 0, 0x40);
    buf = (unsigned char *)BaseAlloc(0x400, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    ncb.ncb_command = 0x33;
    ncb.ncb_length = 0x400;
    ncb.ncb_lana_num = gNetbiosLana;
    ncb.ncb_buffer = buf;
    if (Netbios(&ncb) == 0x34) {
        memset(&ncb, 0, 0x40);
        ncb.ncb_command = 0x32;
        ncb.ncb_lana_num = gNetbiosLana;
        ncb.ncb_callname[0] = 0x14;
        ncb.ncb_callname[2] = 0x0a;
        Netbios(&ncb);
    }
    BaseFree(buf, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    gNbShutdown = 0;
    return 0;
}

// Static helper (retail RVA 0x004a8268, no CodeView symbol) — tears down one session: cancel any
// pending receive NCB, then hang up the session and clear its active bit. Shared by nb_term/nb_sess.
VA(0x004a8268, 0xc2)
static void __fastcall nb_close_session(int session)
{
    NCB ncb;
    if (gNbSessNcb[session].ncb_cmd_cplt == 0xff) {
        memset(&ncb, 0, 0x40);
        ncb.ncb_command = 0x35;
        ncb.ncb_lana_num = gNetbiosLana;
        ncb.ncb_buffer = (unsigned char *)&gNbSessNcb[session];
        Netbios(&ncb);
    }
    if (gNbSessLsn[session] != 0xff) {
        memset(&ncb, 0, 0x40);
        ncb.ncb_lsn = gNbSessLsn[session];
        ncb.ncb_command = 0x12;
        ncb.ncb_lana_num = gNetbiosLana;
        Netbios(&ncb);
        gNetStatus[session] &= 0xfe;
    }
}

VA(0x004a6ecc, 0x207)
extern "C" void __fastcall nb_term(void)
{
    NCB ncb;
    int i;
    tag_Node *node;

    for (i = 0; i < 7; i++)
        nb_close_session(i);
    if (gNbCtlNcb.ncb_cmd_cplt == 0xff) {
        memset(&ncb, 0, 0x40);
        ncb.ncb_command = 0x35;
        ncb.ncb_lana_num = gNetbiosLana;
        ncb.ncb_buffer = (unsigned char *)&gNbCtlNcb;
        Netbios(&ncb);
    }
    if ((gNetStatus[gNbMaxSess] & 2) != 0) {
        memset(&ncb, 0, 0x40);
        memcpy(ncb.ncb_name, &gNbNameBuf[gNbMaxSess * 0x10], 0x10);
        ncb.ncb_command = 0x31;
        ncb.ncb_lana_num = gNetbiosLana;
        Netbios(&ncb);
    }
    EnterCriticalSection(&gNbSndLock);
    while ((node = pop_node(&gNbSndQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    while ((node = pop_node(&gNbFreeQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    LeaveCriticalSection(&gNbSndLock);
    DeleteCriticalSection(&gNbSndLock);
    for (i = 0; i < 9; i++) {
        CloseHandle(gNbEvents[i]);
        gNbEvents[i] = 0;
    }
    gNbShutdown |= 1;
    SetEvent(gNbEvents[0]);
    EnterCriticalSection(&gNbRcvLock);
    while ((node = pop_node(&gNbRcvQueue)) != 0)
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
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
            len = (unsigned short)session;
        else
            len = node->len;
        memcpy(buf, node->data, len);
        BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
        return len;
    }
    return 0;
}

// Static helper (retail RVA 0x004a79c6, no CodeView symbol) — re-registers the netbios group name
// for the current session. Defined here (ahead of its retail position) so nb_snd can call it
// without a forward declaration. Only issued when the control NCB's last command completed.
VA(0x004a79c6, 0xbb)
static void nb_add_name(void)
{
    if (gNbCtlNcb.ncb_cmd_cplt != 0xff) {
        strcpy((char *)gNbSessBuf, gNbGroupName);
        memcpy(gNbSessBuf + strlen(gNbGroupName), &gNbNameBuf[gNbMaxSess * 0x10], 0x10);
        memset(&gNbCtlNcb, 0, 0x40);
        gNbCtlNcb.ncb_command = 0xa2;
        gNbCtlNcb.ncb_num = gNbLocalNum;
        gNbCtlNcb.ncb_length = strlen(gNbGroupName) + 0x10;
        gNbCtlNcb.ncb_buffer = gNbSessBuf;
        gNbCtlNcb.ncb_lana_num = gNetbiosLana;
        Netbios(&gNbCtlNcb);
    }
}

VA(0x004a7186, 0xe4)
extern "C" unsigned short __fastcall nb_snd(short session, short len, void *data)
{
    tag_Node *node;

    if (gNbMaxSess == session && len == 0) {
        nb_add_name();
        return 0;
    }
    if ((gNetStatus[session] & 1) == 0)
        return 8;
    node = (tag_Node *)BaseAlloc(len + 0xb, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    node->len = len;
    node->field_0xa = (unsigned char)session;
    memcpy(node->data, data, len);
    EnterCriticalSection(&gNbSndLock);
    add_node(&gNbSndQueue, node);
    LeaveCriticalSection(&gNbSndLock);
    SetEvent(gNbEvents[0]);
    return 0;
}

// ===== netbios async session state machine (retail RVAs in the 0x4a79c6-0x4a852a static block, no
// CodeView symbols -> file-statics). nb_sess posts async NCBs whose ncb_post callback (also a static
// here) runs on the receiver thread. Defined callback-before-referencer so no forward decls needed. =

// FUN_004a8119 — (re)arm an async receive on a session, retrying transient netbios errors.
VA(0x004a8119, 0x13b)
static void __fastcall nb_arm_recv(int session)
{
    unsigned char rc;
    for (;;) {
        ProcessAssert(gNbSessNcb[session].ncb_retcode != 0xff,
                      "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
        memset(&gNbSessNcb[session], 0, 0x40);
        gNbSessNcb[session].ncb_command = 0x95;
        gNbSessNcb[session].ncb_lsn = gNbSessLsn[session];
        gNbSessNcb[session].ncb_buffer = &gNbRcvData[session * 0x1000];
        gNbSessNcb[session].ncb_length = 0x1000;
        gNbSessNcb[session].ncb_lana_num = gNetbiosLana;
        gNbSessNcb[session].ncb_event = gNbEvents[session + 2];
        rc = Netbios(&gNbSessNcb[session]);
        if (rc < 9) {
            if (rc == 8 || rc == 0)
                return;
        } else {
            if (rc == 10 || rc == 0x18 || rc == 0xff)
                return;
        }
        Sleep(0x32);
    }
}

// FUN_004a7fe9 — ncb_post callback for a completed CALL/LISTEN: on success record the LSN + peer name
// and arm the receive; else retry a bounded number of times.
VA(0x004a7fe9, 0x126)
static void __stdcall nb_call_done(PNCB ncb)
{
    int i;
    char rc;
    for (i = 0; i < 7 && &gNbSessNcb[i] != ncb; i++)
        ;
    if (i < 7) {
        rc = gNbSessNcb[i].ncb_retcode;
        if (rc == 0) {
            gNbSessLsn[i] = gNbSessNcb[i].ncb_lsn;
            memcpy(&gNbNameBuf[i * 0x10], gNbSessNcb[i].ncb_callname, 0x10);
            gNetStatus[i] |= 9;
            nb_arm_recv(i);
        } else if (rc != 0xb && rc != 0x24 && (gNbCallRetries++, gNbCallRetries < 0x14)) {
            Sleep(100);
            Netbios(&gNbSessNcb[i]);
        }
    }
}

// FUN_004a7e4b — issue an async netbios CALL to a named peer on a session.
VA(0x004a7e4b, 0xcf)
static unsigned char __fastcall nb_call(int session, void *name)
{
    memset(&gNbSessNcb[session], 0, 0x40);
    memcpy(gNbSessNcb[session].ncb_callname, name, 0x10);
    memcpy(gNbSessNcb[session].ncb_name, &gNbNameBuf[gNbMaxSess * 0x10], 0x10);
    gNbSessNcb[session].ncb_command = 0x90;
    gNbSessNcb[session].ncb_cmd_cplt = 0xff;
    gNbSessNcb[session].ncb_post = nb_call_done;
    gNbSessNcb[session].ncb_lana_num = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

// FUN_004a7f1a — issue an async netbios LISTEN for an incoming call on a session.
VA(0x004a7f1a, 0xcf)
static unsigned char __fastcall nb_listen(int session, void *name)
{
    memset(&gNbSessNcb[session], 0, 0x40);
    memcpy(gNbSessNcb[session].ncb_callname, name, 0x10);
    memcpy(gNbSessNcb[session].ncb_name, &gNbNameBuf[gNbMaxSess * 0x10], 0x10);
    gNbSessNcb[session].ncb_command = 0x91;
    gNbSessNcb[session].ncb_cmd_cplt = 0xff;
    gNbSessNcb[session].ncb_post = nb_call_done;
    gNbSessNcb[session].ncb_lana_num = gNetbiosLana;
    gNbCallRetries = 0;
    return Netbios(&gNbSessNcb[session]);
}

// FUN_004a7d09 — ncb_post callback for a completed RECEIVE-ANY: if the datagram carries our group
// name, promote it to a real CALL; otherwise re-post the receive.
VA(0x004a7d09, 0x142)
static void __stdcall nb_recv_any_done(PNCB ncb)
{
    int i;
    for (i = 0; i < 7 && &gNbSessNcb[i] != ncb; i++)
        ;
    if (i < 7) {
        if (gNbSessNcb[i].ncb_retcode == 0) {
            if (memcmp(&gNbRcvData[i * 0x1000], gNbGroupName, strlen(gNbGroupName)) == 0) {
                memcpy(&gNbNameBuf[i * 0x10], &gNbRcvData[i * 0x1000] + strlen(gNbGroupName), 0x10);
                nb_call(i, &gNbNameBuf[i * 0x10]);
            } else {
                Netbios(&gNbSessNcb[i]);
            }
        } else if (gNbSessNcb[i].ncb_retcode != 0xb && gNbSessNcb[i].ncb_retcode != 0x24) {
            Netbios(&gNbSessNcb[i]);
        }
    }
}

// FUN_004a7c4b — arm an async RECEIVE-DATAGRAM (any source) on a session.
VA(0x004a7c4b, 0xbe)
static unsigned char __fastcall nb_recv_any(int session)
{
    if (gNbSessNcb[session].ncb_cmd_cplt != 0xff) {
        memset(&gNbSessNcb[session], 0, 0x40);
        gNbSessNcb[session].ncb_command = 0xa3;
        gNbSessNcb[session].ncb_num = gNbLocalNum;
        gNbSessNcb[session].ncb_length = 0x1000;
        gNbSessNcb[session].ncb_buffer = &gNbRcvData[session * 0x1000];
        gNbSessNcb[session].ncb_post = nb_recv_any_done;
        Netbios(&gNbSessNcb[session]);
    }
    return gNbSessNcb[session].ncb_cmd_cplt;
}

// FUN_004a7a81 — ncb_post callback for a completed ADD-NAME.
VA(0x004a7a81, 0x174)
static void __stdcall nb_add_name_done(PNCB ncb)
{
    int j;
    ProcessAssert(&gNbCtlNcb == ncb, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
    switch (ncb->ncb_retcode) {
    case 0x00:
    case 0x24:
        gNbLocalNum = ncb->ncb_num;
        memcpy(&gNbNameBuf[gNbMaxSess * 0x10], ncb->ncb_name, 0x10);
        gNetStatus[gNbMaxSess] |= 2;
        break;
    case 0x0b:
        break;
    case 0x0d:
    case 0x16:
    case 0x19:
    case 0x30:
        for (j = 0xf; j >= 0 && (ncb->ncb_name[j] = ncb->ncb_name[j] + 1,
                                 gNbNameBuf[j + gNbMaxSess * 0x10] == ncb->ncb_name[j]); j--)
            ;
        Netbios(ncb);
        break;
    default:
        sprintf(gText, "Add Name Error: %02x", ncb->ncb_retcode);
        ShutDown(gText);
        gNetStatus[gNbMaxSess] |= 0x80;
        break;
    }
}

// FUN_004a84a3 — format a 16-byte netbios name field: copy up to 15 chars, space-pad the rest.
VA(0x004a84a3, 0x87)
static void __fastcall nb_format_name(char *src, unsigned char *dst)
{
    char *p;
    unsigned int i;
    memset(dst, 0, 0x10);
    i = 0;
    for (p = src; i < 0xf && *p != '\0'; p++) {
        dst[i] = *p;
        i++;
    }
    for (; i < 0xf; i++)
        dst[i] = 0x20;
}

VA(0x004a726a, 0x4cd)
extern "C" int __cdecl nb_sess(short cmd, int session, char *name, int detach)
{
    unsigned short result;
    NCB ncb;

    switch (cmd) {
    case 0:
        gNetStatus[gNbMaxSess] &= 0x7f;
        nb_format_name(name, &gNbNameBuf[gNbMaxSess * 0x10]);
        memset(&gNbSessNcb[gNbMaxSess], 0, 0x40);
        memcpy(gNbSessNcb[gNbMaxSess].ncb_name, &gNbNameBuf[gNbMaxSess * 0x10], 0x10);
        gNbSessNcb[gNbMaxSess].ncb_command = 0xb0;
        gNbSessNcb[gNbMaxSess].ncb_post = nb_add_name_done;
        gNbSessNcb[gNbMaxSess].ncb_cmd_cplt = 0xff;
        gNbSessNcb[gNbMaxSess].ncb_lana_num = gNetbiosLana;
        result = Netbios(&gNbSessNcb[gNbMaxSess]);
        break;
    case 1:
        if (gNbSessNcb[session].ncb_cmd_cplt == 0xff) {
            if ((gNbSessNcb[session].ncb_command & 0x7f) == 0x10 ||
                (gNbSessNcb[session].ncb_command & 0x7f) == 0x23)
                return 0;
            memset(&ncb, 0, 0x40);
            ncb.ncb_command = 0x35;
            ncb.ncb_lana_num = gNetbiosLana;
            ncb.ncb_buffer = (unsigned char *)&gNbSessNcb[session];
            Netbios(&ncb);
        }
        result = nb_recv_any(session);
        break;
    case 2:
        nb_format_name(name, &gNbNameBuf[session * 0x10]);
        result = nb_call(session, &gNbNameBuf[session * 0x10]);
        break;
    case 3:
        nb_snd(gNbMaxSess, 0, 0);
        result = nb_listen(session, gNbListenName);
        break;
    case 4:
        nb_format_name(name, &gNbNameBuf[session * 0x10]);
        result = nb_listen(session, &gNbNameBuf[session * 0x10]);
        break;
    case 5:
        if (gNbMaxSess == session)
            gNbMaxSess = (unsigned char)(unsigned int)name;
        if (gNbSessLsn[session] == 0xff)
            return 0;
        gNbSessLsn[(int)name] = gNbSessLsn[session];
        gNetStatus[(int)name] = gNetStatus[session];
        memcpy(&gNbNameBuf[(int)name * 0x10], &gNbNameBuf[session * 0x10], 0x10);
        nb_arm_recv((int)name);
        if (detach != 0) {
            gNbSessLsn[session] = 0xff;
            gNetStatus[session] = 0;
            memset(&gNbNameBuf[session * 0x10], 0, 0x10);
        }
        result = 0;
        break;
    case 6:
        if (gNbSessNcb[session].ncb_cmd_cplt == 0xff) {
            memset(&ncb, 0, 0x40);
            ncb.ncb_command = 0x35;
            ncb.ncb_lana_num = gNetbiosLana;
            ncb.ncb_buffer = (unsigned char *)&gNbSessNcb[session];
            Netbios(&ncb);
        }
        nb_close_session(session);
        result = 0;
        break;
    case 7:
        gNetStatus[session] &= 0xf7;
        result = 0;
        break;
    case 9:
        memcpy(name, &gNbNameBuf[session * 0x10], 0x10);
        result = 0;
        break;
    default:
        return 1;
    }
    if (result == 0xff)
        result = 0;
    return result;
}

VA(0x004a7737, 0x21)
extern "C" char __fastcall nb_stat(short session) { return gNetStatus[session]; }

// FUN_004a832a — a session's async RECEIVE completed: enqueue its payload for nb_rcv, then re-arm.
VA(0x004a832a, 0x147)
static void __fastcall nb_recv_complete(int session)
{
    tag_Node *node;
    if ((gNbSessNcb[session].ncb_command & 0x7f) == 0x15) {
        switch (gNbSessNcb[session].ncb_retcode) {
        case 0:
            node = (tag_Node *)BaseAlloc(gNbSessNcb[session].ncb_length + 0xb,
                                         "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
            if (node != 0) {
                node->len = gNbSessNcb[session].ncb_length;
                node->field_0xa = (unsigned char)session;
                memcpy(node->data, &gNbRcvData[session * 0x1000], node->len);
                EnterCriticalSection(&gNbRcvLock);
                add_node(&gNbRcvQueue, node);
                LeaveCriticalSection(&gNbRcvLock);
            }
            nb_arm_recv(session);
            break;
        default:
            nb_arm_recv(session);
            break;
        case 8:
        case 10:
        case 0x18:
            gNetStatus[session] &= 0xfe;
        }
    }
}

// nb_thr_ctl — the receiver-thread pump: drain completed per-session receives into the rcv queue,
// then drain the send/free queues, issuing a synchronous NCBSEND for each queued packet.
VA(0x004a7758, 0xdd2)
void nb_thr_ctl(void)
{
    int running;
    DWORD r;
    int i;
    tag_Node *node;
    unsigned char rc;
    int done;

    running = 1;
    r = WaitForMultipleObjects(9, gNbEvents, 0, 0);
    if (r != 0x102) {
        r = WaitForSingleObject(gNbEvents[0], 0);
        if (r == 0)
            ResetEvent(gNbEvents[0]);
        for (i = 0; i < 5; i++) {
            r = WaitForSingleObject(gNbEvents[i + 2], 0);
            if (r == 0) {
                ResetEvent(gNbEvents[i + 2]);
                nb_recv_complete(i);
            }
        }
        while (running) {
            EnterCriticalSection(&gNbSndLock);
            node = pop_node(&gNbFreeQueue);
            if (node == 0)
                node = pop_node(&gNbSndQueue);
            LeaveCriticalSection(&gNbSndLock);
            if (node == 0) {
                running = 0;
            } else {
                memset(&gNbCtlNcb, 0, 0x40);
                gNbCtlNcb.ncb_lsn = gNbSessLsn[node->field_0xa];
                if (gNbCtlNcb.ncb_lsn != 0xff) {
                    memcpy(gNbSessBuf, node->data, node->len);
                    gNbCtlNcb.ncb_buffer = gNbSessBuf;
                    gNbCtlNcb.ncb_length = node->len;
                    gNbCtlNcb.ncb_command = 0x14;
                    gNbCtlNcb.ncb_lana_num = gNetbiosLana;
                    done = 0;
                    while (!done) {
                        rc = Netbios(&gNbCtlNcb);
                        if (rc < 9) {
                            if (rc == 8)
                                gNetStatus[node->field_0xa] &= 0xfe;
                            else if (rc == 0)
                                done = 1;
                        } else {
                            if (rc == 10 || rc == 0x18)
                                gNetStatus[node->field_0xa] &= 0xfe;
                            else if (rc == 0xff)
                                ProcessAssert(0, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
                        }
                    }
                }
                BaseFree(node, "I:\\Projects\\Heroes\\Prog\\SOURCE\\netwin.cpp", __LINE__);
            }
        }
    }
}

