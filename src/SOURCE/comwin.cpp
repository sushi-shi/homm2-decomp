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


static ComPortState s_comPorts[PORT_COUNT];


void add_node(struct tag_Anchor* anchor, struct tag_Node* node) {
    node->next = NULL;
    node->prev = NULL;
    if (anchor->tail != NULL) {
        anchor->tail->next = node;
        node->prev = anchor->tail;
        anchor->tail = node;
    } else {
        anchor->tail = node;
        anchor->head = node;
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

void ShutdownComError(const char* function) {
    char errorName[ERROR_NAME_SIZE];
    char message[ERROR_MESSAGE_SIZE];
    DWORD errorCode = GetLastError();

    switch (errorCode) {
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

    sprintf(message, cWinComError[ERROR_TEXT_HEADER], function, errorCode, errorName);
    strcat(message, cWinComError[ERROR_TEXT_SUGGESTIONS]);
    strcat(message, cWinComError[ERROR_TEXT_CHECK_CABLES]);
    strcat(message, cWinComError[ERROR_TEXT_REBOOT]);
    strcat(message, cWinComError[ERROR_TEXT_CHECK_PORT]);
    strcat(message, cWinComError[ERROR_TEXT_LOWER_BAUD_RATE]);
    ShutDown(message);
}

i16 com_init(u8 portNumber, ComBaudRate baudRate, i32 useDtr) {
    i32 err [[maybe_unused]];
    i32 slot;
    BOOL rv;
    DCB state;
    char portName[PORT_NAME_SIZE];
    COMMTIMEOUTS portTimeouts;

    for (slot = 0; slot < PORT_COUNT; ++slot)
        s_comPorts[slot].handle = INVALID_HANDLE_VALUE;
    for (slot = 0; slot < PORT_COUNT; ++slot) {
        if (s_comPorts[slot].handle == INVALID_HANDLE_VALUE)
            break;
    }
    if (slot >= PORT_COUNT)
        return -1;

    wsprintfA(portName, "COM%d", portNumber);
    s_comPorts[slot].handle =
        CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (s_comPorts[slot].handle == INVALID_HANDLE_VALUE) {
        sprintf(gText, "Opening COM%d", portNumber);
        ShutdownComError(gText);
        return -1;
    }

    state.DCBlength = sizeof(DCB);
    GetCommState(s_comPorts[slot].handle, &state);
    s_comPorts[slot].savedState = state;
    GetCommTimeouts(s_comPorts[slot].handle, &s_comPorts[slot].savedTimeouts);

    switch (baudRate) {
        case COM_BAUD_2400:
            state.BaudRate = BAUD_VALUE_2400;
            break;
        case COM_BAUD_4800:
            state.BaudRate = BAUD_VALUE_4800;
            break;
        case COM_BAUD_9600:
            state.BaudRate = BAUD_VALUE_9600;
            break;
        case COM_BAUD_19200:
            state.BaudRate = BAUD_VALUE_19200;
            break;
        case COM_BAUD_38400:
            state.BaudRate = BAUD_VALUE_38400;
            break;
        default:
            state.BaudRate = (baudRate);
            break;
    }

    state.fParity = 0;
    state.fOutxCtsFlow = 1;
    state.fOutxDsrFlow = useDtr != 0 ? 1 : 0;
    state.fDtrControl = DTR_CONTROL_ENABLE;
    state.fInX = 0;
    state.fOutX = 0;
    state.fNull = 0;
    state.fRtsControl = RTS_CONTROL_HANDSHAKE;
    state.fAbortOnError = 1;
    state.ByteSize = COM_SERIAL_BYTE_SIZE;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;

    rv = SetupComm(s_comPorts[slot].handle, RECEIVE_BUFFER_SIZE, TRANSMIT_BUFFER_SIZE);
    if (rv == 0)
        ShutdownComError("Initialize communications paramaters");
    rv = SetCommState(s_comPorts[slot].handle, &state);
    if (rv == 0)
        ShutdownComError("Configure communications device");

    portTimeouts.ReadIntervalTimeout = MAXDWORD;
    portTimeouts.ReadTotalTimeoutMultiplier = portTimeouts.ReadTotalTimeoutConstant = 0;
    portTimeouts.WriteTotalTimeoutMultiplier = portTimeouts.WriteTotalTimeoutConstant = 0;
    rv = SetCommTimeouts(s_comPorts[slot].handle, &portTimeouts);
    if (rv == 0)
        ShutdownComError("Set communications timeouts");

    init_anchor(&s_comPorts[slot].normalQueue, 1, 0);
    init_anchor(&s_comPorts[slot].priorityQueue, 1, 0);
    return static_cast<i16>(slot);
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
    DWORD err;
    i16 bytesRead[READ_RESULT_WORD_COUNT];
    COMSTAT status;
    BOOL success;
    u32 n;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        success = ClearCommError(s_comPorts[portIndex].handle, &err, &status);
        if (success == 0)
            ShutdownComError("Clear communications error queue");
        n = requested < status.cbInQue ? requested : status.cbInQue;
        if (n != 0) {
            success = ReadFile(
                s_comPorts[portIndex].handle,
                buffer,
                n,
                reinterpret_cast<LPDWORD>(bytesRead),
                NULL
            );
            if (success == 0)
                ShutdownComError("Read communications data");
            return bytesRead[0];
        }
    }
    return 0;
}

i16 com_snd(i16 portIndex, u16, u16 length, const void* data, i32 priority) {
    BOOL result2;
    tag_Node* sendNode2;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        if (length == 0) {
            result2 = SetCommBreak(s_comPorts[portIndex].handle);
            if (result2 == 0)
                ShutdownComError("Set communications break");
            Sleep(BREAK_DELAY);
            result2 = ClearCommBreak(s_comPorts[portIndex].handle);
            if (result2 == 0)
                ShutdownComError("Clear communications break");
            return 0;
        }
        sendNode2 = static_cast<tag_Node*>(
            H2_ALLOC(length + NODE_HEADER_SIZE)
        );
        if (sendNode2 != NULL) {
            sendNode2->len = length;
            memcpy(sendNode2->comData, data, length);
            if (priority != 0)
                add_node(&s_comPorts[portIndex].priorityQueue, sendNode2);
            else
                add_node(&s_comPorts[portIndex].normalQueue, sendNode2);
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
    DWORD sizeWritten;
    u32 totalWritten;
    tag_Node* packetNode;
    BOOL callRv;
    ComPortState* comPort;

    comPort = s_comPorts;

    while (comPort->handle != INVALID_HANDLE_VALUE) {
        packetNode = pop_node(&comPort->priorityQueue);
        if (packetNode == NULL)
            packetNode = pop_node(&comPort->normalQueue);
        if (packetNode == NULL)
            return;
        totalWritten = 0;
        while (comPort->handle != INVALID_HANDLE_VALUE && totalWritten < packetNode->len) {
            callRv = WriteFile(
                comPort->handle,
                packetNode->comData + totalWritten,
                packetNode->len - totalWritten,
                &sizeWritten,
                NULL
            );
            if (callRv == 0)
                ShutdownComError("Write communications data");
            totalWritten += sizeWritten;
        }
        H2_FREE(packetNode);
    }
}
