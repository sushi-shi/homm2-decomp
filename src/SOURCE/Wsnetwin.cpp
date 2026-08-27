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
    IP_ADDRESS_ENTRY_LIMIT = 20,
    SEND_ATTEMPT_LIMIT = 20,
    EXTRA_GUEST_PLAYER_THRESHOLD = 2
H2_ENUM_END(WinsockPrivateConstant)

VA(0x004b1cf0, 0x572)
i16 wsnet_init(void) {
    WinsockStartupMessage startup;
    struct hostent* pHost;
    u_long blockMode;
    char localHostName[WS_TRANSPORT_BUFFER_SIZE];
    i32 plr;

    if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0) {
        sprintf(
            gText,
            /* Об инициировании TCP/IP соединения. Герои II переключатся в оконный режим, чтобы вы
               получили доступ к диалоговым окнам Windows.

               Когда соединение будет установлено, вы сможете вернуться в полноэкранный режим
               нажав 'F4'. */
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
    gbRemoteOn = true;
    saddr_loc.sin_family = AF_INET;
    saddr_loc.sin_port = htons(WS_TRANSPORT_PORT);
    saddr_loc.sin_addr.s_addr = htonl(INADDR_ANY);
    iRc = bind(sd_dg, reinterpret_cast<struct sockaddr*>(&saddr_loc), sizeof(saddr_loc));
    if (iRc == SOCKET_ERROR) {
        sprintf(cWSTextBuffer, "Error During bind(): %d", WSAGetLastError());
        ShutDown(cWSTextBuffer);
    }
    blockMode = 1;
    iRc = ioctlsocket(sd_dg, FIONBIO, &blockMode);
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
        giWaitType = DIALOG_WAIT_WINSOCK_FIRST_GUEST;
        if (giTCPHostStatus != -1) {
            sprintf(
                cWSTextBuffer,
                /* Создание игры по адресу %s.

                   У вас %d гостей из ожидавшихся %d гостей. Нажмите 'ОТМЕНА', чтобы продолжить
                   игру, не дожидаясь  остальных гостей. */
                "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xef\xee \xe0\xe4\xf0\xe5\xf1\xf3 %s.\n\n"
                    "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9 \xe8\xe7 \xee\xe6\xe8\xe4\xe0\xe2\xf8\xe8\xf5\xf1\xff %d \xe3\xee\xf1\xf2\xe5\xe9. "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xed\xe5 \xe4\xee\xe6\xe8\xe4\xe0\xff\xf1\xfc  "
                    "\xee\xf1\xf2\xe0\xeb\xfc\xed\xfb\xf5 \xe3\xee\xf1\xf2\xe5\xe9.",
                inet_ntoa(gIn_addrIP),
                0,
                giTCPNumPlayers - 1
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            sprintf(
                cWSTextBuffer,
                /* Открытие игры на %s

                   Ожидание гостя(ей).

                     Нажмите 'ОТМЕНА', чтобы прервать соединение. */
                "\xce\xf2\xea\xf0\xfb\xf2\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s\n\n"
                    "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe3\xee\xf1\xf2\xff(\xe5\xe9).\n\n  "
                    "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.",
                inet_ntoa(gIn_addrIP)
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
                    /* Создание игры по адресу %s.

                       У вас %d гостей из ожидавшихся %d гостей. Нажмите 'ОТМЕНА', чтобы продолжить
                       игру, не дожидаясь  остальных гостей. */
                    "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xef\xee \xe0\xe4\xf0\xe5\xf1\xf3 %s.\n\n"
                        "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9 \xe8\xe7 \xee\xe6\xe8\xe4\xe0\xe2\xf8\xe8\xf5\xf1\xff %d \xe3\xee\xf1\xf2\xe5\xe9. "
                        "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc \xe8\xe3\xf0\xf3, \xed\xe5 \xe4\xee\xe6\xe8\xe4\xe0\xff\xf1\xfc  "
                        "\xee\xf1\xf2\xe0\xeb\xfc\xed\xfb\xf5 \xe3\xee\xf1\xf2\xe5\xe9.",
                    inet_ntoa(gIn_addrIP),
                    giNumHumanPlayers - 1,
                    giTCPNumPlayers - 1
                );
                NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        } else {
            sprintf(
                cWSTextBuffer,
                /* Создание игры на %s.

                   У вас %d гостей. Нажмите 'ОК', чтобы продолжить или подождите других игроков. */
                "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s.\n\n"
                    "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc "
                    "\xe8\xeb\xe8 \xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2.",
                inet_ntoa(gIn_addrIP),
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
                /* Введите IP адрес сервера.
                   (Например: 220.415.119.223) */
                "\xc2\xe2\xe5\xe4\xe8\xf2\xe5 IP \xe0\xe4\xf0\xe5\xf1 \xf1\xe5\xf0\xe2\xe5\xf0\xe0.\n"
                    "(\xcd\xe0\xef\xf0\xe8\xec\xe5\xf0: 220.415.119.223)",
                cWSTextBuffer,
                IP_ADDRESS_ENTRY_LIMIT,
                NULL,
                0,
                1
            );
        }
        giNetPosToDCOPos[0] = static_cast<i32>(inet_addr(cWSTextBuffer));
        if (giNetPosToDCOPos[0] == static_cast<i32>(INADDR_NONE)) {
            NormalDialog(
                /* Неправильный IP адрес. Попробуйте еще раз. */
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
            /* Поиск сервера. */ "\xcf\xee\xe8\xf1\xea \xf1\xe5\xf0\xe2\xe5\xf0\xe0."
        );
        NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
    }
    return 0;
}

VA(0x004b2262, 0xb3)
void wsnet_term(void) {
    if (sd_dg != INVALID_SOCKET)
        closesocket(sd_dg);
    if (ppDPRcvBuffer != NULL)
        H2_FREE(ppDPRcvBuffer);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize);
    piDPRcvBufferSize = NULL;
    WSACleanup();
    bHostFound = false;
    sd_dg = INVALID_SOCKET;
    iWSLastMsgNumHumanPlayers = 1;
    iWSAttempts = 0;
    iWSNextTickCount = 0;
    iWSWaitForHostStatus = 0;
    CleanupDPVars();
}

VA(0x004b2315, 0x1d3)
void wsSendMessage(
    i32 destination,
    H2_ENUM_PARAM(NetworkPacketType, u8) type,
    u16 size,
    void* data
) {
    u8* packetBuffer = static_cast<u8*>(H2_ALLOC(size + 1));
    i32 attemptCount;
    struct sockaddr_in remote;
    i32 netPlayer;
    i32 error;

    packetBuffer[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(WS_TRANSPORT_PORT);
    if (destination == 0) {
        for (netPlayer = 0; netPlayer < giNumHumanPlayers; netPlayer++) {
            if (netPlayer == giThisNetPos)
                continue;
            attemptCount = 0;
        sendPacket:
            remote.sin_addr.s_addr = giNetPosToDCOPos[netPlayer];
            iRc = sendto(
                sd_dg,
                reinterpret_cast<char*>(packetBuffer),
                size + 1,
                0,
                reinterpret_cast<struct sockaddr*>(&remote),
                sizeof(remote)
            );
            if (iRc == SOCKET_ERROR) {
                error = WSAGetLastError();
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
        remote.sin_addr.s_addr = destination;
        iRc = sendto(
            sd_dg,
            reinterpret_cast<char*>(packetBuffer),
            size + 1,
            0,
            reinterpret_cast<struct sockaddr*>(&remote),
            sizeof(remote)
        );
        if (iRc == SOCKET_ERROR) {
            sprintf(cWSTextBuffer, "Error During sendto(): %d", WSAGetLastError());
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_WAIT_FIRST, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
    }
    H2_FREE(packetBuffer);
}

VA(0x004b24e8, 0x4d)
i32 wsnet_snd(i32 destination, i32 size, void* data) {
    i32 H2_UNUSED(result);

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

VA(0x004b2535, 0x93)
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

VA(0x004b25c8, 0x8b)
void wsProcessMessages(void) {
    struct sockaddr_in remote;
    i32 addressLength = sizeof(remote);
    i32 bufLen;

    for (;;) {
        bufLen = WS_TRANSPORT_BUFFER_SIZE;
        iRc = recvfrom(
            sd_dg,
            rcvBufIn,
            bufLen,
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
        if (static_cast<i32>(remote.sin_addr.s_addr) == giNetPosToDCOPos[giThisNetPos]) {
        } else {
            wsEvaluateMessage(iRc, static_cast<i32>(remote.sin_addr.s_addr));
        }
    }
}

VA(0x004b2653, 0x344)
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
            sprintf(
                cWSTextBuffer,
                "\xd1\xe5\xf0\xe2\xe5\xf0 \xf3\xe6\xe5 \xf1\xee\xe7\xe4\xe0\xeb \xe8\xe3\xf0\xf3 "
                "\xe8 \xed\xe5 \xef\xf0\xe8\xed\xe8\xec\xe0\xe5\xf2 \xed\xee\xe2\xfb\xf5 "
                "\xe8\xe3\xf0\xee\xea\xee\xe2." /* "Сервер уже создал игру и не принимает новых игроков." */
            );
            NormalDialog(cWSTextBuffer, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            ShutDown(NULL);
            break;
        case NETWORK_PACKET_GUEST_ACCEPTED:
            sprintf(
                cWSTextBuffer,
                "\xce\xe6\xe8\xe4\xe0\xfe \xe8\xe3\xf0\xee\xea\xe0 \xe4\xeb\xff "
                "\xed\xe0\xf7\xe0\xeb\xe0 \xe8\xe3\xf0\xfb." /* "Ожидаю игрока для начала игры." */
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
            sprintf(cWSTextBuffer, "Unknown message: %d\n", static_cast<i32>(rcvBufIn[0]));
            if (giDebugLevel > 0) {
                sprintf(gText, cWSTextBuffer);
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            LogStr(cWSTextBuffer);
            break;
    }
}

VA(0x004b2997, 0x1c)
i32 wsWaitForFirstGuest(void) {
    wsProcessMessages();
    if (giNumHumanPlayers > 1)
        return 1;
    return 0;
}

VA(0x004b29b3, 0xaa)
i32 wsWaitForExtraGuests(void) {
    tag_message message;

    wsProcessMessages();
    if (giNumHumanPlayers != iWSLastMsgNumHumanPlayers) {
        if (giTCPHostStatus != -1 && giNumHumanPlayers >= giTCPNumPlayers)
            return 1;
        iWSLastMsgNumHumanPlayers = giNumHumanPlayers;
        sprintf(
            cWSTextBuffer,
            /* Создание игры на %s.

               У вас %d гостей. Нажмите 'ОК', чтобы продолжить или подождите других игроков. */
            "\xd1\xee\xe7\xe4\xe0\xed\xe8\xe5 \xe8\xe3\xf0\xfb \xed\xe0 %s.\n\n"
                "\xd3 \xe2\xe0\xf1 %d \xe3\xee\xf1\xf2\xe5\xe9. \xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xca', \xf7\xf2\xee\xe1\xfb \xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc "
                "\xe8\xeb\xe8 \xef\xee\xe4\xee\xe6\xe4\xe8\xf2\xe5 \xe4\xf0\xf3\xe3\xe8\xf5 \xe8\xe3\xf0\xee\xea\xee\xe2.",
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

VA(0x004b2a5d, 0xfd)
i32 wsWaitForHost(void) {
    switch (iWSWaitForHostStatus) {
        case 0:
            if (iWSNextTickCount > KBTickCount())
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
                sprintf(
                    cWSTextBuffer,
                    /* Сервер не отвечает. Продолжить ожидание? */ "\xd1\xe5\xf0\xe2\xe5\xf0 \xed\xe5 \xee\xf2\xe2\xe5\xf7\xe0\xe5\xf2. "
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

DATA(0x005347b0) b32 bHostFound = false;
DATA(0x0051aa9c) u32 sd_dg = INVALID_SOCKET;
DATA(0x0051aaa0) i32 iWSLastMsgNumHumanPlayers = 1;
DATA(0x005347b4) i32 iWSAttempts = 0;
DATA(0x005347b8) i32 iWSNextTickCount = 0;
DATA(0x005347bc) i32 iWSWaitForHostStatus = 0;
DATA(0x005344e8) i32 iRc;
DATA(0x0053469c) char cWSTextBuffer[WS_TRANSPORT_BUFFER_SIZE];
DATA(0x00534688) struct sockaddr_in saddr_loc;
DATA(0x00534698) u16 wVer;
DATA(0x005344f8) struct WSAData wsadata;
DATA(0x005344f0) struct in_addr gIn_addrIP;
DATA(0x005347a0) struct sockaddr_in saddr_remote;
DATA(0x005344ec) i32 iAddrLen;
