#include <stdio.h>
#include <string.h>
#include <Ints.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/comwin.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/dpnetwin.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/Modem.h>
#include <SOURCE/Netbios.h>
#include <SOURCE/netwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/Wsnetwin.h>
#include <SOURCE/X_GLOBAL.h>

typedef enum RemoteImplementationConstant {
    CRC_FEEDBACK_BIT                 = 0x08000000,
    LOCAL_NET_NAME_BUFFER_SIZE       = 64,
    NET_NAME_INPUT_LIMIT             = NET_PLAYER_INFO_NAME_SIZE - 1,
    DIRECT_LINK_PLAYER_COUNT         = 2,
    MESSAGE_ID_PLAYER_STRIDE         = 100000000,
    CRC_WORD_BIT_COUNT               = 16,
    CRC_STORAGE_WORD_COUNT           = 2,
    GET_REMOTE_DATA_FREE_LINE_OFFSET = 25,
    POLL_REMOTE_ALLOC_LINE_OFFSET    = 235
} RemoteImplementationConstant;

enum class RemoteSetupCommand : i32 {
    SETUP_PLAYER_INFO   = 0x22,
    SETUP_STANDARD_GAME = 0x3d,
    SETUP_CAMPAIGN_GAME = 0x3e
};
using enum RemoteSetupCommand;

#define REMOTE_PACKET(buffer) (reinterpret_cast<RemotePacketHeader*>(buffer))
#define REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define REMOTE_PLAYER_INFO(message) (reinterpret_cast<SNetPlayerInfo*>((message)->payload))

i32 iInOrderCtr = 0;
i32 iCurLastID = 0;
i32 giLastConfirm = -1;
H2EnumStorage<RemoteGameMode, u8> GameMode = REMOTE_GAME_NONE;
i32l lLastHeartbeatSend = 0;
b32 gbInRemoteMain = false;
b32 gbInRemoteCleanup = false;
i32 iIDCtr = 0;
i32 iTimesDropped = 0;
b8 gbInNetSetup = false;
i32 bUseDirectPlay = 0;
i32 bUseWinsock = 0;
i8 bInTimeoutFail = 0;
i32 iBaud[REMOTE_BAUD_RATE_COUNT] = {300, 1200, 2400, 9600, 19200, 38400, 57600};
i32 iIRQ[REMOTE_IRQ_COUNT] = {1, 2, 3, 4, 5, 7, 9};

void RemoteCleanup(void) {
    LogStr("RC1");
    if (gbRemoteOn == 0)
        return;
    LogStr("RC2");
    if (gbInRemoteMain != 0)
        return;
    if (gbInRemoteCleanup != 0)
        return;
    gbInRemoteCleanup = true;
    LogStr("RC3");
    switch (GameMode) {
        case REMOTE_GAME_NETWORK_HOST:
        case REMOTE_GAME_NETWORK_GUEST:
            UnloadRemoteDriver(1);
            break;
        case REMOTE_GAME_MODEM_HOST:
        case REMOTE_GAME_MODEM_GUEST:
            UnloadRemoteDriver(0);
            break;
        default:
            break;
    }
    gbRemoteOn = false;
    xNetHasOldPlayers = 0;
    iInOrderCtr = 0;
    iCurLastID = 0;
    giLastConfirm = -1;
    GameMode = REMOTE_GAME_NONE;
    lLastHeartbeatSend = 0;
    gbInRemoteMain = false;
    iIDCtr = 0;
    iTimesDropped = 0;
    bUseDirectPlay = 0;
    bUseWinsock = 0;
    bInTimeoutFail = 0;
    bUseDirectPlay = 0;
    bUseWinsock = 0;
    bInTimeoutFail = 0;
    iMPNetProtocol = REMOTE_PROTOCOL_NETBIOS;
    iLastDiffSendTo = DIFF_SEND_FORCE_WHOLE;
    gbGotFirstHeartbeat = false;
    gbInRemoteCleanup = false;
}

void RemoteMain(RemoteGameMode gameMode) {
    i8 gotPlayers[REMOTE_PLAYER_COUNT];
    i32 playerState;
    char* gameMsg;
    char* recvData;
    i32 netPlayer;
    i32 pending;
    i32 savedColorMice;
    i32 player;
    i32 setupCounter;

    gbInRemoteMain = true;
    bGotGameType = 0;
    LogStr("In Remote Main");
    LogStr("RM 1");
    for (player = 0; player < REMOTE_PLAYER_COUNT; player++) {
        lLastHeartbeatReceive[player] = REMOTE_INITIAL_HEARTBEAT;
        sprintf(
            gsNetPlayerInfo[player].name,
              "\xc8\xe3\xf0\xee\xea\x20\x25\x64",
            player + 1
        );
    }
    LogStr("RM 2");
    gbRemoteGameOpen = true;
    savedColorMice = bLastMouseOffscreen != 0 ? bLastOnscreenMouseColor : gbColorMice;
    gpMouseManager->SetColorMice(0);
    LogStr("RM 3");
    gbInNetSetup = true;
    if (iMPNetProtocol == REMOTE_PROTOCOL_DIRECT_PLAY)
        bUseDirectPlay = 1;
    else if (iMPNetProtocol == REMOTE_PROTOCOL_WINSOCK)
        bUseWinsock = 1;
    else {
        bUseWinsock = 0;
        bUseDirectPlay = bUseWinsock;
    }
    LogStr("RM 4");
    memset(sMapChangeQueue, 0, sizeof(sMapChangeQueue));
    for (player = 0; player < REMOTE_QUEUE_CAPACITY; player++)
        rcvBuf[player] = NULL;
    LogStr("RM 5");
    memset(iLastIds, 0, REMOTE_RECENT_ID_COUNT);
    GameMode = gameMode;
    LogStr("RM 6");
    memset(gsNetPlayerInfo, 0, sizeof(gsNetPlayerInfo));
    memset(&gsThisNetPlayerInfo, 0, sizeof(gsThisNetPlayerInfo));
    LogStr("RM 7");
    if (giTCPHostStatus != -1) {
        if (strlen(gcTCPName) > 0)
            strcpy(gsThisNetPlayerInfo.name, gcTCPName);
        else
            strcpy(gsThisNetPlayerInfo.name, gConfig.networkDefaultName);
    } else {
        GetDataEntry(

            "\xcf\xee\xe6\xe0\xeb\xf3\xe9\xf1\xf2\xe0, \xe2\xe2\xe5\xe4\xe8\xf2\xe5 \xe8\xec\xff, \xef\xee\xe4 \xea\xee\xf2\xee\xf0\xfb\xec \xe2\xfb \xe1\xf3\xe4\xe5\xf2\xe5 \xe8\xe7\xe2\xe5\xf1\xf2\xed\xfb.",
            gsThisNetPlayerInfo.name,
            NET_NAME_INPUT_LIMIT,
            gConfig.networkDefaultName,
            1,
            0
        );
    }
    strcpy(gConfig.networkDefaultName, gsThisNetPlayerInfo.name);
    WritePrefs();
    strcpy(gsThisNetPlayerInfo.uniqueSystemID, gConfig.uniqueSystemID);
    gsThisNetPlayerInfo.connectionType = NET_PLAYER_CONNECTION_CURRENT;
    gsThisNetPlayerInfo.useRegularCompression = 1;
    gsThisNetPlayerInfo.useDiffCompression = 1;
    gsThisNetPlayerInfo.reserved[0] = 1;
    xNetHasOldPlayers = 0;

    switch (gameMode) {
                    case REMOTE_GAME_NETWORK_HOST:
            gsNetPlayerInfo[0] = gsThisNetPlayerInfo;
            giThisNetPos = 0;
            goto initializeNetwork;
                    case REMOTE_GAME_NETWORK_GUEST:
            giThisNetPos = 1;
        initializeNetwork:
            if (bUseDirectPlay != 0) {
                gbRemoteOn = true;
                dpnet_init();
            } else if (bUseWinsock != 0) {
                wsnet_init();
                gbRemoteOn = true;
            } else {
                nbnet_init();
                gbRemoteOn = true;
            }
            break;
                    case REMOTE_GAME_MODEM_HOST:
            LogStr("MH1");
            gbRemoteOn = true;
            gsNetPlayerInfo[0] = gsThisNetPlayerInfo;
            giThisNetPos = 0;
            ModemSetup(H2EnumIndex(gameMode));
            LogStr("MH2");
            break;
                    case REMOTE_GAME_MODEM_GUEST:
            gbRemoteOn = true;
            giThisNetPos = 1;
            ModemSetup(H2EnumIndex(gameMode));
            break;
    }
    if (bUseDirectPlay == 0 && bUseWinsock == 0)
        giNumHumanPlayers = DIRECT_LINK_PLAYER_COUNT;
    iIDCtr = (giThisNetPos + 1) * MESSAGE_ID_PLAYER_STRIDE;
    gbInNetSetup = false;
    gpMouseManager->SetColorMice(savedColorMice);

    if (bUseDirectPlay == 0 && bUseWinsock == 0) {
        LogStr("RM 2");
        if (giThisNetPos == 0) {
            pending = 1;
            memset(gotPlayers, 0, REMOTE_PLAYER_COUNT);
            while (pending != 0) {
                PollSound();
                LogStr("RM 3");
                recvData = GetRemoteData(1);
                LogStr("RM 4");
                if (recvData != NULL
                    && REMOTE_MESSAGE(recvData)->type == REMOTE_MESSAGE_RELIABLE) {
                    switch (static_cast<RemoteSetupCommand>(REMOTE_MESSAGE(recvData)->command)) {
                        case SETUP_PLAYER_INFO:
                            netPlayer = REMOTE_MESSAGE(recvData)->sender;
                            gsNetPlayerInfo[netPlayer] =
                                *REMOTE_PLAYER_INFO(REMOTE_MESSAGE(recvData));
                            gotPlayers[netPlayer] = 1;
                            if (gsNetPlayerInfo[netPlayer].reserved[0] == 0)
                                xNetHasOldPlayers = 1;
                            break;
                    }
                }
                pending = 0;
                for (player = 1; player < giNumHumanPlayers; player++) {
                    if (gotPlayers[player] == 0)
                        pending = 1;
                }
            }
        } else {
            LogStr("RM 5");
            TransmitRemoteData(
                reinterpret_cast<char*>(&gsThisNetPlayerInfo),
                0,
                sizeof(SNetPlayerInfo),
                H2EnumIndex(SETUP_PLAYER_INFO),
                1,
                1,
                REMOTE_MESSAGE_DEFAULT
            );
            LogStr("RM 6");
        }
    }
    gameMsg = NULL;
    if (giThisNetPos == 0) {
        TransmitRemoteData(
            NULL,
            REMOTE_BROADCAST_PLAYER,
            0,
            static_cast<i8>(
                giSetupGameType == 1 ? SETUP_CAMPAIGN_GAME : SETUP_STANDARD_GAME
            ),
            1,
            1,
            REMOTE_MESSAGE_DEFAULT
        );
    } else {
        while (bGotGameType == 0) {
            PollSound();
            gameMsg = GetRemoteData(1);
            if (gameMsg != NULL
                && REMOTE_MESSAGE(gameMsg)->type == REMOTE_MESSAGE_RELIABLE) {
                setupCounter = 0;
                setupCounter++;
                setupCounter++;
                setupCounter++;
            }
            if (gameMsg != NULL
                && REMOTE_MESSAGE(gameMsg)->type == REMOTE_MESSAGE_RELIABLE
                && REMOTE_MESSAGE(gameMsg)->command == H2EnumIndex(SETUP_CAMPAIGN_GAME)) {
                bGotGameType = 1;
                giSetupGameType = 1;
            }
            if (gameMsg != NULL
                && REMOTE_MESSAGE(gameMsg)->type == REMOTE_MESSAGE_RELIABLE
                && REMOTE_MESSAGE(gameMsg)->command == H2EnumIndex(SETUP_STANDARD_GAME)) {
                bGotGameType = 1;
                giSetupGameType = 0;
            }
        }
    }
    LogStr("Out Remote Main");
    gbInRemoteMain = false;
}

void UnloadRemoteDriver(i16 networkDriver) {
    switch (networkDriver) {
        case 0:
            com_term(0);
            break;
        case 1:
            if (bUseDirectPlay != 0)
                dpnet_term();
            else if (bUseWinsock != 0)
                wsnet_term();
            else
                nb_term();
            break;
    }
}

i32 calc_crc_long(u8* data, i32 length) {
    u32 q;
    u32 cksum;
    u32 sum;
    i32 c;

    cksum = 0;
    sum = 0;
    c = 0;
    while (length-- != 0) {
        q = cksum & CRC_FEEDBACK_BIT;
        cksum <<= 1;
        cksum += *data;
        sum += *data;
        if (q != 0)
            cksum++;
        data++;
    }
    cksum += sum >> CRC_WORD_BIT_COUNT;
    cksum += sum << CRC_WORD_BIT_COUNT;
    return cksum;
}

void calc_crc(u16* crc, u8* data, i32 length) {
    *crc = static_cast<u16>(calc_crc_long(data, length));
}

i32 EncodePacket(u8* data, char source, char destination, i32 length) {
    u16 crc[CRC_STORAGE_WORD_COUNT];

    REMOTE_PACKET(PacketSend)->source = source;
    REMOTE_PACKET(PacketSend)->destination = destination;
    REMOTE_PACKET(PacketSend)->reserved = 0;
    REMOTE_PACKET(PacketSend)->payloadSize = static_cast<char>(length);
    crc[0] = 0;
    REMOTE_PACKET(PacketSend)->crc = crc[0];
    memcpy(PacketSend + REMOTE_PACKET_HEADER_SIZE, data, length);
    calc_crc(crc, reinterpret_cast<u8*>(PacketSend), length + REMOTE_PACKET_HEADER_SIZE);
    REMOTE_PACKET(PacketSend)->crc = crc[0];
    return length + REMOTE_PACKET_HEADER_SIZE;
}

i32 DecodePacket(u8* data, i32) {
    u16 crc;
    i32 res;
    u16 crc2[CRC_STORAGE_WORD_COUNT];
    char text[REMOTE_ERROR_TEXT_SIZE];
    u32 len;

    crc2[0] = 0;
    if (REMOTE_PACKET(packet)->destination != giThisNetPos
        && REMOTE_PACKET(packet)->destination != REMOTE_BROADCAST_PLAYER) {
        sprintf(
            text,
            "not mine %d\n",
            REMOTE_PACKET(packet)->destination
        );
        LogStr(text);
        return 0;
    }
    len = static_cast<u8>(REMOTE_PACKET(packet)->payloadSize);
    crc = REMOTE_PACKET(packet)->crc;
    REMOTE_PACKET(packet)->crc = 0;
    calc_crc(crc2, reinterpret_cast<u8*>(packet), len + REMOTE_PACKET_HEADER_SIZE);
    if (crc != crc2[0]) {
        sprintf(
            text,
            "CRC Check Failed CRC 1 %d CRC 2 %d\n",
            crc,
            crc2[0]
        );
        LogStr(text);
        return 0;
    }
    memcpy(data, packet + REMOTE_PACKET_HEADER_SIZE, len);
    return 1;
}

i32 SendRemoteData(u8* dataToSend, u8*, i32 destination, i32 length) {
    i32 size;
    i32 out;
    i32 retry;
    i32 sendStatus;
    u8 remotePacket[REMOTE_MESSAGE_SIZE];

    out = 1;
    if (destination == REMOTE_BROADCAST_PLAYER && bUseDirectPlay == 0 && bUseWinsock == 0) {
        destination = 1 - giThisNetPos;
    }
    size = EncodePacket(
        dataToSend,
        static_cast<char>(giThisNetPos),
        static_cast<char>(destination),
        length
    );
    switch (GameMode) {
        case REMOTE_GAME_NETWORK_HOST:
        case REMOTE_GAME_NETWORK_GUEST:
            if (bUseDirectPlay != 0) {
                sendStatus = dpnet_snd(destination, size, PacketSend);
            } else if (bUseWinsock != 0) {
                sendStatus = wsnet_snd(destination, size, PacketSend);
            } else {
                do {
                    sendStatus = static_cast<i16>(
                        nb_snd(static_cast<i16>(destination), static_cast<i16>(size), PacketSend)
                    );
                    if (sendStatus != 0) {
                        LogInt(
                            "Bad return on Send Data",
                            destination,
                            sendStatus,
                            size,
                            0,
                            0,
                            LOG_UNUSED_VALUE,
                            LOG_UNUSED_VALUE
                        );
                        out = 0;
                        goto finished;
                    }
                } while (sendStatus != 0);
            }
            break;
        case REMOTE_GAME_MODEM_HOST:
        case REMOTE_GAME_MODEM_GUEST:
            WriteModemPacket(PacketSend, size);
            out = 1;
            break;
    }
finished:
    return out;
}

i32 ReceiveRemoteData(u8*, u8* data, i32 decodeType) {
    i32 result;
    i32 receiveResult;

    result = 1;
    switch (GameMode) {
        case REMOTE_GAME_NETWORK_HOST:
        case REMOTE_GAME_NETWORK_GUEST:
            if (bUseDirectPlay != 0) {
                receiveResult = dpnet_rcv(0, REMOTE_RECEIVE_BUFFER_SIZE, packet);
                if (receiveResult == 0)
                    return 0;
                result = DecodePacket(data, decodeType);
            } else if (bUseWinsock != 0) {
                receiveResult = wsnet_rcv(0, REMOTE_RECEIVE_BUFFER_SIZE, packet);
                if (receiveResult == 0)
                    return 0;
                result = DecodePacket(data, decodeType);
            } else {
                receiveResult = static_cast<i16>(nb_rcv(REMOTE_RECEIVE_BUFFER_SIZE, packet));
                if (receiveResult == 0)
                    return 0;
                result = DecodePacket(data, decodeType);
            }
            break;
        case REMOTE_GAME_MODEM_HOST:
        case REMOTE_GAME_MODEM_GUEST:
            receiveResult = ReadPacket();
            if (receiveResult == 0)
                return 0;
            result = DecodePacket(data, decodeType);
            break;
    }
    return result;
}

i32 TransmitRemoteData(
    char* data,
    i32 destination,
    i32 length,
    i8 command,
    i8 reliable,
    i8 allowRetryDialog,
    RemoteMessageType messageType
) {
    i32 rv;
    i32 i;
    i32 j;
    RemoteMessage msg;
    i32 tries;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    rv = 0;
    tries = 0;
    iIDCtr++;
    msg.sender = static_cast<i8>(giThisNetPos);
    msg.id = iIDCtr;
    if (messageType != REMOTE_MESSAGE_DEFAULT) {
        msg.type = messageType;
    } else {
        msg.type = reliable != 0 ? REMOTE_MESSAGE_RELIABLE : REMOTE_MESSAGE_UNRELIABLE;
    }
    msg.payloadSize = static_cast<u16>(length);
    msg.command = command;
    if (length > 0)
        memcpy(msg.payload, data, length);
    while (rv == 0 && tries <= REMOTE_RETRY_COUNT) {
        rv = SendRemoteData(
            reinterpret_cast<u8*>(&msg),
            NULL,
            destination,
            length + REMOTE_MESSAGE_HEADER_SIZE
        );
        if (reliable == 0 && rv != 0) {
            return 1;
        } else if (rv != 0) {
            i = 0;
            while (i < REMOTE_CONFIRM_POLL_COUNT) {
                ForcePollSound();
                if (giLastConfirm == iIDCtr)
                    return 1;
                rv = 0;
                DelayMilli(REMOTE_CONFIRM_POLL_DELAY);
                i++;
            }
        } else {
            DelayMilli(REMOTE_SEND_RETRY_DELAY);
        }
        if (allowRetryDialog != 0 && tries == REMOTE_RETRY_COUNT && rv == 0) {
            NormalDialog(
                  "\xce\xf8\xe8\xe1\xea\xe0\x20\xef\xe5\xf0\xe5\xf1\xfb\xeb\xea\xe8\x20\xe4\xe0\xed\xed\xfb\xf5\x2e\x20\xcf\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x3f",
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                tries = -1;
        }
        tries++;
    }
    return rv;
}

char* GetRemoteData(i8 remove) {
    i32 oldestOrder;
    i32 queueIndex;
    i32 selected;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return NULL;
    oldestOrder = REMOTE_ORDER_SENTINEL;
    selected = -1;
    for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
        if (rcvBuf[queueIndex] != NULL && iInOrder[queueIndex] < oldestOrder) {
            oldestOrder = iInOrder[queueIndex];
            selected = queueIndex;
        }
    }
    if (selected >= 0) {
        memcpy(rcvBufOut, rcvBuf[selected], REMOTE_MESSAGE_SIZE);
        if (remove != 0) {
            H2_FREE(rcvBuf[selected]);
            rcvBuf[selected] = NULL;
        }
        return rcvBufOut;
    }
    return NULL;
}

void PollRemote(void) {
    i32 oldInPoll;
    i32 rc;
    i32 cnt;
    i32 queueIndex;
    i8 qFull;
    SPlayerExit hostExit;
    i32 timeout;
    SPlayerExit guestExit;
    u8 cmdByte;

    if (gbRemoteOn == 0)
        return;
    if (gbInRemoteCleanup != 0)
        return;
    if (iMPBaseType == MULTIPLAYER_BASE_MODEM) {
        comm_wrt_task();
    } else if (iMPBaseType == MULTIPLAYER_BASE_NETWORK) {
        if (bUseDirectPlay != 0)
            dpProcessMessages();
        else if (bUseWinsock != 0)
            wsProcessMessages();
        else
            nb_thr_ctl();
    }
    if (gbInNetSetup != 0)
        return;
    cnt = 0;
    oldInPoll = gbInPollSound;
    qFull = 0;
    if (KBTickCount() - lLastHeartbeatSend > REMOTE_HEARTBEAT_INTERVAL) {
        REMOTE_MESSAGE(sndBuf)->sender = static_cast<i8>(giThisNetPos);
        REMOTE_MESSAGE(sndBuf)->type = REMOTE_MESSAGE_HEARTBEAT;
        REMOTE_MESSAGE(sndBuf)->payloadSize = 0;
        if (gbThisNetGotAdventureControl != 0) {
            REMOTE_MESSAGE(sndBuf)->command = static_cast<i8>(
                ((giCurPlayer + 1) << REMOTE_HEARTBEAT_PLAYER_SHIFT)
                | REMOTE_HEARTBEAT_CONTROL_FLAG | iCurHourGlassPhase
            );
        } else {
            REMOTE_MESSAGE(sndBuf)->command = 0;
        }
        SendRemoteData(
            reinterpret_cast<u8*>(sndBuf),
            NULL,
            giThisNetPos != 0 && gbThisNetGotAdventureControl == 0
                ? 0
                : REMOTE_BROADCAST_PLAYER,
            REMOTE_HEARTBEAT_MESSAGE_SIZE
        );
        lLastHeartbeatSend = KBTickCount();
    }

    if (giThisNetPos == 0) {
        for (queueIndex = 0; queueIndex < giNumHumanPlayers; queueIndex++) {
            if (queueIndex != giThisNetPos
                && KBTickCount() > lLastHeartbeatReceive[queueIndex] + REMOTE_HOST_TIMEOUT
                && bInTimeoutFail == 0) {
                bInTimeoutFail = 1;
                gbInPollSound = false;
                sprintf(
                    gText,

                    "\xca\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\x27\x25\x73\x27\x20\xed\xe5\x20\xee\xf2\xe2\xe5\xf7\xe0\xe5\xf2\x20\xed\xe0\x20\xe7\xe0\xef\xf0\xee\xf1\xfb\x2e\x20\xc6\xe5\xeb\xe0\xe5\xf2\xe5\x20\xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xed\xe8\xe5\x20\xee\xf2\xe2\xe5\xf2\xe0\x3f",
                    gsNetPlayerInfo[queueIndex].name
                );
                NormalDialog(
                    gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                    lLastHeartbeatReceive[queueIndex] = KBTickCount();
                } else {
                    hostExit.netPosition = static_cast<i8>(queueIndex);
                    hostExit.gamePosition = static_cast<i8>(NetPosToGamePos(queueIndex));
                    hostExit.updateNetworkControl = 1;
                    hostExit.timedOut = 1;
                    hostExit.eliminated = 0;
                    ReceiveRemotePlayerExit(hostExit);
                }
                gbInPollSound = oldInPoll;
                bInTimeoutFail = 0;
            }
        }
    } else {
        timeout = REMOTE_GUEST_TIMEOUT;
        if (giThisNetPos != 1)
            timeout += REMOTE_CHAIN_GUEST_TIMEOUT_INCREMENT;
        if (KBTickCount() > lLastHeartbeatReceive[0] + timeout && bInTimeoutFail == 0) {
            bInTimeoutFail = 1;
            gbInPollSound = false;
            if (giThisNetPos == 1) {
                sprintf(
                    gText,

                    "\xca\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\x27\x25\x73\x27\x20\xed\xe5\x20\xee\xf2\xe2\xe5\xf7\xe0\xe5\xf2\x20\xed\xe0\x20\xe7\xe0\xef\xf0\xee\xf1\xfb\x2e\x20\xc6\xe5\xeb\xe0\xe5\xf2\xe5\x20\xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xed\xe8\xe5\x20\xee\xf2\xe2\xe5\xf2\xe0\x3f",
                    gsNetPlayerInfo[0].name
                );
            } else {
                sprintf(
                    gText,

                    "\xd3\xe4\xe0\xeb\xe5\xed\xed\xee\xe5\x20\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5\x20\xf1\x20\xe4\xf0\xf3\xe3\xe8\xec\xe8\x20\xe8\xe3\xf0\xee\xea\xe0\xec\xe8\x20\xef\xf0\xe5\xf0\xe2\xe0\xed\xee\x2e\x20\xc6\xe5\xeb\xe0\xe5\xf2\xe5\x20\xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xee\xe6\xe8\xe4\xe0\xed\xe8\xe5\x20\xee\xf2\xe2\xe5\xf2\xe0\x3f"
                );
            }
            NormalDialog(
                gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                lLastHeartbeatReceive[0] = KBTickCount();
            } else if (giThisNetPos == 1) {
                guestExit.netPosition = 0;
                guestExit.gamePosition = static_cast<i8>(NetPosToGamePos(0));
                guestExit.updateNetworkControl = 1;
                guestExit.timedOut = 1;
                guestExit.eliminated = 0;
                ReceiveRemotePlayerExit(guestExit);
            } else {
                gpGame->SaveGame(
                      "\xc8\xe3\xf0\xee\xea\x20\xc2\xfb\xf8\xe5\xeb",
                    1,
                    0
                );
                sprintf(
                    gText,

                    "\xc4\xe0\xed\xed\xe0\xff\x20\xe8\xe3\xf0\xe0\x20\xf1\xee\xf5\xf0\xe0\xed\xe5\xed\xe0\x20\xef\xee\xe4\x20\xed\xe0\xe7\xe2\xe0\xed\xe8\xe5\xec\x20\x27\xc8\xe3\xf0\xee\xea\x20\xe2\xfb\xf8\xe5\xeb\x27\x2e\x20\xc6\xe5\xeb\xe0\xe5\xf2\xe5\x20\xef\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x20\xe8\xe3\xf0\xf3\x2c\x20\xe3\xe4\xe5\x20\xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\x20\xe7\xe0\xe9\xec\xe5\xf2\x20\xec\xe5\xf1\xf2\xee\x20\xe2\xfb\xe1\xfb\xe2\xf8\xe8\xf5\x20\xe8\xe3\xf0\xee\xea\xee\xe2\x3f"
                );
                NormalDialog(
                    gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                    DropDownToOnePlayer();
                else
                    ShutDown("");
            }
            gbInPollSound = oldInPoll;
            bInTimeoutFail = 1;
        }
    }

    for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
        if (rcvBuf[queueIndex] != NULL)
            cnt++;
    }
    if (cnt == REMOTE_QUEUE_CAPACITY)
        qFull = 1;
    rc = 1;
    while (rc != 0) {
    nextIncoming:
        rc =
            ReceiveRemoteData(NULL, reinterpret_cast<u8*>(rcvBufIn), REMOTE_BROADCAST_PLAYER);
        if (rc != 0 && REMOTE_MESSAGE(rcvBufIn)->sender != giThisNetPos) {
            if (REMOTE_MESSAGE(rcvBufIn)->type == REMOTE_MESSAGE_CONFIRM) {
                giLastConfirm = REMOTE_MESSAGE(rcvBufIn)->id;
                goto done;
            } else if (REMOTE_MESSAGE(rcvBufIn)->type == REMOTE_MESSAGE_HEARTBEAT) {
                lLastHeartbeatReceive[REMOTE_MESSAGE(rcvBufIn)->sender] = KBTickCount();
                cmdByte = REMOTE_MESSAGE(rcvBufIn)->command;
                if ((cmdByte & REMOTE_HEARTBEAT_CONTROL_FLAG) != 0
                    && gbThisNetGotAdventureControl == 0)
                    iCurHourGlassPhase = cmdByte & REMOTE_HEARTBEAT_PHASE_MASK;
                goto done;
            } else if (qFull) {
                goto done;
            }
            if (REMOTE_MESSAGE(rcvBufIn)->type == REMOTE_MESSAGE_RELIABLE) {
                REMOTE_MESSAGE(sndBuf)->sender = static_cast<i8>(giThisNetPos);
                REMOTE_MESSAGE(sndBuf)->id = REMOTE_MESSAGE(rcvBufIn)->id;
                REMOTE_MESSAGE(sndBuf)->type = REMOTE_MESSAGE_CONFIRM;
                REMOTE_MESSAGE(sndBuf)->payloadSize = 0;
                SendRemoteData(
                    reinterpret_cast<u8*>(sndBuf),
                    NULL,
                    REMOTE_MESSAGE(rcvBufIn)->sender,
                    REMOTE_MESSAGE_HEADER_SIZE
                );
            }
            for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
                if (rcvBuf[queueIndex] != NULL
                    && REMOTE_MESSAGE(rcvBuf[queueIndex])->id
                           == REMOTE_MESSAGE(rcvBufIn)->id) {
                    goto nextIncoming;
                }
            }
            for (queueIndex = 0; queueIndex < REMOTE_RECENT_ID_COUNT; queueIndex++) {
                if (iLastIds[queueIndex] == REMOTE_MESSAGE(rcvBufIn)->id)
                    goto nextIncoming;
            }
            for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
                if (rcvBuf[queueIndex] == NULL) {
                    rcvBuf[queueIndex] =
                        static_cast<char*>(H2_ALLOC(REMOTE_MESSAGE_SIZE));
                    iInOrder[queueIndex] = iInOrderCtr++;
                    memcpy(rcvBuf[queueIndex], rcvBufIn, REMOTE_MESSAGE_SIZE);
                    cnt++;
                    iLastIds[iCurLastID] = REMOTE_MESSAGE(rcvBufIn)->id;
                    iCurLastID = (iCurLastID + 1) % REMOTE_RECENT_ID_COUNT;
                    if (cnt == REMOTE_QUEUE_CAPACITY)
                        goto done;
                    goto nextIncoming;
                }
            }
        }
    }
done:;
}

i32 TransmitAndWait(
    char* bytes,
    i32 destination,
    i32 length,
    i8 command,
    i8 responseCommand,
    char** response
) {
    i32 result;
    i32 clock;
    i8 complete;
    char* receivedData;
    i32 unusedResponseStatus;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    receivedData = NULL;
    result = TransmitRemoteData(
        bytes,
        destination,
        length,
        command,
        1,
        1,
        REMOTE_MESSAGE_DEFAULT
    );
    if (result == 0)
        goto transmitComplete;
    clock = KBTickCount();
    complete = 0;
    while (complete == 0) {
        if (clock + REMOTE_CHAIN_TIMEOUT < KBTickCount()) {
            NormalDialog(
                  "\xce\xf8\xe8\xe1\xea\xe0\x20\xef\xe5\xf0\xe5\xf1\xfb\xeb\xea\xe8\x20\xe4\xe0\xed\xed\xfb\xf5\x2e\x20\xcf\xf0\xee\xe4\xee\xeb\xe6\xe8\xf2\xfc\x3f",
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                clock = KBTickCount();
            } else {
                result = 0;
                goto transmitComplete;
            }
        }
        ForcePollSound();
        receivedData = GetRemoteData(1);
        if (receivedData != NULL)
            unusedResponseStatus = 0;
        if (receivedData != NULL
            && REMOTE_MESSAGE(receivedData)->type == REMOTE_MESSAGE_RELIABLE
            && REMOTE_MESSAGE(receivedData)->command == responseCommand) {
            complete = 1;
        }
    }
    *response = receivedData;
transmitComplete:
    return result;
}

char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
i32 iLastIds[REMOTE_RECENT_ID_COUNT];
char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
char gbUseDiffCompression;
char gbUseRegularCompression;
i32 iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
char gcThisNetName[REMOTE_NET_NAME_SIZE];
i32l lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];
char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
char* rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
i32 bGotGameType;
SNetPlayerInfo gsThisNetPlayerInfo;
