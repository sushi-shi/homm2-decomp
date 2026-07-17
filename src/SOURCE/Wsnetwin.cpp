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
#include <BASE/widget.h>
#include <SOURCE/Wsnetwin.h>

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Wsnetwin.cpp")

DATA(0x004ed2c4) static i16 s_wsInitSourceLineBase = 61;
DATA(0x004ed730) static i16 s_wsTermSourceLineBase = 248;
DATA(0x004ed78c) static i16 s_wsSendSourceLineBase = 279;
DATA(0x004ed830) static i16 s_wsReceiveSourceLineBase = 359;
DATA(0x004ed860) static i16 s_wsEvaluateSourceLineBase = 413;

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
// Complete 0x128 frame/slots and 164/164 relocation targets. First raw residual is +0x478:
// retail loads player then compares giNumHumanPlayers (9 bytes), while the conventional
// source loads the global then compares player (8 bytes). The later invalid-address test is
// ours JE+JMP versus retail JNE. Structured while/do-while and both condition polarities were
// tested; exact pointer-index and retry-label steering was rejected as less source-faithful.
VA(0x004068b0, 0x5b5)
i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    struct hostent* pHost;
    u_long socketMode;
    char localHostName[WS_TRANSPORT_BUFFER_SIZE];
    i32 player;

    if (gConfig.gfx[giCurExe].fullScreen != 0) {
        sprintf(
            gText,
            "About to initiate TCP/IP connection.  Heroes II will now drop from full screen mode "
            "to windowed mode, so that any Windows 95 generated dialog boxes can be seen.\n\nWhen "
            "the connection is successfully made, you can return to full screen mode by pressing "
            "'F4'."
        );
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        SetFullScreenStatus(0);
    }
    gbRemoteOn = 1;
    ppDPRcvBuffer = static_cast<u8**>(
        BaseAlloc(WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*), RETAIL_FILE, s_wsInitSourceLineBase + 0xa)
    );
    piDPRcvBufferSize = static_cast<i32*>(
        BaseAlloc(WS_TRANSPORT_BUFFER_COUNT * sizeof(i32), RETAIL_FILE, s_wsInitSourceLineBase + 0xb)
    );
    memset(ppDPRcvBuffer, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
    memset(piDPRcvBufferSize, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(i32));

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
    iRc = bind(sd_dg, reinterpret_cast<struct sockaddr*>(&saddr_loc), sizeof(saddr_loc));
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
    gIn_addrIP = *reinterpret_cast<struct in_addr*>(pHost->h_addr);
    sprintf(cWSTextBuffer, "%s", inet_ntoa(gIn_addrIP));
    giNetPosToDCOPos[giThisNetPos] = static_cast<i32>(inet_addr(cWSTextBuffer));

    if (GameMode == REMOTE_GAME_NETWORK_HOST) {
        giWaitType = WS_WAIT_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(
                cWSTextBuffer,
                "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional "
                "guests.",
                inet_ntoa(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(
                cWSTextBuffer,
                "Hosting game at %s\n\nWaiting On Guest(s).\n\n  Press 'CANCEL' to abort.",
                inet_ntoa(gIn_addrIP)
            );
            NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        if (gbFunctionComplete == 0)
            ShutDown(0);
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        giWaitType = WS_WAIT_EXTRA_GUESTS;
        if (giTCPHostStatus != -1) {
            if (giTCPNumPlayers > 2) {
                sprintf(
                    cWSTextBuffer,
                    "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                    "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for "
                    "additional guests.",
                    inet_ntoa(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                NormalDialog(cWSTextBuffer, 6, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move "
                "on, or wait for additional guests.",
                inet_ntoa(gIn_addrIP),
                giNumHumanPlayers - 1
            );
            NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        gbRemoteGameOpen = 0;
        startup.playerCount = static_cast<u8>(giNumHumanPlayers);
        memcpy(startup.playerAddresses, giNetPosToDCOPos, sizeof(giNetPosToDCOPos));
        for (player = 1; player < giNumHumanPlayers; player++) {
            startup.netPosition = static_cast<u8>(player);
            wsSendMessage(
                giNetPosToDCOPos[player],
                NETWORK_PACKET_STARTUP,
                sizeof(startup),
                &startup
            );
        }
    } else {
        while (1) {
            if (giTCPHostStatus != -1 && strlen(gcTCPAddress) != 0) {
                strcpy(cWSTextBuffer, gcTCPAddress);
                strcpy(gcTCPAddress, "");
            } else {
                GetDataEntry(
                    "Enter the host IP address.\n(i.e. 220.415.119.223)",
                    cWSTextBuffer,
                    20,
                    0,
                    0,
                    1
                );
            }
            giNetPosToDCOPos[0] = static_cast<i32>(inet_addr(cWSTextBuffer));
            if (giNetPosToDCOPos[0] != static_cast<i32>(INADDR_NONE))
                break;
            NormalDialog("Error in IP Address, please try again.", 5, -1, -1, -1, 0, -1, 0, -1, 0);
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
        BaseFree(ppDPRcvBuffer, RETAIL_FILE, s_wsTermSourceLineBase + 7);
    ppDPRcvBuffer = 0;
    if (piDPRcvBufferSize != 0)
        BaseFree(piDPRcvBufferSize, RETAIL_FILE, s_wsTermSourceLineBase + 0xb);
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
// Complete 0x28 frame/slots and 32/32 relocation targets. First raw residual is +0x8e:
// retail loads netPlayer then compares giNumHumanPlayers (9 bytes), while the conventional
// source loads the global then compares netPlayer (8 bytes). After DelayMilli, ours has two
// five-byte loop continuations and retries at sendto; retail has one and repeats the idempotent
// peer-address assignment. Direct, commuted, nested, do/while, and assignment-condition forms
// were tested; the exact label/pointer-index form was rejected as less source-faithful.
VA(0x00406f37, 0x1f5)
void wsSendMessage(i32 destination, u8 type, u16 size, void* data) {
    u8* packetBuffer = static_cast<u8*>(BaseAlloc(size + 1, RETAIL_FILE, s_wsSendSourceLineBase + 2));
    struct sockaddr_in peerAddress;
    i32 attemptCount;
    i32 error;
    i32 netPlayer;

    packetBuffer[0] = type;
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    peerAddress.sin_family = AF_INET;
    peerAddress.sin_port = htons(WS_TRANSPORT_PORT);
    if (destination == 0) {
        for (netPlayer = 0; netPlayer < giNumHumanPlayers; netPlayer++) {
            if (giThisNetPos == netPlayer)
                continue;
            attemptCount = 0;
            peerAddress.sin_addr.s_addr = giNetPosToDCOPos[netPlayer];
            while ((iRc = sendto(
                        sd_dg,
                        reinterpret_cast<char*>(packetBuffer),
                        size + 1,
                        0,
                        reinterpret_cast<struct sockaddr*>(&peerAddress),
                        sizeof(peerAddress)
                    ))
                   == SOCKET_ERROR) {
                error = WSAGetLastError();
                if (attemptCount < 20) {
                    DelayMilli(WS_TRANSPORT_SEND_RETRY_DELAY);
                } else {
                    sprintf(cWSTextBuffer, "TCP/IP Error During command 'sendto()' # %d", error);
                    NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
                    return;
                }
            }
        }
    } else {
        peerAddress.sin_addr.s_addr = destination;
        iRc = sendto(
            sd_dg,
            reinterpret_cast<char*>(packetBuffer),
            size + 1,
            0,
            reinterpret_cast<struct sockaddr*>(&peerAddress),
            sizeof(peerAddress)
        );
        if (iRc == SOCKET_ERROR) {
            sprintf(cWSTextBuffer, "Error During sendto(): %d", WSAGetLastError());
            NormalDialog(cWSTextBuffer, 5, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
    }
    BaseFree(packetBuffer, RETAIL_FILE, s_wsSendSourceLineBase + 0x39);
}

VA(0x0040712c, 0x61)
i32 wsnet_snd(i32 destination, i32 size, void* data) {
    i32 result;

    wsProcessMessages();
    if (destination != WS_TRANSPORT_BROADCAST_POSITION)
        wsSendMessage(
            giNetPosToDCOPos[destination],
            NETWORK_PACKET_DATA,
            static_cast<u16>(size),
            data
        );
    else
        wsSendMessage(0, NETWORK_PACKET_DATA, static_cast<u16>(size), data);
    return 0;
}

VA(0x0040718d, 0xa7)
i16 wsnet_rcv(i16, u16, void* data) {
    u32 size;

    wsProcessMessages();
    if (iDPRcvBufferTail == iDPRcvBufferHead)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    BaseFree(ppDPRcvBuffer[iDPRcvBufferTail], RETAIL_FILE, s_wsReceiveSourceLineBase + 9);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % WS_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}

// @semantic
// Complete 0x18 frame, stack slots, CFG instruction stream, and 13/13 ordered relocations.
// Only local branch destination bytes +0x62 and +0x74 differ. Continue, positive-call, and
// explicit empty self-packet arms were tried; revisit after later Wsnetwin TU-state changes.
VA(0x00407234, 0xaf)
void wsProcessMessages(void) {
    struct sockaddr_in remote;
    i32 addressLength = sizeof(remote);
    i32 receiveSize;

    while (1) {
        receiveSize = WS_TRANSPORT_BUFFER_SIZE;
        iRc = recvfrom(
            sd_dg,
            rcvBufIn,
            receiveSize,
            0,
            reinterpret_cast<struct sockaddr*>(&remote),
            &addressLength
        );
        if (iRc == SOCKET_ERROR) {
            iRc = WSAGetLastError();
            if (iRc == WSAEWOULDBLOCK)
                break;
        }
        if (iRc == 0)
            break;
        if (giNetPosToDCOPos[giThisNetPos] == static_cast<i32>(remote.sin_addr.s_addr)) {
        } else {
            wsEvaluateMessage(iRc, static_cast<i32>(remote.sin_addr.s_addr));
        }
    }
}

// @semantic: complete message switch, frame, CFG, and all 77 ordered relocation
// identities/addends agree. The first residual is the embedded 0x14-byte jump
// table at RVA 0x7642; the candidate's next public begins one byte earlier, so
// the old full-span raw-identity claim was invalid.
VA(0x004072e3, 0x37d)
void wsEvaluateMessage(u32l size, i32 sender) {
    char* message = rcvBufIn + 1;
    tag_message windowMessage;
    i32 player;

    switch (rcvBufIn[0]) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] =
                static_cast<u8*>(BaseAlloc(size - 1, RETAIL_FILE, s_wsEvaluateSourceLineBase + 10));
            memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
            piDPRcvBufferSize[iDPRcvBufferHead] = size;
            iDPRcvBufferHead = (iDPRcvBufferHead + 1) % WS_TRANSPORT_BUFFER_COUNT;
            break;
        case NETWORK_PACKET_GUEST_ARRIVED:
            if (GameMode == REMOTE_GAME_NETWORK_HOST) {
                if (gbRemoteGameOpen != 0) {
                    for (player = 1; player < giNumHumanPlayers; player++) {
                        if (giNetPosToDCOPos[player] == sender
                            || &gsNetPlayerInfo[player]
                                   == reinterpret_cast<SNetPlayerInfo*>(message)) {
                            wsSendMessage(
                                giNetPosToDCOPos[player],
                                NETWORK_PACKET_GUEST_ACCEPTED,
                                0,
                                0
                            );
                            return;
                        }
                    }
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    LogInt("Got HereIAm from ", sender, -999, -999, -999, -999, -999, -999);
                    gsNetPlayerInfo[giNumHumanPlayers] =
                        *reinterpret_cast<SNetPlayerInfo*>(message);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = 1;
                    wsSendMessage(
                        giNetPosToDCOPos[giNumHumanPlayers],
                        NETWORK_PACKET_GUEST_ACCEPTED,
                        0,
                        0
                    );
                    giNumHumanPlayers++;
                } else {
                    wsSendMessage(sender, NETWORK_PACKET_GUEST_REJECTED, 0, 0);
                }
            }
            break;
        case NETWORK_PACKET_STARTUP:
            giNumHumanPlayers = message[0];
            giThisNetPos = message[1];
            LogInt("WSMSGSTARTUP", giThisNetPos, sender, -999, -999, -999, -999, -999);
            memcpy(giNetPosToDCOPos, message + 2, sizeof(giNetPosToDCOPos));
            bStartUpInfoReceived = 1;
            break;
        case NETWORK_PACKET_GUEST_REJECTED:
            sprintf(
                cWSTextBuffer,
                "The Host already has a game in progress and is not accepting new players."
            );
            NormalDialog(cWSTextBuffer, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(0);
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            sprintf(cWSTextBuffer, "Waiting for other remote player to set up game.");
            windowMessage.type = MESSAGE_WIDGET;
            windowMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            windowMessage.payload.widget.id = 1;
            windowMessage.payload.widget.data.text = cWSTextBuffer;
            pNormalDialogWindow->BroadcastMessage(windowMessage);
            pNormalDialogWindow->DrawWindow();
            bHostFound = 1;
            break;
        default:
            sprintf(cWSTextBuffer, "Unknown message: %d\n", static_cast<i32>(rcvBufIn[0]));
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
i32 wsWaitForFirstGuest(void) {
    wsProcessMessages();
    return giNumHumanPlayers > 1;
}

VA(0x0040768e, 0xb8)
i32 wsWaitForExtraGuests(void) {
    tag_message message;

    wsProcessMessages();
    if (iWSLastMsgNumHumanPlayers != giNumHumanPlayers) {
        if (giTCPHostStatus != -1 && giNumHumanPlayers >= giTCPNumPlayers)
            return 1;
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            cWSTextBuffer,
            "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or "
            "wait for additional guests.",
            inet_ntoa(gIn_addrIP),
            giNumHumanPlayers - 1
        );
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.id = 1;
        message.payload.widget.data.text = cWSTextBuffer;
        pNormalDialogWindow->BroadcastMessage(message);
        pNormalDialogWindow->DrawWindow();
    }
    return 0;
}

VA(0x00407746, 0x126)
i32 wsWaitForHost(void) {
    switch (iWSWaitForHostStatus) {
        case 0:
            if (KBTickCount() < iWSNextTickCount)
                return 0;
            wsProcessMessages();
            if (bHostFound != 0) {
                iWSWaitForHostStatus++;
                return 0;
            }
            wsSendMessage(
                0,
                NETWORK_PACKET_GUEST_ARRIVED,
                sizeof(SNetPlayerInfo),
                &gsThisNetPlayerInfo
            );
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
DATA(0x004ed2ac) i32 bHostFound = 0;
DATA(0x004ed2b0) u32 sd_dg = 0xffffffffU;
DATA(0x004ed2b4) i32 iWSLastMsgNumHumanPlayers = 1;
DATA(0x004ed2b8) i32 iWSAttempts = 0;
DATA(0x004ed2bc) i32 iWSNextTickCount = 0;
DATA(0x004ed2c0) i32 iWSWaitForHostStatus = 0;
DATA(0x00522f88) i32 iRc;
DATA(0x00522f90) char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
DATA(0x00523090) struct sockaddr_in saddr_loc;
DATA(0x005230a0) u16 wVer;
DATA(0x005230a8) struct WSAData wsadata;
DATA(0x00523238) struct in_addr gIn_addrIP;
DATA(0x00523240) struct sockaddr_in saddr_remote;
DATA(0x00523250) i32 iAddrLen;

#undef RETAIL_FILE
