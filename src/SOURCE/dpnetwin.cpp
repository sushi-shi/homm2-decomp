#include <Ints.h>
#include <windows.h>
#include <dplay.h>
#include <string.h>
#include <stdio.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <BASE/heroWindow.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/dpnetwin.h>


enum class DirectPlaySessionOpenFlag : i32 {
    SESSION_OPEN_JOIN = 1,
    SESSION_OPEN_CREATE = 2
};
using enum DirectPlaySessionOpenFlag;

typedef enum DirectPlayStorageConstant {
    RECEIVE_ARGUMENT_STORAGE_COUNT = 2,
    STATUS_TEXT_SIZE = 32
} DirectPlayStorageConstant;

BOOL WINAPI dpEnumServiceProvider(struct _GUID* guid, char* name, DWORD, DWORD, void*) {
    LogStr("ServiceProvider:");
    _strupr(name);
    LogInt(
        name,
        reinterpret_cast<i32>(guid),
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    if (FindStringInString(name, "IPX") != NULL)
        IPXGuid = guid;
    else if (FindStringInString(name, "TCP") != NULL)
        TCPGuid = guid;
    return 1;
}

BOOL WINAPI dpEnumSession(DPSESSIONDESC* session, void*, LPDWORD, DWORD flags) {
    if (flags & DPESC_TIMEDOUT)
        return 0;
    LogStr("Sessions:");
    LogInt(
        session->szSessionName,
        session->dwSession,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    lSessions[iMaxSession] = session->dwSession;
    iMaxSession++;
    return 1;
}

i16 dpnet_init(void) {
    DirectPlayStartupMessage startup;
    typedef HRESULT(WINAPI * DirectPlayCreateFunction)(GUID*, IDirectPlay**, IUnknown*);
    typedef HRESULT(WINAPI * DirectPlayEnumerateFunction)(LPDPENUMDPCALLBACKA, void*);
    DirectPlayEnumerateFunction dpEnumerate;
    DirectPlayCreateFunction createFunction;
    i32 guestIndex;
    i32 rc;

    if (lpIDC != NULL)
        return 0;
    {
        ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*)));
        piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(DP_TRANSPORT_BUFFER_COUNT * sizeof(i32)));
        memset(ppDPRcvBuffer, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
        memset(piDPRcvBufferSize, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(i32));
        hinstDplayx = LoadLibraryA("DPLAYX.DLL");
        if (hinstDplayx == NULL)
            ShutDown("\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc 'DPLAYX.DLL'"
                 );
        createFunction = NULL;
        dpEnumerate = NULL;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayCreate")
        );
        if (createFunction == NULL)
            ShutDown("\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc 'DPLAYX.DLL'"
                 );
        dpEnumerate = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayEnumerateA")
        );
        if (dpEnumerate == NULL)
            ShutDown("\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc 'DPLAYX.DLL'"
                 );
        dpEnumerate(dpEnumServiceProvider, NULL);
        switch (iMPNetProtocol) {
            case DP_PROTOCOL_IPX:
                g_lpGuid = IPXGuid;
                break;
            case DP_PROTOCOL_TCP:
                g_lpGuid = TCPGuid;
                break;
        }
        rc = createFunction(g_lpGuid, &lpIDC, NULL);
        if (rc != DP_OK)
            DPSD(rc, "dpnetwin.cpp", 136);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = true;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_FIRST_GUEST;
            sprintf(
                gText,
                "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe3\xee\xf1\xf2\xff.\n\n  "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                    "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5."

            );
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_GUESTS;
            sprintf(
                gText,
                "\xca \xe2\xe0\xec \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc %d "
                "\xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca' "
                "\xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xeb\xe8 "
                "\xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 "
                "\xe8\xe3\xf0\xee\xea\xee\xe2."
                ,
                giNumHumanPlayers - 1
            );
            NormalDialog(gText, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            gbRemoteGameOpen = false;
            startup.playerCount = static_cast<u8>(giNumHumanPlayers);
            memcpy(startup.playerIds, giNetPosToDCOPos, sizeof(giNetPosToDCOPos));
            for (guestIndex = 1; guestIndex < giNumHumanPlayers; guestIndex++) {
                startup.netPosition = static_cast<u8>(guestIndex);
                dpSendMessage(
                    giNetPosToDCOPos[guestIndex],
                    NETWORK_PACKET_STARTUP,
                    sizeof(startup),
                    &startup
                );
            }
        } else {
            giWaitType = DIALOG_WAIT_DIRECTPLAY_HOST;
            sprintf(
                gText,
                "\xce\xe6\xe8\xe4\xe0\xfe \xe8\xe3\xf0\xee\xea\xe0 \xe4\xeb\xff \xed\xe0\xf7\xe0\xeb\xe0 "
                    "\xe8\xe3\xf0\xfb."

            );
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
        }
    }
    return 0;
}

void CleanupDPVars(void) {
    lpIDC = NULL;
    dcoID = 0;
    IPXGuid = NULL;
    TCPGuid = NULL;
    dphEvent = NULL;
    iDPRcvBufferHead = 0;
    iDPRcvBufferTail = 0;
    ppDPRcvBuffer = NULL;
    piDPRcvBufferSize = NULL;
    bStartUpInfoReceived = false;
    hinstDplayx = NULL;
    iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
    iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
    iWaitForHostWaitCount = 0;
    iEnumCount = 0;
    iLastHereIAmTickCount = 0;
    bInDPSD = false;
    iGUIDCount = 0;
    iLastMsgNumHumanPlayers = 1;
}

void dpnet_term(void) {
    char drainBuffer[DP_TRANSPORT_TERM_DRAIN_READ_SIZE + sizeof(i32)];

    gbRemoteOn = false;
    if (dcoID != 0)
        lpIDC->DestroyPlayer(dcoID);
    lpIDC->Close();
    lpIDC->Release();
    lpIDC = NULL;
    while (dpnet_rcv(0, DP_TRANSPORT_TERM_DRAIN_READ_SIZE, drainBuffer) != 0) {
    }
    if (ppDPRcvBuffer != NULL)
        H2_FREE(ppDPRcvBuffer);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize);
    piDPRcvBufferSize = NULL;
    if (hinstDplayx != NULL)
        FreeLibrary(hinstDplayx);
    hinstDplayx = NULL;
    CleanupDPVars();
}

void dpSendMessage(
    i32 destination,
    NetworkPacketType type,
    u16 size,
    void* data
) {
    u8* message = static_cast<u8*>(H2_ALLOC(size + 1));
    i32 status;

    message[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(message + 1, data, size);
    status = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (status != DP_OK && status != DPERR_INVALIDPLAYER
        && status != DPERR_INVALIDPARAM) {
        DPSD(status, "dpnetwin.cpp", 268);
    }
    H2_FREE(message);
}

i32 dpnet_snd(i32 position, i32 size, void* data) {

    dpProcessMessages();
    dpSendMessage((position == DP_TRANSPORT_BROADCAST_POSITION ? 0 : giNetPosToDCOPos[position]), NETWORK_PACKET_DATA, static_cast<u16>(size), data);
    return 0;
}

i16 dpnet_rcv(i16, u16, void* data) {
    u32 size;

    dpProcessMessages();
    if (iDPRcvBufferHead == iDPRcvBufferTail)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE(ppDPRcvBuffer[iDPRcvBufferTail]);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % DP_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}

u8 dpnet_stat(i16, u16) {
    return 0;
}

i16 __cdecl dpnet_sess(i32, i32, ...) {
    return 0;
}

void dpProcessMessages(void) {
    DWORD size;
    i32 to;
    i32 i [[maybe_unused]];
    i32 j [[maybe_unused]];
    i32 sender;
    i32 receiveResult;

    if (lpIDC == NULL)
        return;
    while (1) {
        size = DP_TRANSPORT_RECEIVE_SIZE;
        receiveResult = lpIDC->Receive(
            reinterpret_cast<LPDPID>(&sender),
            reinterpret_cast<LPDPID>(&to),
            1,
            rcvBufIn,
            &size
        );
        if (receiveResult == DPERR_NOMESSAGES)
            return;
        if (receiveResult != DP_OK)
            DPSD(receiveResult, "dpnetwin.cpp", 335);
        if (sender == 0) {
        } else {
            if (to == 0 || static_cast<DPID>(to) == dcoID)
                dpEvaluateMessage(size, sender);
        }
    }
}

void dpEvaluateMessage(u32l size, i32 sender) {
    DirectPlayStartupMessage* startup = reinterpret_cast<DirectPlayStartupMessage*>(rcvBufIn + 1);
    i32 i;

    switch (static_cast<NetworkPacketType>(rcvBufIn[0])) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC(size - 1));
            memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
            piDPRcvBufferSize[iDPRcvBufferHead] = size;
            iDPRcvBufferHead = (iDPRcvBufferHead + 1) % DP_TRANSPORT_BUFFER_COUNT;
            break;
        case NETWORK_PACKET_GUEST_ARRIVED:
            if (GameMode == REMOTE_GAME_NETWORK_HOST) {
                for (i = 1; i < giNumHumanPlayers; i++) {
                    if (giNetPosToDCOPos[i] == sender) {
                        dpSendMessage(sender, NETWORK_PACKET_GUEST_ACCEPTED, 0, NULL);
                        return;
                    }
                }
                if (gbRemoteGameOpen != 0) {
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    gsNetPlayerInfo[giNumHumanPlayers] =
                        *reinterpret_cast<SNetPlayerInfo*>(startup);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = true;
                    dpSendMessage(sender, NETWORK_PACKET_GUEST_ACCEPTED, 0, NULL);
                    giNumHumanPlayers++;
                } else {
                    dpSendMessage(sender, NETWORK_PACKET_GUEST_REJECTED, 0, NULL);
                }
            }
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            giHostAcceptStatus = HOST_ACCEPT_ACCEPTED;
            break;
        case NETWORK_PACKET_GUEST_REJECTED:
            giHostAcceptStatus = HOST_ACCEPT_REJECTED;
            break;
        case NETWORK_PACKET_STARTUP:
            giNumHumanPlayers = startup->playerCount;
            giThisNetPos = startup->netPosition;
            LogInt(
                "DPMSGSTARTUP",
                giThisNetPos,
                sender,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE
            );
            memcpy(giNetPosToDCOPos, startup->playerIds, sizeof(giNetPosToDCOPos));
            bStartUpInfoReceived = true;
            break;
        default:
            sprintf(gText, "Unknown message: %d\n", static_cast<i32>(rcvBufIn[0]));
            LogStr(gText);
            break;
    }
}

i32 dpWaitForFirstGuest(void) {
    DPSESSIONDESC session;
    i32 rv;

    switch (iDPWaitForFirstGuestStatus) {
        case FIRST_GUEST_CREATE_SESSION:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = H2EnumIndex(SESSION_OPEN_CREATE);
            strcpy(session.szSessionName, "Heroes 2");
            rv = lpIDC->Open(&session);
            if (rv != DP_OK)
                DPSD(rv, "dpnetwin.cpp", 442);
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_DISABLE_COMPRESSION:
            gsThisNetPlayerInfo.useRegularCompression = false;
            gsThisNetPlayerInfo.useDiffCompression = false;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_CREATE_PLAYER:
            rv = lpIDC->CreatePlayer(
                &dcoID,
                "Dude",
                "Heroes Player",
                &dphEvent
            );
            if (rv != DP_OK)
                DPSD(rv, "dpnetwin.cpp", 472);
            giNetPosToDCOPos[0] = dcoID;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_WAIT_FOR_PLAYER:
            dpProcessMessages();
            if (giNumHumanPlayers > 1)
                return 1;
    }
    return 0;
}

i32 dpWaitForExtraGuests(void) {
    tag_message message;

    dpProcessMessages();
    if (giNumHumanPlayers != iLastMsgNumHumanPlayers) {
        iLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            gText,
            "\xca \xe2\xe0\xec \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc %d "
            "\xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca' "
            "\xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xeb\xe8 "
            "\xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 "
            "\xe8\xe3\xf0\xee\xea\xee\xe2."
            ,
            giNumHumanPlayers - 1
        );
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = 1;
        message.payload.widget.data.text = gText;
        pNormalDialogWindow->BroadcastMessage(message);
        pNormalDialogWindow->DrawWindow();
    }
    return 0;
}

i32 dpWaitForHost(void) {
    DPSESSIONDESC session;
    i32 rv;
    char text[STATUS_TEXT_SIZE];
    DWORD timeout;

    sprintf(text, "WFHS %d", static_cast<i32>(iDPWaitForHostStatus));
    AiPrint(text);
    switch (iDPWaitForHostStatus) {
        case HOST_ENUMERATE_SESSIONS:
            if (iWaitForHostWaitCount != 0) {
                iWaitForHostWaitCount--;
                DelayMilli(DP_TRANSPORT_RETRY_DELAY);
                return 0;
            }
            iSessionToTry = 0;
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.guidSession = *g_lpGuid;
            iMaxSession = 0;
            if (iEnumCount % DP_TRANSPORT_ENUM_LONG_INTERVAL != DP_TRANSPORT_ENUM_LONG_PHASE)
                timeout = DP_TRANSPORT_ENUM_SHORT_TIMEOUT;
            else
                timeout = DP_TRANSPORT_ENUM_LONG_TIMEOUT;
            rv =
                lpIDC
                    ->EnumSessions(&session, timeout, dpEnumSession, NULL, 0);
            iEnumCount++;
            if (rv == DPERR_NOSESSIONS) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                return 0;
            }
            if (rv != DP_OK)
                DPSD(rv, "dpnetwin.cpp", 548);
            if (iMaxSession > 0) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                iDPWaitForHostStatus++;
            }
            break;
        case HOST_JOIN_SESSION:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = H2EnumIndex(SESSION_OPEN_JOIN);
            session.dwSession = lSessions[iSessionToTry];
            strcpy(session.szSessionName, "Heroes 2");
            rv = lpIDC->Open(&session);
            if (rv != DP_OK)
                DPSD(rv, "dpnetwin.cpp", 567);
            iDPWaitForHostStatus++;
            break;
        case HOST_CREATE_PLAYER:
            rv = lpIDC->CreatePlayer(
                &dcoID,
                "Dude",
                "Heroes Player",
                &dphEvent
            );
            if (rv != DP_OK)
                DPSD(rv, "dpnetwin.cpp", 577);
            iDPWaitForHostStatus++;
            break;
        case HOST_ANNOUNCE_PLAYER:
            iLastHereIAmTickCount = KBTickCount();
            giHostAcceptStatus = HOST_ACCEPT_PENDING;
            dpSendMessage(
                0,
                NETWORK_PACKET_GUEST_ARRIVED,
                sizeof(SNetPlayerInfo),
                &gsThisNetPlayerInfo
            );
            iDPWaitForHostStatus++;
            break;
        case HOST_WAIT_FOR_ACCEPT:
            dpProcessMessages();
            if (giHostAcceptStatus == HOST_ACCEPT_ACCEPTED) {
                iDPWaitForHostStatus++;
            } else if (giHostAcceptStatus == HOST_ACCEPT_REJECTED) {
                if (iSessionToTry + 1 >= iMaxSession) {
                    iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
                } else {
                    iDPWaitForHostStatus = HOST_JOIN_SESSION;
                    iSessionToTry++;
                }
                rv = lpIDC->Close();
                if (rv != DP_OK)
                    DPSD(rv, "dpnetwin.cpp", 603);
            } else if (iLastHereIAmTickCount + DP_TRANSPORT_ACCEPT_TIMEOUT < KBTickCount()) {
                iDPWaitForHostStatus--;
            }
            break;
        case HOST_WAIT_FOR_STARTUP:
            dpProcessMessages();
            if (bStartUpInfoReceived != 0)
                return 1;
            break;
    }
    return 0;
}

void DPSD(i32 result, const char* file, i32 line) {
    i32 flag [[maybe_unused]];
    char errorText[REMOTE_ERROR_TEXT_SIZE];

    if (bInDPSD != 0)
        return;
    bInDPSD = true;
    flag = 0;
    switch (result) {
        case DP_OK:
            return;
        case DPERR_ALREADYINITIALIZED:
            strcpy(errorText, "DPERR_ALREADYINITIALIZED ");
            break;
        case DPERR_ACCESSDENIED:
            strcpy(errorText, "DPERR_ACCESSDENIED       ");
            break;
        case DPERR_ACTIVEPLAYERS:
            strcpy(errorText, "DPERR_ACTIVEPLAYERS      ");
            break;
        case DPERR_BUFFERTOOSMALL:
            strcpy(errorText, "DPERR_BUFFERTOOSMALL     ");
            break;
        case DPERR_CANTADDPLAYER:
            strcpy(errorText, "DPERR_CANTADDPLAYER      ");
            break;
        case DPERR_CANTCREATEGROUP:
            strcpy(errorText, "DPERR_CANTCREATEGROUP    ");
            break;
        case DPERR_CANTCREATEPLAYER:
            strcpy(errorText, "DPERR_CANTCREATEPLAYER   ");
            break;
        case DPERR_CANTCREATESESSION:
            strcpy(errorText, "DPERR_CANTCREATESESSION  ");
            break;
        case DPERR_CAPSNOTAVAILABLEYET:
            strcpy(errorText, "DPERR_CAPSNOTAVAILABLEYET");
            break;
        case DPERR_EXCEPTION:
            strcpy(errorText, "DPERR_EXCEPTION          ");
            break;
        case DPERR_GENERIC:
            strcpy(errorText, "DPERR_GENERIC            ");
            break;
        case DPERR_INVALIDFLAGS:
            strcpy(errorText, "DPERR_INVALIDFLAGS       ");
            break;
        case DPERR_INVALIDOBJECT:
            strcpy(errorText, "DPERR_INVALIDOBJECT      ");
            break;
        case DPERR_INVALIDPARAMS:
            strcpy(errorText, "DPERR_INVALIDPARAMS      ");
            break;
        case DPERR_INVALIDPLAYER:
            strcpy(errorText, "DPERR_INVALIDPLAYER      ");
            break;
        case DPERR_NOCAPS:
            strcpy(errorText, "DPERR_NOCAPS             ");
            break;
        case DPERR_NOCONNECTION:
            strcpy(errorText, "DPERR_NOCONNECTION       ");
            break;
        case DPERR_OUTOFMEMORY:
            strcpy(errorText, "DPERR_OUTOFMEMORY        ");
            break;
        case DPERR_NOMESSAGES:
            strcpy(errorText, "DPERR_NOMESSAGES         ");
            break;
        case DPERR_NONAMESERVERFOUND:
            strcpy(errorText, "DPERR_NONAMESERVERFOUND  ");
            break;
        case DPERR_NOPLAYERS:
            strcpy(errorText, "DPERR_NOPLAYERS          ");
            break;
        case DPERR_NOSESSIONS:
            strcpy(errorText, "DPERR_NOSESSIONS         ");
            break;
        case DPERR_SENDTOOBIG:
            strcpy(errorText, "DPERR_SENDTOOBIG         ");
            break;
        case DPERR_TIMEOUT:
            strcpy(errorText, "DPERR_TIMEOUT            ");
            break;
        case DPERR_UNAVAILABLE:
            strcpy(errorText, "DPERR_UNAVAILABLE        ");
            break;
        case DPERR_UNSUPPORTED:
            strcpy(errorText, "DPERR_UNSUPPORTED        ");
            break;
        case DPERR_BUSY:
            strcpy(errorText, "DPERR_BUSY               ");
            break;
        case DPERR_USERCANCEL:
            strcpy(errorText, "DPERR_USERCANCEL         ");
            break;
        default:
            strcpy(errorText, "Error type unknown");
            break;
    }
    MessageBeep(0);
    MessageBeep(0);
    MessageBeep(0);
    sprintf(gText, "DirectPlay Error:\n\n'%s'\n\n  File:'%s'\n Line# %d", errorText, file, line);
    LogStr(gText);
    ShutDown(gText);
}

struct IDirectPlay* lpIDC = NULL;
DPID dcoID = 0;
struct _GUID* IPXGuid = NULL;
struct _GUID* TCPGuid = NULL;
HANDLE dphEvent = NULL;
i32 iDPRcvBufferHead = 0;
i32 iDPRcvBufferTail = 0;
u8** ppDPRcvBuffer = NULL;
i32* piDPRcvBufferSize = NULL;
b32 bStartUpInfoReceived = false;
HMODULE hinstDplayx = NULL;
H2SteppedEnumStorage<DirectPlayFirstGuestState, i32>
iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
H2SteppedEnumStorage<DirectPlayHostState, i32> iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
i32 iWaitForHostWaitCount = 0;
i32 iEnumCount = 0;
i32 iLastHereIAmTickCount = 0;
b32 bInDPSD = false;
i32 iGUIDCount = 0;
i32 iLastMsgNumHumanPlayers = 1;
i32 iMaxSession;
DirectPlayHostAcceptStatus giHostAcceptStatus;
struct _GUID* g_lpGuid;
i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
i32 iSessionToTry;
i32l lSessions[DP_TRANSPORT_SESSION_COUNT];
