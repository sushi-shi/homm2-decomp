// Compile-only evidence; never linked into the game.
#include <SOURCE/netwin.h>
#include <stddef.h>
typedef char PayloadSize[sizeof(NetbiosPayload)==4096 ? 1 : -1];
typedef char PayloadByteOwner[offsetof(NetbiosPayload,bytes)==0 ? 1 : -1];
typedef char PayloadArray[sizeof(NetbiosPayload[7])==28672 ? 1 : -1];
typedef char EventsSize[sizeof(NetbiosThreadEvents)==36 ? 1 : -1];
typedef char NineHandles[sizeof(static_cast<NetbiosThreadEvents*>(0)->handles)/sizeof(void*)==9 ? 1 : -1];
typedef char ControlBlockSize[sizeof(NetbiosControlBlock)==64 ? 1 : -1];
