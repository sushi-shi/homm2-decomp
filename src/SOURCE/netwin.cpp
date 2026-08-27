#include <Ints.h>
#include <BASE/Utf8.h>
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
#include <SOURCE/Localization.h>

typedef enum WinsockPrivateConstant {
    IP_ADDRESS_ENTRY_LIMIT = 20,
    SEND_ATTEMPT_LIMIT = 20,
    EXTRA_GUEST_PLAYER_THRESHOLD = 2
} WinsockPrivateConstant;

i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    i32 plr;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0) {
        NormalDialog(
            localization::Tr("network.tcp.fullscreen_warning"),
            1,
            -1, -1, -1, 0, -1, 0, -1, 0
        );
        SetFullScreenStatus(false);
    }
    gbRemoteOn = true;
    ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*)));
    piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(i32)));
    memset(ppDPRcvBuffer, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
    memset(piDPRcvBufferSize, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(i32));

    if (!platform::SocketsStartup()) {
        sprintf(cWSTextBuffer, localization::Tr("network.tcp.socket_start_error"), platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    sd_dg = platform::OpenDatagramSocket();
    if (sd_dg == platform::kInvalidSocket) {
        sprintf(cWSTextBuffer, localization::Tr("network.tcp.socket_open_error"), platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    gbRemoteOn = true;
    if (!platform::BindSocket(sd_dg, WS_TRANSPORT_PORT)) {
        sprintf(cWSTextBuffer, localization::Tr("network.tcp.socket_bind_error"), platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    if (!platform::SetSocketNonBlocking(sd_dg, true)) {
        sprintf(cWSTextBuffer, localization::Tr("network.tcp.socket_mode_error"), platform::LastSocketError());
        ShutDown(cWSTextBuffer);
    }
    gIn_addrIP = platform::LocalHost();
    giNetPosToDCOPos[giThisNetPos] = static_cast<i32>(gIn_addrIP);

    if (GameMode == REMOTE_GAME_NETWORK_HOST) {
        giWaitType = DIALOG_WAIT_WINSOCK_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(
                cWSTextBuffer,
                localization::TrPlural("network.tcp.host.expected_guests", 0),
                platform::HostText(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(
                cWSTextBuffer,
                localization::Tr("network.tcp.host.waiting_guests"),
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
                    localization::TrPlural(
                        "network.tcp.host.expected_guests",
                        giNumHumanPlayers - 1
                    ),
                    platform::HostText(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                localization::TrPlural(
                    "network.tcp.host.connected_guests",
                    giNumHumanPlayers - 1
                ),
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
        do {
            if (giTCPHostStatus != -1 && strlen(gcTCPAddress) > 0) {
                strcpy(cWSTextBuffer, gcTCPAddress);
                strcpy(
                    gcTCPAddress,
                    ""
                );
            } else {
                GetDataEntry(
                    localization::Tr("network.tcp.host_address.prompt"),
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
                    localization::Tr("network.tcp.host_address.invalid"),
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
            }
        } while (giNetPosToDCOPos[0] == -1);
        giWaitType = DIALOG_WAIT_WINSOCK_HOST;
        snprintf(
            cWSTextBuffer,
            sizeof(cWSTextBuffer),
            "%s",
            localization::Tr("network.tcp.host.searching")
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
    bHostFound = false;
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
            for (attemptCount = 0; attemptCount < SEND_ATTEMPT_LIMIT; ++attemptCount) {
                peerAddress.host = static_cast<u32l>(giNetPosToDCOPos[netPlayer]);
                iRc = platform::SendTo(sd_dg, packetBuffer, size + 1, peerAddress);
                if (iRc >= 0) {
                    break;
                }
                error = platform::LastSocketError();
                if (attemptCount + 1 < SEND_ATTEMPT_LIMIT) {
                    DelayMilli(WS_TRANSPORT_SEND_RETRY_DELAY);
                }
            }
            if (iRc < 0) {
                snprintf(
                    cWSTextBuffer,
                    sizeof(cWSTextBuffer),
                    localization::Tr("network.tcp.send_command_error"),
                    error
                );
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
                H2_FREE(packetBuffer);
                return;
            }
        }
    } else {
        peerAddress.host = static_cast<u32l>(destination);
        iRc = platform::SendTo(sd_dg, packetBuffer, size + 1, peerAddress);
        if (iRc < 0) {
            sprintf(cWSTextBuffer, localization::Tr("network.tcp.send_error"), platform::LastSocketError());
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            H2_FREE(packetBuffer);
            return;
        }
    }
    H2_FREE(packetBuffer);
}

i32 wsnet_snd(i32 destination, i32 size, void* data) {

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
                        xNetHasOldPlayers = true;
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
            bStartUpInfoReceived = true;
            break;
        case NETWORK_PACKET_GUEST_REJECTED:
            snprintf(
                cWSTextBuffer,
                sizeof(cWSTextBuffer),
                "%s",
                localization::Tr("network.tcp.host.rejected")
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(NULL);
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            snprintf(
                cWSTextBuffer,
                sizeof(cWSTextBuffer),
                "%s",
                localization::Tr("network.tcp.host.waiting_setup")
            );
            windowMessage.type = MESSAGE_WIDGET;
            windowMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            windowMessage.payload.widget.id = 1;
            windowMessage.payload.widget.data.text = cWSTextBuffer;
            pNormalDialogWindow->BroadcastMessage(windowMessage);
            pNormalDialogWindow->DrawWindow();
            bHostFound = true;
            break;
        default:
            snprintf(
                cWSTextBuffer,
                sizeof(cWSTextBuffer),
                localization::Tr("network.tcp.unknown_message"),
                static_cast<i32>(rcvBufIn[0])
            );
            if (giDebugLevel > 0) {
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cWSTextBuffer);
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
            localization::TrPlural(
                "network.tcp.host.connected_guests",
                giNumHumanPlayers - 1
            ),
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
                snprintf(
                    cWSTextBuffer,
                    sizeof(cWSTextBuffer),
                    "%s",
                    localization::Tr("network.tcp.host.not_responding")
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

b32 bHostFound = false;
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
b32 bStartUpInfoReceived = false;

void CleanupDPVars(void) {
    iDPRcvBufferHead = 0;
    iDPRcvBufferTail = 0;
}

i32 giNetPosToDCOPos[WS_TRANSPORT_PLAYER_LIMIT];
