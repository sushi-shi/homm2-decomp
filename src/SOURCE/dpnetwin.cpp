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

enum {
    SESSION_OPEN_JOIN   = 1,
    SESSION_OPEN_CREATE = 2
};
typedef i32 DirectPlaySessionOpenFlag;
typedef enum DirectPlayResult {
    RESULT_OK               = 0,
    RESULT_INVALID_ARGUMENT = static_cast<i32>(0x80070057),
    RESULT_INVALID_PLAYER   = static_cast<i32>(0x88770096),
    RESULT_NO_MESSAGES      = static_cast<i32>(0x887700be),
    RESULT_NO_SESSIONS      = static_cast<i32>(0x887700dc)
} DirectPlayResult;

typedef enum DirectPlayStorageConstant {
    RECEIVE_ARGUMENT_STORAGE_COUNT = 2,
    STATUS_TEXT_SIZE               = 32
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
    static i16 initSourceLineBase = 95;
    DirectPlayStartupMessage startup;
    typedef HRESULT(WINAPI * DirectPlayCreateFunction)(GUID*, IDirectPlay**, IUnknown*);
    typedef HRESULT(WINAPI * DirectPlayEnumerateFunction)(LPDPENUMDPCALLBACKA, void*);
    DirectPlayEnumerateFunction enumerateFunction;
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
            ShutDown("Can't load 'DPLAYX.DLL'");
        createFunction = NULL;
        enumerateFunction = NULL;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayCreate")
        );
        if (createFunction == NULL)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayEnumerateA")
        );
        if (enumerateFunction == NULL)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction(dpEnumServiceProvider, NULL);
        switch (iMPNetProtocol) {
            case DP_PROTOCOL_IPX:
                g_lpGuid = IPXGuid;
                break;
            case DP_PROTOCOL_TCP:
                g_lpGuid = TCPGuid;
                break;
        }
        rc = createFunction(g_lpGuid, &lpIDC, NULL);
        if (rc != RESULT_OK)
            DPSD(rc, "dpnetwin.cpp", initSourceLineBase + 41);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = true;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_FIRST_GUEST;
            sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_GUESTS;
            sprintf(
                gText,
                "You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for "
                "additional guests.",
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
            sprintf(gText, "Waiting for other remote player to set up game.");
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
    bStartUpInfoReceived = 0;
    hinstDplayx = NULL;
    iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
    iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
    iWaitForHostWaitCount = 0;
    iEnumCount = 0;
    iLastHereIAmTickCount = 0;
    bInDPSD = 0;
    iGUIDCount = 0;
    iLastMsgNumHumanPlayers = 1;
}

void dpnet_term(void) {
    static i16 termSourceLineBase = 219;
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
    static i16 sendSourceLineBase = 254;
    u8* message = static_cast<u8*>(H2_ALLOC(size + 1));
    i32 result;

    message[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(message + 1, data, size);
    result = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (result != RESULT_OK && result != RESULT_INVALID_PLAYER
        && result != RESULT_INVALID_ARGUMENT) {
        DPSD(result, "dpnetwin.cpp", sendSourceLineBase + 14);
    }
    H2_FREE(message);
}

i32 dpnet_snd(i32 position, i32 size, void* data) {
    i32 destination;

    dpProcessMessages();
    if (position == DP_TRANSPORT_BROADCAST_POSITION)
        destination = 0;
    else
        destination = giNetPosToDCOPos[position];
    dpSendMessage(destination, NETWORK_PACKET_DATA, static_cast<u16>(size), data);
    return 0;
}

i16 dpnet_rcv(i16, u16, void* data) {
    static i16 receiveSourceLineBase = 284;
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
    static i16 processSourceLineBase = 312;
    DWORD packetSize[RECEIVE_ARGUMENT_STORAGE_COUNT];
    i32 destinationIds[RECEIVE_ARGUMENT_STORAGE_COUNT];
    i32 senderId;
    i32 receiveResult;

    if (lpIDC == NULL)
        return;
    while (1) {
        packetSize[0] = DP_TRANSPORT_RECEIVE_SIZE;
        receiveResult = lpIDC->Receive(
            reinterpret_cast<LPDPID>(&senderId),
            reinterpret_cast<LPDPID>(destinationIds),
            1,
            rcvBufIn,
            packetSize
        );
        if (receiveResult == RESULT_NO_MESSAGES)
            return;
        if (receiveResult != RESULT_OK)
            DPSD(receiveResult, "dpnetwin.cpp", processSourceLineBase + 23);
        if (senderId == 0) {
        } else {
            if (destinationIds[0] == 0 || destinationIds[0] == dcoID)
                dpEvaluateMessage(packetSize[0], senderId);
        }
    }
}


void dpEvaluateMessage(u32l size, i32 sender) {
    static i16 evaluateSourceLineBase = 355;
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
                for (i = 1; giNumHumanPlayers > i; i++) {
                    if (giNetPosToDCOPos[i] == sender) {
                        dpSendMessage(sender, NETWORK_PACKET_GUEST_ACCEPTED, 0, NULL);
                        return;
                    }
                }
                if (gbRemoteGameOpen != 0) {
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    gsNetPlayerInfo[giNumHumanPlayers] = *reinterpret_cast<SNetPlayerInfo*>(startup);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = 1;
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
            bStartUpInfoReceived = 1;
            break;
        default:
            sprintf(gText, "Unknown message: %d\n", static_cast<i32>(rcvBufIn[0]));
            LogStr(gText);
            break;
    }
}

i32 dpWaitForFirstGuest(void) {
    static i16 firstGuestSourceLineBase = 426;
    DPSESSIONDESC session;
    i32 result;

    switch (iDPWaitForFirstGuestStatus) {
        case FIRST_GUEST_CREATE_SESSION:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = (SESSION_OPEN_CREATE);
            strcpy(session.szSessionName, "Heroes 2");
            result = lpIDC->Open(&session);
            if (result != RESULT_OK)
                DPSD(result, "dpnetwin.cpp", firstGuestSourceLineBase + 16);
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_DISABLE_COMPRESSION:
            gsThisNetPlayerInfo.useRegularCompression = 0;
            gsThisNetPlayerInfo.useDiffCompression = 0;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_CREATE_PLAYER:
            result = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (result != RESULT_OK)
                DPSD(result, "dpnetwin.cpp", firstGuestSourceLineBase + 46);
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
            "You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional "
            "guests.",
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
    static i16 hostSourceLineBase = 510;
    DWORD enumerationTimeout;
    DPSESSIONDESC sessionDescription;
    i32 playResult;
    char statusString[STATUS_TEXT_SIZE];

    sprintf(statusString, "WFHS %d", iDPWaitForHostStatus);
    AiPrint(statusString);
    switch (iDPWaitForHostStatus) {
        case HOST_ENUMERATE_SESSIONS:
            if (iWaitForHostWaitCount != 0) {
                iWaitForHostWaitCount--;
                DelayMilli(DP_TRANSPORT_RETRY_DELAY);
                return 0;
            }
            iSessionToTry = 0;
            memset(&sessionDescription, 0, sizeof(sessionDescription));
            sessionDescription.dwSize = sizeof(sessionDescription);
            sessionDescription.guidSession = *g_lpGuid;
            iMaxSession = 0;
            if (iEnumCount % DP_TRANSPORT_ENUM_LONG_INTERVAL != DP_TRANSPORT_ENUM_LONG_PHASE)
                enumerationTimeout = DP_TRANSPORT_ENUM_SHORT_TIMEOUT;
            else
                enumerationTimeout = DP_TRANSPORT_ENUM_LONG_TIMEOUT;
            playResult =
                lpIDC->EnumSessions(&sessionDescription, enumerationTimeout, dpEnumSession, NULL, 0);
            iEnumCount++;
            if (playResult == RESULT_NO_SESSIONS) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                return 0;
            }
            if (playResult != RESULT_OK)
                DPSD(playResult, "dpnetwin.cpp", hostSourceLineBase + 38);
            if (iMaxSession > 0) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                iDPWaitForHostStatus++;
            }
            break;
        case HOST_JOIN_SESSION:
            memset(&sessionDescription, 0, sizeof(sessionDescription));
            sessionDescription.dwSize = sizeof(sessionDescription);
            sessionDescription.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            sessionDescription.guidSession = *g_lpGuid;
            sessionDescription.dwFlags = (SESSION_OPEN_JOIN);
            sessionDescription.dwSession = lSessions[iSessionToTry];
            strcpy(sessionDescription.szSessionName, "Heroes 2");
            playResult = lpIDC->Open(&sessionDescription);
            if (playResult != RESULT_OK)
                DPSD(playResult, "dpnetwin.cpp", hostSourceLineBase + 57);
            iDPWaitForHostStatus++;
            break;
        case HOST_CREATE_PLAYER:
            playResult = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (playResult != RESULT_OK)
                DPSD(playResult, "dpnetwin.cpp", hostSourceLineBase + 67);
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
                playResult = lpIDC->Close();
                if (playResult != RESULT_OK)
                    DPSD(playResult, "dpnetwin.cpp", hostSourceLineBase + 93);
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

void DPSD(i32 result, char* file, i32 line) {
    char errorText[REMOTE_ERROR_TEXT_SIZE];
    i32 errorFlag;

    if (bInDPSD != 0)
        return;
    bInDPSD = 1;
    errorFlag = 0;
    switch (result) {
        case RESULT_OK:
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
i32 bStartUpInfoReceived = 0;
HMODULE hinstDplayx = NULL;
i32
iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
i32 iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
i32 iWaitForHostWaitCount = 0;
i32 iEnumCount = 0;
i32 iLastHereIAmTickCount = 0;
i32 bInDPSD = 0;
i32 iGUIDCount = 0;
i32 iLastMsgNumHumanPlayers = 1;
i32 iMaxSession;
DirectPlayHostAcceptStatus giHostAcceptStatus;
struct _GUID* g_lpGuid;
i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
i32 iSessionToTry;
i32l lSessions[DP_TRANSPORT_SESSION_COUNT];
