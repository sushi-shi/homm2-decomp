// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\comwin.obj   from: (directly linked into exe)
// functions: 11   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <va.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/comwin.h>

#define COMWIN_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\comwin.cpp"

DATA(0x004f843c) static short s_comTermSourceLineBase = 199;
DATA(0x004f84d8) static short s_comSendSourceLineBase = 247;
DATA(0x004f8540) static short s_comWriteSourceLineBase = 310;
DATA(0x005284b8) static ComPortState s_comPorts[COM_PORT_COUNT];

VA(0x0048a640, 0x74)
void add_node(struct tag_Anchor *anchor, struct tag_Node *node)
{
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
struct tag_Node * pop_node(struct tag_Anchor *anchor)
{
    tag_Node *node = anchor->head;
    if (node != 0)
        anchor->head = node->next;
    if (anchor->head == 0)
        anchor->tail = 0;
    return node;
}

VA(0x0048a700, 0x2e)
void init_anchor(struct tag_Anchor *anchor, int, int)
{
    anchor->head = 0;
    anchor->tail = 0;
}

// @match-note 98.46%: complete 0x260 frame, Win32 error switch, message
// assembly, and shutdown CFG; all 72/72 relocation occurrences and targets
// agree. The first reported residual is the delinked switch-table/local-label
// identity immediately after GetLastError; the diff then decodes table bytes as
// instructions. Literal and named Win32 error case values compile identically.
// Revisit at 95% with the switch data range excluded; do not retry that spelling.
VA(0x0048a72e, 0x3e5)
void ShutdownComError(char *function)
{
    char errorName[COM_ERROR_NAME_SIZE];
    char message[COM_ERROR_MESSAGE_SIZE];
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

// @match-note 97.46%: complete 0x54 frame, two-pass port selection, 0x60 saved
// state layout, DCB flags, timeout setup, and both queue initializers; all 33/33
// relocation occurrences and targets agree. The first non-symbol residual is at
// the baud-rate switch/jump-table boundary after GetCommTimeouts. An initial
// BaudRate assignment scored 95.64%; the explicit default arm is retained.
// Revisit at 95% after excluding switch data; do not retry the initial assignment.
VA(0x0048ab13, 0x34a)
short int com_init(unsigned char portNumber, int baudRate, int useDtr)
{
    char portName[COM_PORT_NAME_SIZE];
    BOOL result;
    DCB state;
    int portIndex;
    COMMTIMEOUTS timeouts;

    for (portIndex = 0; portIndex < COM_PORT_COUNT; ++portIndex)
        s_comPorts[portIndex].handle = INVALID_HANDLE_VALUE;
    for (portIndex = 0; portIndex < COM_PORT_COUNT; ++portIndex) {
        if (s_comPorts[portIndex].handle == INVALID_HANDLE_VALUE)
            break;
    }
    if (portIndex >= COM_PORT_COUNT)
        return -1;

    wsprintfA(portName, "COM%d", portNumber);
    s_comPorts[portIndex].handle =
        CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0,
                    OPEN_EXISTING, 0, 0);
    if (s_comPorts[portIndex].handle == INVALID_HANDLE_VALUE) {
        sprintf(gText, "Opening COM%d", portNumber);
        ShutdownComError(gText);
        return -1;
    }

    state.DCBlength = sizeof(DCB);
    GetCommState(s_comPorts[portIndex].handle, &state);
    s_comPorts[portIndex].savedState = state;
    GetCommTimeouts(s_comPorts[portIndex].handle,
                    &s_comPorts[portIndex].savedTimeouts);

    switch (baudRate) {
    case COM_BAUD_2400:
        state.BaudRate = 2400;
        break;
    case COM_BAUD_4800:
        state.BaudRate = 4800;
        break;
    case COM_BAUD_9600:
        state.BaudRate = 9600;
        break;
    case COM_BAUD_19200:
        state.BaudRate = 19200;
        break;
    case COM_BAUD_38400:
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

    result = SetupComm(s_comPorts[portIndex].handle,
                       COM_RECEIVE_BUFFER_SIZE, COM_TRANSMIT_BUFFER_SIZE);
    if (result == 0)
        ShutdownComError("Initialize communications parameters");
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
    return static_cast<short>(portIndex);
}

VA(0x0048ae5d, 0x11d)
void com_term(short int portIndex)
{
    tag_Node *node;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        SetCommState(s_comPorts[portIndex].handle,
                     &s_comPorts[portIndex].savedState);
        SetCommTimeouts(s_comPorts[portIndex].handle,
                        &s_comPorts[portIndex].savedTimeouts);
        CloseHandle(s_comPorts[portIndex].handle);
        s_comPorts[portIndex].handle = INVALID_HANDLE_VALUE;

        while ((node = pop_node(&s_comPorts[portIndex].normalQueue)) != 0)
            BaseFree(node, COMWIN_SOURCE_FILE, s_comTermSourceLineBase + 13);
        while ((node = pop_node(&s_comPorts[portIndex].priorityQueue)) != 0)
            BaseFree(node, COMWIN_SOURCE_FILE, s_comTermSourceLineBase + 17);
    }
}

// @early-stop
// The complete 0x24 frame/CFG and relocation-masked instruction stream agree;
// all 7/7 relocation occurrences resolve to the same targets. The reported
// 99.88% residual is delinked shared-record, import, and string symbol identity.
VA(0x0048af7a, 0xdd)
short int com_rcv(short int portIndex, unsigned short int requested,
                  void *buffer)
{
    DWORD errors;
    COMSTAT status;
    unsigned int count;
    short bytesRead[2];
    BOOL result;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        result = ClearCommError(s_comPorts[portIndex].handle, &errors, &status);
        if (result == 0)
            ShutdownComError("Clear communications error queue");
        if (status.cbInQue <= requested)
            count = status.cbInQue;
        else
            count = requested;
        if (count != 0) {
            result = ReadFile(s_comPorts[portIndex].handle, buffer, count,
                              reinterpret_cast<LPDWORD>(bytesRead), 0);
            if (result == 0)
                ShutdownComError("Read communications data");
            return bytesRead[0];
        }
    }
    return 0;
}

// @early-stop
// The complete 0x08 frame, break/allocate/queue CFG, and relocation-masked
// instruction stream agree; all 15/15 relocation occurrences resolve to the
// same targets. The 99.88% residual is delinked data/string/import identity.
VA(0x0048b057, 0x145)
short int com_snd(short int portIndex, unsigned short int,
                  unsigned short int length, void *data, int priority)
{
    BOOL result;
    tag_Node *node;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        if (length == 0) {
            result = SetCommBreak(s_comPorts[portIndex].handle);
            if (result == 0)
                ShutdownComError("Set communications break");
            Sleep(COM_BREAK_DELAY);
            result = ClearCommBreak(s_comPorts[portIndex].handle);
            if (result == 0)
                ShutdownComError("Clear communications break");
            return 0;
        }
        node = static_cast<tag_Node *>(
            BaseAlloc(length + COM_NODE_HEADER_SIZE, COMWIN_SOURCE_FILE,
                      s_comSendSourceLineBase + 16));
        if (node != 0) {
            node->len = length;
            memcpy(node->comData, data, length);
            if (priority != 0)
                add_node(&s_comPorts[portIndex].priorityQueue, node);
            else
                add_node(&s_comPorts[portIndex].normalQueue, node);
            return 0;
        }
    }
    return 1;
}

VA(0x0048b19c, 0x13)
short int __cdecl com_sess(int, int, ...) { return 0; }

VA(0x0048b1af, 0x6e)
unsigned char com_stat(short int portIndex, unsigned short int)
{
    DWORD modemStatus;
    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE &&
        GetCommModemStatus(s_comPorts[portIndex].handle, &modemStatus) != 0 &&
        (modemStatus & MS_CTS_ON) != 0 && (modemStatus & MS_RLSD_ON) != 0)
        return 1;
    return 0;
}

// @early-stop
// The complete 0x18 frame, priority/normal queue and partial-write CFG, and
// relocation-masked instruction stream agree; all 8/8 relocation occurrences
// resolve to the same targets. The 99.62% residual is local data/string/import
// symbol identity only.
VA(0x0048b21d, 0xe8)
void comm_wrt_task(void)
{
    DWORD bytesWritten;
    unsigned int totalWritten;
    tag_Node *node;
    BOOL result;
    ComPortState *port = s_comPorts;

    while (port->handle != INVALID_HANDLE_VALUE) {
        node = pop_node(&port->priorityQueue);
        if (node == 0)
            node = pop_node(&port->normalQueue);
        if (node == 0)
            break;
        totalWritten = 0;
        while (port->handle != INVALID_HANDLE_VALUE &&
               totalWritten < node->len) {
            result = WriteFile(port->handle, node->comData + totalWritten,
                               node->len - totalWritten, &bytesWritten, 0);
            if (result == 0)
                ShutdownComError("Write communications data");
            totalWritten += bytesWritten;
        }
        BaseFree(node, COMWIN_SOURCE_FILE, s_comWriteSourceLineBase + 28);
    }
}
