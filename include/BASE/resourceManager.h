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
    RESOURCE_MANAGER_AGGREGATE_LIMIT   = 4,
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
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void DisableLocaleAggregates(void);
    void GetBackdrop(const char*, class bitmap*, i32);
    void GetBackdropAtLoc(const char*, class bitmap*, i32, i32, i32);
    class palette* GetPalette(const char*);
    class bitmap* GetBitmap(const char*);
    class icon* GetIcon(const char*);
    class icon* GetIcon(u32l);
    class tileset* GetTileset(const char*);
    class mouse* GetMouse(const char*);
    class font* GetFont(const char*);
    class sample* GetSample(const char*);
    void Dispose(class resource*);
    void AddResource(class resource*);
    void Expunge(void);
    class resource* Query(u32l);
    void RemoveResource(class resource*);
    i32 LoadAggregateHeader(const char*, bool locale = false, bool required = true);
    void PointToFile(u32l);
    u32l GetFileSize(u32l);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(const char*, i32);
    void Read13(void*);
    void ReadBlock(void*, u32l);
};
#pragma pack(pop)
extern i32 iSaveCtr;
extern i32 lastAggZ[];
extern i32l lastPositionZ[];

#endif
