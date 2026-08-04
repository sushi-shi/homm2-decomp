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
H2_ENUM_END(ModemPrivateConstant)

VA(0x00472ca0, 0x1f8)
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
    iBaudBits = MODEM_BAUD_CLOCK / IDX(gConfig.baudRate[gbDirectConnect]);
    com_init(static_cast<u8>(gConfig.comPort[gbDirectConnect]), COM_BAUD_19200, 0);
    LogStr("MS2");

    if (gbDirectConnect == 0) {
        for (resetAttempt9 = 0; resetAttempt9 < RESET_ATTEMPT_COUNT; ++resetAttempt9) {
            if (gConfig.comPort[gbDirectConnect] >= CONFIG_COM_PORT_1)
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
            "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xef\xee\xe4\xea\xeb\xfe\xf7\xe5\xed\xe8\xff "
            "\xe4\xf0\xf3\xe3\xee\xe3\xee \xea\xee\xec\xef\xfc\xfe\xf2\xe5\xf0\xe0 \xea "
            "\xef\xf0\xff\xec\xee\xec\xf3 "
            "\xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xfe.\n\n\xcd\xe0\xe6\xec\xe8\xf2\xe5 "
            "'\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb \xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc "
            "\xee\xe6\xe8\xe4\xe0\xed\xe8\xe5." /* "Ожидание подключения другого компьютера к прямому соединению.

Нажмите 'ОТМЕНА', чтобы прервать ожидание." */
        );
        NormalDialog(directConnectMessage3, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(NULL);
        LogStr("MS5");
    } else {
        Connect();
    }
}

VA(0x00472e98, 0x8e)
i32l Dial(void) {
    char dialCommand[MODEM_COMMAND_BUFFER_SIZE];
    iLastDialPos = 0;
    sprintf(dialCommand, "ATDT%s", numbuf);
    sprintf(gText, "%s %s", "\xc7\xe2\xee\xed\xfe..." /* "Звоню..." */, numbuf);
    GUIModemCommand(gText, dialCommand);
    sprintf(gText, "%s %s", "\xc7\xe2\xee\xed\xfe..." /* "Звоню..." */, numbuf);
    if (GUIModemResponse(gText, "CONNECT") != 0)
        return 1;
    return 0;
}

VA(0x00472f26, 0x42)
i32l Wait(void) {
    GUIModemResponse(
        "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe7\xe2\xee\xed\xea\xe0..." /* "Ожидание звонка..." */,
        "RING"
    );
    GUIModemCommand(
        "\xc8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xff \xec\xee\xe4\xe5\xec\xe0..." /* "Инициализация модема..." */
        ,
        "ATA"
    );
    if (GUIModemResponse(
            "\xd3\xf1\xf2\xe0\xed\xee\xe2\xea\xe0 \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xff..." /* "Установка соединения..." */
            ,
            "CONNECT"
        )
        != 0)
        return 1;
    return 0;
}

VA(0x00472f68, 0x6c)
void GUIModemCommand(char* message, char* command) {
    iLastActionTime = 0;
    iModemCommandPos = 0;
    giWaitType = DIALOG_WAIT_MODEM_COMMAND;
    strcpy(cModemCommand, command);
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
}

VA(0x00472fd4, 0x7e)
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

VA(0x00473052, 0x61)
void ModemCommand(char* command) {
    char modemText[MODEM_WORK_TEXT_SIZE];
    i32 commandLength = strlen(command);
    i32 commandPosition0;
    for (commandPosition0 = 0; commandPosition0 < commandLength; ++commandPosition0) {
        write_buffer(command + commandPosition0, 1);
        DelayMilli(MODEM_COMMAND_DELAY);
    }
    write_buffer("\r", 1);
}

VA(0x004730b3, 0x75)
i8 GUIModemResponse(char* message, char* response) {
    memset(GUIMRresponse, 0, MODEM_RESPONSE_SIZE);
    GUIMRrespptr = 0;
    strcpy(GUIMRresp, response);
    giWaitType = DIALOG_WAIT_MODEM_RESPONSE;
    NormalDialog(message, NORMAL_DIALOG_WAIT_LAST, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(NULL);
    return 0;
}

VA(0x00473128, 0xb3)
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

VA(0x004731db, 0x42)
i32 write_buffer(char* buffer, i32 length) {
    if (outque.writePosition + length + MODEM_QUEUE_GUARD > MODEM_OUT_QUEUE_SIZE)
        return 0;
    com_snd(0, 0, static_cast<u16>(length), buffer, 0);
    return 1;
}

VA(0x0047321d, 0x33)
i32 read_byte(void) {
    u8 value[READ_STORAGE_SIZE];
    i32 received = com_rcv(0, 1, value);
    if (received == 1)
        return value[0];
    else
        return -1;
}

VA(0x00473250, 0x1c)
void write_byte(i32 value) {
    com_snd(0, 0, 1, &value, 0);
}

VA(0x0047326c, 0x19c)
void Connect(void) {
    char idMessage[HANDSHAKE_TEXT_CAPACITY];
    u32 seed = KBTickCount();
    i32 packetResult;
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

        stime = KBTickCount();
        if (oldsec / MILLISECONDS_PER_SECOND != stime / MILLISECONDS_PER_SECOND) {
            oldsec = stime;
            sprintf(idMessage, "ID%s_%i", idstr, localstage);
            WriteModemPacket(idMessage, strlen(idMessage));
        }
        PollSound();
    } while (localstage < MODEM_CONNECTION_READY_STAGE);
    while (ReadPacket()) {
    }
}

VA(0x00473408, 0x1e4)
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
            if (oldsec / MILLISECONDS_PER_SECOND != stime / MILLISECONDS_PER_SECOND) {
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

VA(0x004735ec, 0x100)
char ReadPacket(void) {
    i32 input;
    if (inque.writePosition > MODEM_QUEUE_INPUT_SIZE - IDX(INPUT_QUEUE_GUARD)) {
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

VA(0x004736ec, 0x10b)
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

i32 iBaudBits = COM_SERIAL_BYTE_SIZE;
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
i32 stime;
char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
i32 iLastDialPos;
i32 remotestage;
char GUIMRresp[MODEM_RESPONSE_SIZE];
i32 oldsec;
inque_t inque;
i32 iLastActionTime;
char GUIMRresponse[MODEM_RESPONSE_SIZE];
outque_t outque;
