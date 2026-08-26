#include <PLATFORM/Sockets.h>

#include <cstdio>
#include <cstring>

#ifdef _WIN32

#include <winsock2.h>

#else

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

namespace platform {
namespace {

#ifdef _WIN32

using Handle = SOCKET;

int CloseHandle(Handle handle) { return ::closesocket(handle); }

#else

using Handle = int;

int CloseHandle(Handle handle) { return ::close(handle); }

#endif

Handle Native(Socket socket) { return static_cast<Handle>(socket); }

char gHostText[32];

}

bool SocketsStartup() {
#ifdef _WIN32
    WSADATA data;
    return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
#else
    return true;
#endif
}

void SocketsShutdown() {
#ifdef _WIN32
    ::WSACleanup();
#endif
}

Socket OpenDatagramSocket() {
    const Handle handle = ::socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    if (handle == INVALID_SOCKET) {
        return kInvalidSocket;
    }
#else
    if (handle < 0) {
        return kInvalidSocket;
    }
#endif
    return static_cast<Socket>(handle);
}

void CloseSocket(Socket socket) {
    if (socket != kInvalidSocket) {
        CloseHandle(Native(socket));
    }
}

bool BindSocket(Socket socket, u16 port) {
    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(port);
    address.sin_addr.s_addr = ::htonl(INADDR_ANY);
    return ::bind(Native(socket), reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0;
}

bool SetSocketNonBlocking(Socket socket, bool nonBlocking) {
#ifdef _WIN32
    u_long mode = nonBlocking ? 1 : 0;
    return ::ioctlsocket(Native(socket), static_cast<long>(FIONBIO), &mode) == 0;
#else
    int flags = ::fcntl(Native(socket), F_GETFL, 0);
    if (flags < 0) {
        return false;
    }
    flags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return ::fcntl(Native(socket), F_SETFL, flags) == 0;
#endif
}

i32 SendTo(Socket socket, const void* data, i32 size, const Address& destination) {
    if (size < 0 || (size > 0 && data == nullptr)) {
        return -1;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = ::htons(destination.port);
    address.sin_addr.s_addr = static_cast<u32l>(destination.host);
    return static_cast<i32>(::sendto(
        Native(socket),
        static_cast<const char*>(data),
#ifdef _WIN32
        static_cast<int>(size),
#else
        static_cast<std::size_t>(size),
#endif
        0,
        reinterpret_cast<sockaddr*>(&address),
        sizeof(address)
    ));
}

i32 ReceiveFrom(Socket socket, void* data, i32 size, Address* source) {
    if (size < 0 || (size > 0 && data == nullptr)) {
        return -1;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
#ifdef _WIN32
    int length = sizeof(address);
#else
    socklen_t length = sizeof(address);
#endif
    const i32 received = static_cast<i32>(::recvfrom(
        Native(socket),
        static_cast<char*>(data),
#ifdef _WIN32
        static_cast<int>(size),
#else
        static_cast<std::size_t>(size),
#endif
        0,
        reinterpret_cast<sockaddr*>(&address),
        &length
    ));
    if (received >= 0 && source != nullptr) {
        source->host = static_cast<u32l>(address.sin_addr.s_addr);
        source->port = ::ntohs(address.sin_port);
    }
    return received;
}

u32l LocalHost() {
    char name[256];
    if (::gethostname(name, sizeof(name) - 1) != 0) {
        return 0;
    }
    name[sizeof(name) - 1] = '\0';

    const hostent* host = ::gethostbyname(name);
    if (host == nullptr || host->h_addr_list == nullptr || host->h_addr_list[0] == nullptr) {
        return 0;
    }

    u32l address = 0;
    std::memcpy(&address, host->h_addr_list[0], sizeof(address));
    return address;
}

const char* HostText(u32l host) {
    const u8* octet = reinterpret_cast<const u8*>(&host);
    std::snprintf(gHostText, sizeof(gHostText), "%u.%u.%u.%u", octet[0], octet[1], octet[2],
                  octet[3]);
    return gHostText;
}

u32l HostFromText(const char* text) {
    if (text == nullptr) {
        return 0;
    }
    return static_cast<u32l>(::inet_addr(text));
}

i32 LastSocketError() {
#ifdef _WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

}
