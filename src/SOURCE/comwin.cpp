#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Ints.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/comwin.h>

typedef enum ComConstant {
    PORT_COUNT           = 2,
    PORT_NAME_SIZE       = 12,
    ERROR_NAME_SIZE      = 100,
    ERROR_MESSAGE_SIZE   = 500,
    RECEIVE_BUFFER_SIZE  = 0x2000,
    TRANSMIT_BUFFER_SIZE = 0x1000,
    BREAK_DELAY          = 500,
    NODE_HEADER_SIZE     = 10
} ComConstant;

typedef enum ComErrorText {
    ERROR_TEXT_HEADER          = 0,
    ERROR_TEXT_SUGGESTIONS     = 1,
    ERROR_TEXT_CHECK_CABLES    = 2,
    ERROR_TEXT_REBOOT          = 3,
    ERROR_TEXT_CHECK_PORT      = 4,
    ERROR_TEXT_LOWER_BAUD_RATE = 5
} ComErrorText;

typedef enum ComSerialConstant {
    BAUD_VALUE_2400        = 2400,
    BAUD_VALUE_4800        = 4800,
    BAUD_VALUE_9600        = 9600,
    BAUD_VALUE_19200       = 19200,
    BAUD_VALUE_38400       = 38400,
    READ_RESULT_WORD_COUNT = 2
} ComSerialConstant;


static i16 s_comTermSourceLineBase = 199;
static i16 s_comSendSourceLineBase = 247;
static i16 s_comWriteSourceLineBase = 310;
static ComPortState s_comPorts[PORT_COUNT];


void add_node(struct tag_Anchor* anchor, struct tag_Node* node) {
    node->next = NULL;
    node->prev = node->next;
    if (anchor->tail != NULL) {
        anchor->tail->next = node;
        node->prev = anchor->tail;
        anchor->tail = node;
    } else {
        anchor->tail = node;
        anchor->head = anchor->tail;
    }
}

struct tag_Node* pop_node(struct tag_Anchor* anchor) {
    tag_Node* node = anchor->head;
    if (node != NULL)
        anchor->head = node->next;
    if (anchor->head == NULL)
        anchor->tail = NULL;
    return node;
}

void init_anchor(struct tag_Anchor* anchor, i32, i32) {
    anchor->head = NULL;
    anchor->tail = NULL;
}

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

    sprintf(message, cWinComError[ERROR_TEXT_HEADER], function, error, errorName);
    strcat(message, cWinComError[ERROR_TEXT_SUGGESTIONS]);
    strcat(message, cWinComError[ERROR_TEXT_CHECK_CABLES]);
    strcat(message, cWinComError[ERROR_TEXT_REBOOT]);
    strcat(message, cWinComError[ERROR_TEXT_CHECK_PORT]);
    strcat(message, cWinComError[ERROR_TEXT_LOWER_BAUD_RATE]);
    ShutDown(message);
}

i16 com_init(u8 portNumber, ComBaudRate baudRate, i32 useDtr) {
    char portName[PORT_NAME_SIZE];
    BOOL success;
    DCB portState;
    i32 portIndex;
    COMMTIMEOUTS commTimeouts;

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
        CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (s_comPorts[portIndex].handle == INVALID_HANDLE_VALUE) {
        sprintf(gText, "Opening COM%d", portNumber);
        ShutdownComError(gText);
        return -1;
    }

    portState.DCBlength = sizeof(DCB);
    GetCommState(s_comPorts[portIndex].handle, &portState);
    s_comPorts[portIndex].savedState = portState;
    GetCommTimeouts(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedTimeouts);

    switch (baudRate) {
        case COM_BAUD_2400:
            portState.BaudRate = BAUD_VALUE_2400;
            break;
        case COM_BAUD_4800:
            portState.BaudRate = BAUD_VALUE_4800;
            break;
        case COM_BAUD_9600:
            portState.BaudRate = BAUD_VALUE_9600;
            break;
        case COM_BAUD_19200:
            portState.BaudRate = BAUD_VALUE_19200;
            break;
        case COM_BAUD_38400:
            portState.BaudRate = BAUD_VALUE_38400;
            break;
        default:
            portState.BaudRate = (baudRate);
            break;
    }

    portState.fParity = 0;
    portState.fOutxCtsFlow = 1;
    if (useDtr != 0)
        portState.fOutxDsrFlow = 1;
    else
        portState.fOutxDsrFlow = 0;
    portState.fDtrControl = DTR_CONTROL_ENABLE;
    portState.fInX = 0;
    portState.fOutX = 0;
    portState.fNull = 0;
    portState.fRtsControl = RTS_CONTROL_HANDSHAKE;
    portState.fAbortOnError = 1;
    portState.ByteSize = COM_SERIAL_BYTE_SIZE;
    portState.Parity = NOPARITY;
    portState.StopBits = ONESTOPBIT;

    success =
        SetupComm(s_comPorts[portIndex].handle, RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
    if (success == 0)
        ShutdownComError("Initialize communications paramaters");
    success = SetCommState(s_comPorts[portIndex].handle, &portState);
    if (success == 0)
        ShutdownComError("Configure communications device");

    commTimeouts.ReadIntervalTimeout = MAXDWORD;
    commTimeouts.ReadTotalTimeoutConstant = 0;
    commTimeouts.ReadTotalTimeoutMultiplier = 0;
    commTimeouts.WriteTotalTimeoutConstant = 0;
    commTimeouts.WriteTotalTimeoutMultiplier = 0;
    success = SetCommTimeouts(s_comPorts[portIndex].handle, &commTimeouts);
    if (success == 0)
        ShutdownComError("Set communications timeouts");

    init_anchor(&s_comPorts[portIndex].normalQueue, 1, 0);
    init_anchor(&s_comPorts[portIndex].priorityQueue, 1, 0);
    return static_cast<i16>(portIndex);
}

void com_term(i16 portIndex) {
    tag_Node* node;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        SetCommState(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedState);
        SetCommTimeouts(s_comPorts[portIndex].handle, &s_comPorts[portIndex].savedTimeouts);
        CloseHandle(s_comPorts[portIndex].handle);
        s_comPorts[portIndex].handle = INVALID_HANDLE_VALUE;

        while ((node = pop_node(&s_comPorts[portIndex].normalQueue)) != NULL)
            H2_FREE(node);
        while ((node = pop_node(&s_comPorts[portIndex].priorityQueue)) != NULL)
            H2_FREE(node);
    }
}

i16 com_rcv(i16 portIndex, u16 requested, void* buffer) {
    COMSTAT status;
    DWORD commErrors;
    u32 count;
    i16 bytesRead[READ_RESULT_WORD_COUNT];
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
                NULL
            );
            if (result == 0)
                ShutdownComError("Read communications data");
            return bytesRead[0];
        }
    }
    return 0;
}

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
        sendNode = static_cast<tag_Node*>(
            H2_ALLOC(length + NODE_HEADER_SIZE)
        );
        if (sendNode != NULL) {
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

i16 __cdecl com_sess(i32, i32, ...) {
    return 0;
}

u8 com_stat(i16 portIndex, u16) {
    DWORD modemStatus;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE
        && GetCommModemStatus(s_comPorts[portIndex].handle, &modemStatus) != 0
        && (modemStatus & MS_CTS_ON) != 0 && (modemStatus & MS_RLSD_ON) != 0)
        return 1;
    return 0;
}

void comm_wrt_task(void) {
    DWORD bytesWritten;
    u32 writtenTotal;
    tag_Node* packetNode;
    BOOL callResult;
    ComPortState* comPort;

    comPort = s_comPorts;

    while (comPort->handle != INVALID_HANDLE_VALUE) {
        packetNode = pop_node(&comPort->priorityQueue);
        if (packetNode == NULL)
            packetNode = pop_node(&comPort->normalQueue);
        if (packetNode == NULL)
            return;
        writtenTotal = 0;
        while (comPort->handle != INVALID_HANDLE_VALUE && writtenTotal < packetNode->len) {
            callResult = WriteFile(
                comPort->handle,
                packetNode->comData + writtenTotal,
                packetNode->len - writtenTotal,
                &bytesWritten,
                NULL
            );
            if (callResult == 0)
                ShutdownComError("Write communications data");
            writtenTotal += bytesWritten;
        }
        H2_FREE(packetNode);
    }
}
