// Compile-only wire-layout evidence; never linked into the game.
#include <SOURCE/REMOTE.h>
#include <SOURCE/REMOTE_SAVE.h>
#include <SOURCE/ADVMGR_TYPES.h>
#include <SOURCE/CURSOR.h>
#include <stddef.h>

typedef char InitSize[sizeof(RemoteSaveInitialization)==16 ? 1 : -1];
typedef char InitDataSize[offsetof(RemoteSaveInitialization,dataSize)==0 ? 1 : -1];
typedef char InitCrc[offsetof(RemoteSaveInitialization,crc)==4 ? 1 : -1];
typedef char InitWireCrc[offsetof(RemoteSaveInitialization,wireCrc)==8 ? 1 : -1];
typedef char InitPlayerExited[offsetof(RemoteSaveInitialization,playerExited)==12 ? 1 : -1];
typedef char ChunkSize[sizeof(RemoteSaveChunk)==247 ? 1 : -1];
typedef char ChunkIndex[offsetof(RemoteSaveChunk,packetIndex)==0 ? 1 : -1];
typedef char ChunkData[offsetof(RemoteSaveChunk,data)==2 ? 1 : -1];
typedef char ChunkDataSize[sizeof(static_cast<RemoteSaveChunk*>(0)->data)==245 ? 1 : -1];
typedef char SentChunkSize[REMOTE_SAVE_CHUNK_SIZE==200 ? 1 : -1];
typedef char BufferSize[sizeof(RemoteSaveBuffer)==256 ? 1 : -1];
typedef char BufferInit[offsetof(RemoteSaveBuffer,init)==0 ? 1 : -1];
typedef char BufferChunk[offsetof(RemoteSaveBuffer,chunk)==0 ? 1 : -1];
typedef char BufferBytes[offsetof(RemoteSaveBuffer,bytes)==0 ? 1 : -1];
typedef char AdventurePayloadSize[sizeof(AdventureRemotePayload)==247 ? 1 : -1];
typedef char AdventureSave[offsetof(AdventureRemotePayload,save)==0 ? 1 : -1];
typedef char AdventureExit[offsetof(AdventureRemotePayload,playerExit)==0 ? 1 : -1];
typedef char AdventureBytes[sizeof(static_cast<AdventureRemotePayload*>(0)->bytes)==247 ? 1 : -1];
typedef char MessageSize[sizeof(RemoteMessage)==256 ? 1 : -1];
typedef char MessagePayload[offsetof(RemoteMessage,payload)==9 ? 1 : -1];
typedef char MessagePayloadSize[sizeof(static_cast<RemoteMessage*>(0)->payload)==247 ? 1 : -1];
typedef char SaveMessageSize[sizeof(RemoteSaveMessage)==256 ? 1 : -1];
typedef char SaveMessagePayload[offsetof(RemoteSaveMessage,payload)==9 ? 1 : -1];
typedef char SaveMessagePayloadSize[sizeof(static_cast<RemoteSaveMessage*>(0)->payload)==247 ? 1 : -1];
typedef char KbMessageSize[sizeof(KbRemotePacket)==256 ? 1 : -1];
typedef char KbId[offsetof(KbRemotePacket,id)==1 ? 1 : -1];
typedef char KbType[offsetof(KbRemotePacket,type)==5 ? 1 : -1];
typedef char KbCommand[offsetof(KbRemotePacket,command)==6 ? 1 : -1];
typedef char KbLength[offsetof(KbRemotePacket,payloadSize)==7 ? 1 : -1];
typedef char KbPayload[offsetof(KbRemotePacket,payload)==9 ? 1 : -1];
typedef char KbPayloadBytes[sizeof(static_cast<KbRemotePacket*>(0)->payload.data)==247 ? 1 : -1];
typedef char SetupFits[sizeof(OldMainNetSetup)==212 ? 1 : -1];
typedef char MapChangesFit[sizeof(sMapChangeLastFew)==44 ? 1 : -1];
typedef char ExitFits[sizeof(SPlayerExit)==7 ? 1 : -1];

void CheckSaveInit(RemoteSaveInitialization&);
void CheckSaveIndex(i16&);
void CheckSaveCrc(i32&);
void CheckSaveLayouts(RemoteSaveBuffer* output, AdventureRemotePayload* input,
                     KbRemotePacket* kb) {
    CheckSaveInit(output->init);
    CheckSaveInit(input->save);
    CheckSaveInit(kb->payload.save);
    CheckSaveIndex(output->chunk.packetIndex);
    CheckSaveCrc(output->init.crc);
    CheckSaveCrc(output->init.wireCrc);
}
