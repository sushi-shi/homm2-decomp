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

H2_ENUM_CLASS_BEGIN(DirectPlaySessionOpenFlag)
    SESSION_OPEN_JOIN   = 1,
    SESSION_OPEN_CREATE = 2
H2_ENUM_CLASS_END(DirectPlaySessionOpenFlag)

H2_ENUM_CLASS_BEGIN(DirectPlayWaitType)
    WAIT_FIRST_GUEST  = 8,
    WAIT_EXTRA_GUESTS = 9,
    WAIT_HOST         = 10
H2_ENUM_CLASS_END(DirectPlayWaitType)

H2_ENUM_CLASS_BEGIN(DirectPlayResult)
    RESULT_OK               = 0,
    RESULT_INVALID_ARGUMENT = static_cast<i32>(0x80070057),
    RESULT_INVALID_PLAYER   = static_cast<i32>(0x88770096),
    RESULT_NO_MESSAGES      = static_cast<i32>(0x887700be),
    RESULT_NO_SESSIONS      = static_cast<i32>(0x887700dc)
H2_ENUM_CLASS_END(DirectPlayResult)

H2_ENUM_BEGIN(DirectPlayImplementationConstant)
    RECEIVE_ARGUMENT_STORAGE_COUNT = 2,
    STATUS_TEXT_SIZE               = 32,
    INIT_BUFFER_POINTER_LINE       = 7,
    INIT_BUFFER_SIZE_LINE          = 8,
    INIT_CREATE_LINE               = 41,
    TERM_BUFFER_POINTER_LINE       = 14,
    TERM_BUFFER_SIZE_LINE          = 18,
    SEND_ALLOC_LINE                = 2,
    SEND_ERROR_LINE                = 14,
    SEND_FREE_LINE                 = 16,
    RECEIVE_FREE_LINE              = 9,
    PROCESS_ERROR_LINE             = 23,
    EVALUATE_ALLOC_LINE            = 8,
    FIRST_GUEST_OPEN_LINE          = 16,
    FIRST_GUEST_PLAYER_LINE        = 46,
    HOST_ENUMERATE_LINE            = 38,
    HOST_OPEN_LINE                 = 57,
    HOST_PLAYER_LINE               = 67,
    HOST_CLOSE_LINE                = 93
H2_ENUM_END(DirectPlayImplementationConstant)

H2_ENUM_BEGIN(DirectPlayHostAcceptStatus)
    HOST_ACCEPT_PENDING  = 0,
    HOST_ACCEPT_ACCEPTED = 1,
    HOST_ACCEPT_REJECTED = 2
H2_ENUM_END(DirectPlayHostAcceptStatus)

H2_ENUM_BEGIN(DirectPlayFirstGuestState)
    FIRST_GUEST_CREATE_SESSION      = 0,
    FIRST_GUEST_DISABLE_COMPRESSION = 1,
    FIRST_GUEST_CREATE_PLAYER       = 2,
    FIRST_GUEST_WAIT_FOR_PLAYER     = 3
H2_ENUM_END(DirectPlayFirstGuestState)

H2_ENUM_BEGIN(DirectPlayHostState)
    HOST_ENUMERATE_SESSIONS = 0,
    HOST_JOIN_SESSION       = 1,
    HOST_CREATE_PLAYER      = 2,
    HOST_ANNOUNCE_PLAYER    = 3,
    HOST_WAIT_FOR_ACCEPT    = 4,
    HOST_WAIT_FOR_STARTUP   = 5
H2_ENUM_END(DirectPlayHostState)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\dpnetwin.cpp")

VA(0x0041eda0, 0x95)
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

VA(0x0041ee35, 0x7a)
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

VA(0x0041eeaf, 0x311)
i16 dpnet_init(void) {
    DATA(0x004ef83c) static i16 initSourceLineBase = 95; // NOLINT(readability-magic-numbers)
    DirectPlayStartupMessage startup;
    typedef HRESULT(WINAPI * DirectPlayCreateFunction)(GUID*, IDirectPlay**, IUnknown*);
    typedef HRESULT(WINAPI * DirectPlayEnumerateFunction)(LPDPENUMDPCALLBACK, void*);
    DirectPlayCreateFunction createFunction;
    DirectPlayEnumerateFunction enumerateFunction;
    i32 guestIndex;
    i32 result;

    if (lpIDC != NULL)
        return 0;
    {
        ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*),
            initSourceLineBase + INIT_BUFFER_POINTER_LINE
        ));
        piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(i32),
            initSourceLineBase + INIT_BUFFER_SIZE_LINE
        ));
        memset(ppDPRcvBuffer, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
        memset(piDPRcvBufferSize, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(i32));
        hinstDplayx = LoadLibraryA("DPLAYX.DLL");
        if (hinstDplayx == NULL)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = NULL;
        createFunction = NULL;
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
        result = createFunction(g_lpGuid, &lpIDC, NULL);
        if (result != IDX(RESULT_OK))
            DPSD(result, RETAIL_FILE, initSourceLineBase + INIT_CREATE_LINE);

        if (GameMode == IDX(REMOTE_GAME_NETWORK_HOST)) {
            gbRemoteGameOpen = true;
            giWaitType = IDX(WAIT_FIRST_GUEST);
            sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = IDX(WAIT_EXTRA_GUESTS);
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
                    IDX(NETWORK_PACKET_STARTUP),
                    sizeof(startup),
                    &startup
                );
            }
        } else {
            giWaitType = IDX(WAIT_HOST);
            sprintf(gText, "Waiting for other remote player to set up game.");
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
        }
    }
    return 0;
}

VA(0x0041f1c0, 0xce)
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

VA(0x0041f28e, 0x116)
void dpnet_term(void) {
    DATA(0x004efa00) static i16 termSourceLineBase = 219; // NOLINT(readability-magic-numbers)
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
        H2_FREE(ppDPRcvBuffer, termSourceLineBase + TERM_BUFFER_POINTER_LINE);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize, termSourceLineBase + TERM_BUFFER_SIZE_LINE);
    piDPRcvBufferSize = NULL;
    if (hinstDplayx != NULL)
        FreeLibrary(hinstDplayx);
    hinstDplayx = NULL;
    CleanupDPVars();
}

VA(0x0041f3a4, 0xee)
void dpSendMessage(i32 destination, u8 type, u16 size, void* data) {
    DATA(0x004efa5c) static i16 sendSourceLineBase = 254; // NOLINT(readability-magic-numbers)
    u8* message =
        static_cast<u8*>(H2_ALLOC(size + 1, sendSourceLineBase + SEND_ALLOC_LINE));
    i32 result;

    message[0] = type;
    if (size != 0)
        memcpy(message + 1, data, size);
    result = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (result != IDX(RESULT_OK) && result != IDX(RESULT_INVALID_PLAYER)
        && result != IDX(RESULT_INVALID_ARGUMENT)) {
        DPSD(result, RETAIL_FILE, sendSourceLineBase + SEND_ERROR_LINE);
    }
    H2_FREE(message, sendSourceLineBase + SEND_FREE_LINE);
}

VA(0x0041f492, 0x5a)
i32 dpnet_snd(i32 position, i32 size, void* data) {
    i32 destination;

    dpProcessMessages();
    if (position == DP_TRANSPORT_BROADCAST_POSITION)
        destination = 0;
    else
        destination = giNetPosToDCOPos[position];
    dpSendMessage(destination, IDX(NETWORK_PACKET_DATA), static_cast<u16>(size), data);
    return 0;
}

VA(0x0041f4ec, 0xa7)
i16 dpnet_rcv(i16, u16, void* data) {
    DATA(0x004efae4) static i16 receiveSourceLineBase = 284; // NOLINT(readability-magic-numbers)
    u32 size;

    dpProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE(
        ppDPRcvBuffer[iDPRcvBufferTail], receiveSourceLineBase + RECEIVE_FREE_LINE
    );
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % DP_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}

VA(0x0041f593, 0x1d)
u8 dpnet_stat(i16, u16) {
    return 0;
}

VA(0x0041f5b0, 0x13)
i16 __cdecl dpnet_sess(i32, i32, ...) {
    return 0;
}

VA(0x0041f5c3, 0xbe)
void dpProcessMessages(void) {
    DATA(0x004efb14) static i16 processSourceLineBase = 312; // NOLINT(readability-magic-numbers)
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
        if (receiveResult == IDX(RESULT_NO_MESSAGES))
            return;
        if (receiveResult != IDX(RESULT_OK))
            DPSD(receiveResult, RETAIL_FILE, processSourceLineBase + PROCESS_ERROR_LINE);
        if (senderId == 0) {
        } else {
            if (destinationIds[0] == 0 || destinationIds[0] == OD_STEER(dcoID))
                dpEvaluateMessage(packetSize[0], senderId);
        }
    }
}

// Named one-byte retail stub.
VA(0x0041f681, 0x274)
void dpEvaluateMessage(u32l size, i32 sender) {
    DATA(0x004efb44) static i16 evaluateSourceLineBase = 355; // NOLINT(readability-magic-numbers)
    char* ptr = rcvBufIn + 1;
    DirectPlayStartupMessage* startup;
    i32 i;

    switch (rcvBufIn[0]) {
        case IDX(NETWORK_PACKET_DATA):
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC(
                size - 1, evaluateSourceLineBase + EVALUATE_ALLOC_LINE
            ));
            memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
            piDPRcvBufferSize[iDPRcvBufferHead] = size;
            iDPRcvBufferHead = (iDPRcvBufferHead + 1) % DP_TRANSPORT_BUFFER_COUNT;
            break;
        case IDX(NETWORK_PACKET_GUEST_ARRIVED):
            if (GameMode == IDX(REMOTE_GAME_NETWORK_HOST)) {
                for (i = 1; i < giNumHumanPlayers; i++) {
                    if (giNetPosToDCOPos[i] == sender) {
                        dpSendMessage(sender, IDX(NETWORK_PACKET_GUEST_ACCEPTED), 0, NULL);
                        return;
                    }
                }
                if (gbRemoteGameOpen != 0) {
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    gsNetPlayerInfo[giNumHumanPlayers] = *reinterpret_cast<SNetPlayerInfo*>(ptr);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = 1;
                    dpSendMessage(sender, IDX(NETWORK_PACKET_GUEST_ACCEPTED), 0, NULL);
                    giNumHumanPlayers++;
                } else {
                    dpSendMessage(sender, IDX(NETWORK_PACKET_GUEST_REJECTED), 0, NULL);
                }
            }
            break;
        case IDX(NETWORK_PACKET_GUEST_ACCEPTED):
            giHostAcceptStatus = HOST_ACCEPT_ACCEPTED;
            break;
        case IDX(NETWORK_PACKET_GUEST_REJECTED):
            giHostAcceptStatus = HOST_ACCEPT_REJECTED;
            break;
        case IDX(NETWORK_PACKET_STARTUP):
            startup = reinterpret_cast<DirectPlayStartupMessage*>(ptr);
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

VA(0x0041f8f5, 0x182)
i32 dpWaitForFirstGuest(void) {
    DATA(0x004efb9c) static i16 firstGuestSourceLineBase = 426; // NOLINT(readability-magic-numbers)
    DPSESSIONDESC session;
    i32 result;

    switch (iDPWaitForFirstGuestStatus) {
        case FIRST_GUEST_CREATE_SESSION:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = IDX(SESSION_OPEN_CREATE);
            strcpy(session.szSessionName, "Heroes 2");
            result = lpIDC->Open(&session);
            if (result != IDX(RESULT_OK))
                DPSD(result, RETAIL_FILE, firstGuestSourceLineBase + FIRST_GUEST_OPEN_LINE);
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_DISABLE_COMPRESSION:
            gsThisNetPlayerInfo.useRegularCompression = 0;
            gsThisNetPlayerInfo.useDiffCompression = 0;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_CREATE_PLAYER:
            result = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (result != IDX(RESULT_OK))
                DPSD(result, RETAIL_FILE, firstGuestSourceLineBase + FIRST_GUEST_PLAYER_LINE);
            giNetPosToDCOPos[0] = dcoID;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_WAIT_FOR_PLAYER:
            dpProcessMessages();
            if (giNumHumanPlayers > 1)
                return 1;
            break;
    }
    return 0;
}

VA(0x0041fa77, 0x84)
i32 dpWaitForExtraGuests(void) {
    tag_message message;

    dpProcessMessages();
    if (iLastMsgNumHumanPlayers != OD_STEER(giNumHumanPlayers)) {
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

VA(0x0041fafb, 0x3d2)
i32 dpWaitForHost(void) {
    DATA(0x004efc78) static i16 hostSourceLineBase = 510; // NOLINT(readability-magic-numbers)
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
            if (playResult == IDX(RESULT_NO_SESSIONS)) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                return 0;
            }
            if (playResult != IDX(RESULT_OK))
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + HOST_ENUMERATE_LINE);
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
            sessionDescription.dwFlags = IDX(SESSION_OPEN_JOIN);
            sessionDescription.dwSession = lSessions[iSessionToTry];
            strcpy(sessionDescription.szSessionName, "Heroes 2");
            playResult = lpIDC->Open(&sessionDescription);
            if (playResult != IDX(RESULT_OK))
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + HOST_OPEN_LINE);
            iDPWaitForHostStatus++;
            break;
        case HOST_CREATE_PLAYER:
            playResult = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (playResult != IDX(RESULT_OK))
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + HOST_PLAYER_LINE);
            iDPWaitForHostStatus++;
            break;
        case HOST_ANNOUNCE_PLAYER:
            iLastHereIAmTickCount = KBTickCount();
            giHostAcceptStatus = HOST_ACCEPT_PENDING;
            dpSendMessage(
                0,
                IDX(NETWORK_PACKET_GUEST_ARRIVED),
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
                if (playResult != IDX(RESULT_OK))
                    DPSD(playResult, RETAIL_FILE, hostSourceLineBase + HOST_CLOSE_LINE);
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

VA(0x0041fecd, 0x5eb)
void DPSD(i32 result, char* file, i32 line) {
    char errorText[REMOTE_ERROR_TEXT_SIZE];
    i32 errorFlag;

    if (bInDPSD != 0)
        return;
    bInDPSD = 1;
    errorFlag = 0;
    switch (result) {
        case IDX(RESULT_OK):
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

DATA(0x004ef7c8) struct IDirectPlay* lpIDC = NULL;
DATA(0x004ef7cc) DPID dcoID = 0;
DATA(0x004ef7d0) struct _GUID* IPXGuid = NULL;
DATA(0x004ef7d4) struct _GUID* TCPGuid = NULL;
DATA(0x004ef7d8) HANDLE dphEvent = NULL;
DATA(0x004ef7dc) i32 iDPRcvBufferHead = 0;
DATA(0x004ef7e0) i32 iDPRcvBufferTail = 0;
DATA(0x004ef7e4) u8** ppDPRcvBuffer = NULL;
DATA(0x004ef7e8) i32* piDPRcvBufferSize = NULL;
DATA(0x004ef7ec) i32 bStartUpInfoReceived = 0;
DATA(0x004ef7f0) HMODULE hinstDplayx = NULL;
DATA(0x004ef7f4) i32 iDPWaitForFirstGuestStatus = 0;
DATA(0x004ef7f8) i32 iDPWaitForHostStatus = 0;
DATA(0x004ef7fc) i32 iWaitForHostWaitCount = 0;
DATA(0x004ef800) i32 iEnumCount = 0;
DATA(0x004ef804) i32 iLastHereIAmTickCount = 0;
DATA(0x004ef808) i32 bInDPSD = 0;
DATA(0x004ef80c) i32 iGUIDCount = 0;
DATA(0x004ef810) i32 iLastMsgNumHumanPlayers = 1;
DATA(0x00525060) i32 iMaxSession;
DATA(0x00525064) i32 giHostAcceptStatus;
DATA(0x00525068) struct _GUID* g_lpGuid;
DATA(0x00525070) i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
DATA(0x00525088) i32 iSessionToTry;
DATA(0x00525090) i32l lSessions[DP_TRANSPORT_SESSION_COUNT];

#undef RETAIL_FILE
