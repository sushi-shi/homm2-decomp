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
// @match-note
// Complete 0xdc frame, reset/dial/wait/direct-connect CFG, queue/config layouts, and
// all 52 relocations agree. Residuals begin in local-slot ordering and string aliases;
// tried branch and switch dispatch plus separate/shared final Connect calls.
// Revisit after config/queue layout evidence changes or in the last-mile phase.
VA(0x0040c8f0, 0x24e)
void ModemSetup(int mode)
{
    char directConnectMessage[104];
    int resetAttempt;
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
        for (resetAttempt = 0; resetAttempt < 2; ++resetAttempt) {
            if (gConfig.comPort[gbDirectConnect] < 1)
                sprintf(command, "ATZ");
            else
                sprintf(command, gConfig.modemInitString);
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
            return;
        }
        break;
    case MODEM_MODE_WAIT:
        if (gbDirectConnect == 0 && Wait() != 0) {
            RemoteCleanup();
            GameMode = 0;
            return;
        }
        break;
    default:
        return;
    }

    if (gbDirectConnect != 0) {
        LogStr("MS4");
        WFDCStage = 0;
        giWaitType = MODEM_WAIT_DIRECT_CONNECT;
        strcpy(directConnectMessage,
               "Waiting for other computer to log in to direct connection.\n\n"
               "Press 'CANCEL' to abort.");
        NormalDialog(directConnectMessage, 6, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
        LogStr("MS5");
    }
    Connect();
}

// @match-note
// Complete 0x2c frame and dial command/response CFG; all 19 relocations agree.
// Residual is pooled-string/local-slot identity after trying direct and staged returns.
// Revisit in the last-mile phase.
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

// @match-note
// Complete ring/answer/connect sequence and all 9 relocations agree. Remaining bytes
// are pooled-string identities and continuation jumps after direct/staged returns.
// Revisit in the last-mile phase.
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

// @match-note
// Complete timed character-send CFG and 0x4 local frame; all 13 relocations agree.
// First residual is local strlen slot ordering, followed by one continuation jump;
// tried inline strlen and the retained explicit length local. Revisit last-mile.
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
    }
    write_buffer("\r", 1);
    return 1;
}

// @match-note
// Complete 0x8 frame, per-character delay loop, and all 5 relocations agree. Residuals
// are the carriage-return string alias and local-slot order after inline/stored strlen.
// Revisit in the last-mile phase.
VA(0x0040cd3e, 0x8e)
void ModemCommand(char *command)
{
    int commandLength = strlen(command);
    int commandPosition;
    for (commandPosition = 0; commandPosition < commandLength;
         ++commandPosition) {
        write_buffer(command + commandPosition, 1);
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

// @match-note
// Complete response accumulation/truncation/compare CFG and all 20 relocations agree.
// Residuals are branch continuation placement around the >17 truncation arm; tried
// nested returns and the retained shared final compare. Revisit in the last-mile phase.
VA(0x0040ce4e, 0xe2)
signed char GUIModemResponseExec(void)
{
    GUIMRc = read_byte();
    if (GUIMRc == -1)
        return 0;
    if (GUIMRc == '\n' || GUIMRrespptr == MODEM_RESPONSE_SIZE - 1) {
        GUIMRresponse[GUIMRrespptr] = 0;
        if (GUIMRrespptr > 17) {
            GUIMRresponse[17] = 0;
            return 0;
        }
    } else if (GUIMRc >= 32) {
        GUIMRresponse[GUIMRrespptr] = static_cast<char>(GUIMRc);
        ++GUIMRrespptr;
        return 0;
    }
    if (strncmp(GUIMRresponse, GUIMRresp, strlen(GUIMRresp)) != 0) {
        GUIMRrespptr = 0;
        return 0;
    }
    return 1;
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

// @match-note
// Complete 0x8 frame and receive-result semantics; the sole relocation agrees. The
// first residual is the compiler temporary slot for sign-extending com_rcv's short;
// tried short and int result locals. Revisit in the last-mile phase.
VA(0x0040cf7e, 0x47)
int read_byte(void)
{
    unsigned char value[4];
    int received = com_rcv(0, 1, value);
    if (received == 1)
        return value[0];
    return -1;
}

VA(0x0040cfc5, 0x27)
void write_byte(int value)
{
    com_snd(0, 0, 1, &value, 0);
}

// @match-note
// Complete 0x18 frame, ID handshake/retry/drain CFG, and all 47 relocations agree.
// Residuals begin with unsigned tick modulo temporaries and branch trampolines; tried
// direct modulo, signed/unsigned seed locals, and both time-compare orders. Revisit last-mile.
VA(0x0040cfec, 0x1bb)
void Connect(void)
{
    char idMessage[20];
    unsigned int idSeed = KBTickCount();
    sprintf(idstr, "%06d", idSeed % 1000000);
    oldsec = -1;
    remotestage = 0;
    localstage = remotestage;
    for (;;) {
        if (ReadPacket()) {
            packet[packetlen] = 0;
            if (packetlen == 10 && strncmp(packet, "ID", 2) == 0) {
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
        }

        stime = KBTickCount();
        if (stime / 1000 != oldsec / 1000) {
            oldsec = stime;
            sprintf(idMessage, "ID%s_%i", idstr, localstage);
            WriteModemPacket(idMessage, strlen(idMessage));
        }
        PollSound();
        if (localstage >= 1) {
            while (ReadPacket()) {
            }
            return;
        }
    }
}

// @match-note
// Complete 0x1c frame, three-stage switch CFG, and all 49 relocations agree. Residuals
// are stage/tick temporary slots and switch continuation jumps after trying if chains,
// direct switch, and signed/unsigned ID seeds. Revisit in the last-mile phase.
VA(0x0040d1a7, 0x211)
int WaitForDirectConnect(void)
{
    char idMessage[20];
    int stage = WFDCStage;
    switch (stage) {
    case 0: {
        unsigned int idSeed = KBTickCount();
        sprintf(idstr, "%06d", idSeed % 1000000);
        oldsec = -1;
        remotestage = 0;
        localstage = remotestage;
        ++WFDCStage;
        break;
    }
    case 1:
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
        if (localstage >= 1)
            ++WFDCStage;
        break;
    case 2:
        if (ReadPacket() == 0)
            return 1;
        break;
    }
    return 0;
}

// @match-note
// Complete escape/reset/overflow state machine and all 21 relocations agree. The first
// residuals are delinked inque+4 and string identities, followed by continuation jumps;
// tried goto and nested-loop forms, retaining retail body order. Revisit last-mile.
VA(0x0040d3b8, 0x127)
char ReadPacket(void)
{
    int input;
    if (inque.writePosition > MODEM_QUEUE_INPUT_SIZE - 4) {
        LogStr("OverFlow1");
        inque.writePosition = 0;
        newpacket = 1;
    }
    for (;;) {
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
                }
                if (input == 0) {
                    newpacket = 1;
                    break;
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
                break;
            }
            packet[packetlen] = static_cast<char>(input);
            ++packetlen;
        }
    }
}

// @match-note
// Complete 0x234 frame, byte-stuffing loop, queue retry CFG, and all 4 relocations agree.
// Residuals are local slot allocation and final loop continuations after trying separate
// source/index locals and the retained in-place parameter/index form. Revisit last-mile.
VA(0x0040d4df, 0xff)
void WriteModemPacket(char *buffer, int length)
{
    int encodedPosition = 0;
    if (length > MODEM_PACKET_PAYLOAD_SIZE) {
        LogStr("TOO LONG");
    } else {
        int remaining;
        char encoded[MODEM_ENCODED_PACKET_SIZE];

        encoded[encodedPosition] = MODEM_ESCAPE_BYTE;
        ++encodedPosition;
        encoded[encodedPosition] = 0;
        ++encodedPosition;
        remaining = length;
        while (--remaining != 0) {
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
DATA(0x004ede50) int iBaudBits;
DATA(0x004ede54) int inescape;
DATA(0x004ede58) int newpacket;
DATA(0x004ede5c) int packetlen;
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
