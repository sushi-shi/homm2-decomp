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
#include <BASE/Misc.h>
#include <_globals_model.h>
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

VA(0x004a6ecc, 0x207)
extern "C" int __fastcall nb_term(void) { return 0; }

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

VA(0x004a726a, 0x4cd)
extern "C" int __cdecl nb_sess(void) { return 0; }

VA(0x004a7737, 0x21)
extern "C" char __fastcall nb_stat(short session) { return gNetStatus[session]; }

VA(0x004a7758, 0xdd2)
void nb_thr_ctl(void) {}

