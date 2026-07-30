#include <winsock.h>

#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int WSAStartup(WORD, LPWSADATA data) {
    if (data != nullptr) {
        std::memset(data, 0, sizeof(*data));
        data->wVersion = 0x0101;
        data->wHighVersion = 0x0101;
        std::strcpy(data->szDescription, "platform sockets");
        data->iMaxSockets = 32767;
        data->iMaxUdpDg = 65467;
    }
    return 0;
}

int WSACleanup() { return 0; }

int WSAGetLastError() {

    switch (errno) {
    case EWOULDBLOCK:
        return WSAEWOULDBLOCK;
    case EINPROGRESS:
        return WSAEINPROGRESS;
    case ECONNRESET:
        return WSAECONNRESET;
    default:
        return errno;
    }
}

int closesocket(SOCKET socket) { return ::close(socket); }

int ioctlsocket(SOCKET socket, long command, unsigned long* argument) {
    if (command == static_cast<long>(FIONBIO)) {

        int flags = ::fcntl(socket, F_GETFL, 0);
        if (flags < 0) {
            return -1;
        }
        const bool nonBlocking = argument != nullptr && *argument != 0;
        flags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        return ::fcntl(socket, F_SETFL, flags);
    }
    return ::ioctl(socket, static_cast<unsigned long>(command), argument);
}
