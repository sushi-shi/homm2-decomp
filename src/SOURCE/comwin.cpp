#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <va.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/comwin.h>

H2_ENUM_BEGIN(ComConstant)
    PORT_COUNT           = 2,
    PORT_NAME_SIZE       = 12,
    ERROR_NAME_SIZE      = 100,
    ERROR_MESSAGE_SIZE   = 500,
    RECEIVE_BUFFER_SIZE  = 0x2000,
    TRANSMIT_BUFFER_SIZE = 0x1000,
    BREAK_DELAY          = 500,
    NODE_HEADER_SIZE     = 10
H2_ENUM_END(ComConstant)

H2_ENUM_CLASS_BEGIN(ComBaudRate)
    COM_BAUD_2400  = 1,
    COM_BAUD_4800  = 2,
    COM_BAUD_9600  = 3,
    COM_BAUD_19200 = 4,
    COM_BAUD_38400 = 5
H2_ENUM_CLASS_END(ComBaudRate)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\comwin.cpp"

DATA(0x004f843c) static i16 s_comTermSourceLineBase = 199;
DATA(0x004f84d8) static i16 s_comSendSourceLineBase = 247;
DATA(0x004f8540) static i16 s_comWriteSourceLineBase = 310;
DATA(0x005284b8) static ComPortState s_comPorts[PORT_COUNT];


VA(0x0048a640, 0x74)
void add_node(struct tag_Anchor* anchor, struct tag_Node* node) {
    node->next = 0;
    node->prev = node->next;
    if (anchor->tail != 0) {
        anchor->tail->next = node;
        node->prev = anchor->tail;
        anchor->tail = node;
    } else {
        anchor->tail = node;
        anchor->head = anchor->tail;
    }
}

VA(0x0048a6b4, 0x4c)
struct tag_Node* pop_node(struct tag_Anchor* anchor) {
    tag_Node* node = anchor->head;
    if (node != 0)
        anchor->head = node->next;
    if (anchor->head == 0)
        anchor->tail = 0;
    return node;
}

VA(0x0048a700, 0x2e)
void init_anchor(struct tag_Anchor* anchor, i32, i32) {
    anchor->head = 0;
    anchor->tail = 0;
}

// @semantic: first residual is the embedded 0x4c-byte jump table at RVA 0x8a968.
VA(0x0048a72e, 0x3e5)
void ShutdownComError(char* function) {
    char errorName[ERROR_NAME_SIZE];
    char message[ERROR_MESSAGE_SIZE];
    DWORD error = GetLastError();

    switch (error) {
        case ERROR_INVALID_FUNCTION:
            strcpy(errorName, "ERROR_INVALID_FUNCTION    ");
            break;
        case ERROR_FILE_NOT_FOUND:
            strcpy(errorName, "ERROR_FILE_NOT_FOUND      ");
            break;
        case ERROR_PATH_NOT_FOUND:
            strcpy(errorName, "ERROR_PATH_NOT_FOUND      ");
            break;
        case ERROR_TOO_MANY_OPEN_FILES:
            strcpy(errorName, "ERROR_TOO_MANY_OPEN_FILES ");
            break;
        case ERROR_ACCESS_DENIED:
            strcpy(errorName, "ERROR_ACCESS_DENIED       ");
            break;
        case ERROR_INVALID_HANDLE:
            strcpy(errorName, "ERROR_INVALID_HANDLE      ");
            break;
        case ERROR_ARENA_TRASHED:
            strcpy(errorName, "ERROR_ARENA_TRASHED       ");
            break;
        case ERROR_NOT_ENOUGH_MEMORY:
            strcpy(errorName, "ERROR_NOT_ENOUGH_MEMORY   ");
            break;
        case ERROR_INVALID_BLOCK:
            strcpy(errorName, "ERROR_INVALID_BLOCK       ");
            break;
        case ERROR_BAD_ENVIRONMENT:
            strcpy(errorName, "ERROR_BAD_ENVIRONMENT     ");
            break;
        case ERROR_BAD_FORMAT:
            strcpy(errorName, "ERROR_BAD_FORMAT          ");
            break;
        case ERROR_INVALID_ACCESS:
            strcpy(errorName, "ERROR_INVALID_ACCESS      ");
            break;
        case ERROR_INVALID_DATA:
            strcpy(errorName, "ERROR_INVALID_DATA        ");
            break;
        case ERROR_INVALID_DRIVE:
            strcpy(errorName, "ERROR_INVALID_DRIVE       ");
            break;
        case ERROR_CURRENT_DIRECTORY:
            strcpy(errorName, "ERROR_CURRENT_DIRECTORY   ");
            break;
        case ERROR_NOT_SAME_DEVICE:
            strcpy(errorName, "ERROR_NOT_SAME_DEVICE     ");
            break;
        case ERROR_NO_MORE_FILES:
            strcpy(errorName, "ERROR_NO_MORE_FILES       ");
            break;
        case ERROR_ALREADY_EXISTS:
            strcpy(errorName, "ERROR_ALREADY_EXISTS      ");
            break;
        default:
            strcpy(errorName, "UNKNOWN_ERROR             ");
            break;
    }

    sprintf(message, cWinComError[0], function, error, errorName);
    strcat(message, cWinComError[1]);
    strcat(message, cWinComError[2]);
    strcat(message, cWinComError[3]);
    strcat(message, cWinComError[4]);
    strcat(message, cWinComError[5]);
    ShutDown(message);
}

// @semantic: first residual is the embedded 0x14-byte baud-rate jump table at RVA 0x8ace2.
VA(0x0048ab13, 0x34a)
i16 com_init(u8 portNumber, i32 baudRate, i32 useDtr) {
    char portName[PORT_NAME_SIZE];
    BOOL result;
    DCB state;
    i32 portIndex;
    COMMTIMEOUTS timeouts;

    for (portIndex = 0; portIndex < PORT_COUNT; ++portIndex)
        s_comPorts[portIndex].handle = INVALID_HANDLE_VALUE;
    for (portIndex = 0; portIndex < PORT_COUNT; ++portIndex) {
        if (s_comPorts[portIndex].handle == INVALID_HANDLE_VALUE)
            break;
    }
    if (portIndex >= PORT_COUNT)
        return -1;

    wsprintfA(portName, "COM%d", portNumber);
    s_comPorts[portIndex].handle =
        CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (s_comPorts[portIndex].handle == INVALID_HANDLE_VALUE) {
        sprintf(gText, "Opening COM%d", portNumber);
        ShutdownComError(gText);
        return -1;
    }

    state.DCBlength = sizeof(DCB);
    GetCommState(s_comPorts[portIndex].handle, &state);
    s_comPorts[portIndex].savedState = state;
    GetCommTimeouts(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedTimeouts);

    switch (baudRate) {
        case IDX(COM_BAUD_2400):
            state.BaudRate = 2400;
            break;
        case IDX(COM_BAUD_4800):
            state.BaudRate = 4800;
            break;
        case IDX(COM_BAUD_9600):
            state.BaudRate = 9600;
            break;
        case IDX(COM_BAUD_19200):
            state.BaudRate = 19200;
            break;
        case IDX(COM_BAUD_38400):
            state.BaudRate = 38400;
            break;
        default:
            state.BaudRate = baudRate;
            break;
    }

    state.fParity = 0;
    state.fOutxCtsFlow = 1;
    if (useDtr != 0)
        state.fOutxDsrFlow = 1;
    else
        state.fOutxDsrFlow = 0;
    state.fDtrControl = DTR_CONTROL_ENABLE;
    state.fInX = 0;
    state.fOutX = 0;
    state.fNull = 0;
    state.fRtsControl = RTS_CONTROL_HANDSHAKE;
    state.fAbortOnError = 1;
    state.ByteSize = 8;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;

    result =
        SetupComm(s_comPorts[portIndex].handle, RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
    if (result == 0)
        ShutdownComError("Initialize communications paramaters");
    result = SetCommState(s_comPorts[portIndex].handle, &state);
    if (result == 0)
        ShutdownComError("Configure communications device");

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    result = SetCommTimeouts(s_comPorts[portIndex].handle, &timeouts);
    if (result == 0)
        ShutdownComError("Set communications timeouts");

    init_anchor(&s_comPorts[portIndex].normalQueue, 1, 0);
    init_anchor(&s_comPorts[portIndex].priorityQueue, 1, 0);
    return static_cast<i16>(portIndex);
}

VA(0x0048ae5d, 0x11d)
void com_term(i16 portIndex) {
    tag_Node* node;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        SetCommState(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedState);
        SetCommTimeouts(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedTimeouts);
        CloseHandle(s_comPorts[portIndex].handle);
        s_comPorts[portIndex].handle = INVALID_HANDLE_VALUE;

        while ((node = pop_node(&s_comPorts[portIndex].normalQueue)) != 0)
            H2_FREE(node, 212);
        while ((node = pop_node(&s_comPorts[portIndex].priorityQueue)) != 0)
            H2_FREE(node, 216);
    }
}

VA(0x0048af7a, 0xdd)
i16 com_rcv(i16 portIndex, u16 requested, void* buffer) {
    COMSTAT status;
    DWORD commErrors;
    u32 count;
    i16 bytesRead[2];
    BOOL result;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        result = ClearCommError(s_comPorts[portIndex].handle, &commErrors, &status);
        if (result == 0)
            ShutdownComError("Clear communications error queue");
        if (status.cbInQue <= requested)
            count = status.cbInQue;
        else
            count = requested;
        if (count != 0) {
            result = ReadFile(
                s_comPorts[portIndex].handle,
                buffer,
                count,
                reinterpret_cast<LPDWORD>(bytesRead),
                0
            );
            if (result == 0)
                ShutdownComError("Read communications data");
            return bytesRead[0];
        }
    }
    return 0;
}

VA(0x0048b057, 0x145)
i16 com_snd(i16 portIndex, u16, u16 length, void* data, i32 priority) {
    BOOL result;
    tag_Node* sendNode;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        if (length == 0) {
            result = SetCommBreak(s_comPorts[portIndex].handle);
            if (result == 0)
                ShutdownComError("Set communications break");
            Sleep(BREAK_DELAY);
            result = ClearCommBreak(s_comPorts[portIndex].handle);
            if (result == 0)
                ShutdownComError("Clear communications break");
            return 0;
        }
        sendNode = static_cast<tag_Node*>(H2_ALLOC(length + NODE_HEADER_SIZE, 263));
        if (sendNode != 0) {
            sendNode->len = length;
            memcpy(sendNode->comData, data, length);
            if (priority != 0)
                add_node(&s_comPorts[portIndex].priorityQueue, sendNode);
            else
                add_node(&s_comPorts[portIndex].normalQueue, sendNode);
            return 0;
        }
    }
    return 1;
}

VA(0x0048b19c, 0x13)
i16 __cdecl com_sess(i32, i32, ...) {
    return 0;
}

VA(0x0048b1af, 0x6e)
u8 com_stat(i16 portIndex, u16) {
    DWORD modemStatus;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE
        && GetCommModemStatus(s_comPorts[portIndex].handle, &modemStatus) != 0
        && (modemStatus & MS_CTS_ON) != 0 && (modemStatus & MS_RLSD_ON) != 0)
        return 1;
    return 0;
}

// @semantic: sole raw residual is the exit jump displacement at +0x4d.
VA(0x0048b21d, 0xe8)
void comm_wrt_task(void) {
    DWORD bytesWritten;
    u32 writtenTotal;
    tag_Node* packetNode;
    BOOL callResult;
    ComPortState* comPort;

    comPort = s_comPorts;

    while (comPort->handle != INVALID_HANDLE_VALUE) {
        packetNode = pop_node(&comPort->priorityQueue);
        if (packetNode == 0)
            packetNode = pop_node(&comPort->normalQueue);
        if (packetNode == 0)
            break;
        writtenTotal = 0;
        while (comPort->handle != INVALID_HANDLE_VALUE && writtenTotal < packetNode->len) {
            callResult = WriteFile(
                comPort->handle,
                packetNode->comData + writtenTotal,
                packetNode->len - writtenTotal,
                &bytesWritten,
                0
            );
            if (callResult == 0)
                ShutdownComError("Write communications data");
            writtenTotal += bytesWritten;
        }
        H2_FREE(packetNode, 338);
    }
}

#undef RETAIL_FILE
