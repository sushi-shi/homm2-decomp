#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/Modem.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/comwin.h>
#include <SOURCE/kbwin.h>

H2_ENUM_BEGIN(ModemPrivateConstant)
    SETUP_TEXT_CAPACITY       = 104,
    RESET_ATTEMPT_COUNT       = 2,
    PRINTABLE_CHARACTER_FIRST = 32,
    READ_STORAGE_SIZE         = 4,
    HANDSHAKE_TEXT_CAPACITY   = 20,
    HANDSHAKE_PACKET_SIZE     = 10,
    HANDSHAKE_PREFIX_SIZE     = 2,
    HANDSHAKE_ID_SIZE         = 6,
    HANDSHAKE_STAGE_INDEX     = 9,
    MILLISECONDS_PER_SECOND   = 1000,
    INPUT_QUEUE_GUARD         = 4
H2_ENUM_END(ModemPrivateConstant)

VA(0x0040c8f0, 0x24e)
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
    iBaudBits = MODEM_BAUD_CLOCK / gConfig.baudRate[gbDirectConnect];
    com_init(static_cast<u8>(gConfig.comPort[gbDirectConnect]), IDX(COM_BAUD_19200), 0);
    LogStr("MS2");

    if (gbDirectConnect == 0) {
        for (resetAttempt9 = 0; resetAttempt9 < RESET_ATTEMPT_COUNT; ++resetAttempt9) {
            if (gConfig.comPort[gbDirectConnect] >= 1)
                sprintf(command, gConfig.modemInitString);
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
                GameMode = 0;
            }
            break;
        case MODEM_MODE_WAIT:
            if (gbDirectConnect == 0 && Wait() != 0) {
                RemoteCleanup();
                GameMode = 0;
            }
            break;
        default:
            return;
    }

    if (gbDirectConnect != 0) {
        LogStr("MS4");
        WFDCStage = MODEM_CONNECTION_INIT_STAGE;
        giWaitType = MODEM_WAIT_DIRECT_CONNECT;
        strcpy(
            directConnectMessage3,
            "Waiting for other computer to log in to direct connection.\n\n"
            "Press 'CANCEL' to abort."
        );
        NormalDialog(
            directConnectMessage3,
            NORMAL_DIALOG_WAIT_LAST,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
        LogStr("MS5");
    } else {
        Connect();
    }
}

VA(0x0040cb3e, 0x9e)
i32l Dial(void) {
    char dialCommand[MODEM_COMMAND_BUFFER_SIZE];
    iLastDialPos = 0;
    sprintf(dialCommand, "ATDT%s", numbuf);
    sprintf(gText, "%s %s", "Dialing...", numbuf);
    GUIModemCommand(gText, dialCommand);
    sprintf(gText, "%s %s", "Dialing...", numbuf);
    return GUIModemResponse(gText, "CONNECT") != 0;
}

VA(0x0040cbdc, 0x54)
i32l Wait(void) {
    GUIModemResponse("Waiting for ring...", "RING");
    GUIModemCommand("Initializing modem...", "ATA");
    return GUIModemResponse("Establishing connection...", "CONNECT") != 0;
}

VA(0x0040cc30, 0x79)
void GUIModemCommand(char* message, char* command) {
    iLastActionTime = 0;
    iModemCommandPos = 0;
    giWaitType = MODEM_WAIT_COMMAND;
    strcpy(cModemCommand, command);
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
}

VA(0x0040cca9, 0x95)
i8 GUIModemCommandExec(void) {
    i32 commandLength;
    if (KBTickCount() < iLastActionTime + MODEM_COMMAND_INTERVAL)
        return 0;

    iLastActionTime = KBTickCount();
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

VA(0x0040cd3e, 0x8e)
void ModemCommand(char* command) {
    char modemText[MODEM_WORK_TEXT_SIZE];
    i32 commandLength = strlen(command);
    i32 commandPosition0;
    for (commandPosition0 = 0; commandPosition0 < OD_STEER(commandLength); ++commandPosition0) {
        write_buffer(command + commandPosition0, 1);
        DelayMilli(MODEM_COMMAND_DELAY);
    }
    write_buffer("\r", 1);
}

VA(0x0040cdcc, 0x82)
i8 GUIModemResponse(char* message, char* response) {
    memset(GUIMRresponse, 0, MODEM_RESPONSE_SIZE);
    GUIMRrespptr = 0;
    strcpy(GUIMRresp, response);
    giWaitType = MODEM_WAIT_RESPONSE;
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
    return 0;
}

VA(0x0040ce4e, 0xe2)
i8 GUIModemResponseExec(void) {
    GUIMRc = read_byte();
    if (GUIMRc == -1)
        return 0;
    if (GUIMRc == '\n' || GUIMRrespptr == MODEM_RESPONSE_SIZE - 1) {
        GUIMRresponse[GUIMRrespptr] = 0;
        if (GUIMRrespptr > MODEM_RESPONSE_TRUNCATE_INDEX) {
            TruncateModemResponse();
        }
    } else {
        if (GUIMRc >= PRINTABLE_CHARACTER_FIRST) {
            GUIMRresponse[GUIMRrespptr] = static_cast<char>(GUIMRc);
            ++GUIMRrespptr;
        }
        return 0;
    }
    for (;;) {
        if (strncmp(GUIMRresponse, GUIMRresp, strlen(GUIMRresp)) != 0) {
            GUIMRrespptr = 0;
            return 0;
        } else {
            return 1;
        }
    }
}

VA(0x0040cf30, 0x4e)
i32 write_buffer(char* buffer, i32 length) {
    if (outque.writePosition + length + MODEM_QUEUE_GUARD > MODEM_OUT_QUEUE_SIZE)
        return 0;
    com_snd(0, 0, static_cast<u16>(length), buffer, 0);
    return 1;
}

VA(0x0040cf7e, 0x47)
i32 read_byte(void) {
    u8 value[READ_STORAGE_SIZE];
    i32 received = com_rcv(0, 1, value);
    if (received == 1)
        return value[0];
    else
        return -1;
}

VA(0x0040cfc5, 0x27)
void write_byte(i32 value) {
    com_snd(0, 0, 1, &value, 0);
}

VA(0x0040cfec, 0x1bb)
void Connect(void) {
    char idMessage[HANDSHAKE_TEXT_CAPACITY];
    u32 idSeed = KBTickCount();
    i32 packetResult;
    idSeed %= MODEM_ID_MODULUS;
    sprintf(idstr, "%06d", idSeed);
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

        stime = KBTickCount();
        if (OD_STEER(oldsec) / MILLISECONDS_PER_SECOND != stime / MILLISECONDS_PER_SECOND) {
            oldsec = stime;
            sprintf(idMessage, "ID%s_%i", idstr, localstage);
            WriteModemPacket(idMessage, strlen(idMessage));
        }
        PollSound();
    } while (localstage < MODEM_CONNECTION_READY_STAGE);
    while (ReadPacket()) {
    }
}

VA(0x0040d1a7, 0x211)
i32 WaitForDirectConnect(void) {
    char idMessage[HANDSHAKE_TEXT_CAPACITY];
    switch (WFDCStage) {
        case MODEM_CONNECTION_INIT_STAGE: {
            u32 idSeed = KBTickCount();
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
            stime = KBTickCount();
            if (OD_STEER(oldsec) / MILLISECONDS_PER_SECOND
                != stime / MILLISECONDS_PER_SECOND) {
                oldsec = stime;
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

VA(0x0040d3b8, 0x127)
char ReadPacket(void) {
    i32 input;
    if (inque.writePosition > MODEM_QUEUE_INPUT_SIZE - INPUT_QUEUE_GUARD) {
        LogStr("OverFlow1");
        inque.writePosition = 0;
        newpacket = 1;
    }
readPacketStart:
    if (newpacket != 0) {
        packetlen = 0;
        newpacket = 0;
    }
    for (;;) {
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
                continue;
            }
        }
        if (packetlen >= MODEM_PACKET_PAYLOAD_SIZE) {
            newpacket = 1;
            LogStr("OverFlow2");
            goto readPacketStart;
        }
        packet[packetlen] = static_cast<char>(input);
        ++packetlen;
    }
}

VA(0x0040d4df, 0xFF)
void WriteModemPacket(char* buffer, i32 length) {
    i32 encodedPosition = 0;
    if (length > MODEM_PACKET_PAYLOAD_SIZE) {
        LogStr("TOO LONG");
        return;
    }

    i32 remaining5;
    char encoded[MODEM_ENCODED_PACKET_SIZE];

    encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
    ++encodedPosition;
    encoded[encodedPosition] = 0;
    ++encodedPosition;
    while ((remaining5 = length--) != 0) {
        if (*buffer == MODEM_ESCAPE_BYTE) {
            encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
            ++encodedPosition;
        }
        encoded[encodedPosition] = *buffer;
        ++buffer;
        ++encodedPosition;
    }
    encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
    ++encodedPosition;
    encoded[encodedPosition] = MODEM_PACKET_END;
    ++encodedPosition;
    while (write_buffer(encoded, encodedPosition) == 0)
        ForcePollSound();
}

DATA(0x004ede50) i32 iBaudBits = COM_SERIAL_BYTE_SIZE;
DATA(0x004ede54) i32 inescape = 0;
DATA(0x004ede58) i32 newpacket = 0;
DATA(0x004ede5c) i32 packetlen = 0;
DATA(0x00523280) char idstr[MODEM_ID_SIZE];
DATA(0x00523288) i32 GUIMRc;
DATA(0x0052328c) i32 iModemCommandPos;
DATA(0x00523290) i32 GUIMRrespptr;
DATA(0x00523294) i32 localstage;
DATA(0x00523298) char numbuf[MODEM_NUMBER_BUFFER_SIZE];
DATA(0x005232bc) i32 WFDCStage;
DATA(0x005232c0) char remoteidstr[MODEM_ID_SIZE];
DATA(0x005232c8) i32 stime;
DATA(0x005232d0) char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
DATA(0x005232f8) i32 iLastDialPos;
DATA(0x005232fc) i32 remotestage;
DATA(0x00523300) char GUIMRresp[MODEM_RESPONSE_SIZE];
DATA(0x00523350) i32 oldsec;
DATA(0x00523358) inque_t inque;
DATA(0x00524360) i32 iLastActionTime;
DATA(0x00524368) char GUIMRresponse[MODEM_RESPONSE_SIZE];
DATA(0x005243b8) outque_t outque;
