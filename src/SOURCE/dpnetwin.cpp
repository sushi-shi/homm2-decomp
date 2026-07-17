// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\dpnetwin.obj   from: (directly linked into exe)
// functions: 16   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\dpnetwin.cpp")

VA(0x0041eda0, 0x95)
BOOL WINAPI dpEnumServiceProvider(struct _GUID* guid, char* name, DWORD, DWORD, void*) {
    LogStr("ServiceProvider:");
    _strupr(name);
    LogInt(name, reinterpret_cast<i32>(guid), -999, -999, -999, -999, -999, -999);
    if (FindStringInString(name, "IPX") != 0)
        IPXGuid = guid;
    else if (FindStringInString(name, "TCP") != 0)
        TCPGuid = guid;
    return 1;
}

VA(0x0041ee35, 0x7a)
BOOL WINAPI dpEnumSession(DPSESSIONDESC* session, void*, LPDWORD, DWORD flags) {
    if (flags & 1)
        return 0;
    LogStr("Sessions:");
    LogInt(session->szSessionName, session->dwSession, -999, -999, -999, -999, -999, -999);
    lSessions[iMaxSession] = session->dwSession;
    iMaxSession++;
    return 1;
}

// @semantic: Current dpnetwin.cpp/header epoch: semantics, CFG, the 0x30 frame,
// and all 71 semantic ordered relocations agree. Renaming player to guestIndex
// recovers retail's -0x8 slot and improves 99.02577% to 99.04124%. The remaining
// loop residual loads giNumHumanPlayers before guestIndex and emits JGE, while
// retail reverses the loads and emits JLE. Ten bounded attempts exhausted seven
// identifier_rename spellings, relational reversal, their rejected overlapping
// combination, and OD_STEER(guestIndex). Earlier residuals are delinked LoadLibraryA/
// GetProcAddress import-call shapes. Revisit only after relevant import-thunk,
// declaration, source/TU/header, or comparison-epoch changes.
VA(0x0041eeaf, 0x311)
i16 dpnet_init(void) {
    DATA(0x004ef83c) static i16 initSourceLineBase = DP_SOURCE_LINE_INIT_BASE;
    DirectPlayStartupMessage startup;
    typedef HRESULT(WINAPI * DirectPlayCreateFunction)(GUID*, IDirectPlay**, IUnknown*);
    typedef HRESULT(WINAPI * DirectPlayEnumerateFunction)(LPDPENUMDPCALLBACK, void*);
    DirectPlayCreateFunction createFunction;
    DirectPlayEnumerateFunction enumerateFunction;
    i32 guestIndex;
    i32 result;

    if (lpIDC != 0)
        return 0;
    {
        ppDPRcvBuffer = static_cast<u8**>(BaseAlloc(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*),
            RETAIL_FILE,
            initSourceLineBase + DP_SOURCE_LINE_INIT_BUFFER_ALLOC_OFFSET
        ));
        piDPRcvBufferSize = static_cast<i32*>(BaseAlloc(
            DP_TRANSPORT_BUFFER_COUNT * sizeof(i32),
            RETAIL_FILE,
            initSourceLineBase + DP_SOURCE_LINE_INIT_SIZE_ALLOC_OFFSET
        ));
        memset(ppDPRcvBuffer, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
        memset(piDPRcvBufferSize, 0, DP_TRANSPORT_BUFFER_COUNT * sizeof(i32));
        hinstDplayx = LoadLibraryA("DPLAYX.DLL");
        if (hinstDplayx == 0)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = 0;
        createFunction = 0;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayCreate")
        );
        if (createFunction == 0)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(hinstDplayx, "DirectPlayEnumerateA")
        );
        if (enumerateFunction == 0)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction(dpEnumServiceProvider, 0);
        switch (iMPNetProtocol) {
            case DP_PROTOCOL_IPX:
                g_lpGuid = IPXGuid;
                break;
            case DP_PROTOCOL_TCP:
                g_lpGuid = TCPGuid;
                break;
        }
        result = createFunction(g_lpGuid, &lpIDC, 0);
        if (result != DP_RESULT_OK)
            DPSD(result, RETAIL_FILE, initSourceLineBase + DP_SOURCE_LINE_INIT_CREATE_OFFSET);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = 1;
            giWaitType = DP_WAIT_FIRST_GUEST;
            sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, 6, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(0);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = DP_WAIT_EXTRA_GUESTS;
            sprintf(
                gText,
                "You have %d guest(s) now logged in.  Click 'OK' to move on, or wait for "
                "additional guests.",
                giNumHumanPlayers - 1
            );
            NormalDialog(gText, 5, -1, -1, -1, 0, -1, 0, -1, 0);
            gbRemoteGameOpen = 0;
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
            giWaitType = DP_WAIT_HOST;
            sprintf(gText, "Waiting for other remote player to set up game.");
            NormalDialog(gText, 6, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(0);
        }
    }
    return 0;
}

VA(0x0041f1c0, 0xce)
void CleanupDPVars(void) {
    lpIDC = 0;
    dcoID = 0;
    IPXGuid = 0;
    TCPGuid = 0;
    dphEvent = 0;
    iDPRcvBufferHead = 0;
    iDPRcvBufferTail = 0;
    ppDPRcvBuffer = 0;
    piDPRcvBufferSize = 0;
    bStartUpInfoReceived = 0;
    hinstDplayx = 0;
    iDPWaitForFirstGuestStatus = 0;
    iDPWaitForHostStatus = 0;
    iWaitForHostWaitCount = 0;
    iEnumCount = 0;
    iLastHereIAmTickCount = 0;
    bInDPSD = 0;
    iGUIDCount = 0;
    iLastMsgNumHumanPlayers = 1;
}

VA(0x0041f28e, 0x116)
void dpnet_term(void) {
    DATA(0x004efa00) static i16 termSourceLineBase = DP_SOURCE_LINE_TERM_BASE;
    char drainBuffer[DP_TRANSPORT_TERM_DRAIN_READ_SIZE + sizeof(i32)];

    gbRemoteOn = 0;
    if (dcoID != 0)
        lpIDC->DestroyPlayer(dcoID);
    lpIDC->Close();
    lpIDC->Release();
    lpIDC = 0;
    while (dpnet_rcv(0, DP_TRANSPORT_TERM_DRAIN_READ_SIZE, drainBuffer) != 0) {
    }
    if (ppDPRcvBuffer != 0)
        BaseFree(
            ppDPRcvBuffer,
            RETAIL_FILE,
            termSourceLineBase + DP_SOURCE_LINE_TERM_BUFFER_FREE_OFFSET
        );
    ppDPRcvBuffer = 0;
    if (piDPRcvBufferSize != 0)
        BaseFree(
            piDPRcvBufferSize,
            RETAIL_FILE,
            termSourceLineBase + DP_SOURCE_LINE_TERM_SIZE_FREE_OFFSET
        );
    piDPRcvBufferSize = 0;
    if (hinstDplayx != 0)
        FreeLibrary(hinstDplayx);
    hinstDplayx = 0;
    CleanupDPVars();
}

VA(0x0041f3a4, 0xee)
void dpSendMessage(i32 destination, u8 type, u16 size, void* data) {
    DATA(0x004efa5c) static i16 sendSourceLineBase = DP_SOURCE_LINE_SEND_BASE;
    u8* message = static_cast<u8*>(
        BaseAlloc(size + 1, RETAIL_FILE, sendSourceLineBase + DP_SOURCE_LINE_SEND_ALLOC_OFFSET)
    );
    i32 result;

    message[0] = type;
    if (size != 0)
        memcpy(message + 1, data, size);
    result = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (result != DP_RESULT_OK && result != DP_RESULT_INVALID_PLAYER
        && result != DP_RESULT_INVALID_ARGUMENT) {
        DPSD(result, RETAIL_FILE, sendSourceLineBase + DP_SOURCE_LINE_SEND_ERROR_OFFSET);
    }
    BaseFree(message, RETAIL_FILE, sendSourceLineBase + DP_SOURCE_LINE_SEND_FREE_OFFSET);
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
    DATA(0x004efae4) static i16 receiveSourceLineBase = DP_SOURCE_LINE_RECEIVE_BASE;
    u32 size;

    dpProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    BaseFree(
        ppDPRcvBuffer[iDPRcvBufferTail],
        RETAIL_FILE,
        receiveSourceLineBase + DP_SOURCE_LINE_RECEIVE_FREE_OFFSET
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
    DATA(0x004efb14) static i16 processSourceLineBase = DP_SOURCE_LINE_PROCESS_BASE;
    DWORD packetSize[2];
    i32 destinationIds[2];
    i32 senderId;
    i32 receiveResult;

    if (lpIDC == 0)
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
        if (receiveResult == DP_RESULT_NO_MESSAGES)
            return;
        if (receiveResult != DP_RESULT_OK)
            DPSD(
                receiveResult,
                RETAIL_FILE,
                processSourceLineBase + DP_SOURCE_LINE_PROCESS_ERROR_OFFSET
            );
        if (senderId == 0) {
        } else {
            if (destinationIds[0] == 0 || destinationIds[0] == OD_STEER(dcoID))
                dpEvaluateMessage(packetSize[0], senderId);
        }
    }
}

// @early-stop
// Current dpnetwin.cpp/header epoch: the base symbol is 0x273 bytes versus the
// provisional 0x274 retail span, so the relocation-only hard gate correctly
// refuses to overlap the following function. Across the complete base range,
// instruction semantics, case-body order, and all 52 ordered relocation targets
// agree; the residual is the delinked switch jump-table/local-label identity and
// one-byte boundary artifact. Revisit only after function-boundary, source/TU/
// header, or comparison-epoch changes.
VA(0x0041f681, 0x274)
void dpEvaluateMessage(u32l size, i32 sender) {
    DATA(0x004efb44) static i16 evaluateSourceLineBase = DP_SOURCE_LINE_EVALUATE_BASE;
    char* ptr = rcvBufIn + 1;
    i32 i;

    switch (rcvBufIn[0]) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(BaseAlloc(
                size - 1,
                RETAIL_FILE,
                evaluateSourceLineBase + DP_SOURCE_LINE_EVALUATE_ALLOC_OFFSET
            ));
            memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
            piDPRcvBufferSize[iDPRcvBufferHead] = size;
            iDPRcvBufferHead = (iDPRcvBufferHead + 1) % DP_TRANSPORT_BUFFER_COUNT;
            break;
        case NETWORK_PACKET_GUEST_ARRIVED:
            if (GameMode == REMOTE_GAME_NETWORK_HOST) {
                for (i = 1; i < giNumHumanPlayers; i++) {
                    if (giNetPosToDCOPos[i] == sender) {
                        dpSendMessage(sender, NETWORK_PACKET_GUEST_ACCEPTED, 0, 0);
                        return;
                    }
                }
                if (gbRemoteGameOpen != 0) {
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    gsNetPlayerInfo[giNumHumanPlayers] = *reinterpret_cast<SNetPlayerInfo*>(ptr);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = 1;
                    dpSendMessage(sender, NETWORK_PACKET_GUEST_ACCEPTED, 0, 0);
                    giNumHumanPlayers++;
                } else {
                    dpSendMessage(sender, NETWORK_PACKET_GUEST_REJECTED, 0, 0);
                }
            }
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            giHostAcceptStatus = 1;
            break;
        case NETWORK_PACKET_GUEST_REJECTED:
            giHostAcceptStatus = 2;
            break;
        case NETWORK_PACKET_STARTUP:
            giNumHumanPlayers = ptr[0];
            giThisNetPos = ptr[1];
            LogInt("DPMSGSTARTUP", giThisNetPos, sender, -999, -999, -999, -999, -999);
            memcpy(giNetPosToDCOPos, ptr + 2, sizeof(giNetPosToDCOPos));
            bStartUpInfoReceived = 1;
            break;
        default:
            sprintf(gText, "Unknown message: %d\n", static_cast<i32>(rcvBufIn[0]));
            LogStr(gText);
            break;
    }
}

// @semantic: Current dpnetwin.cpp/header epoch: the 0x84 frame and all session,
// result, and compiler slots match. Excluding the retail jump table at +0x166/0x10, every
// non-jump opcode and operand and all 33 ordered relocations agree. One five-byte
// continuation after case 3 shifts the dispatch/table and epilogue; the two
// target literal aliases are proven gsThisNetPlayerInfo +26/+27 fields. Revisit
// only after the source/TU/header or comparison epoch changes.
VA(0x0041f8f5, 0x182)
i32 dpWaitForFirstGuest(void) {
    DATA(0x004efb9c) static i16 firstGuestSourceLineBase = DP_SOURCE_LINE_FIRST_GUEST_BASE;
    DPSESSIONDESC session;
    i32 result;

    switch (iDPWaitForFirstGuestStatus) {
        case 0:
            memset(&session, 0, sizeof(session));
            session.dwSize = sizeof(session);
            session.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            session.guidSession = *g_lpGuid;
            session.dwFlags = DP_SESSION_OPEN_CREATE;
            strcpy(session.szSessionName, "Heroes 2");
            result = lpIDC->Open(&session);
            if (result != DP_RESULT_OK)
                DPSD(
                    result,
                    RETAIL_FILE,
                    firstGuestSourceLineBase + DP_SOURCE_LINE_FIRST_GUEST_OPEN_OFFSET
                );
            iDPWaitForFirstGuestStatus++;
            break;
        case 1:
            gsThisNetPlayerInfo.useRegularCompression = 0;
            gsThisNetPlayerInfo.useDiffCompression = 0;
            iDPWaitForFirstGuestStatus++;
            break;
        case 2:
            result = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (result != DP_RESULT_OK)
                DPSD(
                    result,
                    RETAIL_FILE,
                    firstGuestSourceLineBase + DP_SOURCE_LINE_FIRST_GUEST_CREATE_OFFSET
                );
            giNetPosToDCOPos[0] = dcoID;
            iDPWaitForFirstGuestStatus++;
            break;
        case 3:
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

// @early-stop 99.85%: all 0x3d2 relocation-masked bytes, the 0xa8 frame and
// status/result/session/timeout slots, and all 78 ordered relocations match.
// The objdiff residual is only delinked switch-table/local-label identity.
VA(0x0041fafb, 0x3d2)
i32 dpWaitForHost(void) {
    DATA(0x004efc78) static i16 hostSourceLineBase = DP_SOURCE_LINE_HOST_BASE;
    DWORD enumerationTimeout;
    DPSESSIONDESC sessionDescription;
    i32 playResult;
    char statusString[32];

    sprintf(statusString, "WFHS %d", iDPWaitForHostStatus);
    AiPrint(statusString);
    switch (iDPWaitForHostStatus) {
        case 0:
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
                lpIDC->EnumSessions(&sessionDescription, enumerationTimeout, dpEnumSession, 0, 0);
            iEnumCount++;
            if (playResult == DP_RESULT_NO_SESSIONS) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                return 0;
            }
            if (playResult != DP_RESULT_OK)
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + DP_SOURCE_LINE_HOST_ENUM_OFFSET);
            if (iMaxSession > 0) {
                iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
                iDPWaitForHostStatus++;
            }
            break;
        case 1:
            memset(&sessionDescription, 0, sizeof(sessionDescription));
            sessionDescription.dwSize = sizeof(sessionDescription);
            sessionDescription.dwMaxPlayers = DP_TRANSPORT_MAX_PLAYERS;
            sessionDescription.guidSession = *g_lpGuid;
            sessionDescription.dwFlags = DP_SESSION_OPEN_JOIN;
            sessionDescription.dwSession = lSessions[iSessionToTry];
            strcpy(sessionDescription.szSessionName, "Heroes 2");
            playResult = lpIDC->Open(&sessionDescription);
            if (playResult != DP_RESULT_OK)
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + DP_SOURCE_LINE_HOST_OPEN_OFFSET);
            iDPWaitForHostStatus++;
            break;
        case 2:
            playResult = lpIDC->CreatePlayer(&dcoID, "Dude", "Heroes Player", &dphEvent);
            if (playResult != DP_RESULT_OK)
                DPSD(playResult, RETAIL_FILE, hostSourceLineBase + DP_SOURCE_LINE_HOST_CREATE_OFFSET);
            iDPWaitForHostStatus++;
            break;
        case 3:
            iLastHereIAmTickCount = KBTickCount();
            giHostAcceptStatus = 0;
            dpSendMessage(
                0,
                NETWORK_PACKET_GUEST_ARRIVED,
                sizeof(SNetPlayerInfo),
                &gsThisNetPlayerInfo
            );
            iDPWaitForHostStatus++;
            break;
        case 4:
            dpProcessMessages();
            if (giHostAcceptStatus == 1) {
                iDPWaitForHostStatus++;
            } else if (giHostAcceptStatus == 2) {
                if (iSessionToTry + 1 >= iMaxSession) {
                    iDPWaitForHostStatus = 0;
                } else {
                    iDPWaitForHostStatus = 1;
                    iSessionToTry++;
                }
                playResult = lpIDC->Close();
                if (playResult != DP_RESULT_OK)
                    DPSD(playResult, RETAIL_FILE, hostSourceLineBase + DP_SOURCE_LINE_HOST_CLOSE_OFFSET);
            } else if (iLastHereIAmTickCount + DP_TRANSPORT_ACCEPT_TIMEOUT < KBTickCount()) {
                iDPWaitForHostStatus--;
            }
            break;
        case 5:
            dpProcessMessages();
            if (bStartUpInfoReceived != 0)
                return 1;
            break;
    }
    return 0;
}

VA(0x0041fecd, 0x5eb)
void DPSD(i32 result, char* file, i32 line) {
    char errorText[200];
    i32 errorFlag;

    if (bInDPSD != 0)
        return;
    bInDPSD = 1;
    errorFlag = 0;
    switch (result) {
        case DP_RESULT_OK:
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

// @data-layout-note
// Fresh VC 4.2 storage contains 103 real definitions: 97 initialized owners
// in one 0x8e3-byte .data section and six loader-zero owners in one 0x54-byte
// .bss section. Retail contributes 0x8e8 bytes at 0xef7c8..0xf00b0 and 0x58
// bytes at 0x125060..0x1250b8; this TU owns no .rdata.
//
// All 97 initialized logical payloads are byte-exact. The eight function-local
// signed-short source-line owners are proved at 0xef83c, 0xefa00, 0xefa5c,
// 0xefae4, 0xefb14, 0xefb44, 0xefb9c, and 0xefc78. Each leaves two zero
// alignment bytes, and the final 0x2f-byte DPSD format leaves five terminal
// zero bytes. Those 21 bytes exactly explain the candidate/retail extent gap.
// The .data payload SHA-256 is
// fd3770c478657698e62fb1e6a8c5f3cf94af7d5b764ce5e26237b7e2d1bf74b6.
//
// All 238 code references to dpnetwin-owned storage resolve to the exact owner
// with addend zero: 227 pair at the same function-relative DIR32/HIGHLOW site,
// and 11 pair in complete equal-count ordered streams after validation against
// the surrounding source-DATA anchors. Retail BSS has only two four-byte zero
// alignment gaps at 0x12506c and 0x12508c; all six logical owners and all BSS
// reference targets are exact.
//
// Retail payloads also prove the multiline host/guest dialog text, the newline
// in the unknown-message log, the Dude/Heroes Player argument order, and the
// multiline DPSD format. Do not restore raw string-dereference line constants,
// duplicate owners, aliases, padding, cursor replay, or synthetic identities.
// ---- globals (definitions, RVA order) ----
DATA(0x004ef7c8) struct IDirectPlay* lpIDC = 0;
DATA(0x004ef7cc) DPID dcoID = 0;
DATA(0x004ef7d0) struct _GUID* IPXGuid = 0;
DATA(0x004ef7d4) struct _GUID* TCPGuid = 0;
DATA(0x004ef7d8) HANDLE dphEvent = 0;
DATA(0x004ef7dc) i32 iDPRcvBufferHead = 0;
DATA(0x004ef7e0) i32 iDPRcvBufferTail = 0;
DATA(0x004ef7e4) u8** ppDPRcvBuffer = 0;
DATA(0x004ef7e8) i32* piDPRcvBufferSize = 0;
DATA(0x004ef7ec) i32 bStartUpInfoReceived = 0;
DATA(0x004ef7f0) HMODULE hinstDplayx = 0;
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
DATA(0x00525070) i32 giNetPosToDCOPos[6];
DATA(0x00525088) i32 iSessionToTry;
DATA(0x00525090) i32l lSessions[10];

#undef RETAIL_FILE
