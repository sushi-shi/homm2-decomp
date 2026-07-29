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

H2_ENUM_BEGIN(DirectPlayResult)
    RESULT_OK               = 0,
    RESULT_INVALID_ARGUMENT = static_cast<i32>(0x80070057),
    RESULT_INVALID_PLAYER   = static_cast<i32>(0x88770096),
    RESULT_NO_MESSAGES      = static_cast<i32>(0x887700be),
    RESULT_NO_SESSIONS      = static_cast<i32>(0x887700dc)
H2_ENUM_END(DirectPlayResult)

H2_ENUM_BEGIN(DirectPlayStorageConstant)
    RECEIVE_ARGUMENT_STORAGE_COUNT = 2,
    STATUS_TEXT_SIZE               = 32
H2_ENUM_END(DirectPlayStorageConstant)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\dpnetwin.cpp")

VA(0x0041eda0, 0x95)
BOOL WINAPI dpEnumServiceProvider(struct _GUID* guid, char* name, DWORD, DWORD, void*) {
    LogStr(DATA_COMPGEN(0x004ef814, dpEnumServiceProviderServiceProvider, "ServiceProvider:"));
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
    if (FindStringInString(name, DATA_COMPGEN(0x004ef828, dpEnumServiceProviderIPX, "IPX")) != NULL)
        IPXGuid = guid;
    else if (FindStringInString(name, DATA_COMPGEN(0x004ef82c, dpEnumServiceProviderTCP, "TCP")) != NULL)
        TCPGuid = guid;
    return 1;
}

VA(0x0041ee35, 0x7a)
BOOL WINAPI dpEnumSession(DPSESSIONDESC* session, void*, LPDWORD, DWORD flags) {
    if (flags & DPESC_TIMEDOUT)
        return 0;
    LogStr(DATA_COMPGEN(0x004ef830, dpEnumSessionSessions, "Sessions:"));
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
    DirectPlayEnumerateFunction enumerateFunction;
    DirectPlayCreateFunction createFunction;
    i32 guestIndex;
    i32 result;

    if (lpIDC != NULL)
        return 0;
    {
        ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC_AT(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*), DATA_COMPGEN(0x004ef86c, dpnet_initSourceFile, RETAIL_FILE),
            initSourceLineBase + 7
        ));
        piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC_AT(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(i32), DATA_COMPGEN(0x004ef918, dpnet_initSourceFile2, RETAIL_FILE),
            initSourceLineBase + 8
        ));
        memset(ppDPRcvBuffer, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
        memset(piDPRcvBufferSize, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(i32));
        hinstDplayx = LoadLibraryA(DATA_COMPGEN(0x004ef898, dpnetInitDPLAYXDLL, "DPLAYX.DLL"));
        if (hinstDplayx == NULL)
            ShutDown(DATA_COMPGEN(0x004ef8a4, dpnetInitCanTLoadDPLAYXDLL, "Can't load 'DPLAYX.DLL'"));
        createFunction = NULL;
        enumerateFunction = NULL;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(hinstDplayx, DATA_COMPGEN(0x004ef8bc, dpnetInitDirectPlayCreate, "DirectPlayCreate"))
        );
        if (createFunction == NULL)
            ShutDown(DATA_COMPGEN(0x004ef8d0, dpnetInitCanTLoadDPLAYXDLL2, "Can't load 'DPLAYX.DLL'"));
        enumerateFunction = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(hinstDplayx, DATA_COMPGEN(0x004ef8e8, dpnetInitDirectPlayEnumerateA, "DirectPlayEnumerateA"))
        );
        if (enumerateFunction == NULL)
            ShutDown(DATA_COMPGEN(0x004ef900, dpnetInitCanTLoadDPLAYXDLL3, "Can't load 'DPLAYX.DLL'"));
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
        if (result != RESULT_OK)
            DPSD(result, DATA_COMPGEN(0x004ef840, dpnetInitIProjectsHeroesProgSOURCEDpnetwin, RETAIL_FILE), initSourceLineBase + 41);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = true;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_FIRST_GUEST;
            sprintf(gText, DATA_COMPGEN(0x004ef944, dpnetInitWaitingOnGuestPressCANCELTo, "Waiting On Guest.\n\n  Press 'CANCEL' to abort."));
            NormalDialog(gText, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = DIALOG_WAIT_DIRECTPLAY_GUESTS;
            sprintf(
                gText,
                DATA_COMPGEN(0x004ef974, dpnetInitYouHaveDGuestSNow, "You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for "
                "additional guests."),
                giNumHumanPlayers - 1
            );
            NormalDialog(gText, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            gbRemoteGameOpen = false;
            startup.playerCount = static_cast<u8>(giNumHumanPlayers);
            memcpy(startup.playerIds, giNetPosToDCOPos, sizeof(giNetPosToDCOPos));
            for (guestIndex = 1; giNumHumanPlayers > guestIndex; guestIndex++) {
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
            sprintf(gText, DATA_COMPGEN(0x004ef9d0, dpnetInitWaitingForOtherRemotePlayerTo, "Waiting for other remote player to set up game."));
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
        H2_FREE_AT(ppDPRcvBuffer, DATA_COMPGEN(0x004efa04, dpnet_termSourceFile, RETAIL_FILE), termSourceLineBase + 14);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE_AT(piDPRcvBufferSize, DATA_COMPGEN(0x004efa30, dpnet_termSourceFile2, RETAIL_FILE), termSourceLineBase + 18);
    piDPRcvBufferSize = NULL;
    if (hinstDplayx != NULL)
        FreeLibrary(hinstDplayx);
    hinstDplayx = NULL;
    CleanupDPVars();
}

VA(0x0041f3a4, 0xee)
void dpSendMessage(
    i32 destination,
    H2_ENUM_PARAM(NetworkPacketType, u8) type,
    u16 size,
    void* data
) {
    DATA(0x004efa5c) static i16 sendSourceLineBase = 254; // NOLINT(readability-magic-numbers)
    u8* message = static_cast<u8*>(H2_ALLOC_AT(size + 1, DATA_COMPGEN(0x004efa8c, dpSendMessageSourceFile, RETAIL_FILE), sendSourceLineBase + 2));
    i32 result;

    message[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(message + 1, data, size);
    result = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (result != RESULT_OK && result != RESULT_INVALID_PLAYER
        && result != RESULT_INVALID_ARGUMENT) {
        DPSD(result, DATA_COMPGEN(0x004efa60, dpSendMessageIProjectsHeroesProgSOURCEDpnetwin, RETAIL_FILE), sendSourceLineBase + 14);
    }
    H2_FREE_AT(message, DATA_COMPGEN(0x004efab8, dpSendMessageSourceFile2, RETAIL_FILE), sendSourceLineBase + 16);
}

VA(0x0041f492, 0x5a)
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

VA(0x0041f4ec, 0xa7)
i16 dpnet_rcv(i16, u16, void* data) {
    DATA(0x004efae4) static i16 receiveSourceLineBase = 284; // NOLINT(readability-magic-numbers)
    u32 size;

    dpProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE_AT(
        ppDPRcvBuffer[iDPRcvBufferTail], DATA_COMPGEN(0x004efae8, dpnet_rcvSourceFile, RETAIL_FILE), receiveSourceLineBase + 9
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
        if (receiveResult == RESULT_NO_MESSAGES)
            return;
        if (receiveResult != RESULT_OK)
            DPSD(receiveResult, DATA_COMPGEN(0x004efb18, dpProcessMessagesIProjectsHeroesProgSOURCEDpnetwin, RETAIL_FILE), processSourceLineBase + 23);
        if (senderId == 0) {
        } else {
            if (destinationIds[0] == 0 || destinationIds[0] == dcoID)
                dpEvaluateMessage(packetSize[0], senderId);
        }
    }
}

// Named one-byte retail stub.
VA(0x0041f681, 0x274)
void dpEvaluateMessage(u32l size, i32 sender) {
    DATA(0x004efb44) static i16 evaluateSourceLineBase = 355; // NOLINT(readability-magic-numbers)
    DirectPlayStartupMessage* startup = reinterpret_cast<DirectPlayStartupMessage*>(rcvBufIn + 1);
    i32 i;

    switch (static_cast<NetworkPacketType>(rcvBufIn[0])) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC_AT(
                size - 1, DATA_COMPGEN(0x004efb48, dpEvaluateMessageSourceFile, RETAIL_FILE), evaluateSourceLineBase + 8
            ));
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
                DATA_COMPGEN(0x004efb74, dpEvaluateMessageDPMSGSTARTUP, "DPMSGSTARTUP"),
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
            sprintf(gText, DATA_COMPGEN(0x004efb84, dpEvaluateMessageUnknownMessageD, "Unknown message: %d\n"), static_cast<i32>(rcvBufIn[0]));
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
            strcpy(session.szSessionName, DATA_COMPGEN(0x004efba0, dpWaitForFirstGuestHeroes2, "Heroes 2"));
            result = lpIDC->Open(&session);
            if (result != RESULT_OK)
                DPSD(result, DATA_COMPGEN(0x004efbac, dpWaitForFirstGuestIProjectsHeroesProgSOURCEDpnetwin, RETAIL_FILE), firstGuestSourceLineBase + 16);
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_DISABLE_COMPRESSION:
            gsThisNetPlayerInfo.useRegularCompression = 0;
            gsThisNetPlayerInfo.useDiffCompression = 0;
            iDPWaitForFirstGuestStatus++;
            break;
        case FIRST_GUEST_CREATE_PLAYER:
            result = lpIDC->CreatePlayer(&dcoID, DATA_COMPGEN(0x004efbe8, dpWaitForFirstGuestDude, "Dude"), DATA_COMPGEN(0x004efbd8, dpWaitForFirstGuestHeroesPlayer, "Heroes Player"), &dphEvent);
            if (result != RESULT_OK)
                DPSD(result, DATA_COMPGEN(0x004efbf0, dpWaitForFirstGuestIProjectsHeroesProgSOURCEDpnetwin2, RETAIL_FILE), firstGuestSourceLineBase + 46);
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

VA(0x0041fa77, 0x84)
i32 dpWaitForExtraGuests(void) {
    tag_message message;

    dpProcessMessages();
    if (iLastMsgNumHumanPlayers != giNumHumanPlayers) {
        iLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            gText,
            DATA_COMPGEN(0x004efc1c, dpWaitForExtraGuestsYouHaveDGuestSNow, "You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional "
            "guests."),
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

    sprintf(statusString, DATA_COMPGEN(0x004efc7c, dpWaitForHostWFHSD, "WFHS %d"), iDPWaitForHostStatus);
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
                DPSD(playResult, DATA_COMPGEN(0x004efc84, dpWaitForHostIProjectsHeroesProgSOURCEDpnetwin, RETAIL_FILE), hostSourceLineBase + 38);
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
            strcpy(sessionDescription.szSessionName, DATA_COMPGEN(0x004efcb0, dpWaitForHostHeroes2, "Heroes 2"));
            playResult = lpIDC->Open(&sessionDescription);
            if (playResult != RESULT_OK)
                DPSD(playResult, DATA_COMPGEN(0x004efcbc, dpWaitForHostIProjectsHeroesProgSOURCEDpnetwin2, RETAIL_FILE), hostSourceLineBase + 57);
            iDPWaitForHostStatus++;
            break;
        case HOST_CREATE_PLAYER:
            playResult = lpIDC->CreatePlayer(&dcoID, DATA_COMPGEN(0x004efcf8, dpWaitForHostDude, "Dude"), DATA_COMPGEN(0x004efce8, dpWaitForHostHeroesPlayer, "Heroes Player"), &dphEvent);
            if (playResult != RESULT_OK)
                DPSD(playResult, DATA_COMPGEN(0x004efd00, dpWaitForHostIProjectsHeroesProgSOURCEDpnetwin3, RETAIL_FILE), hostSourceLineBase + 67);
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
                    DPSD(playResult, DATA_COMPGEN(0x004efd2c, dpWaitForHostIProjectsHeroesProgSOURCEDpnetwin4, RETAIL_FILE), hostSourceLineBase + 93);
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
        case RESULT_OK:
            return;
        case DPERR_ALREADYINITIALIZED:
            strcpy(errorText, DATA_COMPGEN(0x004efd58, dPSDDPERRALREADYINITIALIZED, "DPERR_ALREADYINITIALIZED "));
            break;
        case DPERR_ACCESSDENIED:
            strcpy(errorText, DATA_COMPGEN(0x004efd74, dPSDDPERRACCESSDENIED, "DPERR_ACCESSDENIED       "));
            break;
        case DPERR_ACTIVEPLAYERS:
            strcpy(errorText, DATA_COMPGEN(0x004efd90, dPSDDPERRACTIVEPLAYERS, "DPERR_ACTIVEPLAYERS      "));
            break;
        case DPERR_BUFFERTOOSMALL:
            strcpy(errorText, DATA_COMPGEN(0x004efdac, dPSDDPERRBUFFERTOOSMALL, "DPERR_BUFFERTOOSMALL     "));
            break;
        case DPERR_CANTADDPLAYER:
            strcpy(errorText, DATA_COMPGEN(0x004efdc8, dPSDDPERRCANTADDPLAYER, "DPERR_CANTADDPLAYER      "));
            break;
        case DPERR_CANTCREATEGROUP:
            strcpy(errorText, DATA_COMPGEN(0x004efde4, dPSDDPERRCANTCREATEGROUP, "DPERR_CANTCREATEGROUP    "));
            break;
        case DPERR_CANTCREATEPLAYER:
            strcpy(errorText, DATA_COMPGEN(0x004efe00, dPSDDPERRCANTCREATEPLAYER, "DPERR_CANTCREATEPLAYER   "));
            break;
        case DPERR_CANTCREATESESSION:
            strcpy(errorText, DATA_COMPGEN(0x004efe1c, dPSDDPERRCANTCREATESESSION, "DPERR_CANTCREATESESSION  "));
            break;
        case DPERR_CAPSNOTAVAILABLEYET:
            strcpy(errorText, DATA_COMPGEN(0x004efe38, dPSDDPERRCAPSNOTAVAILABLEYET, "DPERR_CAPSNOTAVAILABLEYET"));
            break;
        case DPERR_EXCEPTION:
            strcpy(errorText, DATA_COMPGEN(0x004efe54, dPSDDPERREXCEPTION, "DPERR_EXCEPTION          "));
            break;
        case DPERR_GENERIC:
            strcpy(errorText, DATA_COMPGEN(0x004efe70, dPSDDPERRGENERIC, "DPERR_GENERIC            "));
            break;
        case DPERR_INVALIDFLAGS:
            strcpy(errorText, DATA_COMPGEN(0x004efe8c, dPSDDPERRINVALIDFLAGS, "DPERR_INVALIDFLAGS       "));
            break;
        case DPERR_INVALIDOBJECT:
            strcpy(errorText, DATA_COMPGEN(0x004efea8, dPSDDPERRINVALIDOBJECT, "DPERR_INVALIDOBJECT      "));
            break;
        case DPERR_INVALIDPARAMS:
            strcpy(errorText, DATA_COMPGEN(0x004efec4, dPSDDPERRINVALIDPARAMS, "DPERR_INVALIDPARAMS      "));
            break;
        case DPERR_INVALIDPLAYER:
            strcpy(errorText, DATA_COMPGEN(0x004efee0, dPSDDPERRINVALIDPLAYER, "DPERR_INVALIDPLAYER      "));
            break;
        case DPERR_NOCAPS:
            strcpy(errorText, DATA_COMPGEN(0x004efefc, dPSDDPERRNOCAPS, "DPERR_NOCAPS             "));
            break;
        case DPERR_NOCONNECTION:
            strcpy(errorText, DATA_COMPGEN(0x004eff18, dPSDDPERRNOCONNECTION, "DPERR_NOCONNECTION       "));
            break;
        case DPERR_OUTOFMEMORY:
            strcpy(errorText, DATA_COMPGEN(0x004eff34, dPSDDPERROUTOFMEMORY, "DPERR_OUTOFMEMORY        "));
            break;
        case DPERR_NOMESSAGES:
            strcpy(errorText, DATA_COMPGEN(0x004eff50, dPSDDPERRNOMESSAGES, "DPERR_NOMESSAGES         "));
            break;
        case DPERR_NONAMESERVERFOUND:
            strcpy(errorText, DATA_COMPGEN(0x004eff6c, dPSDDPERRNONAMESERVERFOUND, "DPERR_NONAMESERVERFOUND  "));
            break;
        case DPERR_NOPLAYERS:
            strcpy(errorText, DATA_COMPGEN(0x004eff88, dPSDDPERRNOPLAYERS, "DPERR_NOPLAYERS          "));
            break;
        case DPERR_NOSESSIONS:
            strcpy(errorText, DATA_COMPGEN(0x004effa4, dPSDDPERRNOSESSIONS, "DPERR_NOSESSIONS         "));
            break;
        case DPERR_SENDTOOBIG:
            strcpy(errorText, DATA_COMPGEN(0x004effc0, dPSDDPERRSENDTOOBIG, "DPERR_SENDTOOBIG         "));
            break;
        case DPERR_TIMEOUT:
            strcpy(errorText, DATA_COMPGEN(0x004effdc, dPSDDPERRTIMEOUT, "DPERR_TIMEOUT            "));
            break;
        case DPERR_UNAVAILABLE:
            strcpy(errorText, DATA_COMPGEN(0x004efff8, dPSDDPERRUNAVAILABLE, "DPERR_UNAVAILABLE        "));
            break;
        case DPERR_UNSUPPORTED:
            strcpy(errorText, DATA_COMPGEN(0x004f0014, dPSDDPERRUNSUPPORTED, "DPERR_UNSUPPORTED        "));
            break;
        case DPERR_BUSY:
            strcpy(errorText, DATA_COMPGEN(0x004f0030, dPSDDPERRBUSY, "DPERR_BUSY               "));
            break;
        case DPERR_USERCANCEL:
            strcpy(errorText, DATA_COMPGEN(0x004f004c, dPSDDPERRUSERCANCEL, "DPERR_USERCANCEL         "));
            break;
        default:
            strcpy(errorText, DATA_COMPGEN(0x004f0068, dPSDErrorTypeUnknown, "Error type unknown"));
            break;
    }
    MessageBeep(0);
    MessageBeep(0);
    MessageBeep(0);
    sprintf(gText, DATA_COMPGEN(0x004f007c, dPSDDirectPlayErrorSFileSLine, "DirectPlay Error:\n\n'%s'\n\n  File:'%s'\n Line# %d"), errorText, file, line);
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
DATA(0x004ef7f4)
H2_ENUM_STORAGE_STEPPED(DirectPlayFirstGuestState, i32)
iDPWaitForFirstGuestStatus = FIRST_GUEST_CREATE_SESSION;
DATA(0x004ef7f8)
H2_ENUM_STORAGE_STEPPED(DirectPlayHostState, i32) iDPWaitForHostStatus = HOST_ENUMERATE_SESSIONS;
DATA(0x004ef7fc) i32 iWaitForHostWaitCount = 0;
DATA(0x004ef800) i32 iEnumCount = 0;
DATA(0x004ef804) i32 iLastHereIAmTickCount = 0;
DATA(0x004ef808) i32 bInDPSD = 0;
DATA(0x004ef80c) i32 iGUIDCount = 0;
DATA(0x004ef810) i32 iLastMsgNumHumanPlayers = 1;
DATA(0x00525060) i32 iMaxSession;
DATA(0x00525064) DirectPlayHostAcceptStatus giHostAcceptStatus;
DATA(0x00525068) struct _GUID* g_lpGuid;
DATA(0x00525070) i32 giNetPosToDCOPos[DP_TRANSPORT_STARTUP_MAPPING_COUNT];
DATA(0x00525088) i32 iSessionToTry;
DATA(0x00525090) i32l lSessions[DP_TRANSPORT_SESSION_COUNT];

#undef RETAIL_FILE
