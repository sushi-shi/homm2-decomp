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
    IP_ADDRESS_ENTRY_LIMIT       = 20,
    SEND_ATTEMPT_LIMIT           = 20,
    EXTRA_GUEST_PLAYER_THRESHOLD = 2
} WinsockPrivateConstant;


static i16 s_wsInitSourceLineBase = 61;
static i16 s_wsTermSourceLineBase = 248;
static i16 s_wsSendSourceLineBase = 279;
static i16 s_wsReceiveSourceLineBase = 359;
static i16 s_wsEvaluateSourceLineBase = 413;

i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    i32 player;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0) {
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
                "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for additional "
                "guests.",
                platform::HostText(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(
                cWSTextBuffer,
                "Hosting game at %s\n\nWaiting On Guest(s).\n\n  Press 'CANCEL' to abort.",
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
                    "Hosting game at %s.\n\nYou have %d guest(s) out of an expected total of %d "
                    "guest(s) now logged in.  Click 'CANCEL' to move on without waiting for "
                    "additional guests.",
                    platform::HostText(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move "
                "on, or wait for additional guests.",
                platform::HostText(gIn_addrIP),
                giNumHumanPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
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
    retryAddress:
        if (giTCPHostStatus != -1 && strlen(gcTCPAddress) != 0) {
            strcpy(cWSTextBuffer, gcTCPAddress);
            strcpy(gcTCPAddress, "");
        } else {
            GetDataEntry(
                "Enter the host IP address.\n(i.e. 220.415.119.223)",
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
                "Error in IP Address, please try again.",
                NORMAL_DIALOG_WAIT_FIRST,
                -1, -1, -1, 0, -1, 0, -1, 0
            );
            goto retryAddress;
        }
        giWaitType = DIALOG_WAIT_WINSOCK_HOST;
        sprintf(cWSTextBuffer, "Searching for host.");
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
    i32 error;
    i32 netPlayer;

    packetBuffer[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    peerAddress.port = WS_TRANSPORT_PORT;
    if (destination == 0) {
        for (netPlayer = 0; giNumHumanPlayers > netPlayer; netPlayer++) {
            if (giThisNetPos == netPlayer)
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
    if (iDPRcvBufferTail == iDPRcvBufferHead)
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
                "The Host already has a game in progress and is not accepting new players."
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(NULL);
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
    if (iWSLastMsgNumHumanPlayers != giNumHumanPlayers) {
        if (giTCPHostStatus != -1 && giNumHumanPlayers >= giTCPNumPlayers)
            return 1;
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            cWSTextBuffer,
            "Hosting game at %s.\n\nYou have %d guest(s) now logged in.  Click 'OK' to move on, or "
            "wait for additional guests.",
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
