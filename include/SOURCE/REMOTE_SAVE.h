#ifndef HOMM2_REMOTE_SAVE_H
#define HOMM2_REMOTE_SAVE_H

#include <Ints.h>
#include <SOURCE/REMOTE_TYPES.h>

typedef enum RemoteSaveLayoutConstant {
    REMOTE_SAVE_BUFFER_SIZE    = 256,
    REMOTE_SAVE_CHUNK_SIZE     = 200,
    REMOTE_SAVE_CHUNK_CAPACITY = REMOTE_MESSAGE_PAYLOAD_SIZE - sizeof(i16)
} RemoteSaveLayoutConstant;

#pragma pack(push, 1)
struct RemoteSaveChunk {
    i16 packetIndex;
    char data[REMOTE_SAVE_CHUNK_CAPACITY];
};


union RemoteSaveBuffer {
    RemoteSaveInitialization init;
    RemoteSaveChunk chunk;
    char bytes[REMOTE_SAVE_BUFFER_SIZE];
};

struct RemoteSaveMessage {
    i8 sender;
    i32 id;
    i8 type;
    i8 command;
    i16 payloadSize;
    union {
        RemoteSaveChunk chunk;
        char bytes[REMOTE_MESSAGE_PAYLOAD_SIZE];
    } payload;
};
#pragma pack(pop)

#endif
