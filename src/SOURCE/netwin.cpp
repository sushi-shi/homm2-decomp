#include <Ints.h>
#include <PLATFORM/Sockets.h>
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
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/netwin.h>

typedef enum WinsockPrivateConstant {
    IP_ADDRESS_ENTRY_LIMIT = 20,
    SEND_ATTEMPT_LIMIT = 20,
    EXTRA_GUEST_PLAYER_THRESHOLD = 2
} WinsockPrivateConstant;

i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    i32 plr;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0) {
        sprintf(
            gText,


            "\xce\xe1 \xe8\xed\xe8\xf6\xe8\xe8\xf0\xee\xe2\xe0\xed\xe8\xe8 TCP/IP \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff. "
                "\xc3\xe5\xf0\xee\xe8 II \xef\xe5\xf0\xe5\xea\xeb\xfe\xf7\xe0\xf2\xf1\xff \xe2 \xee\xea\xee\xed\xed\xfb\xe9 \xf0\xe5\xe6\xe8\xec, \xf7\xf2\xee\xe1\xfb \xe2\xfb "
                "\xef\xee\xeb\xf3\xf7\xe8\xeb\xe8 \xe4\xee\xf1\xf2\xf3\xef \xea \xe4\xe8\xe0\xeb\xee\xe3\xee\xe2\xfb\xec \xee\xea\xed\xe0\xec Windows.\n\n"
                "\xca\xee\xe3\xe4\xe0 \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5 \xe1\xf3\xe4\xe5\xf2 \xf3\xf1\xf2\xe0\xed\xee\xe2\xeb\xe5\xed\xee, \xe2\xfb \xf1\xec\xee\xe6\xe5\xf2\xe5 \xe2\xe5\xf0\xed\xf3\xf2\xfc\xf1\xff \xe2 "
                "\xef\xee\xeb\xed\xee\xfd\xea\xf0\xe0\xed\xed\xfb\xe9 \xf0\xe5\xe6\xe8\xec \xed\xe0\xe6\xe0\xe2 'F4'."
        );
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        SetFullScreenStatus(0);
    }
    gbRemoteOn = true;
    ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*)));
    piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(i32)));
    memset(ppDPRcvBuffer, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
    memset(piDPRcvBufferSize, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(i32));

    if (!platform::SocketsStartup()) {
        sprintf(cWSTextBuffer, "Error starting sockets: %d", platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    sd_dg = platform::OpenDatagramSocket();
    if (sd_dg == platform::kInvalidSocket) {
        sprintf(cWSTextBuffer, "Error opening socket: %d", platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    gbRemoteOn = true;
    if (!platform::BindSocket(sd_dg, WS_TRANSPORT_PORT)) {
        sprintf(cWSTextBuffer, "Error binding socket: %d", platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    if (!platform::SetSocketNonBlocking(sd_dg, true)) {
        sprintf(cWSTextBuffer, "Error setting socket mode: %d", platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    gIn_addrIP = platform::LocalHost();
    giNetPosToDCOPos[giThisNetPos] = static_cast<i32>(gIn_addrIP);

    if (GameMode == REMOTE_GAME_NETWORK_HOST) {
        giWaitType = DIALOG_WAIT_WINSOCK_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(
                cWSTextBuffer,
                "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xef\xee \xe0\xe4\xf0\xe5\xf1\xf3 %s.\n\n"
                    "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9 \xe8\xe7 \xee\xe6\xe8\xe4\xe0\xe2\xf8\xe8\xf5\xf1\xff %d \xe3\xee\xf1\xf2\xe5\xe9. "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xed\xe5 \xe4\xee\xe6\xe8\xe4\xe0\xff\xf1\xfc  "
                    "\xee\xf1\xf2\xe0\xeb\xfc\xed\xfb\xf5 \xe3\xee\xf1\xf2\xe5\xe9.",
                platform::HostText(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(
                cWSTextBuffer,
                "\xce\xf2\xea\xf0\xfb\xf2\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s\n\n"
                    "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe3\xee\xf1\xf2\xff(\xe5\xe9).\n\n  "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.",
                platform::HostText(gIn_addrIP)
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        giWaitType = DIALOG_WAIT_WINSOCK_GUESTS;
        if (giTCPHostStatus != -1) {
            if (giTCPNumPlayers > EXTRA_GUEST_PLAYER_THRESHOLD) {
                sprintf(
                    cWSTextBuffer,
                    "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xef\xee \xe0\xe4\xf0\xe5\xf1\xf3 %s.\n\n"
                        "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9 \xe8\xe7 \xee\xe6\xe8\xe4\xe0\xe2\xf8\xe8\xf5\xf1\xff %d \xe3\xee\xf1\xf2\xe5\xe9. "
                        "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xed\xe5 \xe4\xee\xe6\xe8\xe4\xe0\xff\xf1\xfc  "
                        "\xee\xf1\xf2\xe0\xeb\xfc\xed\xfb\xf5 \xe3\xee\xf1\xf2\xe5\xe9.",
                    platform::HostText(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s.\n\n"
                    "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc "
                    "\xe8\xeb\xe8 \xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2.",
                platform::HostText(gIn_addrIP),
                giNumHumanPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        gbRemoteGameOpen = false;
        startup.playerCount = static_cast<u8>(giNumHumanPlayers);
        memcpy(startup.playerAddresses, giNetPosToDCOPos, sizeof(giNetPosToDCOPos));
        for (plr = 1; plr < giNumHumanPlayers; plr++) {
            startup.netPosition = static_cast<u8>(plr);
            wsSendMessage(
                giNetPosToDCOPos[plr],
                NETWORK_PACKET_STARTUP,
                sizeof(startup),
                &startup
            );
        }
    } else {
    retryAddress:
        if (giTCPHostStatus != -1 && strlen(gcTCPAddress) > 0) {
            strcpy(cWSTextBuffer, gcTCPAddress);
            strcpy(
                gcTCPAddress,
                ""
            );
        } else {
            GetDataEntry(


                "\xc2\xe2\xe5\xe4\xe8\xf2\xe5 IP \xe0\xe4\xf0\xe5\xf1 \xf1\xe5\xf0\xe2\xe5\xf0\xe0.\n"
                    "(\xcd\xe0\xef\xf0\xe8\xec\xe5\xf0: 220.415.119.223)",
                cWSTextBuffer,
                IP_ADDRESS_ENTRY_LIMIT,
                NULL,
                0,
                1
            );
        }
        giNetPosToDCOPos[0] = static_cast<i32>(platform::HostFromText(cWSTextBuffer));
        if (giNetPosToDCOPos[0] == -1) {
            NormalDialog(

                "\xcd\xe5\xef\xf0\xe0\xe2\xe8\xeb\xfc\xed\xfb\xe9 IP \xe0\xe4\xf0\xe5\xf1. "
                    "\xcf\xee\xef\xf0\xee\xe1\xf3\xe9\xf2\xe5 \xe5\xf9\xe5 \xf0\xe0\xe7.",
                NORMAL_DIALOG_WAIT_FIRST,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            goto retryAddress;
        }
        giWaitType = DIALOG_WAIT_WINSOCK_HOST;
        sprintf(
            cWSTextBuffer,
              "\xcf\xee\xe8\xf1\xea \xf1\xe5\xf0\xe2\xe5\xf0\xe0."
        );
        NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
    }
    return 0;
}

void wsnet_term(void) {
    if (sd_dg != platform::kInvalidSocket)
        platform::CloseSocket(sd_dg);
    if (ppDPRcvBuffer != NULL)
        H2_FREE(ppDPRcvBuffer);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize);
    piDPRcvBufferSize = NULL;
    platform::SocketsShutdown();
    bHostFound = 0;
    sd_dg = platform::kInvalidSocket;
    iWSLastMsgNumHumanPlayers = 1;
    iWSAttempts = 0;
    iWSNextTickCount = 0;
    iWSWaitForHostStatus = 0;
    CleanupDPVars();
}

void wsSendMessage(
    i32 destination,
    NetworkPacketType type,
    u16 size,
    void* data
) {
    u8* packetBuffer = static_cast<u8*>(H2_ALLOC(size + 1));
    platform::Address peerAddress;
    i32 attemptCount;
    i32 netPlayer;
    i32 error;

    packetBuffer[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    peerAddress.port = WS_TRANSPORT_PORT;
    if (destination == 0) {
        for (netPlayer = 0; netPlayer < giNumHumanPlayers; netPlayer++) {
            if (netPlayer == giThisNetPos)
                continue;
            attemptCount = 0;
        sendPacket:
            peerAddress.host = static_cast<u32l>(giNetPosToDCOPos[netPlayer]);
            iRc = platform::SendTo(sd_dg, packetBuffer, size + 1, peerAddress);
            if (iRc < 0) {
                error = platform::LastSocketError();
                if (attemptCount < SEND_ATTEMPT_LIMIT) {
                    DelayMilli(WS_TRANSPORT_SEND_RETRY_DELAY);
                    goto sendPacket;
                }
                sprintf(cWSTextBuffer, "TCP/IP Error During command 'sendto()' # %d", error);
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
                return;
            }
        }
    } else {
        peerAddress.host = static_cast<u32l>(destination);
        iRc = platform::SendTo(sd_dg, packetBuffer, size + 1, peerAddress);
        if (iRc < 0) {
            sprintf(cWSTextBuffer, "Error sending: %d", platform::LastSocketError());
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
    }
    H2_FREE(packetBuffer);
}

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

i16 wsnet_rcv(i16, u16, void* data) {
    u32 size;

    wsProcessMessages();
    if (iDPRcvBufferHead == iDPRcvBufferTail)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE(ppDPRcvBuffer[iDPRcvBufferTail]);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % WS_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}

void wsProcessMessages(void) {
    platform::Address remote;
    i32 receiveSize;

    while (1) {
        receiveSize = WS_TRANSPORT_BUFFER_SIZE;
        iRc = platform::ReceiveFrom(sd_dg, rcvBufIn, receiveSize, &remote);
        if (iRc < 0)
            return;
        if (iRc == 0)
            return;
        if (giNetPosToDCOPos[giThisNetPos] != static_cast<i32>(remote.host)) {
            wsEvaluateMessage(iRc, static_cast<i32>(remote.host));
        }
    }
}

void wsEvaluateMessage(u32l size, i32 sender) {
    char* message = rcvBufIn + 1;
    tag_message windowMessage;
    i32 player;

    switch (static_cast<NetworkPacketType>(rcvBufIn[0])) {
        case NETWORK_PACKET_DATA:
            ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC(size - 1));
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
                        "Got HereIAm from ",
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
                "WSMSGSTARTUP",
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
                "\xd1\xe5\xf0\xe2\xe5\xf0 \xf3\xe6\xe5 \xf1\xee\xe7\xe4\xe0\xeb \xe8\xe3\xf0\xf3 "
                "\xe8 \xed\xe5 \xef\xf0\xe8\xed\xe8\xec\xe0\xe5\xf2 \xed\xee\xe2\xfb\xf5 "
                "\xe8\xe3\xf0\xee\xea\xee\xe2."
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(NULL);
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            sprintf(
                cWSTextBuffer,
                "\xce\xe6\xe8\xe4\xe0\xfe \xe8\xe3\xf0\xee\xea\xe0 \xe4\xeb\xff "
                "\xed\xe0\xf7\xe0\xeb\xe0 \xe8\xe3\xf0\xfb."
            );
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
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            LogStr(cWSTextBuffer);
            break;
    }
}

i32 wsWaitForFirstGuest(void) {
    wsProcessMessages();
    if (giNumHumanPlayers > 1)
        return 1;
    return 0;
}

i32 wsWaitForExtraGuests(void) {
    tag_message message;

    wsProcessMessages();
    if (giNumHumanPlayers != iWSLastMsgNumHumanPlayers) {
        if (giTCPHostStatus != -1 && giNumHumanPlayers >= giTCPNumPlayers)
            return 1;
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            cWSTextBuffer,
            "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s.\n\n"
                "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc "
                "\xe8\xeb\xe8 \xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2.",
            platform::HostText(gIn_addrIP),
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

i32 wsWaitForHost(void) {
    switch (iWSWaitForHostStatus) {
        case 0:
            if (iWSNextTickCount > platform::Ticks())
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
            iWSNextTickCount = platform::Ticks() + WS_TRANSPORT_HOST_RETRY_DELAY;
            iWSAttempts++;
            if (iWSAttempts > WS_TRANSPORT_HOST_RETRY_LIMIT) {
                sprintf(
                    cWSTextBuffer,
                      "\xd1\xe5\xf0\xe2\xe5\xf0 \xed\xe5 \xee\xf2\xe2\xe5\xf7\xe0\xe5\xf2. "
                        "\xcf\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xee\xe6\xe8\xe4\xe0\xed\xe8\xe5? "
                );
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

i32 bHostFound = 0;
platform::Socket sd_dg = platform::kInvalidSocket;
i32 iWSLastMsgNumHumanPlayers = 1;
i32 iWSAttempts = 0;
i32 iWSNextTickCount = 0;
i32 iWSWaitForHostStatus = 0;
i32 iRc;
char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
u32l gIn_addrIP;
i32 iAddrLen;

i32 iDPRcvBufferHead = 0;
i32 iDPRcvBufferTail = 0;
u8** ppDPRcvBuffer = NULL;
i32* piDPRcvBufferSize = NULL;
i32 bStartUpInfoReceived = 0;

void CleanupDPVars(void) {
    iDPRcvBufferHead = 0;
    iDPRcvBufferTail = 0;
}

i32 giNetPosToDCOPos[WS_TRANSPORT_PLAYER_LIMIT];
