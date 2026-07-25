#include <va.h>
#include <windows.h>
#include <winsock.h>
#include <stddef.h>
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

H2_ENUM_BEGIN(WinsockPrivateConstant)
    IP_ADDRESS_ENTRY_LIMIT       = 20,
    SEND_ATTEMPT_LIMIT           = 20,
    EXTRA_GUEST_MIN_PLAYER_COUNT = 3
H2_ENUM_END(WinsockPrivateConstant)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\Wsnetwin.cpp")

DATA(0x004ed2c4) static i16 s_wsInitSourceLineBase = 61; // NOLINT(readability-magic-numbers)
DATA(0x004ed730) static i16 s_wsTermSourceLineBase = 248; // NOLINT(readability-magic-numbers)
DATA(0x004ed78c) static i16 s_wsSendSourceLineBase = 279; // NOLINT(readability-magic-numbers)
DATA(0x004ed830) static i16 s_wsReceiveSourceLineBase = 359; // NOLINT(readability-magic-numbers)
DATA(0x004ed860) static i16 s_wsEvaluateSourceLineBase = 413; // NOLINT(readability-magic-numbers)

inline void ShowPlainDialog(char* text, i32 type) {
    NormalDialog(text, type, -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x004068b0, 0x5b5)
i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    struct hostent* pHost;
    u_long socketMode;
    char localHostName[WS_TRANSPORT_BUFFER_SIZE];
    i32 player;

    if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0) {
        sprintf(
            gText,
            DATA_COMPGEN(0x004ed2c8, wsnetInitAboutToInitiateTCPIPConnection, "About to initiate TCP/IP connection.  Heroes II will now drop from full screen mode "
            "to windowed mode, so that any Windows 95 generated dialog boxes can be seen.\n\nWhen "
            "the connection is successfully made, you can return to full screen mode by pressing "
            "'F4'.")
        );
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        SetFullScreenStatus(0);
    }
    gbRemoteOn = true;
    ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC_AT(
        WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*), DATA_COMPGEN(0x004ed3cc, wsnet_initSourceFile, RETAIL_FILE),
        s_wsInitSourceLineBase + 10
    ));
    piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC_AT(
        WS_TRANSPORT_BUFFER_COUNT * sizeof(i32), DATA_COMPGEN(0x004ed3f8, wsnet_initSourceFile2, RETAIL_FILE),
        s_wsInitSourceLineBase + 11
    ));
    memset(ppDPRcvBuffer, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
    memset(piDPRcvBufferSize, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(i32));

    wVer = MAKEWORD(1, 1);
    iRc = WSAStartup(wVer, &wsadata);
    if (iRc != 0) {
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed424, wsnetInitErrorDuringWSAStartupD, "Error During WSAStartup(): %d"), WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    sd_dg = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd_dg == INVALID_SOCKET) {
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed444, wsnetInitErrorDuringSocketD, "Error During socket(): %d"), WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    gbRemoteOn = true;
    saddr_loc.sin_family = AF_INET;
    saddr_loc.sin_port = htons(WS_TRANSPORT_PORT);
    saddr_loc.sin_addr.s_addr = htonl(INADDR_ANY);
    iRc = bind(sd_dg, reinterpret_cast<struct sockaddr*>(&saddr_loc), sizeof(saddr_loc));
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed460, wsnetInitErrorDuringBindD, "Error During bind(): %d"), WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    socketMode = 1;
    iRc = ioctlsocket(sd_dg, FIONBIO, &socketMode);
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed478, wsnetInitErrorDuringIoctlsocketD, "Error During ioctlsocket(): %d"), WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    iRc = gethostname(localHostName, sizeof(localHostName) - 1);
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed498, wsnetInitErrorDuringGethostnameD, "Error During gethostname(): %d"), WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    pHost = gethostbyname(localHostName);
    gIn_addrIP = *reinterpret_cast<struct in_addr*>(pHost->h_addr);
    sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed4b8, wsnetInitS, "%s"), inet_ntoa(gIn_addrIP));
    giNetPosToDCOPos[giThisNetPos] = static_cast<i32>(inet_addr(cWSTextBuffer));

    if (GameMode == REMOTE_GAME_NETWORK_HOST) {
        giWaitType = DIALOG_WAIT_WINSOCK_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(
                cWSTextBuffer,
                DATA_COMPGEN(0x004ed4bc, wsnetInitHostingGameAtSYouHave, "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional "
                "guests."),
                inet_ntoa(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            ShowPlainDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST);
        } else {
            sprintf(
                cWSTextBuffer,
                DATA_COMPGEN(0x004ed560, wsnetInitHostingGameAtSWaitingOn, "Hosting game at %s\n\nWaiting On Guest(s).\n\n  Press 'CANCEL' to abort."),
                inet_ntoa(gIn_addrIP)
            );
            ShowPlainDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST);
        }
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        giWaitType = DIALOG_WAIT_WINSOCK_GUESTS;
        if (giTCPHostStatus != -1) {
            if (giTCPNumPlayers >= EXTRA_GUEST_MIN_PLAYER_COUNT) {
                sprintf(
                    cWSTextBuffer,
                    DATA_COMPGEN(0x004ed5a8, wsnetInitHostingGameAtSYouHave2, "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                    "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for "
                    "additional guests."),
                    inet_ntoa(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                ShowPlainDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                DATA_COMPGEN(0x004ed64c, wsnetInitHostingGameAtSYouHave3, "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move "
                "on, or wait for additional guests."),
                inet_ntoa(gIn_addrIP),
                giNumHumanPlayers - 1
            );
            ShowPlainDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST);
        }
        gbRemoteGameOpen = false;
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
                strcpy(gcTCPAddress, DATA_COMPGEN(0x004ed6bc, wsnetInitEmptyString, ""));
            } else {
                GetDataEntry(
                    DATA_COMPGEN(0x004ed6c0, wsnetInitEnterTheHostIPAddressI, "Enter the host IP address.\n(i.e. 220.415.119.223)"),
                    cWSTextBuffer,
                    IP_ADDRESS_ENTRY_LIMIT,
                    NULL,
                    0,
                    1
                );
            }
            giNetPosToDCOPos[0] = static_cast<i32>(inet_addr(cWSTextBuffer));
            if (giNetPosToDCOPos[0] != static_cast<i32>(INADDR_NONE))
                break;
            ShowPlainDialog(DATA_COMPGEN(0x004ed6f4, wsnetInitErrorInIPAddressPleaseTry, "Error in IP Address, please try again."), NORMAL_DIALOG_WAIT_FIRST);
        }
        giWaitType = DIALOG_WAIT_WINSOCK_HOST;
        sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed71c, wsnetInitSearchingForHost, "Searching for host."));
        ShowPlainDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST);
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
    }
    return 0;
}

VA(0x00406e65, 0xd2)
void wsnet_term(void) {
    if (sd_dg != INVALID_SOCKET)
        closesocket(sd_dg);
    if (ppDPRcvBuffer != NULL)
        H2_FREE_AT(ppDPRcvBuffer, DATA_COMPGEN(0x004ed734, wsnet_termSourceFile, RETAIL_FILE), s_wsTermSourceLineBase + 7);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE_AT(piDPRcvBufferSize, DATA_COMPGEN(0x004ed760, wsnet_termSourceFile2, RETAIL_FILE), s_wsTermSourceLineBase + 11);
    piDPRcvBufferSize = NULL;
    WSACleanup();
    bHostFound = 0;
    sd_dg = INVALID_SOCKET;
    iWSLastMsgNumHumanPlayers = 1;
    iWSAttempts = 0;
    iWSNextTickCount = 0;
    iWSWaitForHostStatus = 0;
    CleanupDPVars();
}

VA(0x00406f37, 0x1f5)
void wsSendMessage(
    i32 destination,
    H2_ENUM_PARAM(NetworkPacketType, u8) type,
    u16 size,
    void* data
) {
    u8* packetBuffer = static_cast<u8*>(H2_ALLOC_AT(size + 1, DATA_COMPGEN(0x004ed790, wsSendMessageSourceFile, RETAIL_FILE), s_wsSendSourceLineBase + 2));
    struct sockaddr_in peerAddress;
    i32 attemptCount;
    i32 error;
    i32 netPlayer;

    packetBuffer[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    peerAddress.sin_family = AF_INET;
    peerAddress.sin_port = htons(WS_TRANSPORT_PORT);
    if (destination == 0) {
        for (netPlayer = 0; giNumHumanPlayers > netPlayer; netPlayer++) {
            if (giThisNetPos == netPlayer)
                continue;
            attemptCount = 0;
            peerAddress.sin_addr.s_addr = giNetPosToDCOPos[netPlayer];
        sendPacket:
            iRc = sendto(
                sd_dg,
                reinterpret_cast<char*>(packetBuffer),
                size + 1,
                0,
                reinterpret_cast<struct sockaddr*>(&peerAddress),
                sizeof(peerAddress)
            );
            if (iRc == SOCKET_ERROR) {
                error = WSAGetLastError();
                if (attemptCount < SEND_ATTEMPT_LIMIT) {
                    DelayMilli(WS_TRANSPORT_SEND_RETRY_DELAY);
                    goto sendPacket;
                }
                sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed7bc, wsSendMessageTCPIPErrorDuringCommandSendto, "TCP/IP Error During command 'sendto()' # %d"), error);
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
                return;
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
            sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed7e8, wsSendMessageErrorDuringSendtoD, "Error During sendto(): %d"), WSAGetLastError());
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
    }
    H2_FREE_AT(packetBuffer, DATA_COMPGEN(0x004ed804, wsSendMessageSourceFile2, RETAIL_FILE), s_wsSendSourceLineBase + 57);
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
    H2_FREE_AT(
        ppDPRcvBuffer[iDPRcvBufferTail], DATA_COMPGEN(0x004ed834, wsnet_rcvSourceFile, RETAIL_FILE),
        s_wsReceiveSourceLineBase + 9
    );
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % WS_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}

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
                return;
        }
        if (iRc == 0)
            return;
        if (giNetPosToDCOPos[giThisNetPos] == static_cast<i32>(remote.sin_addr.s_addr)) {
        } else {
            wsEvaluateMessage(iRc, static_cast<i32>(remote.sin_addr.s_addr));
        }
    }
}

// Byte-exact against retail: the startup fields are read through the raw
// message bytes (sign-extended char loads, no struct-typed local) and the
// three INFO/CONFIRM dialogs call NormalDialog directly. Keep these spellings.
VA(0x004072e3, 0x37d)
void wsEvaluateMessage(u32l size, i32 sender) {
    char* message = rcvBufIn + 1;
    tag_message windowMessage;
    i32 player;

    switch (static_cast<NetworkPacketType>(rcvBufIn[0])) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC_AT(
                size - 1, DATA_COMPGEN(0x004ed864, wsEvaluateMessageSourceFile, RETAIL_FILE),
                s_wsEvaluateSourceLineBase + 10
            ));
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
                                NULL
                            );
                            return;
                        }
                    }
                    giNetPosToDCOPos[giNumHumanPlayers] = sender;
                    LogInt(
                        DATA_COMPGEN(0x004ed890, wsEvaluateMessageGotHereIAmFrom, "Got HereIAm from "),
                        sender,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE,
                        LOG_UNUSED_VALUE
                    );
                    gsNetPlayerInfo[giNumHumanPlayers] =
                        *reinterpret_cast<SNetPlayerInfo*>(message);
                    if (gsNetPlayerInfo[giNumHumanPlayers].reserved[0] == 0)
                        xNetHasOldPlayers = 1;
                    wsSendMessage(
                        giNetPosToDCOPos[giNumHumanPlayers],
                        NETWORK_PACKET_GUEST_ACCEPTED,
                        0,
                        NULL
                    );
                    giNumHumanPlayers++;
                } else {
                    wsSendMessage(sender, NETWORK_PACKET_GUEST_REJECTED, 0, NULL);
                }
            }
            break;
        case NETWORK_PACKET_STARTUP:
            giNumHumanPlayers = *(message + offsetof(WinsockStartupMessage, playerCount));
            giThisNetPos = *(message + offsetof(WinsockStartupMessage, netPosition));
            LogInt(
                DATA_COMPGEN(0x004ed8a4, wsEvaluateMessageWSMSGSTARTUP, "WSMSGSTARTUP"),
                giThisNetPos,
                sender,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE,
                LOG_UNUSED_VALUE
            );
            memcpy(
                giNetPosToDCOPos,
                message + offsetof(WinsockStartupMessage, playerAddresses),
                sizeof(giNetPosToDCOPos)
            );
            bStartUpInfoReceived = 1;
            break;
        case NETWORK_PACKET_GUEST_REJECTED:
            sprintf(
                cWSTextBuffer,
                DATA_COMPGEN(0x004ed8b4, wsEvaluateMessageTheHostAlreadyHasAGame, "The Host already has a game in progress and is not accepting new players.")
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(NULL);
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed900, wsEvaluateMessageWaitingForOtherRemotePlayerTo, "Waiting for other remote player to set up game."));
            windowMessage.type = MESSAGE_WIDGET;
            windowMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            windowMessage.payload.widget.id = 1;
            windowMessage.payload.widget.data.text = cWSTextBuffer;
            pNormalDialogWindow->BroadcastMessage(windowMessage);
            pNormalDialogWindow->DrawWindow();
            bHostFound = 1;
            break;
        default:
            sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed930, wsEvaluateMessageUnknownMessageD, "Unknown message: %d\n"), static_cast<i32>(rcvBufIn[0]));
            if (giDebugLevel > 0) {
                sprintf(gText, cWSTextBuffer);
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            LogStr(cWSTextBuffer);
            break;
    }
}

VA(0x00407660, 0x2e)
i32 wsWaitForFirstGuest(void) {
    wsProcessMessages();
    if (giNumHumanPlayers > 1)
        return 1;
    return 0;
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
            DATA_COMPGEN(0x004ed948, wsWaitForExtraGuestsHostingGameAtSYouHave, "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or "
            "wait for additional guests."),
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
                sprintf(cWSTextBuffer, DATA_COMPGEN(0x004ed9b8, wsWaitForHostTheHostIsNotRespondingKeep, "The Host is not responding.  Keep waiting?"));
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult != NORMAL_DIALOG_BUTTON_FIVE)
                    ShutDown(NULL);
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

DATA(0x004ed2ac) i32 bHostFound = 0;
DATA(0x004ed2b0) u32 sd_dg = INVALID_SOCKET;
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
