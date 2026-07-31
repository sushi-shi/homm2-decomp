#ifndef HOMM2_PLATFORM_SOCKETS_H
#define HOMM2_PLATFORM_SOCKETS_H

#include <Ints.h>

namespace platform {

using Socket = i32;

inline constexpr Socket kInvalidSocket = -1;

// An IPv4 endpoint. The address is kept the way the wire carries it, which is
// also how the game stores the peer it is talking to.
struct Address {
    u32l host = 0;
    u16 port = 0;
};

bool SocketsStartup();
void SocketsShutdown();

// Datagrams are all the game ever asks for.
Socket OpenDatagramSocket();
void CloseSocket(Socket socket);

bool BindSocket(Socket socket, u16 port);
bool SetSocketNonBlocking(Socket socket, bool nonBlocking);

i32 SendTo(Socket socket, const void* data, i32 size, const Address& destination);
i32 ReceiveFrom(Socket socket, void* data, i32 size, Address* source);

// Zero when the host has no address to offer.
u32l LocalHost();

// Dotted quad, in a buffer that the next call reuses.
const char* HostText(u32l host);
u32l HostFromText(const char* text);

i32 LastSocketError();

}

#endif
