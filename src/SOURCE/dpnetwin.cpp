#include <va.h>
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

#define RETAIL_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Game\\DPNETWIN.CPP"

H2_ENUM_CLASS_BEGIN(DirectPlaySessionOpenFlag)
    SESSION_OPEN_JOIN = 1,
    SESSION_OPEN_CREATE = 2
H2_ENUM_CLASS_END(DirectPlaySessionOpenFlag)

H2_ENUM_BEGIN(DirectPlayResult)
    RESULT_OK = 0,
    RESULT_INVALID_ARGUMENT = static_cast<i32>(0x80070057),
    RESULT_INVALID_PLAYER = static_cast<i32>(0x88770096),
    RESULT_NO_MESSAGES = static_cast<i32>(0x887700be),
    RESULT_NO_SESSIONS = static_cast<i32>(0x887700dc)
H2_ENUM_END(DirectPlayResult)

H2_ENUM_BEGIN(DirectPlayStorageConstant)
    RECEIVE_ARGUMENT_STORAGE_COUNT = 2,
    STATUS_TEXT_SIZE = 32
H2_ENUM_END(DirectPlayStorageConstant)

VA(0x00436770, 0x81)
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

VA(0x004367f1, 0x75)
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

VA(0x00436866, 0x2bf)
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
                /* "Невозможно загрузить 'DPLAYX.DLL'" */);
        createFunction = NULL;
        dpEnumerate = NULL;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayCreate")
        );
        if (createFunction == NULL)
            ShutDown("\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc 'DPLAYX.DLL'"
                /* "Невозможно загрузить 'DPLAYX.DLL'" */);
        dpEnumerate = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayEnumerateA")
        );
        if (dpEnumerate == NULL)
            ShutDown("\xcd\xe5\xe2\xee\xe7\xec\xee\xe6\xed\xee \xe7\xe0\xe3\xf0\xf3\xe7\xe8\xf2\xfc 'DPLAYX.DLL'"
                /* "Невозможно загрузить 'DPLAYX.DLL'" */);
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
        if (rc != RESULT_OK)
            DPSD(rc, RETAIL_FILE, 136);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = true;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_FIRST_GUEST;
            sprintf(
                gText,
                "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe3\xee\xf1\xf2\xff.\n\n  "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                    "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5."
                    /* "Ожидание гостя.\n\n  Нажмите 'ОТМЕНА', чтобы прервать соединение." */
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
                "\xe8\xe3\xf0\xee\xea\xee\xe2." /* "К вам присоединились %d гостей. Нажмите 'ОК' чтобы продолжить или подождите других игроков." */
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
                    /* "Ожидаю игрока для начала игры." */
            );
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
        }
    }
    return 0;
}

VA(0x00436b25, 0xc3)
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

VA(0x00436be8, 0xed)
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

VA(0x00436cd5, 0xcf)
void dpSendMessage(
    i32 destination,
    H2_ENUM_PARAM(NetworkPacketType, u8) type,
    u16 size,
    void* data
) {
    u8* message = static_cast<u8*>(H2_ALLOC(size + 1));
    i32 status;

    message[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(message + 1, data, size);
    status = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (status != RESULT_OK && status != RESULT_INVALID_PLAYER
        && status != RESULT_INVALID_ARGUMENT) {
        DPSD(status, RETAIL_FILE, 268);
    }
    H2_FREE(message);
}

VA(0x00436da4, 0x48)
i32 dpnet_snd(i32 position, i32 size, void* data) {

    dpProcessMessages();
    dpSendMessage((position == DP_TRANSPORT_BROADCAST_POSITION ? 0 : giNetPosToDCOPos[position]), NETWORK_PACKET_DATA, static_cast<u16>(size), data);
    return 0;
}

VA(0x00436dec, 0x93)
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

VA(0x00436e7f, 0x14)
u8 dpnet_stat(i16, u16) {
    return 0;
}

VA(0x00436e93, 0x8)
i16 __cdecl dpnet_sess(i32, i32, ...) {
    return 0;
}

VA(0x00436e9b, 0x98)
void dpProcessMessages(void) {
    DWORD size;
    i32 to;
    i32 i;
    i32 j;  // i and j are unreferenced; retail's frame reserves both slots
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
        if (receiveResult == RESULT_NO_MESSAGES)
            return;
        if (receiveResult != RESULT_OK)
            DPSD(receiveResult, RETAIL_FILE, 335);
        if (sender == 0) {
        } else {
            if (to == 0 || to == dcoID)
                dpEvaluateMessage(size, sender);
        }
    }
}

VA(0x00436f33, 0x244)
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

VA(0x00437177, 0x16a)
i32 dpWaitForFirstGuest(void) {
    DPSESSIONDESC session;
    i32 rv;

    switch (iDPWaitForFirstGuestStatus) {
        case FIRST_GUEST_CREATE_SESSION:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = IDX(SESSION_OPEN_CREATE);
            strcpy(session.szSessionName, "Heroes 2");
            rv = lpIDC->Open(&session);
            if (rv != RESULT_OK)
                DPSD(rv, RETAIL_FILE, 442);
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_DISABLE_COMPRESSION:
            gsThisNetPlayerInfo.useRegularCompression = 0;
            gsThisNetPlayerInfo.useDiffCompression = 0;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_CREATE_PLAYER:
            rv = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (rv != RESULT_OK)
                DPSD(rv, RETAIL_FILE, 472);
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

VA(0x004372e1, 0x7c)
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
            "\xe8\xe3\xf0\xee\xea\xee\xe2." /* "К вам присоединились %d гостей. Нажмите 'ОК' чтобы продолжить или подождите других игроков." */
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

VA(0x0043735d, 0x396)
i32 dpWaitForHost(void) {
    DPSESSIONDESC session;
    i32 rv;
    char text[STATUS_TEXT_SIZE];
    DWORD timeout;

    sprintf(text, "WFHS %d", iDPWaitForHostStatus);
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
            if (rv == RESULT_NO_SESSIONS) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                return 0;
            }
            if (rv != RESULT_OK)
                DPSD(rv, RETAIL_FILE, 548);
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
            session.dwFlags = IDX(SESSION_OPEN_JOIN);
            session.dwSession = lSessions[iSessionToTry];
            strcpy(session.szSessionName, "Heroes 2");
            rv = lpIDC->Open(&session);
            if (rv != RESULT_OK)
                DPSD(rv, RETAIL_FILE, 567);
            iDPWaitForHostStatus++;
            break;
        case HOST_CREATE_PLAYER:
            rv = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (rv != RESULT_OK)
                DPSD(rv, RETAIL_FILE, 577);
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
                if (rv != RESULT_OK)
                    DPSD(rv, RETAIL_FILE, 603);
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

VA(0x004376f3, 0x5d5)
void DPSD(i32 result, char* file, i32 line) {
    i32 flag;
    char errorText[REMOTE_ERROR_TEXT_SIZE];

    if (bInDPSD != 0)
        return;
    bInDPSD = 1;
    flag = 0;
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

DATA(0x005242b4) struct IDirectPlay* lpIDC = NULL;
DATA(0x005242b8) DPID dcoID = 0;
DATA(0x005242bc) struct _GUID* IPXGuid = NULL;
DATA(0x005242c0) struct _GUID* TCPGuid = NULL;
DATA(0x005242c4) HANDLE dphEvent = NULL;
DATA(0x005242c8) i32 iDPRcvBufferHead = 0;
DATA(0x005242cc) i32 iDPRcvBufferTail = 0;
DATA(0x005242d0) u8** ppDPRcvBuffer = NULL;
DATA(0x005242d4) i32* piDPRcvBufferSize = NULL;
DATA(0x005242d8) i32 bStartUpInfoReceived = 0;
DATA(0x005242dc) HMODULE hinstDplayx = NULL;
DATA(0x005242e0) H2_ENUM_STORAGE_STEPPED(DirectPlayFirstGuestState, i32)
iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
DATA(0x005242e4) H2_ENUM_STORAGE_STEPPED(DirectPlayHostState, i32) iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
DATA(0x005242e8) i32 iWaitForHostWaitCount = 0;
DATA(0x005242ec) i32 iEnumCount = 0;
DATA(0x005242f0) i32 iLastHereIAmTickCount = 0;
DATA(0x005242f4) i32 bInDPSD = 0;
DATA(0x005242f8) i32 iGUIDCount = 0;
DATA(0x004f1ba0) i32 iLastMsgNumHumanPlayers = 1;
DATA(0x00524280) i32 iMaxSession;
DATA(0x005242b0) DirectPlayHostAcceptStatus giHostAcceptStatus;
DATA(0x0052427c) struct _GUID* g_lpGuid;
DATA(0x00524264) i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
DATA(0x00524284) i32 iSessionToTry;
DATA(0x00524288) i32l lSessions[DP_TRANSPORT_SESSION_COUNT];
