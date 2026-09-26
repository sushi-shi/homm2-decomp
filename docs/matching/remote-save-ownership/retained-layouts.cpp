// Final owner definitions; compile-only layout/type assertions live in remote-save-layout-probe.cpp.
// Initial macro-collision and data[200] diagnostic history: docs/reconstruction/U09-U14.md.
// The retained data[245] correction is the real payload capacity, not extra allocation or padding.
/*
#ifndef HOMM2_REMOTE_SAVE_H
#define HOMM2_REMOTE_SAVE_H

#include <va.h>
#include <SOURCE/REMOTE_TYPES.h>

H2_ENUM_BEGIN(RemoteSaveLayoutConstant)
    REMOTE_SAVE_BUFFER_SIZE    = 256,
    REMOTE_SAVE_CHUNK_SIZE     = 200,
    REMOTE_SAVE_CHUNK_CAPACITY = REMOTE_MESSAGE_PAYLOAD_SIZE - sizeof(i16)
H2_ENUM_END(RemoteSaveLayoutConstant)

#pragma pack(push, 1)
struct RemoteSaveChunk {
    i16 packetIndex;
    char data[REMOTE_SAVE_CHUNK_CAPACITY];
};

// Reused by the init, data and acknowledgement-request commands.
union RemoteSaveBuffer {
    RemoteSaveInitialization init;
    RemoteSaveChunk chunk;
    char bytes[REMOTE_SAVE_BUFFER_SIZE];
};

struct RemoteSaveMessage {
    i8 sender;
    i32 id;
    H2_ENUM_STORAGE(RemoteMessageType, i8) type;
    i8 command;
    i16 payloadSize;
    union {
        RemoteSaveChunk chunk;
        char bytes[REMOTE_MESSAGE_PAYLOAD_SIZE];
    } payload;
};
#pragma pack(pop)
SIZE(RemoteSaveChunk, REMOTE_MESSAGE_PAYLOAD_SIZE);
SIZE(RemoteSaveBuffer, REMOTE_SAVE_BUFFER_SIZE);
SIZE(RemoteSaveMessage, REMOTE_MESSAGE_SIZE);

#endif

*/
