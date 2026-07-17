// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Wsnetwin.obj   from: (directly linked into exe)
// functions: 10   data: 14
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/dpnetwin.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <BASE/message.h>
#include <SOURCE/Wsnetwin.h>

#define WSFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Wsnetwin.cpp")
#define WS_SOURCE_LINE_INIT_BASE 61
#define WS_SOURCE_LINE_TERM_BASE 248
#define WS_SOURCE_LINE_SEND_BASE 279
#define WS_SOURCE_LINE_RECEIVE_BASE 359
#define WS_SOURCE_LINE_EVALUATE_BASE 413

DATA(0x004ed2c4) static short s_wsInitSourceLineBase = WS_SOURCE_LINE_INIT_BASE;
DATA(0x004ed730) static short s_wsTermSourceLineBase = WS_SOURCE_LINE_TERM_BASE;
DATA(0x004ed78c) static short s_wsSendSourceLineBase = WS_SOURCE_LINE_SEND_BASE;
DATA(0x004ed830) static short s_wsReceiveSourceLineBase = WS_SOURCE_LINE_RECEIVE_BASE;
DATA(0x004ed860) static short s_wsEvaluateSourceLineBase = WS_SOURCE_LINE_EVALUATE_BASE;

// @data-layout-note Retail initialized storage is 0xed2ac+0x73c. Candidate
// .data is 0x737 across 11 source DATA definitions and 32 private literals;
// every payload matches retail after mapping the candidate permutation. All 67
// initialized-data references pair at the same function-relative sites with
// addend zero. Five source-line shorts leave ten alignment bytes, and the final
// literal leaves five terminal zero bytes; neither is a separate owner.
// Retail loader-zero storage is 0x122f88+0x2d0 across eight public definitions.
// All 72 candidate BSS references pair: saddr_loc uses addends 0/2/4 and every
// other reference uses addend zero. iAddrLen and saddr_remote have no code
// references, but their public RVAs and exact candidate identities/types prove
// ownership. The 18 uncovered BSS bytes are alignment only; no .rdata is owned.

// @semantic
// Complete 0x128 frame/slots and 164/164 relocation targets. First raw residual is the
// +0x4b3 local branch byte; the first CFG divergence is the final invalid-address test
// (ours JE+JMP, retail JNE). Tried direct/commuted/SIB loop tests and if/else/continue/break arms;
// revisit after later Wsnetwin source/header changes alter TU-cumulative block selection.
VA(0x004068b0, 0x5b5)
short int wsnet_init(void) {
    WinsockStartupMessage startup;
    struct hostent *pHost;
    unsigned long socketMode;
    char localHostName[WS_TRANSPORT_BUFFER_SIZE];
    int player;

    if (gConfig.gfx[giCurExe].fullScreen != 0) {
        sprintf(gText, "About to initiate TCP/IP connection.  Heroes II will now drop from full screen mode to windowed mode, so that any Windows 95 generated dialog boxes can be seen.\n\nWhen the connection is successfully made, you can return to full screen mode by pressing 'F4'.");
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        SetFullScreenStatus(0);
    }
    gbRemoteOn = 1;
    ppDPRcvBuffer = static_cast<unsigned char **>(
        BaseAlloc(WS_TRANSPORT_BUFFER_COUNT * sizeof(unsigned char *), WSFILE,
                  s_wsInitSourceLineBase + 0xa));
    piDPRcvBufferSize = static_cast<int *>(
        BaseAlloc(WS_TRANSPORT_BUFFER_COUNT * sizeof(int), WSFILE,
                  s_wsInitSourceLineBase + 0xb));
    memset(ppDPRcvBuffer, 0,
           WS_TRANSPORT_BUFFER_COUNT * sizeof(unsigned char *));
    memset(piDPRcvBufferSize, 0,
           WS_TRANSPORT_BUFFER_COUNT * sizeof(int));

    wVer = MAKEWORD(1, 1);
    iRc = WSAStartup(wVer, &wsadata);
    if (iRc != 0) {
        sprintf(cWSTextBuffer, "Error During WSAStartup(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    sd_dg = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd_dg == INVALID_SOCKET) {
        sprintf(cWSTextBuffer, "Error During socket(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    gbRemoteOn = 1;
    saddr_loc.sin_family = AF_INET;
    saddr_loc.sin_port = htons(WS_TRANSPORT_PORT);
    saddr_loc.sin_addr.s_addr = htonl(INADDR_ANY);
    iRc = bind(sd_dg, reinterpret_cast<struct sockaddr *>(&saddr_loc),
               sizeof(saddr_loc));
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, "Error During bind(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    socketMode = 1;
    iRc = ioctlsocket(sd_dg, FIONBIO, &socketMode);
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, "Error During ioctlsocket(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    iRc = gethostname(localHostName, sizeof(localHostName) - 1);
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, "Error During gethostname(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    pHost = gethostbyname(localHostName);
    gIn_addrIP = *reinterpret_cast<struct in_addr *>(pHost->h_addr);
    sprintf(cWSTextBuffer, "%s", inet_ntoa(gIn_addrIP));
    giNetPosToDCOPos[giThisNetPos] = static_cast<int>(inet_addr(cWSTextBuffer));

    if (GameMode == REMOTE_GAME_NETWORK_HOST) {
        giWaitType = WS_WAIT_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(cWSTextBuffer,
                    "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional guests.",
                    inet_ntoa(gIn_addrIP), 0, giTCPNumPlayers - 1);
            NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(cWSTextBuffer,
                    "Hosting game at %s\n\nWaiting On Guest(s).\n\n  Press 'CANCEL' to abort.",
                    inet_ntoa(gIn_addrIP));
            NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        if (gbFunctionComplete == 0)
            ShutDown(0);
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        giWaitType = WS_WAIT_EXTRA_GUESTS;
        if (giTCPHostStatus != -1) {
            if (giTCPNumPlayers > 2) {
                sprintf(cWSTextBuffer,
                        "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional guests.",
                        inet_ntoa(gIn_addrIP), giNumHumanPlayers - 1,
                        giTCPNumPlayers - 1);
                NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(cWSTextBuffer,
                    "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests.",
                    inet_ntoa(gIn_addrIP), giNumHumanPlayers - 1);
            NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        gbRemoteGameOpen = 0;
        startup.playerCount = static_cast<unsigned char>(giNumHumanPlayers);
        memcpy(startup.playerAddresses, giNetPosToDCOPos,
               sizeof(giNetPosToDCOPos));
        for (player = 1; 0[&giNumHumanPlayers] > player; player++) {
            startup.netPosition = static_cast<unsigned char>(player);
            wsSendMessage(giNetPosToDCOPos[player], WS_MESSAGE_STARTUP,
                          sizeof(startup), &startup);
        }
    } else {
        while (1) {
            if (giTCPHostStatus != -1 && strlen(gcTCPAddress) != 0) {
                strcpy(cWSTextBuffer, gcTCPAddress);
                strcpy(gcTCPAddress, "");
            } else {
                GetDataEntry("Enter the host IP address.\n(i.e. 220.415.119.223)",
                             cWSTextBuffer, 20, 0, 0, 1);
            }
            giNetPosToDCOPos[0] = static_cast<int>(inet_addr(cWSTextBuffer));
            if (giNetPosToDCOPos[0] != static_cast<int>(INADDR_NONE))
                break;
            NormalDialog("Error in IP Address, please try again.",
                         5, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        giWaitType = WS_WAIT_HOST;
        sprintf(cWSTextBuffer, "Searching for host.");
        NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
    }
    return 0;
}

VA(0x00406e65, 0xd2)
void wsnet_term(void) {
    if (sd_dg != INVALID_SOCKET)
        closesocket(sd_dg);
    if (ppDPRcvBuffer != 0)
        BaseFree(ppDPRcvBuffer, WSFILE, s_wsTermSourceLineBase + 7);
    ppDPRcvBuffer = 0;
    if (piDPRcvBufferSize != 0)
        BaseFree(piDPRcvBufferSize, WSFILE, s_wsTermSourceLineBase + 0xb);
    piDPRcvBufferSize = 0;
    WSACleanup();
    bHostFound = 0;
    sd_dg = INVALID_SOCKET;
    iWSLastMsgNumHumanPlayers = 1;
    iWSAttempts = 0;
    iWSNextTickCount = 0;
    iWSWaitForHostStatus = 0;
    CleanupDPVars();
}

// @semantic
// Complete 0x28 frame/slots and 32/32 relocation targets. First raw residual is the +0x7b
// local branch byte; the first opcode-count divergence is two extra five-byte return
// trampolines after the broadcast send-error dialog. Tried both SOCKET_ERROR polarities,
// direct/commuted/SIB player tests, and nested/direct return arms; revisit after TU-state changes.
VA(0x00406f37, 0x1f5)
void wsSendMessage(int destination, unsigned char type, unsigned short int size,
                   void *data) {
    unsigned char *packetBuffer = static_cast<unsigned char *>(
        BaseAlloc(size + 1, WSFILE, s_wsSendSourceLineBase + 2));
    struct sockaddr_in peerAddress;
    int attemptCount;
    int error;
    int netPlayer;

    packetBuffer[0] = type;
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    peerAddress.sin_family = AF_INET;
    peerAddress.sin_port = htons(WS_TRANSPORT_PORT);
    if (destination == 0) {
        for (netPlayer = 0; netPlayer < giNumHumanPlayers; netPlayer++) {
            if (giThisNetPos == 0[&netPlayer])
                continue;
            attemptCount = 0;
            peerAddress.sin_addr.s_addr = giNetPosToDCOPos[netPlayer];
            while (1) {
                iRc = sendto(sd_dg, reinterpret_cast<char *>(packetBuffer), size + 1, 0,
                             reinterpret_cast<struct sockaddr *>(&peerAddress),
                             sizeof(peerAddress));
                if (iRc == SOCKET_ERROR) {
                    error = WSAGetLastError();
                    if (attemptCount < 20) {
                        DelayMilli(WS_TRANSPORT_SEND_RETRY_DELAY);
                        continue;
                    }
                    sprintf(cWSTextBuffer,
                            "TCP/IP Error During command 'sendto()' # %d", error);
                    NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
                    return;
                }
                break;
            }
        }
    } else {
        peerAddress.sin_addr.s_addr = destination;
        iRc = sendto(sd_dg, reinterpret_cast<char *>(packetBuffer), size + 1, 0,
                     reinterpret_cast<struct sockaddr *>(&peerAddress), sizeof(peerAddress));
        if (iRc == SOCKET_ERROR) {
            sprintf(cWSTextBuffer, "Error During sendto(): %d", WSAGetLastError());
            NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
    }
    BaseFree(packetBuffer, WSFILE, s_wsSendSourceLineBase + 0x39);
}

VA(0x0040712c, 0x61)
int wsnet_snd(int destination, int size, void *data) {
    int result;

    wsProcessMessages();
    if (destination != WS_TRANSPORT_BROADCAST_POSITION)
        wsSendMessage(giNetPosToDCOPos[destination], WS_MESSAGE_DATA,
                      static_cast<unsigned short>(size), data);
    else
        wsSendMessage(0, WS_MESSAGE_DATA, static_cast<unsigned short>(size), data);
    return 0;
}

VA(0x0040718d, 0xa7)
short int wsnet_rcv(short int, unsigned short int, void *data) {
    unsigned int size;

    wsProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    BaseFree(ppDPRcvBuffer[iDPRcvBufferTail], WSFILE,
             s_wsReceiveSourceLineBase + 9);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % WS_TRANSPORT_BUFFER_COUNT;
    return static_cast<short>(size);
}

// @semantic
// Complete 0x18 frame, stack slots, CFG instruction stream, and 13/13 ordered relocations.
// Only local branch destination bytes +0x62 and +0x74 differ. Continue, positive-call, and
// explicit empty self-packet arms were tried; revisit after later Wsnetwin TU-state changes.
VA(0x00407234, 0xaf)
void wsProcessMessages(void) {
    struct sockaddr_in remote;
    int addressLength = sizeof(remote);
    int receiveSize;

    while (1) {
        receiveSize = WS_TRANSPORT_BUFFER_SIZE;
        iRc = recvfrom(sd_dg, rcvBufIn, receiveSize, 0,
                       reinterpret_cast<struct sockaddr *>(&remote), &addressLength);
        if (iRc == SOCKET_ERROR) {
            iRc = WSAGetLastError();
            if (iRc == WSAEWOULDBLOCK)
                break;
        }
        if (iRc == 0)
            break;
        if (giNetPosToDCOPos[giThisNetPos] ==
            static_cast<int>(remote.sin_addr.s_addr)) {
        } else {
            wsEvaluateMessage(iRc, static_cast<int>(remote.sin_addr.s_addr));
        }
    }
}

// @semantic: complete message switch, frame, CFG, and all 77 ordered relocation
// identities/addends agree. The first residual is the embedded 0x14-byte jump
// table at RVA 0x7642; the candidate's next public begins one byte earlier, so
// the old full-span raw-identity claim was invalid.
VA(0x004072e3, 0x37d)
void wsEvaluateMessage(unsigned long int size, int sender) {
    char *message = rcvBufIn + 1;
    tag_message windowMessage;
    int player;

    switch (rcvBufIn[0]) {
    case WS_MESSAGE_DATA:
        ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<unsigned char *>(
            BaseAlloc(size - 1, WSFILE, s_wsEvaluateSourceLineBase + 10));
        memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
        piDPRcvBufferSize[iDPRcvBufferHead] = size;
        iDPRcvBufferHead = (iDPRcvBufferHead + 1) % WS_TRANSPORT_BUFFER_COUNT;
        break;
    case WS_MESSAGE_GUEST_ARRIVED:
        if (GameMode == REMOTE_GAME_NETWORK_HOST) {
            if (gbRemoteGameOpen != 0) {
                for (player = 1; player < giNumHumanPlayers; player++) {
                    if (giNetPosToDCOPos[player] == sender ||
                        &gsNetPlayerInfo[player] ==
                            reinterpret_cast<SNetPlayerInfo *>(message)) {
                        wsSendMessage(giNetPosToDCOPos[player],
                                      WS_MESSAGE_GUEST_ACCEPTED, 0, 0);
                        return;
                    }
                }
                giNetPosToDCOPos[giNumHumanPlayers] = sender;
                LogInt("Got HereIAm from ", sender, -999, -999, -999, -999, -999, -999);
                gsNetPlayerInfo[giNumHumanPlayers] =
                    *reinterpret_cast<SNetPlayerInfo *>(message);
                if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                    xNetHasOldPlayers = 1;
                wsSendMessage(giNetPosToDCOPos[giNumHumanPlayers],
                              WS_MESSAGE_GUEST_ACCEPTED, 0, 0);
                giNumHumanPlayers++;
            } else {
                wsSendMessage(sender, WS_MESSAGE_GUEST_REJECTED, 0, 0);
            }
        }
        break;
    case WS_MESSAGE_STARTUP:
        giNumHumanPlayers = message[0];
        giThisNetPos = message[1];
        LogInt("WSMSGSTARTUP", giThisNetPos, sender,
               -999, -999, -999, -999, -999);
        memcpy(giNetPosToDCOPos, message + 2, sizeof(giNetPosToDCOPos));
        bStartUpInfoReceived = 1;
        break;
    case WS_MESSAGE_GUEST_REJECTED:
        sprintf(cWSTextBuffer,
                "The Host already has a game in progress and is not accepting new players.");
        NormalDialog(cWSTextBuffer, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        ShutDown(0);
        break;
    case WS_MESSAGE_GUEST_ACCEPTED:
        sprintf(cWSTextBuffer, "Waiting for other remote player to set up game.");
        windowMessage.type = 0x200;
        windowMessage.payload.widget.command = 3;
        windowMessage.payload.widget.id = 1;
        windowMessage.payload.widget.data.text = cWSTextBuffer;
        pNormalDialogWindow->BroadcastMessage(windowMessage);
        pNormalDialogWindow->DrawWindow();
        bHostFound = 1;
        break;
    default:
        sprintf(cWSTextBuffer, "Unknown message: %d\n",
                static_cast<int>(rcvBufIn[0]));
        if (giDebugLevel > 0) {
            sprintf(gText, cWSTextBuffer);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        LogStr(cWSTextBuffer);
        break;
    }
}

// @semantic: all non-branch bytes and both ordered relocations agree; only the
// +0x1e local JMP displacement differs, with retail targeting the epilogue and
// this form its equivalent trailing JMP. Explicit if/else scored 94.12%.
VA(0x00407660, 0x2e)
int wsWaitForFirstGuest(void) {
    wsProcessMessages();
    return giNumHumanPlayers > 1;
}

VA(0x0040768e, 0xb8)
int wsWaitForExtraGuests(void) {
    tag_message message;

    wsProcessMessages();
    if (iWSLastMsgNumHumanPlayers != 0[&giNumHumanPlayers]) {
        if (giTCPHostStatus != -1 &&
            !(giTCPNumPlayers > 0[&giNumHumanPlayers]))
            return 1;
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(cWSTextBuffer,
                "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or wait for additional guests.",
                inet_ntoa(gIn_addrIP), giNumHumanPlayers - 1);
        message.type = 0x200;
        message.payload.widget.command = 3;
        message.payload.widget.id = 1;
        message.payload.widget.data.text = cWSTextBuffer;
        pNormalDialogWindow->BroadcastMessage(message);
        pNormalDialogWindow->DrawWindow();
    }
    return 0;
}

VA(0x00407746, 0x126)
int wsWaitForHost(void) {
    switch (iWSWaitForHostStatus) {
    case 0:
        if (KBTickCount() < iWSNextTickCount)
            return 0;
        wsProcessMessages();
        if (bHostFound != 0) {
            iWSWaitForHostStatus++;
            return 0;
        }
        wsSendMessage(0, WS_MESSAGE_GUEST_ARRIVED, sizeof(SNetPlayerInfo),
                      &gsThisNetPlayerInfo);
        iWSNextTickCount = KBTickCount() + WS_TRANSPORT_HOST_RETRY_DELAY;
        iWSAttempts++;
        if (iWSAttempts > WS_TRANSPORT_HOST_RETRY_LIMIT) {
            sprintf(cWSTextBuffer, "The Host is not responding.  Keep waiting?");
            NormalDialog(cWSTextBuffer, 2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult != 0x7805)
                ShutDown(0);
            iWSAttempts = 0;
        }
        break;
    case 1:
        wsProcessMessages();
        if (bStartUpInfoReceived != 0)
            return 1;
        break;
    }
    return 0;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ed2ac) int bHostFound = 0;
DATA(0x004ed2b0) unsigned int sd_dg = 0xffffffffU;
DATA(0x004ed2b4) int iWSLastMsgNumHumanPlayers = 1;
DATA(0x004ed2b8) int iWSAttempts = 0;
DATA(0x004ed2bc) int iWSNextTickCount = 0;
DATA(0x004ed2c0) int iWSWaitForHostStatus = 0;
DATA(0x00522f88) int iRc;
DATA(0x00522f90) char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
DATA(0x00523090) struct sockaddr_in saddr_loc;
DATA(0x005230a0) unsigned short wVer;
DATA(0x005230a8) struct WSAData wsadata;
DATA(0x00523238) struct in_addr gIn_addrIP;
DATA(0x00523240) struct sockaddr_in saddr_remote;
DATA(0x00523250) int iAddrLen;
