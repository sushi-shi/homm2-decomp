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
#include <_carcass_types.h>
#include <SOURCE/dpnetwin.h>

#define DPFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\dpnetwin.cpp")
#define DP_INIT_LINE_BASE (*reinterpret_cast<const short *>("_"))
#define DP_TERM_LINE_BASE (*reinterpret_cast<const short *>("\xdb"))
#define DP_SEND_LINE_BASE (*reinterpret_cast<const short *>("\xfe"))
#define DP_RCV_LINE_BASE (*reinterpret_cast<const short *>("\x1c\x01"))
#define DP_PROCESS_LINE_BASE (*reinterpret_cast<const short *>("\x38\x01"))
#define DP_EVALUATE_LINE_BASE (*reinterpret_cast<const short *>("\x63\x01"))
#define DP_FIRST_GUEST_LINE_BASE (*reinterpret_cast<const short *>("\xaa\x01"))
#define DP_HOST_LINE_BASE (*reinterpret_cast<const short *>("\xfe\x01"))

VA(0x0041eda0, 0x95)
int __stdcall dpEnumServiceProvider(struct _GUID *guid, char *name,
                                    unsigned long int, unsigned long int, void *) {
    LogStr("ServiceProvider:");
    _strupr(name);
    LogInt(name, reinterpret_cast<int>(guid), -999, -999, -999, -999, -999, -999);
    if (FindStringInString(name, "IPX") != 0)
        IPXGuid = guid;
    else if (FindStringInString(name, "TCP") != 0)
        TCPGuid = guid;
    return 1;
}

VA(0x0041ee35, 0x7a)
int __stdcall dpEnumSession(DPSESSIONDESC *session, void *,
                            unsigned long int *, unsigned long int flags) {
    if (flags & 1)
        return 0;
    LogStr("Sessions:");
    LogInt(reinterpret_cast<char *>(session) + 0x24,
           *reinterpret_cast<int *>(reinterpret_cast<char *>(session) + 0x14),
           -999, -999, -999, -999, -999, -999);
    lSessions[iMaxSession] =
        *reinterpret_cast<long *>(reinterpret_cast<char *>(session) + 0x14);
    iMaxSession++;
    return 1;
}

// @match-note 99.95%: semantics, CFG, the 0x30 frame/local slots, and all 71
// ordered relocations agree. The first raw residual at +0x8d is the delinked
// LoadLibraryA/import-call shape; later residuals are the corresponding
// GetProcAddress imports and local branch displacements. Direct Win32 calls and
// explicitly typed resolver pointers are already retained. Revisit after import
// thunk normalization or shared Win32 declaration recovery.
VA(0x0041eeaf, 0x311)
short int dpnet_init(void) {
    DirectPlayStartupMessage startup;
    typedef HRESULT (WINAPI *DirectPlayCreateFunction)(GUID *, IDirectPlay **, IUnknown *);
    typedef HRESULT (WINAPI *DirectPlayEnumerateFunction)(LPDPENUMDPCALLBACK, void *);
    DirectPlayCreateFunction createFunction;
    DirectPlayEnumerateFunction enumerateFunction;
    int player;
    int result;

    if (lpIDC != 0)
        return 0;
    {
        ppDPRcvBuffer = static_cast<unsigned char **>(
            BaseAlloc(DP_TRANSPORT_BUFFER_COUNT * sizeof(unsigned char *), DPFILE,
                      DP_INIT_LINE_BASE + 7));
        piDPRcvBufferSize = static_cast<int *>(
            BaseAlloc(DP_TRANSPORT_BUFFER_COUNT * sizeof(int), DPFILE,
                      DP_INIT_LINE_BASE + 8));
        memset(ppDPRcvBuffer, 0,
               DP_TRANSPORT_BUFFER_COUNT * sizeof(unsigned char *));
        memset(piDPRcvBufferSize, 0,
               DP_TRANSPORT_BUFFER_COUNT * sizeof(int));
        hinstDplayx = LoadLibraryA("DPLAYX.DLL");
        if (hinstDplayx == 0)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = 0;
        createFunction = 0;
        createFunction = reinterpret_cast<DirectPlayCreateFunction>(
            GetProcAddress(static_cast<HMODULE>(hinstDplayx), "DirectPlayCreate"));
        if (createFunction == 0)
            ShutDown("Can't load 'DPLAYX.DLL'");
        enumerateFunction = reinterpret_cast<DirectPlayEnumerateFunction>(
            GetProcAddress(static_cast<HMODULE>(hinstDplayx), "DirectPlayEnumerateA"));
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
            DPSD(result, DPFILE, DP_INIT_LINE_BASE + 0x29);

        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            gbRemoteGameOpen = 1;
            giWaitType = DP_WAIT_FIRST_GUEST;
            sprintf(gText, "Waiting On Guest. Press 'CANCEL' to quit.");
            NormalDialog(gText, 6, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(0);
            iLastMsgNumHumanPlayers = giNumHumanPlayers;
            giWaitType = DP_WAIT_EXTRA_GUESTS;
            sprintf(gText, "You have %d guest(s) now logged in.",
                    giNumHumanPlayers - 1);
            NormalDialog(gText, 5, -1, -1, -1, 0, -1, 0, -1, 0);
            gbRemoteGameOpen = 0;
            startup.playerCount = static_cast<unsigned char>(giNumHumanPlayers);
            memcpy(startup.playerIds, giNetPosToDCOPos,
                   sizeof(giNetPosToDCOPos));
            for (player = 1; player < giNumHumanPlayers; player++) {
                startup.netPosition = static_cast<unsigned char>(player);
                dpSendMessage(giNetPosToDCOPos[player], DP_MESSAGE_STARTUP,
                              sizeof(startup), &startup);
            }
        } else {
            giWaitType = DP_WAIT_HOST;
            sprintf(gText, "Waiting for other remote player to log in.");
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

// @match-note 99.99%: semantics, CFG, the 0x68 drain-buffer frame, and all 27
// ordered relocations agree. The first non-relocation byte residual is the local
// branch at +0x65; the only structural disassembly residual is FreeLibrary's
// imported-call representation near +0xf9. Direct and casted HMODULE spellings
// are exhausted. Revisit with the import-thunk normalization work.
VA(0x0041f28e, 0x116)
void dpnet_term(void) {
    char drainBuffer[DP_TRANSPORT_TERM_DRAIN_SIZE];

    gbRemoteOn = 0;
    if (dcoID != 0)
        lpIDC->DestroyPlayer(dcoID);
    lpIDC->Close();
    lpIDC->Release();
    lpIDC = 0;
    while (dpnet_rcv(0, DP_TRANSPORT_TERM_DRAIN_SIZE, drainBuffer) != 0) {
    }
    if (ppDPRcvBuffer != 0)
        BaseFree(ppDPRcvBuffer, DPFILE, DP_TERM_LINE_BASE + 0xe);
    ppDPRcvBuffer = 0;
    if (piDPRcvBufferSize != 0)
        BaseFree(piDPRcvBufferSize, DPFILE, DP_TERM_LINE_BASE + 0x12);
    piDPRcvBufferSize = 0;
    if (hinstDplayx != 0)
        FreeLibrary(static_cast<HMODULE>(hinstDplayx));
    hinstDplayx = 0;
    CleanupDPVars();
}

VA(0x0041f3a4, 0xee)
void dpSendMessage(int destination, unsigned char type, unsigned short int size,
                   void *data) {
    unsigned char *message = static_cast<unsigned char *>(
        BaseAlloc(size + 1, DPFILE, DP_SEND_LINE_BASE + 2));
    int result;

    message[0] = type;
    if (size != 0)
        memcpy(message + 1, data, size);
    result = lpIDC->Send(dcoID, destination, 0, message, size + 1);
    if (result != DP_RESULT_OK && result != DP_RESULT_INVALID_PLAYER &&
        result != DP_RESULT_INVALID_ARGUMENT) {
        DPSD(result, DPFILE, DP_SEND_LINE_BASE + 0xe);
    }
    BaseFree(message, DPFILE, DP_SEND_LINE_BASE + 0x10);
}

VA(0x0041f492, 0x5a)
int dpnet_snd(int position, int size, void *data) {
    int destination;

    dpProcessMessages();
    if (position == DP_TRANSPORT_BROADCAST_POSITION)
        destination = 0;
    else
        destination = giNetPosToDCOPos[position];
    dpSendMessage(destination, DP_MESSAGE_DATA, static_cast<unsigned short>(size), data);
    return 0;
}

VA(0x0041f4ec, 0xa7)
short int dpnet_rcv(short int, unsigned short int, void *data) {
    unsigned int size;

    dpProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    BaseFree(ppDPRcvBuffer[iDPRcvBufferTail], DPFILE, DP_RCV_LINE_BASE + 9);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % DP_TRANSPORT_BUFFER_COUNT;
    return static_cast<short>(size);
}

VA(0x0041f593, 0x1d)
unsigned char dpnet_stat(short int, unsigned short int) { return 0; }

VA(0x0041f5b0, 0x13)
short int __cdecl dpnet_sess(int, int, ...) { return 0; }

// @early-stop 99.91%: frame 0x18 and senderId/packetSize/destinationIds/
// receiveResult slots match, all 9 ordered relocations agree, and every
// non-jump opcode and operand matches. Base is exactly five bytes larger because
// the no-messages branch targets one extra continuation jump at +0xb4; retail
// skips that jump and targets the common epilogue directly.
VA(0x0041f5c3, 0xbe)
void dpProcessMessages(void) {
    unsigned long packetSize[2];
    int destinationIds[2];
    int senderId;
    int receiveResult;

    if (lpIDC == 0)
        return;
    while (1) {
        packetSize[0] = DP_TRANSPORT_RECEIVE_SIZE;
        receiveResult = lpIDC->Receive(reinterpret_cast<unsigned long *>(&senderId),
                                       reinterpret_cast<unsigned long *>(destinationIds),
                                       1, rcvBufIn, packetSize);
        if (receiveResult == DP_RESULT_NO_MESSAGES)
            break;
        if (receiveResult != DP_RESULT_OK)
            DPSD(receiveResult, DPFILE, DP_PROCESS_LINE_BASE + 0x17);
        if (senderId == 0) {
        } else {
            if (destinationIds[0] == 0 ||
                destinationIds[0] == 0[&dcoID])
                dpEvaluateMessage(packetSize[0], senderId);
        }
    }
}

// @early-stop 99.81%: all 0x274 relocation-masked bytes and all 52 ordered
// relocations match. The retained objdiff residual is solely the delinked switch
// jump-table/local-label identity; case-body order and external targets agree.
VA(0x0041f681, 0x274)
void dpEvaluateMessage(unsigned long int size, int sender) {
    char *ptr = rcvBufIn + 1;
    int i;

    switch (rcvBufIn[0]) {
    case DP_MESSAGE_DATA:
        ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<unsigned char *>(
            BaseAlloc(size - 1, DPFILE, DP_EVALUATE_LINE_BASE + 8));
        memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
        piDPRcvBufferSize[iDPRcvBufferHead] = size;
        iDPRcvBufferHead = (iDPRcvBufferHead + 1) % DP_TRANSPORT_BUFFER_COUNT;
        break;
    case DP_MESSAGE_GUEST_ARRIVED:
        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            for (i = 1; i < giNumHumanPlayers; i++) {
                if (giNetPosToDCOPos[i] == sender) {
                    dpSendMessage(sender, DP_MESSAGE_GUEST_ACCEPTED, 0, 0);
                    return;
                }
            }
            if (gbRemoteGameOpen != 0) {
                giNetPosToDCOPos[giNumHumanPlayers] = sender;
                gsNetPlayerInfo[giNumHumanPlayers] =
                    *reinterpret_cast<SNetPlayerInfo *>(ptr);
                if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                    xNetHasOldPlayers = 1;
                dpSendMessage(sender, DP_MESSAGE_GUEST_ACCEPTED, 0, 0);
                giNumHumanPlayers++;
            } else {
                dpSendMessage(sender, DP_MESSAGE_GUEST_REJECTED, 0, 0);
            }
        }
        break;
    case DP_MESSAGE_GUEST_ACCEPTED:
        giHostAcceptStatus = 1;
        break;
    case DP_MESSAGE_GUEST_REJECTED:
        giHostAcceptStatus = 2;
        break;
    case DP_MESSAGE_STARTUP:
        giNumHumanPlayers = ptr[0];
        giThisNetPos = ptr[1];
        LogInt("DPMSGSTARTUP", giThisNetPos, sender, -999, -999, -999, -999, -999);
        memcpy(giNetPosToDCOPos, ptr + 2, sizeof(giNetPosToDCOPos));
        bStartUpInfoReceived = 1;
        break;
    default:
        sprintf(gText, "Unknown message: %d", static_cast<int>(rcvBufIn[0]));
        LogStr(gText);
        break;
    }
}

// @early-stop 98.79%: frame 0x84 and all session/result/compiler slots match.
// Excluding the retail jump table at +0x166/0x10, every non-jump opcode and
// operand matches and all 33 ordered relocations agree. Base is exactly five
// bytes larger: one continuation jump after case 3 shifts the dispatch/table and
// epilogue by five bytes; the two target literal aliases resolve to the proven
// gsThisNetPlayerInfo +26/+27 fields.
VA(0x0041f8f5, 0x182)
int dpWaitForFirstGuest(void) {
    DPSESSIONDESC session;
    int result;

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
            DPSD(result, DPFILE, DP_FIRST_GUEST_LINE_BASE + 0x10);
        iDPWaitForFirstGuestStatus++;
        break;
    case 1:
        gsThisNetPlayerInfo.useRegularCompression = 0;
        gsThisNetPlayerInfo.useDiffCompression = 0;
        iDPWaitForFirstGuestStatus++;
        break;
    case 2:
        result = lpIDC->CreatePlayer(&dcoID, "Heroes Player", "Dude",
                                     reinterpret_cast<HANDLE *>(&dphEvent));
        if (result != DP_RESULT_OK)
            DPSD(result, DPFILE, DP_FIRST_GUEST_LINE_BASE + 0x2e);
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
int dpWaitForExtraGuests(void) {
    tag_message message;

    dpProcessMessages();
    if (giNumHumanPlayers != iLastMsgNumHumanPlayers) {
        iLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(gText, "You have %d guest(s) now logged in.",
                giNumHumanPlayers - 1);
        message.type = 0x200;
        message.payload.widget.command = 3;
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
int dpWaitForHost(void) {
    unsigned long enumerationTimeout;
    DPSESSIONDESC sessionDescription;
    int playResult;
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
        if (iEnumCount % DP_TRANSPORT_ENUM_LONG_INTERVAL !=
            DP_TRANSPORT_ENUM_LONG_PHASE)
            enumerationTimeout = DP_TRANSPORT_ENUM_SHORT_TIMEOUT;
        else
            enumerationTimeout = DP_TRANSPORT_ENUM_LONG_TIMEOUT;
        playResult = lpIDC->EnumSessions(&sessionDescription, enumerationTimeout,
                                         dpEnumSession, 0, 0);
        iEnumCount++;
        if (playResult == DP_RESULT_NO_SESSIONS) {
            iWaitForHostWaitCount = DP_TRANSPORT_RETRY_WAIT_COUNT;
            return 0;
        }
        if (playResult != DP_RESULT_OK)
            DPSD(playResult, DPFILE, DP_HOST_LINE_BASE + 0x26);
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
            DPSD(playResult, DPFILE, DP_HOST_LINE_BASE + 0x39);
        iDPWaitForHostStatus++;
        break;
    case 2:
        playResult = lpIDC->CreatePlayer(&dcoID, "Heroes Player", "Dude",
                                         reinterpret_cast<HANDLE *>(&dphEvent));
        if (playResult != DP_RESULT_OK)
            DPSD(playResult, DPFILE, DP_HOST_LINE_BASE + 0x43);
        iDPWaitForHostStatus++;
        break;
    case 3:
        iLastHereIAmTickCount = KBTickCount();
        giHostAcceptStatus = 0;
        dpSendMessage(0, DP_MESSAGE_GUEST_ARRIVED, sizeof(SNetPlayerInfo),
                      &gsThisNetPlayerInfo);
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
                DPSD(playResult, DPFILE, DP_HOST_LINE_BASE + 0x5d);
        } else if (iLastHereIAmTickCount + DP_TRANSPORT_ACCEPT_TIMEOUT <
                   KBTickCount()) {
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
void DPSD(int result, char *file, int line) {
    char errorText[200];
    int errorFlag;

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
    sprintf(gText, "DirectPlay Error:  %s File: %s Line: %d",
            errorText, file, line);
    LogStr(gText);
    ShutDown(gText);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ef7c8) struct IDirectPlay *lpIDC = 0;
DATA(0x004ef7cc) unsigned long dcoID = 0;
DATA(0x004ef7d0) struct _GUID *IPXGuid = 0;
DATA(0x004ef7d4) struct _GUID *TCPGuid = 0;
DATA(0x004ef7d8) void *dphEvent = 0;
DATA(0x004ef7dc) int iDPRcvBufferHead = 0;
DATA(0x004ef7e0) int iDPRcvBufferTail = 0;
DATA(0x004ef7e4) unsigned char **ppDPRcvBuffer = 0;
DATA(0x004ef7e8) int *piDPRcvBufferSize = 0;
DATA(0x004ef7ec) int bStartUpInfoReceived = 0;
DATA(0x004ef7f0) void *hinstDplayx = 0;
DATA(0x004ef7f4) int iDPWaitForFirstGuestStatus = 0;
DATA(0x004ef7f8) int iDPWaitForHostStatus = 0;
DATA(0x004ef7fc) int iWaitForHostWaitCount = 0;
DATA(0x004ef800) int iEnumCount = 0;
DATA(0x004ef804) int iLastHereIAmTickCount = 0;
DATA(0x004ef808) int bInDPSD = 0;
DATA(0x004ef80c) int iGUIDCount = 0;
DATA(0x004ef810) int iLastMsgNumHumanPlayers = 1;
DATA(0x00525060) int iMaxSession;
DATA(0x00525064) int giHostAcceptStatus;
DATA(0x00525068) struct _GUID *g_lpGuid;
DATA(0x00525070) int giNetPosToDCOPos[6];
DATA(0x00525088) int iSessionToTry;
DATA(0x00525090) long lSessions[10];
