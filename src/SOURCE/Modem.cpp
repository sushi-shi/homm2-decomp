// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Modem.obj   from: (directly linked into exe)
// functions: 15   data: 22
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
VA(0x0040c8f0, 0x24e)
void ModemSetup(int mode)
{
    char directConnectMessage3[104];
    int resetAttempt9;
    char command[104];

    LogStr("MS1");
    gbRemoteOn = 1;
    inque.writePosition = 0;
    inque.readPosition = 0;
    outque.writePosition = 0;
    outque.readPosition = 0;
    iBaudBits = MODEM_BAUD_CLOCK / gConfig.baudRate[gbDirectConnect];
    com_init(static_cast<unsigned char>(gConfig.comPort[gbDirectConnect]), 4, 0);
    LogStr("MS2");

    if (gbDirectConnect == 0) {
        for (resetAttempt9 = 0; resetAttempt9 < 2; ++resetAttempt9) {
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
        strcpy(directConnectMessage3,
               "Waiting for other computer to log in to direct connection.\n\n"
               "Press 'CANCEL' to abort.");
        NormalDialog(directConnectMessage3, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
        LogStr("MS5");
    } else {
        Connect();
    }
}

// @early-stop: all non-relocation bytes in the declared 0x9e-byte span agree and
// all 19 relocation offsets are present. The residual is only delinked literal-pool
// identities for the dial format/status/CONNECT strings.
VA(0x0040cb3e, 0x9e)
long int Dial(void)
{
    char dialCommand[40];
    iLastDialPos = 0;
    sprintf(dialCommand, "ATDT%s", numbuf);
    sprintf(gText, "%s %s", "Dialing...", numbuf);
    GUIModemCommand(gText, dialCommand);
    sprintf(gText, "%s %s", "Dialing...", numbuf);
    return GUIModemResponse(gText, "CONNECT") != 0;
}

// @early-stop: all non-relocation bytes in the declared 0x54-byte span agree and
// all 9 relocation offsets are present. The residual is only delinked literal-pool
// identities for the ring/answer/connection strings.
VA(0x0040cbdc, 0x54)
long int Wait(void)
{
    GUIModemResponse("Waiting for ring...", "RING");
    GUIModemCommand("Initializing modem...", "ATA");
    return GUIModemResponse("Establishing connection...", "CONNECT") != 0;
}

VA(0x0040cc30, 0x79)
void GUIModemCommand(char *message, char *command)
{
    iLastActionTime = 0;
    iModemCommandPos = 0;
    giWaitType = MODEM_WAIT_COMMAND;
    strcpy(cModemCommand, command);
    NormalDialog(message, 6, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(0);
}

VA(0x0040cca9, 0x95)
signed char GUIModemCommandExec(void)
{
    int commandLength;
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
void ModemCommand(char *command)
{
    char modemText[MODEM_WORK_TEXT_SIZE];
    int commandLength = strlen(command);
    int commandPosition0;
    for (commandPosition0 = 0; commandPosition0 < 0[&commandLength];
         ++commandPosition0) {
        write_buffer(command + commandPosition0, 1);
        DelayMilli(MODEM_COMMAND_DELAY);
    }
    write_buffer("\r", 1);
}

VA(0x0040cdcc, 0x82)
signed char GUIModemResponse(char *message, char *response)
{
    memset(GUIMRresponse, 0, MODEM_RESPONSE_SIZE);
    GUIMRrespptr = 0;
    strcpy(GUIMRresp, response);
    giWaitType = MODEM_WAIT_RESPONSE;
    NormalDialog(message, 6, -1, -1, -1, 0, -1, 0, -1, 0);
    if (gbFunctionComplete == 0)
        ShutDown(0);
    return 0;
}

// @early-stop: all 0xe2 bytes agree after masking the 20 aligned relocation
// fields. The only residual is the target delinker's synthetic literal name for
// the interior byte at GUIMRresponse + MODEM_RESPONSE_TRUNCATE_INDEX.
VA(0x0040ce4e, 0xe2)
signed char GUIModemResponseExec(void)
{
    GUIMRc = read_byte();
    if (GUIMRc == -1)
        return 0;
    if (GUIMRc == '\n' || GUIMRrespptr == MODEM_RESPONSE_SIZE - 1) {
        GUIMRresponse[GUIMRrespptr] = 0;
        if (GUIMRrespptr > MODEM_RESPONSE_TRUNCATE_INDEX) {
            TruncateModemResponse();
        }
    } else {
        if (GUIMRc >= 32) {
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
int write_buffer(char *buffer, int length)
{
    if (outque.writePosition + length + MODEM_QUEUE_GUARD >
        MODEM_OUT_QUEUE_SIZE)
        return 0;
    com_snd(0, 0, static_cast<unsigned short>(length), buffer, 0);
    return 1;
}

VA(0x0040cf7e, 0x47)
int read_byte(void)
{
    unsigned char value[4];
    int received = com_rcv(0, 1, value);
    if (received == 1)
        return value[0];
    else
        return -1;
}

VA(0x0040cfc5, 0x27)
void write_byte(int value)
{
    com_snd(0, 0, 1, &value, 0);
}

VA(0x0040cfec, 0x1bb)
void Connect(void)
{
    char idMessage[20];
    unsigned int idSeed = KBTickCount();
    // The retail /Od frame retains this unused word between idMessage and idSeed.
    int packetResult;
    idSeed %= MODEM_ID_MODULUS;
    sprintf(idstr, "%06d", idSeed);
    oldsec = -1;
    remotestage = 0;
    localstage = remotestage;
    do {
        if (ReadPacket()) {
            packet[packetlen] = 0;
            if (packetlen != 10)
                continue;
            if (strncmp(packet, "ID", 2) != 0)
                continue;
            if (strncmp(packet + 2, idstr, 6) == 0) {
                sprintf(gText,
                        "Duplicate ID Strings!\nSorry Please Try Again\n");
                GOut(gText);
                RemoteCleanup();
            }
            strncpy(remoteidstr, packet + 2, 6);
            remotestage = packet[9] - '0';
            localstage = remotestage + 1;
            oldsec = -1;
        }

        stime = KBTickCount();
        if (stime / 1000 != oldsec / 1000) {
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
int WaitForDirectConnect(void)
{
    char idMessage[20];
    switch (WFDCStage) {
    case MODEM_CONNECTION_INIT_STAGE: {
        unsigned int idSeed = KBTickCount();
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
            if (packetlen != 10)
                return 0;
            if (strncmp(packet, "ID", 2) != 0)
                return 0;
            if (strncmp(packet + 2, idstr, 6) == 0) {
                sprintf(gText,
                        "Duplicate ID Strings!\nSorry Please Try Again\n");
                GOut(gText);
                RemoteCleanup();
            }
            strncpy(remoteidstr, packet + 2, 6);
            remotestage = packet[9] - '0';
            localstage = remotestage + 1;
            oldsec = -1;
        }
        stime = KBTickCount();
        if (stime / 1000 != oldsec / 1000) {
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
char ReadPacket(void)
{
    int input;
    if (inque.writePosition > MODEM_QUEUE_INPUT_SIZE - 4) {
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

// @early-stop: all non-relocation bytes in the declared 0xff-byte span agree and all
// 4 relocation offsets are present. The recovered loop assigns `remaining5 = length--`,
// placing remaining at -0x228 and the encoded buffer at -0x224; the sole residual is
// the delinked `TOO LONG` literal-pool identity.
VA(0x0040d4df, 0xff)
void WriteModemPacket(char *buffer, int length)
{
    int encodedPosition = 0;
    if (length > MODEM_PACKET_PAYLOAD_SIZE) {
        LogStr("TOO LONG");
    } else {
        int remaining5;
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
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ede50) int iBaudBits = 8;
DATA(0x004ede54) int inescape = 0;
DATA(0x004ede58) int newpacket = 0;
DATA(0x004ede5c) int packetlen = 0;
DATA(0x00523280) char idstr[MODEM_ID_SIZE];
DATA(0x00523288) int GUIMRc;
DATA(0x0052328c) int iModemCommandPos;
DATA(0x00523290) int GUIMRrespptr;
DATA(0x00523294) int localstage;
DATA(0x00523298) char numbuf[MODEM_NUMBER_BUFFER_SIZE];
DATA(0x005232bc) int WFDCStage;
DATA(0x005232c0) char remoteidstr[MODEM_ID_SIZE];
DATA(0x005232c8) int stime;
DATA(0x005232d0) char cModemCommand[MODEM_COMMAND_BUFFER_SIZE];
DATA(0x005232f8) int iLastDialPos;
DATA(0x005232fc) int remotestage;
DATA(0x00523300) char GUIMRresp[MODEM_RESPONSE_SIZE];
DATA(0x00523350) int oldsec;
DATA(0x00523358) inque_t inque;
DATA(0x00524360) int iLastActionTime;
DATA(0x00524368) char GUIMRresponse[MODEM_RESPONSE_SIZE];
DATA(0x005243b8) outque_t outque;
