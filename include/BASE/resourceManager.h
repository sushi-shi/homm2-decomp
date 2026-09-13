#ifndef HOMM2_BASE_RESOURCEMANAGER_H
#define HOMM2_BASE_RESOURCEMANAGER_H

#include <Ints.h>
#include "baseManager.h"

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

typedef enum ResourceManagerLayoutConstant {
    // Two locale overlays plus Ironfist, expansion, and base archives.
    RESOURCE_MANAGER_AGGREGATE_LIMIT   = 5,
    RESOURCE_MANAGER_READ13_BYTES      = 0xd,
    RESOURCE_MANAGER_FILENAME_CAPACITY = 0x3e8
} ResourceManagerLayoutConstant;

class resourceManager : public baseManager {
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
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void DisableLocaleAggregates(void);
    void GetBackdrop(const char* name, class bitmap* backdrop, i32 useIcon);
    void GetBackdropAtLoc(const char* filename, class bitmap* destination, i32 destinationX, i32 destinationY, i32 useIcon);
    class palette* GetPalette(const char* name);
    class bitmap* GetBitmap(const char* name);
    class icon* GetIcon(const char* name);
    class icon* GetIcon(u32l resourceId);
    class tileset* GetTileset(const char* name);
    class mouse* GetMouse(const char*);
    class font* GetFont(const char* name);
    class sample* GetSample(const char* name);
    void Dispose(class resource* resourceToDispose);
    void AddResource(class resource* newResource);
    void Expunge(void);
    class resource* Query(u32l resourceId);
    void RemoveResource(class resource* resourceToRemove);
    i32 LoadAggregateHeader(const char* aggregateName, bool locale = false, bool required = true);
    void PointToFile(u32l fileId);
    u32l GetFileSize(u32l fileId);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(const char* name, i32 translate);
    void Read13(void* destination);
    void ReadBlock(void* destination, u32l size);
};
#pragma pack(pop)
extern i32 iSaveCtr;
extern i32 lastAggZ[];
extern i32l lastPositionZ[];

#endif
