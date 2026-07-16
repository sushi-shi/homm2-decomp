// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REMOTE.OBJ   from: (directly linked into exe)
// functions: 13   data: 30
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

#define REMOTE_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\REMOTE.CPP"
#define REMOTE_PACKET(buffer) \
    (reinterpret_cast<RemotePacketHeader *>(buffer))
#define REMOTE_MESSAGE(buffer) \
    (reinterpret_cast<RemoteMessage *>(buffer))
#define REMOTE_PLAYER_INFO(message) \
    (reinterpret_cast<SNetPlayerInfo *>((message)->payload))

// @data-layout-note Retail's initialized REMOTE contribution is
// 0x116f60..0x11733c (0x3dc); candidate .data is 0x3db both before and after
// restoring the retail newline in the CRC failure format at retail offset 0x158.
// The newline consumes an existing candidate padding byte, and the corrected
// literal payload is now exact. The two signed-short source-line owners are
// independently proved at 0x117118=0x02cc (GetRemoteData, one HIGHLOW use with
// addend 25) and 0x117148=0x02f5 (PollRemote, one HIGHLOW use with addend 235).
// VC4.2 hoists them to candidate offsets 0x0 and 0x4; retail interleaves them at
// contribution offsets 0x1b8 and 0x1e8. After accounting for those moves, every
// other candidate byte maps to retail, which has one terminal alignment zero at
// offset 0x3db. This produces the three expected data anchor bases.
// Retail's loader-zero contribution is 0x12a268..0x12adc0 (0xb58), while the
// candidate .bss is 0xb50. Every PE reference in that range resolves inside the
// 15 known public objects and their proven field/addend spans; no private BSS
// owner occupies an alignment gap. The residual comes from final-link/common
// allocation order and tail alignment, reflected by the distinct public anchor
// bases, not missing source storage. Revisit only with original COFF/link-order
// evidence. Do not add padding, aliases, synthetic identities, cursor snapping,
// guessed allocations, or section pragmas to force either contribution extent.
DATA(0x00517118) static short gGetRemoteDataLineBase = 716;
DATA(0x00517148) static short gPollRemoteLineBase = 757;

// @match-note
// Complete cleanup guards, backend switch, and all state resets. The 0x04 frame
// and switch temporary at -0x04 agree, including the retail duplicate protocol
// reset block. All 38 relocation occurrences and external targets agree. The
// first residual is the delinked switch continuation/local-label trampoline.
// Empty positive arms and direct early exits were both tested. Revisit in the
// 95% last-mile phase when local-label walls enter the active queue.
VA(0x004a3080, 0x188)
void RemoteCleanup(void)
{
    LogStr("RC1");
    if (gbRemoteOn == 0) {
    } else {
        LogStr("RC2");
        if (gbInRemoteMain != 0) {
        } else {
            if (gbInRemoteCleanup != 0) {
            } else {
                gbInRemoteCleanup = 1;
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
                }
                gbRemoteOn = 0;
                xNetHasOldPlayers = 0;
                iInOrderCtr = 0;
                iCurLastID = 0;
                giLastConfirm = -1;
                GameMode = 0;
                lLastHeartbeatSend = 0;
                gbInRemoteMain = 0;
                iIDCtr = 0;
                iTimesDropped = 0;
                bUseDirectPlay = 0;
                bUseWinsock = 0;
                bInTimeoutFail = 0;
                bUseDirectPlay = 0;
                bUseWinsock = 0;
                bInTimeoutFail = 0;
                iMPNetProtocol = 0;
                iLastDiffSendTo = -2;
                gbGotFirstHeartbeat = 0;
                gbInRemoteCleanup = 0;
            }
        }
    }
}

// @match-note
// Complete setup, transport initialization, player exchange, and game-type
// synchronization CFG. The 0x74 frame and live slots agree, including the legacy
// 64-byte name buffer, player-state word, setup counter, and command switch temp.
// All 143 raw relocation entries and external targets agree. llvm-objdump starts
// the retail post-table block one byte inside its compare at .text+0x559; the raw
// relocation at +0x55c has a zero addend, matching base +0x557. Tested protocol
// polarity, log placement, struct copies, and host-name arm order. Revisit in the
// 95% last-mile phase when local-label walls enter the active queue.
VA(0x004a3208, 0x6da)
void RemoteMain(int gameMode)
{
    unsigned char receivedPlayers[REMOTE_PLAYER_COUNT];
    int playerState;
    char *incomingData;
    int waitingForPlayers;
    int netPlayer;
    int savedColorMice;
    char netNameBuffer[64];
    int player;
    char *remoteGameType;
    int setupCounter;

    gbInRemoteMain = 1;
    bGotGameType = 0;
    LogStr("In Remote Main");
    LogStr("RM 1");
    for (player = 0; player < REMOTE_PLAYER_COUNT; player++) {
        lLastHeartbeatReceive[player] = REMOTE_INITIAL_HEARTBEAT;
        sprintf(gsNetPlayerInfo[player].name, "Player %d", player + 1);
    }
    LogStr("RM 2");
    gbRemoteGameOpen = 1;
    if (bLastMouseOffscreen != 0)
        savedColorMice = bLastOnscreenMouseColor;
    else
        savedColorMice = gbColorMice;
    gpMouseManager->SetColorMice(0);
    LogStr("RM 3");
    gbInNetSetup = 1;
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
        rcvBuf[player] = 0;
    LogStr("RM 5");
    memset(iLastIds, 0, REMOTE_RECENT_ID_COUNT);
    GameMode = static_cast<unsigned char>(gameMode);
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
        GetDataEntry("Please enter a 'handle' by which you will be known.",
            gsThisNetPlayerInfo.name, 20, gConfig.networkDefaultName, 1, 0);
    }
    strcpy(gConfig.networkDefaultName, gsThisNetPlayerInfo.name);
    WritePrefs();
    strcpy(gsThisNetPlayerInfo.uniqueSystemID, gConfig.uniqueSystemID);
    gsThisNetPlayerInfo.connectionType = 2;
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
                gbRemoteOn = 1;
            } else {
                wsnet_init();
                gbRemoteOn = 1;
            }
        } else {
            gbRemoteOn = 1;
            dpnet_init();
        }
        break;
    case REMOTE_GAME_MODEM_HOST:
        LogStr("MH1");
        gbRemoteOn = 1;
        gsNetPlayerInfo[0] = gsThisNetPlayerInfo;
        giThisNetPos = 0;
        ModemSetup(gameMode);
        LogStr("MH2");
        break;
    case REMOTE_GAME_MODEM_GUEST:
        gbRemoteOn = 1;
        giThisNetPos = 1;
        ModemSetup(gameMode);
        break;
    }
    if (bUseDirectPlay == 0 && bUseWinsock == 0)
        giNumHumanPlayers = 2;
    iIDCtr = (giThisNetPos + 1) * 100000000;
    gbInNetSetup = 0;
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
                if (incomingData != 0 &&
                    REMOTE_MESSAGE(incomingData)->type == REMOTE_MESSAGE_RELIABLE) {
                    switch (REMOTE_MESSAGE(incomingData)->command) {
                    case REMOTE_SETUP_PLAYER_INFO:
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
            TransmitRemoteData(reinterpret_cast<char *>(&gsThisNetPlayerInfo),
                0, sizeof(SNetPlayerInfo), REMOTE_SETUP_PLAYER_INFO, 1, 1, -1);
            LogStr("RM 6");
        }
    }
    remoteGameType = 0;
    if (giThisNetPos == 0) {
        TransmitRemoteData(0, REMOTE_BROADCAST_PLAYER, 0,
            static_cast<signed char>(giSetupGameType == 1 ?
                REMOTE_SETUP_CAMPAIGN_GAME : REMOTE_SETUP_STANDARD_GAME),
            1, 1, -1);
    } else {
        while (bGotGameType == 0) {
            PollSound();
            remoteGameType = GetRemoteData(1);
            if (remoteGameType != 0 &&
                REMOTE_MESSAGE(remoteGameType)->type == REMOTE_MESSAGE_RELIABLE) {
                setupCounter = 0;
                setupCounter++;
                setupCounter++;
                setupCounter++;
            }
            if (remoteGameType != 0 &&
                REMOTE_MESSAGE(remoteGameType)->type == REMOTE_MESSAGE_RELIABLE &&
                REMOTE_MESSAGE(remoteGameType)->command == REMOTE_SETUP_CAMPAIGN_GAME) {
                bGotGameType = 1;
                giSetupGameType = 1;
            }
            if (remoteGameType != 0 &&
                REMOTE_MESSAGE(remoteGameType)->type == REMOTE_MESSAGE_RELIABLE &&
                REMOTE_MESSAGE(remoteGameType)->command == REMOTE_SETUP_STANDARD_GAME) {
                bGotGameType = 1;
                giSetupGameType = 0;
            }
        }
    }
    LogStr("Out Remote Main");
    gbInRemoteMain = 0;
}

VA(0x004a38e2, 0x85)
void UnloadRemoteDriver(short int networkDriver)
{
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
int calc_crc_long(unsigned char *data, int length)
{
    unsigned int shifted;
    unsigned int crc;
    unsigned int sum;
    int unused;

    crc = 0;
    sum = 0;
    unused = 0;
    while (length-- != 0) {
        shifted = crc & 0x08000000;
        crc <<= 1;
        crc += *data;
        sum += *data;
        if (shifted != 0)
            crc++;
        data++;
    }
    crc += sum >> 16;
    crc += sum << 16;
    return crc;
}

VA(0x004a39f4, 0x2c)
void calc_crc(unsigned short int *crc, unsigned char *data, int length)
{
    *crc = static_cast<unsigned short>(calc_crc_long(data, length));
}

VA(0x004a3a20, 0x87)
int EncodePacket(unsigned char *data, char source, char destination, int length)
{
    unsigned short crc[2];

    REMOTE_PACKET(PacketSend)->source = source;
    REMOTE_PACKET(PacketSend)->destination = destination;
    REMOTE_PACKET(PacketSend)->reserved = 0;
    REMOTE_PACKET(PacketSend)->payloadSize = static_cast<char>(length);
    crc[0] = 0;
    REMOTE_PACKET(PacketSend)->crc = crc[0];
    memcpy(PacketSend + REMOTE_PACKET_HEADER_SIZE, data, length);
    calc_crc(crc, reinterpret_cast<unsigned char *>(PacketSend),
        length + REMOTE_PACKET_HEADER_SIZE);
    REMOTE_PACKET(PacketSend)->crc = crc[0];
    return length + REMOTE_PACKET_HEADER_SIZE;
}

// @early-stop
// The relocation-masked instruction stream, 0xcc frame, stack slots, and CFG are
// byte-identical. All 17 relocation sites and target classes agree; the remaining
// report residual is solely delinked packet/global, literal, and local-label
// identity, with no code-byte difference.
VA(0x004a3aa7, 0x13a)
int DecodePacket(unsigned char *data, int)
{
    unsigned short receivedCRC;
    unsigned short calculatedCRC[2];
    unsigned int length;
    char errorText[200];

    calculatedCRC[0] = 0;
    if (REMOTE_PACKET(packet)->destination != giThisNetPos &&
        REMOTE_PACKET(packet)->destination != REMOTE_BROADCAST_PLAYER) {
        sprintf(errorText, "not mine %d\n", REMOTE_PACKET(packet)->destination);
        LogStr(errorText);
        return 0;
    }
    length = static_cast<unsigned char>(REMOTE_PACKET(packet)->payloadSize);
    receivedCRC = REMOTE_PACKET(packet)->crc;
    REMOTE_PACKET(packet)->crc = 0;
    calc_crc(calculatedCRC, reinterpret_cast<unsigned char *>(packet),
        length + REMOTE_PACKET_HEADER_SIZE);
    if (receivedCRC != calculatedCRC[0]) {
        sprintf(errorText, "CRC Check Failed CRC 1 %d CRC 2 %d\n",
            receivedCRC, calculatedCRC[0]);
        LogStr(errorText);
        return 0;
    }
    memcpy(data, packet + REMOTE_PACKET_HEADER_SIZE, length);
    return 1;
}

// @match-note
// Complete destination folding and all network/modem switch bodies, in retail
// DirectPlay/Winsock/NetBIOS body order. The 0x11c frame is exact: size -0x04,
// result -0x08, backend status -0x0c, the retail-proven dead 260-byte aggregate
// through -0x110, fastcall spills -0x114/-0x118, and switch temporary -0x11c.
// All 23 relocations agree; the first comparison boundary is the delinked switch
// dispatch. Both nested inverse and positive backend polarities were tested.
// Revisit in the 95% last-mile phase.
VA(0x004a3be1, 0x18e)
int SendRemoteData(unsigned char *dataToSend, unsigned char *, int destination,
                   int length)
{
    int sendStatus;
    int out;
    int size;
    unsigned char remotePacket[REMOTE_MESSAGE_SIZE + sizeof(int)];

    out = 1;
    if (destination == REMOTE_BROADCAST_PLAYER &&
        bUseDirectPlay == 0 && bUseWinsock == 0) {
        destination = 1 - giThisNetPos;
    }
    size = EncodePacket(dataToSend, static_cast<char>(giThisNetPos),
        static_cast<char>(destination), length);
    switch (GameMode) {
    case REMOTE_GAME_NETWORK_HOST:
    case REMOTE_GAME_NETWORK_GUEST:
        if (bUseDirectPlay != 0) {
            sendStatus = dpnet_snd(destination, size, PacketSend);
        } else if (bUseWinsock != 0) {
            sendStatus = wsnet_snd(destination, size, PacketSend);
        } else {
            sendStatus = static_cast<short>(
                nb_snd(static_cast<short>(destination),
                    static_cast<short>(size), PacketSend));
            if (sendStatus != 0) {
                LogInt("Bad return on Send Data", destination,
                    sendStatus, size, 0, 0, -999, -999);
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

// @early-stop
// Complete DirectPlay, Winsock, NetBIOS, and modem receive/decode paths. The 0x14
// frame and slots agree: result -0x04, 32-bit receive status -0x08, fastcall spills
// -0x0c/-0x10, and switch temporary -0x14. The functions have the same 0x158-byte
// size and every non-relocation byte is identical. The only six raw byte differences
// are switch relocation addends at +0x135/+0x136/+0x139/+0x13d/+0x141/+0x145;
// all 19 relocation sites and external targets agree.
VA(0x004a3d6f, 0x158)
int ReceiveRemoteData(unsigned char *, unsigned char *data, int decodeType)
{
    int result;
    int receiveResult;

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
            receiveResult = static_cast<short>(
                nb_rcv(REMOTE_RECEIVE_BUFFER_SIZE, packet));
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

// @match-note
// Complete message construction, retry/dialog loop, confirmation polling, and
// reliable/unreliable exits. The 0x118 frame is exact: result -0x04, message through
// -0x108, retail-proven dead word -0x10c, attempt -0x110, poll -0x114, and fastcall
// spills through -0x118. All 15 relocation targets agree. The first residual is a
// retail five-byte continuation after the unreliable-success return, followed by
// the iIDCtr/giLastConfirm load order. Guarded/early-return loops, both comparison
// operand orders, and both message-type polarities were tested. Revisit in the 95%
// last-mile phase.
VA(0x004a3ec7, 0x21a)
int TransmitRemoteData(char *data, int destination, int length,
                       signed char command, signed char reliable,
                       signed char allowRetryDialog, signed char messageType)
{
    int poll;
    int attempt0;
    RemoteMessage outgoing;
    int unused4;
    int result;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    result = 0;
    attempt0 = 0;
    iIDCtr++;
    outgoing.sender = static_cast<signed char>(giThisNetPos);
    outgoing.id = iIDCtr;
    if (messageType != -1) {
        outgoing.type = messageType;
    } else {
        if (reliable != 0)
            outgoing.type = REMOTE_MESSAGE_RELIABLE;
        else
            outgoing.type = REMOTE_MESSAGE_UNRELIABLE;
    }
    outgoing.payloadSize = static_cast<unsigned short>(length);
    outgoing.command = command;
    if (length > 0)
        memcpy(outgoing.payload, data, length);
    while (result == 0 && attempt0 <= REMOTE_RETRY_COUNT) {
        result = SendRemoteData(
            reinterpret_cast<unsigned char *>(&outgoing), 0, destination,
            length + REMOTE_MESSAGE_HEADER_SIZE);
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
        if (allowRetryDialog != 0 && attempt0 == REMOTE_RETRY_COUNT &&
            result == 0) {
            NormalDialog("Error sending data.  Keep trying??", 2,
                -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK)
                attempt0 = -1;
        }
        attempt0++;
    }
    return result;
}

VA(0x004a40e1, 0x10b)
char * GetRemoteData(signed char remove)
{
    int oldestOrder;
    int queueIndex;
    int selected;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 0;
    oldestOrder = REMOTE_ORDER_SENTINEL;
    selected = -1;
    for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY;
         queueIndex++) {
        if (rcvBuf[queueIndex] != 0 &&
            iInOrder[queueIndex] < oldestOrder) {
            oldestOrder = iInOrder[queueIndex];
            selected = queueIndex;
        }
    }
    if (selected >= 0) {
        memcpy(rcvBufOut, rcvBuf[selected], REMOTE_MESSAGE_SIZE);
        if (remove != 0) {
            BaseFree(rcvBuf[selected], REMOTE_SOURCE_FILE,
                gGetRemoteDataLineBase + 25);
            rcvBuf[selected] = 0;
        }
        return rcvBufOut;
    }
    return 0;
}

// @match-note
// Complete backend pumping, heartbeat, host/guest timeout recovery, confirmation,
// duplicate suppression, queue allocation, and recent-ID rotation. The retail
// queue-full snapshot is byte-sized; its -0x14 slot, queue count -0x0c, saved poll
// state -0x08, both seven-byte exit records, and the 0x30 frame agree. All 132
// relocation occurrences and external targets agree. Ignoring delinked literal
// identities, the first code residual is the host-loop queueIndex/giThisNetPos load
// order; both operand orders, empty guards, and arm polarities were tested. Revisit
// in the 95% last-mile phase.
VA(0x004a41ec, 0x6f4)
void PollRemote(void)
{
    int savedInPollSound;
    int queueCount;
    int timeout;
    int queueIndex;
    int receiveResult;
    signed char netCommand;
    signed char queueFull;
    int destination;
    SPlayerExit hostExit;
    SPlayerExit guestExit;

    if (gbRemoteOn == 0) {
    } else {
    if (gbInRemoteCleanup != 0) {
    } else {
    if (iMPBaseType == REMOTE_BASE_MODEM) {
        comm_wrt_task();
    } else if (iMPBaseType == REMOTE_BASE_NETWORK) {
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
        REMOTE_MESSAGE(sndBuf)->sender =
            static_cast<signed char>(giThisNetPos);
        REMOTE_MESSAGE(sndBuf)->type =
            REMOTE_MESSAGE_HEARTBEAT;
        REMOTE_MESSAGE(sndBuf)->payloadSize = 0;
        if (gbThisNetGotAdventureControl != 0) {
            REMOTE_MESSAGE(sndBuf)->command =
                static_cast<signed char>(
                    ((giCurPlayer + 1) << 4) | iCurHourGlassPhase |
                    REMOTE_HEARTBEAT_CONTROL_FLAG);
        } else {
            REMOTE_MESSAGE(sndBuf)->command = 0;
        }
        if (giThisNetPos == 0 || gbThisNetGotAdventureControl != 0)
            destination = REMOTE_BROADCAST_PLAYER;
        else
            destination = 0;
        SendRemoteData(reinterpret_cast<unsigned char *>(sndBuf), 0,
            destination, REMOTE_HEARTBEAT_MESSAGE_SIZE);
        lLastHeartbeatSend = KBTickCount();
    }

    if (giThisNetPos == 0) {
        for (queueIndex = 0; queueIndex < giNumHumanPlayers; queueIndex++) {
            if (queueIndex != giThisNetPos &&
                lLastHeartbeatReceive[queueIndex] + REMOTE_HOST_TIMEOUT <
                    KBTickCount() &&
                bInTimeoutFail == 0) {
                bInTimeoutFail = 1;
                gbInPollSound = 0;
                sprintf(gText,
                    "%s's computer is not responding.  Do you wish to keep waiting for a response?",
                    gsNetPlayerInfo[queueIndex].name);
                NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK) {
                    lLastHeartbeatReceive[queueIndex] = KBTickCount();
                } else {
                    hostExit.netPosition = static_cast<signed char>(queueIndex);
                    hostExit.gamePosition = static_cast<signed char>(
                        NetPosToGamePos(queueIndex));
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
        if (lLastHeartbeatReceive[0] + timeout < KBTickCount() &&
            bInTimeoutFail == 0) {
            bInTimeoutFail = 1;
            gbInPollSound = 0;
            if (giThisNetPos == 1) {
                sprintf(gText,
                    "%s's computer is not responding.  Do you wish to keep waiting for a response?",
                    gsNetPlayerInfo[0].name);
            } else {
                sprintf(gText,
                    "Your remote connection to the other players appears to be broken.  Do you wish to keep waiting for a response?");
            }
            NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK) {
                lLastHeartbeatReceive[0] = KBTickCount();
            } else if (giThisNetPos == 1) {
                guestExit.netPosition = 0;
                guestExit.gamePosition = static_cast<signed char>(
                    NetPosToGamePos(0));
                guestExit.updateNetworkControl = 1;
                guestExit.timedOut = 1;
                guestExit.eliminated = 0;
                ReceiveRemotePlayerExit(guestExit);
            } else {
                gpGame->SaveGame("PLYREXIT", 1, 0);
                sprintf(gText,
                    "The current game has been saved as 'PLYREXIT'. Do you wish to keep playing with the computer filling in for the other humans?");
                NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK)
                    DropDownToOnePlayer();
                else
                    ShutDown("");
            }
            gbInPollSound = savedInPollSound;
            bInTimeoutFail = 1;
        }
    }

    for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
        if (rcvBuf[queueIndex] != 0)
            queueCount++;
    }
    if (queueCount == REMOTE_QUEUE_CAPACITY)
        queueFull = 1;
    receiveResult = 1;
    while (receiveResult != 0) {
        receiveResult = ReceiveRemoteData(0,
            reinterpret_cast<unsigned char *>(rcvBufIn),
            REMOTE_BROADCAST_PLAYER);
        if (receiveResult == 0 ||
            REMOTE_MESSAGE(rcvBufIn)->sender == giThisNetPos)
            continue;
        if (REMOTE_MESSAGE(rcvBufIn)->type ==
            REMOTE_MESSAGE_CONFIRM) {
            giLastConfirm = REMOTE_MESSAGE(rcvBufIn)->id;
            return;
        }
        if (REMOTE_MESSAGE(rcvBufIn)->type ==
            REMOTE_MESSAGE_HEARTBEAT) {
            lLastHeartbeatReceive[
                REMOTE_MESSAGE(rcvBufIn)->sender] =
                KBTickCount();
            netCommand =
                REMOTE_MESSAGE(rcvBufIn)->command;
            if ((netCommand & REMOTE_HEARTBEAT_CONTROL_FLAG) == 0)
                return;
            if (gbThisNetGotAdventureControl != 0)
                return;
            iCurHourGlassPhase = netCommand & REMOTE_HEARTBEAT_PHASE_MASK;
            return;
        }
        if (queueFull)
            return;
        if (REMOTE_MESSAGE(rcvBufIn)->type ==
            REMOTE_MESSAGE_RELIABLE) {
            REMOTE_MESSAGE(sndBuf)->sender =
                static_cast<signed char>(giThisNetPos);
            REMOTE_MESSAGE(sndBuf)->id =
                REMOTE_MESSAGE(rcvBufIn)->id;
            REMOTE_MESSAGE(sndBuf)->type =
                REMOTE_MESSAGE_CONFIRM;
            REMOTE_MESSAGE(sndBuf)->payloadSize = 0;
            SendRemoteData(reinterpret_cast<unsigned char *>(sndBuf), 0,
                REMOTE_MESSAGE(rcvBufIn)->sender,
                REMOTE_MESSAGE_HEADER_SIZE);
        }
        for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
            if (rcvBuf[queueIndex] != 0 &&
                REMOTE_MESSAGE(rcvBuf[queueIndex])->id ==
                    REMOTE_MESSAGE(rcvBufIn)->id) {
                break;
            }
        }
        if (queueIndex < REMOTE_QUEUE_CAPACITY)
            continue;
        for (queueIndex = 0; queueIndex < REMOTE_RECENT_ID_COUNT; queueIndex++) {
            if (iLastIds[queueIndex] ==
                REMOTE_MESSAGE(rcvBufIn)->id)
                break;
        }
        if (queueIndex < REMOTE_RECENT_ID_COUNT)
            continue;
        for (queueIndex = 0; queueIndex < REMOTE_QUEUE_CAPACITY; queueIndex++) {
            if (rcvBuf[queueIndex] == 0)
                break;
        }
        if (queueIndex >= REMOTE_QUEUE_CAPACITY)
            continue;
        rcvBuf[queueIndex] = static_cast<char *>(BaseAlloc(
            REMOTE_MESSAGE_SIZE, REMOTE_SOURCE_FILE,
            gPollRemoteLineBase + 235));
        iInOrder[queueIndex] = iInOrderCtr++;
        memcpy(rcvBuf[queueIndex], rcvBufIn, REMOTE_MESSAGE_SIZE);
        queueCount++;
        iLastIds[iCurLastID] =
            REMOTE_MESSAGE(rcvBufIn)->id;
        iCurLastID = (iCurLastID + 1) % REMOTE_RECENT_ID_COUNT;
        if (queueCount == REMOTE_QUEUE_CAPACITY)
            return;
    }
    }
    }
}

// @match-note
// Complete transmit, timeout/retry dialog, response filtering, and common result
// path. The 0x1c frame is exact: wait start -0x04, result -0x08, response -0x0c,
// byte completion flag -0x10, retail-proven dead response state -0x14, and fastcall
// spills -0x18/-0x1c. All 11 relocation targets agree. The first residual is one
// retail five-byte continuation after successful transmit, with one final local-label
// jump at the epilogue. Empty positive arms, direct cancellation returns, common
// breaks, compound predicates, and the explicit common label were tested. Revisit
// in the 95% last-mile phase.
VA(0x004a48e0, 0x163)
int TransmitAndWait(char *data, int destination, int length,
                    signed char command, signed char responseCommand,
                    char **response)
{
    char *receivedData;
    int result;
    int waitStart;
    signed char complete;
    int unusedResponseState;

    if (gbRemoteOn == 0 || gbInNetSetup != 0)
        return 1;
    receivedData = 0;
    result = TransmitRemoteData(data, destination, length, command, 1, 1, -1);
    if (result == 0) {
    } else {
        waitStart = KBTickCount();
        complete = 0;
        while (complete == 0) {
            if (waitStart + REMOTE_CHAIN_TIMEOUT < KBTickCount()) {
                NormalDialog("Error sending data.  Keep trying??", 2,
                    -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == PLAYER_EXIT_CONFIRM_OK) {
                    waitStart = KBTickCount();
                } else {
                    result = 0;
                    goto transmitComplete;
                }
            }
            ForcePollSound();
            receivedData = GetRemoteData(1);
            if (receivedData != 0)
                unusedResponseState = 0;
            if (receivedData != 0 &&
                REMOTE_MESSAGE(receivedData)->type == REMOTE_MESSAGE_RELIABLE &&
                REMOTE_MESSAGE(receivedData)->command == responseCommand) {
                complete = 1;
            }
        }
        *response = receivedData;
    }
transmitComplete:
    return result;
}

// ---- globals (definitions, RVA order) ----
DATA(0x00516f60) int iInOrderCtr = 0;
DATA(0x00516f64) int iCurLastID = 0;
DATA(0x00516f68) int giLastConfirm = -1;
DATA(0x00516f6c) unsigned char GameMode = 0;
DATA(0x00516f70) long lLastHeartbeatSend = 0;
DATA(0x00516f74) int gbInRemoteMain = 0;
DATA(0x00516f78) int gbInRemoteCleanup = 0;
DATA(0x00516f7c) int iIDCtr = 0;
DATA(0x00516f80) int iTimesDropped = 0;
DATA(0x00516f84) signed char gbInNetSetup = 0;
DATA(0x00516f88) int bUseDirectPlay = 0;
DATA(0x00516f8c) int bUseWinsock = 0;
DATA(0x00516f90) signed char bInTimeoutFail = 0;
DATA(0x00516f98) int iBaud[REMOTE_BAUD_RATE_COUNT] = {
    300, 1200, 2400, 9600, 19200, 38400, 57600, 0
};
DATA(0x00516fb8) int iIRQ[REMOTE_IRQ_COUNT] = { 1, 2, 3, 4, 5, 7, 9 };
DATA(0x0052a268) char rcvBufOut[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a378) int iLastIds[REMOTE_RECENT_ID_COUNT];
DATA(0x0052a3f0) char PacketSend[REMOTE_ENCODED_BUFFER_SIZE];
DATA(0x0052a4fc) char gbUseDiffCompression;
DATA(0x0052a500) char gbUseRegularCompression;
DATA(0x0052a508) int iInOrder[REMOTE_QUEUE_STORAGE_COUNT];
DATA(0x0052a730) char sndBuf[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a840) char gcThisNetName[32];
DATA(0x0052a860) long lLastHeartbeatReceive[REMOTE_PLAYER_COUNT];
DATA(0x0052a878) char packet[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052a988) SNetPlayerInfo gsNetPlayerInfo[REMOTE_PLAYER_COUNT];
DATA(0x0052aa58) char rcvBufIn[REMOTE_TRANSPORT_BUFFER_SIZE];
DATA(0x0052ab68) char *rcvBuf[REMOTE_QUEUE_STORAGE_COUNT];
DATA(0x0052ad90) int bGotGameType;
DATA(0x0052ad98) SNetPlayerInfo gsThisNetPlayerInfo;
