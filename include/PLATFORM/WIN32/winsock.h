#ifndef HOMM2_PLATFORM_WIN32_WINSOCK_H
#define HOMM2_PLATFORM_WIN32_WINSOCK_H

#ifdef _WIN32

#include_next <winsock.h>

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <windows.h>

typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr* PSOCKADDR;
typedef struct sockaddr* LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in* PSOCKADDR_IN;
typedef struct sockaddr_in* LPSOCKADDR_IN;
typedef struct hostent HOSTENT;
typedef struct hostent* PHOSTENT;
typedef struct hostent* LPHOSTENT;

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128

struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
};
typedef WSAData* LPWSADATA;

#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

int WSAStartup(WORD versionRequested, LPWSADATA data);
int WSACleanup();
int WSAGetLastError();
int closesocket(SOCKET socket);
int ioctlsocket(SOCKET socket, long command, unsigned long* argument);

#define WSAEWOULDBLOCK 10035
#define WSAEINPROGRESS 10036
#define WSAECONNRESET 10054
#define FIONBIO 0x8004667E

#endif

#endif
