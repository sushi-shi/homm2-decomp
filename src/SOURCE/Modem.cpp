#include <Ints.h>
#include <BASE/Utf8.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/Modem.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Network.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>

typedef enum ModemPrivateConstant {
    SETUP_TEXT_CAPACITY = 104,
    RESET_ATTEMPT_COUNT = 2,
    PRINTABLE_CHARACTER_FIRST = 32,
    READ_STORAGE_SIZE = 4,
    HANDSHAKE_TEXT_CAPACITY = 20,
    HANDSHAKE_PACKET_SIZE = 10,
    HANDSHAKE_PREFIX_SIZE = 2,
    HANDSHAKE_ID_SIZE = 6,
    HANDSHAKE_STAGE_INDEX = 9,
    MILLISECONDS_PER_SECOND = 1000,
    INPUT_QUEUE_GUARD = 4
} ModemPrivateConstant;

void ModemSetup(i32 mode) {
    char directConnectMessage3[SETUP_TEXT_CAPACITY];
    i32 resetAttempt9;
    char command[SETUP_TEXT_CAPACITY];

    LogStr("MS1");
    gbRemoteOn = true;
    inque.writePosition = 0;
    inque.readPosition = 0;
    outque.writePosition = 0;
    outque.readPosition = 0;
    iBaudBits = MODEM_BAUD_CLOCK / H2EnumIndex(gConfig.baudRate[gbDirectConnect]);
    com_init(static_cast<u8>(gConfig.comPort[gbDirectConnect]), COM_BAUD_19200, 0);
    LogStr("MS2");

    if (gbDirectConnect == 0) {
        for (resetAttempt9 = 0; resetAttempt9 < RESET_ATTEMPT_COUNT; ++resetAttempt9) {
            if (gConfig.comPort[gbDirectConnect] >= CONFIG_COM_PORT_1)
                utf8::Copy(command, sizeof(command), gConfig.modemInitString);
            else
                sprintf(command, "ATZ");
            PollSound();
            ModemCommand(command);
            DelayMilli(MODEM_RESET_DELAY);
            ModemCommand("\r");
            DelayMilli(MODEM_COMMAND_DELAY);
            PollSound();
        }
    }

    LogStr("MS3");
    switch (mode) {
        case MODEM_MODE_DIAL:
            if (gbDirectConnect == 0 && Dial() != 0) {
                RemoteCleanup();
                GameMode = REMOTE_GAME_NONE;
            }
            break;
        case MODEM_MODE_WAIT:
            if (gbDirectConnect == 0 && Wait() != 0) {
                RemoteCleanup();
                GameMode = REMOTE_GAME_NONE;
            }
            break;
        default:
            return;
    }

    if (gbDirectConnect != 0) {
        LogStr("MS4");
        WFDCStage = MODEM_CONNECTION_INIT_STAGE;
        giWaitType = DIALOG_WAIT_DIRECT_CONNECT;
        strcpy(
            directConnectMessage3,
            localization::Tr("network.modem.direct_wait")

        );
        NormalDialog(directConnectMessage3, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
        LogStr("MS5");
    } else {
        Connect();
    }
}

i32l Dial(void) {
    char dialCommand[MODEM_COMMAND_BUFFER_SIZE];
    iLastDialPos = 0;
    sprintf(dialCommand, "ATDT%s", numbuf);
    sprintf(gText, localization::Tr("network.modem.dialing"), numbuf);
    GUIModemCommand(gText, dialCommand);
    sprintf(gText, localization::Tr("network.modem.dialing"), numbuf);
    if (GUIModemResponse(gText, "CONNECT") != 0)
        return 1;
    return 0;
}

i32l Wait(void) {
    GUIModemResponse(
        localization::Tr("network.modem.waiting_for_ring")  ,
        "RING"
    );
    GUIModemCommand(
        localization::Tr("network.modem.initializing")
        ,
        "ATA"
    );
    if (GUIModemResponse(
            localization::Tr("network.modem.establishing")
            ,
            "CONNECT"
        )
        != 0)
        return 1;
    return 0;
}

void GUIModemCommand(const char* message, const char* command) {
    iLastActionTime = 0;
    iModemCommandPos = 0;
    giWaitType = DIALOG_WAIT_MODEM_COMMAND;
    strcpy(cModemCommand, command);
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
}

i8 GUIModemCommandExec(void) {
    i32 commandLength;
    if (platform::Ticks() < iLastActionTime + MODEM_COMMAND_INTERVAL)
        return 0;

    iLastActionTime = platform::Ticks();
    commandLength = strlen(cModemCommand);
    if (iModemCommandPos < commandLength) {
        write_buffer(cModemCommand + iModemCommandPos, 1);
        ++iModemCommandPos;
        return 0;
    } else {
        write_buffer("\r", 1);
        return 1;
    }
}

void ModemCommand(const char* command) {
    i32 commandLength = strlen(command);
    i32 commandPosition0;
    for (commandPosition0 = 0; commandPosition0 < commandLength; ++commandPosition0) {
        write_buffer(command + commandPosition0, 1);
        DelayMilli(MODEM_COMMAND_DELAY);
    }
    write_buffer("\r", 1);
}

i8 GUIModemResponse(const char* message, const char* response) {
    memset(GUIMRresponse, 0, MODEM_RESPONSE_SIZE);
    GUIMRrespptr = 0;
    strcpy(GUIMRresp, response);
    giWaitType = DIALOG_WAIT_MODEM_RESPONSE;
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
    return 0;
}

i8 GUIModemResponseExec(void) {
    GUIMRc = read_byte();
    if (GUIMRc == -1)
        return 0;
    if (GUIMRc == '\n' || GUIMRrespptr == MODEM_RESPONSE_SIZE - 1) {
        GUIMRresponse[GUIMRrespptr] = 0;
        if (GUIMRrespptr > MODEM_RESPONSE_TRUNCATE_INDEX) {
            TruncateModemResponse();
        }
        goto compareResponse;
    } else {
        if (GUIMRc >= PRINTABLE_CHARACTER_FIRST) {
            GUIMRresponse[GUIMRrespptr] = static_cast<char>(GUIMRc);
            ++GUIMRrespptr;
        }
        return 0;
    }
compareResponse:
    if (strncmp(GUIMRresponse, GUIMRresp, strlen(GUIMRresp)) != 0) {
        GUIMRrespptr = 0;
        return 0;
    } else {
        return 1;
    }
}

i32 write_buffer(const char* buffer, i32 length) {
    if (outque.writePosition + length + MODEM_QUEUE_GUARD > MODEM_OUT_QUEUE_SIZE)
        return 0;
    com_snd(0, 0, static_cast<u16>(length), buffer, 0);
    return 1;
}

i32 read_byte(void) {
    u8 value[READ_STORAGE_SIZE];
    i32 received = com_rcv(0, 1, value);
    if (received == 1)
        return value[0];
    else
        return -1;
}

void write_byte(i32 value) {
    com_snd(0, 0, 1, &value, 0);
}

void Connect(void) {
    char idMessage[HANDSHAKE_TEXT_CAPACITY];
    u32 seed = platform::Ticks();

    seed %= MODEM_ID_MODULUS;
    sprintf(idstr, "%06d", seed);
    oldsec = -1;
    remotestage = 0;
    localstage = remotestage;
    do {
        if (ReadPacket()) {
            packet[packetlen] = 0;
            if (packetlen != HANDSHAKE_PACKET_SIZE)
                continue;
            if (strncmp(packet, "ID", HANDSHAKE_PREFIX_SIZE) != 0)
                continue;
            if (strncmp(packet + HANDSHAKE_PREFIX_SIZE, idstr, HANDSHAKE_ID_SIZE) == 0) {
                sprintf(gText, "Duplicate ID Strings!\nSorry Please Try Again\n");
                GOut(gText);
                RemoteCleanup();
            }
            strncpy(remoteidstr, packet + HANDSHAKE_PREFIX_SIZE, HANDSHAKE_ID_SIZE);
            remotestage = packet[HANDSHAKE_STAGE_INDEX] - '0';
            localstage = remotestage + 1;
            oldsec = -1;
        }

        gModemTimestamp = platform::Ticks();
        if (gModemTimestamp / MILLISECONDS_PER_SECOND != oldsec / MILLISECONDS_PER_SECOND) {
            oldsec = gModemTimestamp;
            sprintf(idMessage, "ID%s_%i", idstr, localstage);
            WriteModemPacket(idMessage, strlen(idMessage));
        }
        PollSound();
    } while (localstage < MODEM_CONNECTION_READY_STAGE);
    while (ReadPacket()) {
    }
}

i32 WaitForDirectConnect(void) {
    char idMessage[HANDSHAKE_TEXT_CAPACITY];
    switch (WFDCStage) {
        case MODEM_CONNECTION_INIT_STAGE: {
            u32 idSeed = platform::Ticks();
            idSeed %= MODEM_ID_MODULUS;
            sprintf(idstr, "%06d", idSeed);
            oldsec = -1;
            remotestage = 0;
            localstage = remotestage;
            ++WFDCStage;
            break;
        }
        case MODEM_CONNECTION_HANDSHAKE_STAGE:
            if (ReadPacket()) {
                packet[packetlen] = 0;
                if (packetlen != HANDSHAKE_PACKET_SIZE)
                    return 0;
                if (strncmp(packet, "ID", HANDSHAKE_PREFIX_SIZE) != 0)
                    return 0;
                if (strncmp(packet + HANDSHAKE_PREFIX_SIZE, idstr, HANDSHAKE_ID_SIZE) == 0) {
                    sprintf(gText, "Duplicate ID Strings!\nSorry Please Try Again\n");
                    GOut(gText);
                    RemoteCleanup();
                }
                strncpy(remoteidstr, packet + HANDSHAKE_PREFIX_SIZE, HANDSHAKE_ID_SIZE);
                remotestage = packet[HANDSHAKE_STAGE_INDEX] - '0';
                localstage = remotestage + 1;
                oldsec = -1;
            }
            gModemTimestamp = platform::Ticks();
            if (gModemTimestamp / MILLISECONDS_PER_SECOND != oldsec / MILLISECONDS_PER_SECOND) {
                oldsec = gModemTimestamp;
                sprintf(idMessage, "ID%s_%i", idstr, localstage);
                WriteModemPacket(idMessage, strlen(idMessage));
            }
            if (localstage >= MODEM_CONNECTION_READY_STAGE)
                ++WFDCStage;
            break;
        case MODEM_CONNECTION_READY_STAGE:
            if (ReadPacket() == 0)
                return 1;
            break;
    }
    return 0;
}

char ReadPacket(void) {
    i32 input;
    if (inque.writePosition > MODEM_QUEUE_INPUT_SIZE - H2EnumIndex(INPUT_QUEUE_GUARD)) {
        LogStr("OverFlow1");
        inque.writePosition = 0;
        newpacket = 1;
    }
readPacketStart:
    if (newpacket != 0) {
        packetlen = 0;
        newpacket = 0;
    }
    do {
    readNextByte:
        input = read_byte();
        if (input < 0)
            return 0;
        if (inescape != 0) {
            inescape = 0;
            if (input == MODEM_PACKET_END) {
                newpacket = 1;
                return 1;
            } else if (input == 0) {
                newpacket = 1;
                goto readPacketStart;
            }
        } else {
            if (input == MODEM_ESCAPE_BYTE) {
                inescape = 1;
                goto readNextByte;
            }
        }
        if (packetlen >= MODEM_PACKET_PAYLOAD_SIZE) {
            newpacket = 1;
            LogStr("OverFlow2");
            goto readPacketStart;
        }
        packet[packetlen] = static_cast<char>(input);
        ++packetlen;
    } while (1);
}

void WriteModemPacket(char* buffer, i32 length) {
    i32 encodedPosition = 0;
    if (length > MODEM_PACKET_PAYLOAD_SIZE) {
        LogStr("TOO LONG");
        return;
    }

    char encoded[MODEM_ENCODED_PACKET_SIZE];

    encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
    ++encodedPosition;
    encoded[encodedPosition] = 0;
    ++encodedPosition;
    while (length--) {
        if (*buffer == MODEM_ESCAPE_BYTE) {
            encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
            ++encodedPosition;
        }
        encoded[encodedPosition] = *buffer;
        ++encodedPosition;
        ++buffer;
    }
    encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
    ++encodedPosition;
    encoded[encodedPosition] = MODEM_PACKET_END;
    ++encodedPosition;
    while (write_buffer(encoded, encodedPosition) == 0)
        ForcePollSound();
}

i32 iBaudBits = SERIAL_BYTE_SIZE;
i32 inescape = 0;
i32 newpacket = 0;
i32 packetlen = 0;
char idstr[MODEM_ID_SIZE];
i32 GUIMRc;
i32 iModemCommandPos;
i32 GUIMRrespptr;
i32 localstage;
char numbuf[MODEM_NUMBER_BUFFER_SIZE];
i32 WFDCStage;
char remoteidstr[MODEM_ID_SIZE];
i32 gModemTimestamp;
char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
i32 iLastDialPos;
i32 remotestage;
char GUIMRresp[MODEM_RESPONSE_SIZE];
i32 oldsec;
inque_t inque;
i32 iLastActionTime;
char GUIMRresponse[MODEM_RESPONSE_SIZE];
outque_t outque;
