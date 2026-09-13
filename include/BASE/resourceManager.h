#ifndef HOMM2_BASE_RESOURCEMANAGER_H
#define HOMM2_BASE_RESOURCEMANAGER_H

#include <va.h>
#include "baseManager.h"

class MIDIWrap;
class bitmap;
class font;
class icon;
class mouse;
class palette;
class resource;
class sample;
class tileset;
struct tag_message;

#pragma pack(push, 1)
struct aggEntry {
    u32l id;
    i32l offset;
    u32l size;
};

H2_ENUM_BEGIN(ResourceManagerLayoutConstant)
    RESOURCE_MANAGER_AGGREGATE_LIMIT   = 2,
    RESOURCE_MANAGER_READ13_BYTES      = 0xd,
    RESOURCE_MANAGER_FILENAME_CAPACITY = 0x3e8
H2_ENUM_END(ResourceManagerLayoutConstant)

class resourceManager H2_FINAL : public baseManager {
public:
    resource* m_resourceListHead;
    i32 m_numAggregates;
    i32 m_curAggregate;
    i32 m_aggregateFd[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    aggEntry* m_aggregateDir[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    i32 m_aggregateEntryCount[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    b32 m_expunging;
    i32 m_reserved;
    char m_lastFileName[RESOURCE_MANAGER_FILENAME_CAPACITY];
    i32 m_lastFileId;
    resourceManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message&) OVERRIDE;
    void GetBackdrop(H2_CONST char* name, class bitmap* backdrop, i32 useIcon);
    void GetBackdropAtLoc(H2_CONST char* filename, class bitmap* destination, i32 destinationX, i32 destinationY, i32 useIcon);
    class palette* GetPalette(H2_CONST char* name);
    class bitmap* GetBitmap(H2_CONST char* name);
    class icon* GetIcon(H2_CONST char* name);
    class icon* GetIcon(u32l resourceId);
    class tileset* GetTileset(H2_CONST char* name);
    class mouse* GetMouse(H2_CONST char*);
    class font* GetFont(H2_CONST char* name);
    class sample* GetSample(H2_CONST char* name);
    class MIDIWrap* GetMIDIWrap(H2_CONST char* name);
    void Dispose(class resource* resourceToDispose);
    void AddResource(class resource* newResource);
    void Expunge(void);
    class resource* Query(u32l resourceId);
    void RemoveResource(class resource* resourceToRemove);
    i32 LoadAggregateHeader(H2_CONST char* aggregateName);
    void PointToFile(u32l fileId);
    u32l GetFileSize(u32l fileId);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(H2_CONST char* name, i32 translate);
    void Read13(i8* destination);
    void ReadBlock(i8* destination, u32l size);
};
#pragma pack(pop)
SIZE(resourceManager, 0x44e);
extern i32 iSaveCtr;
extern i32 lastAggZ[];
extern i32l lastPositionZ[];

#endif
