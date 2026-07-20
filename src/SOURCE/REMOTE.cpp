#include <stdio.h>
#include <string.h>
#include <va.h>
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

H2_ENUM_BEGIN(RemoteImplementationConstant)
    CRC_FEEDBACK_BIT                 = 0x08000000,
    LOCAL_NET_NAME_BUFFER_SIZE       = 64,
    NET_NAME_INPUT_LIMIT             = NET_PLAYER_INFO_NAME_SIZE - 1,
    DIRECT_LINK_PLAYER_COUNT         = 2,
    MESSAGE_ID_PLAYER_STRIDE         = 100000000,
    CRC_WORD_BIT_COUNT               = 16,
    CRC_STORAGE_WORD_COUNT           = 2,
    GET_REMOTE_DATA_FREE_LINE_OFFSET = 25,
    POLL_REMOTE_ALLOC_LINE_OFFSET    = 235
H2_ENUM_END(RemoteImplementationConstant)

H2_ENUM_CLASS_BEGIN(RemoteSetupCommand)
    SETUP_PLAYER_INFO   = 0x22,
    SETUP_STANDARD_GAME = 0x3d,
    SETUP_CAMPAIGN_GAME = 0x3e
H2_ENUM_CLASS_END(RemoteSetupCommand)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\REMOTE.CPP"
#define REMOTE_PACKET(buffer) (reinterpret_cast<RemotePacketHeader*>(buffer))
#define REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define REMOTE_PLAYER_INFO(message) (reinterpret_cast<SNetPlayerInfo*>((message)->payload))

DATA(0x00516f60) i32 iInOrderCtr = 0;
DATA(0x00516f64) i32 iCurLastID = 0;
DATA(0x00516f68) i32 giLastConfirm = -1;
DATA(0x00516f6c) u8 GameMode = 0;
DATA(0x00516f70) i32l lLastHeartbeatSend = 0;
DATA(0x00516f74) b32 gbInRemoteMain = false;
DATA(0x00516f78) b32 gbInRemoteCleanup = false;
DATA(0x00516f7c) i32 iIDCtr = 0;
DATA(0x00516f80) i32 iTimesDropped = 0;
DATA(0x00516f84) b8 gbInNetSetup = false;
DATA(0x00516f88) i32 bUseDirectPlay = 0;
DATA(0x00516f8c) i32 bUseWinsock = 0;
DATA(0x00516f90) i8 bInTimeoutFail = 0;
DATA(0x00516f98) i32
    iBaud[REMOTE_BAUD_RATE_COUNT] = {300, 1200, 2400, 9600, 19200, 38400, 57600, 0};
DATA(0x00516fb8) i32 iIRQ[REMOTE_IRQ_COUNT] = {1, 2, 3, 4, 5, 7, 9};
DATA(0x00517118) static i16 gGetRemoteDataLineBase = 716; // NOLINT(readability-magic-numbers)
DATA(0x00517148) static i16 gPollRemoteLineBase = 757; // NOLINT(readability-magic-numbers)

VA(0x004a3080, 0x188)
void RemoteCleanup(void) {
    LogStr("RC1");
    if (gbRemoteOn == 0) {
    } else {
        LogStr("RC2");
        if (gbInRemoteMain != 0) {
        } else {
            if (gbInRemoteCleanup != 0) {
            } else {
                gbInRemoteCleanup = true;
                LogStr("RC3");
                switch (static_cast<RemoteGameMode>(GameMode)) {
        case REMOTE_GAME_NETWORK_HOST:
        case REMOTE_GAME_NETWORK_GUEST:
                        UnloadRemoteDriver(1);
                        break;
        case REMOTE_GAME_MODEM_HOST:
        case REMOTE_GAME_MODEM_GUEST:
                        UnloadRemoteDriver(0);
                        break;
                }
                gbRemoteOn = false;
                xNetHasOldPlayers = 0;
                iInOrderCtr = 0;
                iCurLastID = 0;
                giLastConfirm = -1;
                GameMode = static_cast<u8>(REMOTE_GAME_NONE);
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
        }
    }
}

VA(0x004a3208, 0x6da)
void RemoteMain(RemoteGameMode gameMode) {
    u8 receivedPlayers[REMOTE_PLAYER_COUNT];
    i32 playerState;
    char* incomingData;
    i32 waitingForPlayers;
    i32 netPlayer;
    i32 savedColorMice;
    char netNameBuffer[LOCAL_NET_NAME_BUFFER_SIZE];
    i32 player;
    char* remoteGameType;
    i32 setupCounter;

    gbInRemoteMain = true;
    bGotGameType = 0;
    LogStr("In Remote Main");
    LogStr("RM 1");
    for (player = 0; player < REMOTE_PLAYER_COUNT; player++) {
        lLastHeartbeatReceive[player] = REMOTE_INITIAL_HEARTBEAT;
        sprintf(gsNetPlayerInfo[player].name, "Player %d", player + 1);
    }
    LogStr("RM 2");
    gbRemoteGameOpen = true;
    if (bLastMouseOffscreen != 0)
        savedColorMice = bLastOnscreenMouseColor;
    else
        savedColorMice = gbColorMice;
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
    GameMode = static_cast<u8>(gameMode);
    LogStr("RM 6");
    memset(gsNetPlayerInfo, 0, sizeof(gsNetPlayerInfo));
    memset(&gsThisNetPlayerInfo, 0, sizeof(gsThisNetPlayerInfo));
    LogStr("RM 7");
    if (giTCPHostStatus != -1) {
        if (strlen(gcTCPName) == 0)
            strcpy(gsThisNetPlayerInfo.name, gConfig.networkDefaultName);
        else
            strcpy(gsThisNetPlayerInfo.name, gcTCPName);
    } else {
        GetDataEntry(
            "Please enter a 'handle' by which you will be known.",
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
            if (bUseDirectPlay == 0) {
                if (bUseWinsock == 0) {
                    nbnet_init();
                    gbRemoteOn = true;
                } else {
                    wsnet_init();
                    gbRemoteOn = true;
                }
            } else {
                gbRemoteOn = true;
                dpnet_init();
            }
            break;
                    case REMOTE_GAME_MODEM_HOST:
            LogStr("MH1");
            gbRemoteOn = true;
            gsNetPlayerInfo[0] = gsThisNetPlayerInfo;
            giThisNetPos = 0;
            ModemSetup(IDX(gameMode));
            LogStr("MH2");
            break;
                    case REMOTE_GAME_MODEM_GUEST:
            gbRemoteOn = true;
            giThisNetPos = 1;
            ModemSetup(IDX(gameMode));
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
            waitingForPlayers = 1;
            memset(receivedPlayers, 0, REMOTE_PLAYER_COUNT);
            while (waitingForPlayers != 0) {
                PollSound();
                LogStr("RM 3");
                incomingData = GetRemoteData(1);
                LogStr("RM 4");
                if (incomingData != NULL
                    && REMOTE_MESSAGE(incomingData)->type == IDX(REMOTE_MESSAGE_RELIABLE)) {
                    switch (static_cast<RemoteSetupCommand>(REMOTE_MESSAGE(incomingData)->command)) {
                        case SETUP_PLAYER_INFO:
                            netPlayer = REMOTE_MESSAGE(incomingData)->sender;
                            gsNetPlayerInfo[netPlayer] =
                                *REMOTE_PLAYER_INFO(REMOTE_MESSAGE(incomingData));
                            receivedPlayers[netPlayer] = 1;
                            if (gsNetPlayerInfo[netPlayer].reserved[0] == 0)
                                xNetHasOldPlayers = 1;
                            break;
                    }
                }
                waitingForPlayers = 0;
                for (player = 1; player < giNumHumanPlayers; player++) {
                    if (receivedPlayers[player] == 0)
                        waitingForPlayers = 1;
                }
            }
        } else {
            LogStr("RM 5");
            TransmitRemoteData(
                reinterpret_cast<char*>(&gsThisNetPlayerInfo),
                0,
                sizeof(SNetPlayerInfo),
                IDX(SETUP_PLAYER_INFO),
                1,
                1,
                -1
            );
            LogStr("RM 6");
        }
    }
    remoteGameType = NULL;
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
            -1
        );
    } else {
        while (bGotGameType == 0) {
            PollSound();
            remoteGameType = GetRemoteData(1);
            if (remoteGameType != NULL
                && REMOTE_MESSAGE(remoteGameType)->type == IDX(REMOTE_MESSAGE_RELIABLE)) {
                setupCounter = 0;
                setupCounter++;
                setupCounter++;
                setupCounter++;
            }
            if (remoteGameType != NULL
                && REMOTE_MESSAGE(remoteGameType)->type == IDX(REMOTE_MESSAGE_RELIABLE)
                && REMOTE_MESSAGE(remoteGameType)->command == IDX(SETUP_CAMPAIGN_GAME)) {
                bGotGameType = 1;
                giSetupGameType = 1;
            }
            if (remoteGameType != NULL
                && REMOTE_MESSAGE(remoteGameType)->type == IDX(REMOTE_MESSAGE_RELIABLE)
                && REMOTE_MESSAGE(remoteGameType)->command == IDX(SETUP_STANDARD_GAME)) {
                bGotGameType = 1;
                giSetupGameType = 0;
            }
        }
    }
    LogStr("Out Remote Main");
    gbInRemoteMain = false;
}

VA(0x004a38e2, 0x85)
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

VA(0x004a3967, 0x8d)
i32 calc_crc_long(u8* data, i32 length) {
    u32 shifted;
    u32 crc;
    u32 sum;
    i32 unused;

    crc = 0;
    sum = 0;
    unused = 0;
    while (length-- != 0) {
        shifted = crc & CRC_FEEDBACK_BIT;
        crc <<= 1;
        crc += *data;
        sum += *data;
        if (shifted != 0)
            crc++;
        data++;
    }
    crc += sum >> CRC_WORD_BIT_COUNT;
    crc += sum << CRC_WORD_BIT_COUNT;
    return crc;
}

VA(0x004a39f4, 0x2c)
void calc_crc(u16* crc, u8* data, i32 length) {
    *crc = static_cast<u16>(calc_crc_long(data, length));
}

VA(0x004a3a20, 0x87)
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

VA(0x004a3aa7, 0x13a)
i32 DecodePacket(u8* data, i32) {
    u16 receivedCRC;
    u16 calculatedCRC[CRC_STORAGE_WORD_COUNT];
    u32 length;
    char errorText[REMOTE_ERROR_TEXT_SIZE];

    calculatedCRC[0] = 0;
    if (REMOTE_PACKET(packet)->destination != giThisNetPos
        && REMOTE_PACKET(packet)->destination != REMOTE_BROADCAST_PLAYER) {
        sprintf(errorText, "not mine %d\n", REMOTE_PACKET(packet)->destination);
        LogStr(errorText);
        return 0;
    }
    length = static_cast<u8>(REMOTE_PACKET(packet)->payloadSize);
    receivedCRC = REMOTE_PACKET(packet)->crc;
    REMOTE_PACKET(packet)->crc = 0;
    calc_crc(calculatedCRC, reinterpret_cast<u8*>(packet), length + REMOTE_PACKET_HEADER_SIZE);
    if (receivedCRC != calculatedCRC[0]) {
        sprintf(errorText, "CRC Check Failed CRC 1 %d CRC 2 %d\n", receivedCRC, calculatedCRC[0]);
        LogStr(errorText);
        return 0;
    }
    memcpy(data, packet + REMOTE_PACKET_HEADER_SIZE, length);
    return 1;
}

VA(0x004a3be1, 0x18e)
i32 SendRemoteData(u8* dataToSend, u8*, i32 destination, i32 length) {
    i32 sendStatus;
    i32 out;
    i32 size;
    u8 remotePacket[REMOTE_MESSAGE_SIZE + sizeof(i32)];

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
    switch (static_cast<RemoteGameMode>(GameMode)) {
        case REMOTE_GAME_NETWORK_HOST:
        case REMOTE_GAME_NETWORK_GUEST:
            if (bUseDirectPlay != 0) {
                sendStatus = dpnet_snd(destination, size, PacketSend);
            } else if (bUseWinsock != 0) {
                sendStatus = wsnet_snd(destination, size, PacketSend);
            } else {
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
                }
            }
            break;
        case REMOTE_GAME_MODEM_HOST:
        case REMOTE_GAME_MODEM_GUEST:
            WriteModemPacket(PacketSend, size);
            out = 1;
            break;
    }
    return out;
}

VA(0x004a3d6f, 0x158)
i32 ReceiveRemoteData(u8*, u8* data, i32 decodeType) {
    i32 result;
    i32 receiveResult;

    result = 1;
    switch (static_cast<RemoteGameMode>(GameMode)) {
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

VA(0x004a3ec7, 0x21a)
i32 TransmitRemoteData(
    char* data,
    i32 destination,
    i32 length,
    i8 command,
    i8 reliable,
    i8 allowRetryDialog,
    i8 messageType
) {
    i32 poll;
    i32 attempt0;
    RemoteMessage outgoing;
    i32 unused4;
    i32 result;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    result = 0;
    attempt0 = 0;
    iIDCtr++;
    outgoing.sender = static_cast<i8>(giThisNetPos);
    outgoing.id = iIDCtr;
    if (messageType != -1) {
        outgoing.type = messageType;
    } else {
        if (reliable != 0)
            outgoing.type = IDX(REMOTE_MESSAGE_RELIABLE);
        else
            outgoing.type = IDX(REMOTE_MESSAGE_UNRELIABLE);
    }
    outgoing.payloadSize = static_cast<u16>(length);
    outgoing.command = command;
    if (length > 0)
        memcpy(outgoing.payload, data, length);
    while (result == 0 && attempt0 <= REMOTE_RETRY_COUNT) {
        result = SendRemoteData(
            reinterpret_cast<u8*>(&outgoing),
            NULL,
            destination,
            length + REMOTE_MESSAGE_HEADER_SIZE
        );
        if (reliable == 0 && result != 0)
            return 1;
        if (result != 0) {
            poll = 0;
            while (poll < REMOTE_CONFIRM_POLL_COUNT) {
                ForcePollSound();
                if (iIDCtr == giLastConfirm)
                    return 1;
                result = 0;
                DelayMilli(REMOTE_CONFIRM_POLL_DELAY);
                poll++;
            }
        } else {
            DelayMilli(REMOTE_SEND_RETRY_DELAY);
        }
        if (allowRetryDialog != 0 && attempt0 == REMOTE_RETRY_COUNT && result == 0) {
            NormalDialog(
                "Error sending data.  Keep trying??",
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
                attempt0 = -1;
        }
        attempt0++;
    }
    return result;
}

VA(0x004a40e1, 0x10b)
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
            H2_FREE(
                rcvBuf[selected], gGetRemoteDataLineBase + GET_REMOTE_DATA_FREE_LINE_OFFSET
            );
            rcvBuf[selected] = NULL;
        }
        return rcvBufOut;
    }
    return NULL;
}

VA(0x004a41ec, 0x6f4)
void PollRemote(void) {
    i32 savedInPollSound;
    i32 queueCount;
    i32 timeout;
    i32 queueIndex;
    i32 receiveResult;
    i8 netCommand;
    i8 queueFull;
    i32 destination;
    SPlayerExit hostExit;
    SPlayerExit guestExit;

    if (gbRemoteOn == 0) {
    } else {
        if (gbInRemoteCleanup != 0) {
        } else {
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
            queueCount = 0;
            savedInPollSound = gbInPollSound;
            queueFull = 0;
            if (KBTickCount() - lLastHeartbeatSend > REMOTE_HEARTBEAT_INTERVAL) {
                REMOTE_MESSAGE(sndBuf)->sender = static_cast<i8>(giThisNetPos);
                REMOTE_MESSAGE(sndBuf)->type = IDX(REMOTE_MESSAGE_HEARTBEAT);
                REMOTE_MESSAGE(sndBuf)->payloadSize = 0;
                if (gbThisNetGotAdventureControl != 0) {
                    REMOTE_MESSAGE(sndBuf)->command = static_cast<i8>(
                        ((giCurPlayer + 1) << REMOTE_HEARTBEAT_PLAYER_SHIFT) | iCurHourGlassPhase
                        | REMOTE_HEARTBEAT_CONTROL_FLAG
                    );
                } else {
                    REMOTE_MESSAGE(sndBuf)->command = 0;
                }
                if (giThisNetPos == 0 || gbThisNetGotAdventureControl != 0)
                    destination = REMOTE_BROADCAST_PLAYER;
                else
                    destination = 0;
                SendRemoteData(
                    reinterpret_cast<u8*>(sndBuf),
                    NULL,
                    destination,
                    REMOTE_HEARTBEAT_MESSAGE_SIZE
                );
                lLastHeartbeatSend = KBTickCount();
            }

            if (giThisNetPos == 0) {
                for (queueIndex = 0; queueIndex < giNumHumanPlayers; queueIndex++) {
                    if (queueIndex != giThisNetPos
                        && lLastHeartbeatReceive[queueIndex] + REMOTE_HOST_TIMEOUT < KBTickCount()
                        && bInTimeoutFail == 0) {
                        bInTimeoutFail = 1;
                        gbInPollSound = false;
                        sprintf(
                            gText,
                            "%s's computer is not responding.  Do you wish to keep waiting for a "
                            "response?",
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
                        gbInPollSound = savedInPollSound;
                        bInTimeoutFail = 0;
                    }
                }
            } else {
                timeout = REMOTE_GUEST_TIMEOUT;
                if (giThisNetPos != 1)
                    timeout += REMOTE_CHAIN_GUEST_TIMEOUT_INCREMENT;
                if (lLastHeartbeatReceive[0] + timeout < KBTickCount() && bInTimeoutFail == 0) {
                    bInTimeoutFail = 1;
                    gbInPollSound = false;
                    if (giThisNetPos == 1) {
                        sprintf(
                            gText,
                            "%s's computer is not responding.  Do you wish to keep waiting for a "
                            "response?",
                            gsNetPlayerInfo[0].name
                        );
                    } else {
                        sprintf(
                            gText,
                            "Your remote connection to the other players appears to be broken.  Do "
                            "you wish to keep waiting for a response?"
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
                        gpGame->SaveGame("PLYREXIT", 1, 0);
                        sprintf(
                            gText,
                            "The current game has been saved as 'PLYREXIT'. Do you wish to keep "
                            "playing with the computer filling in for the other humans?"
                        );
                        NormalDialog(
                            gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                            DropDownToOnePlayer();
                        else
                            ShutDown("");
                    }
                    gbInPollSound = savedInPollSound;
                    bInTimeoutFail = 1;
                }
            }

            for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
                if (rcvBuf[queueIndex] != NULL)
                    queueCount++;
            }
            if (queueCount == REMOTE_QUEUE_CAPACITY)
                queueFull = 1;
            receiveResult = 1;
            while (receiveResult != 0) {
                receiveResult =
                    ReceiveRemoteData(NULL, reinterpret_cast<u8*>(rcvBufIn), REMOTE_BROADCAST_PLAYER);
                if (receiveResult == 0 || REMOTE_MESSAGE(rcvBufIn)->sender == giThisNetPos)
                    continue;
                if (REMOTE_MESSAGE(rcvBufIn)->type == IDX(REMOTE_MESSAGE_CONFIRM)) {
                    giLastConfirm = REMOTE_MESSAGE(rcvBufIn)->id;
                    return;
                }
                if (REMOTE_MESSAGE(rcvBufIn)->type == IDX(REMOTE_MESSAGE_HEARTBEAT)) {
                    lLastHeartbeatReceive[REMOTE_MESSAGE(rcvBufIn)->sender] = KBTickCount();
                    netCommand = REMOTE_MESSAGE(rcvBufIn)->command;
                    if ((netCommand & REMOTE_HEARTBEAT_CONTROL_FLAG) == 0)
                        return;
                    if (gbThisNetGotAdventureControl != 0)
                        return;
                    iCurHourGlassPhase = netCommand & REMOTE_HEARTBEAT_PHASE_MASK;
                    return;
                }
                if (queueFull)
                    return;
                if (REMOTE_MESSAGE(rcvBufIn)->type == IDX(REMOTE_MESSAGE_RELIABLE)) {
                    REMOTE_MESSAGE(sndBuf)->sender = static_cast<i8>(giThisNetPos);
                    REMOTE_MESSAGE(sndBuf)->id = REMOTE_MESSAGE(rcvBufIn)->id;
                    REMOTE_MESSAGE(sndBuf)->type = IDX(REMOTE_MESSAGE_CONFIRM);
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
                        && REMOTE_MESSAGE(rcvBuf[queueIndex])->id == REMOTE_MESSAGE(rcvBufIn)->id) {
                        break;
                    }
                }
                if (queueIndex < REMOTE_QUEUE_CAPACITY)
                    continue;
                for (queueIndex = 0; queueIndex < REMOTE_RECENT_ID_COUNT; queueIndex++) {
                    if (iLastIds[queueIndex] == REMOTE_MESSAGE(rcvBufIn)->id)
                        break;
                }
                if (queueIndex < REMOTE_RECENT_ID_COUNT)
                    continue;
                for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
                    if (rcvBuf[queueIndex] == NULL)
                        break;
                }
                if (queueIndex >= REMOTE_QUEUE_CAPACITY)
                    continue;
                rcvBuf[queueIndex] = static_cast<char*>(H2_ALLOC(
                    REMOTE_MESSAGE_SIZE, gPollRemoteLineBase + POLL_REMOTE_ALLOC_LINE_OFFSET
                ));
                iInOrder[queueIndex] = iInOrderCtr++;
                memcpy(rcvBuf[queueIndex], rcvBufIn, REMOTE_MESSAGE_SIZE);
                queueCount++;
                iLastIds[iCurLastID] = REMOTE_MESSAGE(rcvBufIn)->id;
                iCurLastID = (iCurLastID + 1) % REMOTE_RECENT_ID_COUNT;
                if (queueCount == REMOTE_QUEUE_CAPACITY)
                    return;
            }
        }
    }
}

VA(0x004a48e0, 0x163)
i32 TransmitAndWait(
    char* data,
    i32 destination,
    i32 length,
    i8 command,
    i8 responseCommand,
    char** response
) {
    char* receivedData;
    i32 result;
    i32 waitStart;
    i8 complete;
    i32 unusedResponseState;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    receivedData = NULL;
    result = TransmitRemoteData(data, destination, length, command, 1, 1, -1);
    if (result == 0) {
    } else {
        waitStart = KBTickCount();
        complete = 0;
        while (complete == 0) {
            if (waitStart + REMOTE_CHAIN_TIMEOUT < KBTickCount()) {
                NormalDialog(
                    "Error sending data.  Keep trying??",
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
                    waitStart = KBTickCount();
                } else {
                    result = 0;
                    goto transmitComplete;
                }
            }
            ForcePollSound();
            receivedData = GetRemoteData(1);
            if (receivedData != NULL)
                unusedResponseState = 0;
            if (receivedData != NULL
                && REMOTE_MESSAGE(receivedData)->type == IDX(REMOTE_MESSAGE_RELIABLE)
                && REMOTE_MESSAGE(receivedData)->command == responseCommand) {
                complete = 1;
            }
        }
        *response = receivedData;
    }
transmitComplete:
    return result;
}

DATA(0x0052a268) char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a378) i32 iLastIds[REMOTE_RECENT_ID_COUNT];
DATA(0x0052a3f0) char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
DATA(0x0052a4fc) char gbUseDiffCompression;
DATA(0x0052a500) char gbUseRegularCompression;
DATA(0x0052a508) i32 iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
DATA(0x0052a730) char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a840) char gcThisNetName[REMOTE_NET_NAME_SIZE];
DATA(0x0052a860) i32l lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
DATA(0x0052a878) char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a988) SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];
DATA(0x0052aa58) char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052ab68) char* rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
DATA(0x0052ad90) i32 bGotGameType;
DATA(0x0052ad98) SNetPlayerInfo gsThisNetPlayerInfo;

#undef RETAIL_FILE
