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
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message&) OVERRIDE;
    void GetBackdrop(H2_CONST char*, class bitmap*, i32);
    void GetBackdropAtLoc(H2_CONST char*, class bitmap*, i32, i32, i32);
    class palette* GetPalette(H2_CONST char*);
    class bitmap* GetBitmap(H2_CONST char*);
    class icon* GetIcon(H2_CONST char*);
    class icon* GetIcon(u32l);
    class tileset* GetTileset(H2_CONST char*);
    class mouse* GetMouse(H2_CONST char*);
    class font* GetFont(H2_CONST char*);
    class sample* GetSample(H2_CONST char*);
    class MIDIWrap* GetMIDIWrap(H2_CONST char*);
    void Dispose(class resource*);
    void AddResource(class resource*);
    void Expunge(void);
    class resource* Query(u32l);
    void RemoveResource(class resource*);
    i32 LoadAggregateHeader(H2_CONST char*);
    void PointToFile(u32l);
    u32l GetFileSize(u32l);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(H2_CONST char*, i32);
    void Read13(i8*);
    void ReadBlock(i8*, u32l);
};
#pragma pack(pop)
SIZE(resourceManager, 0x44e);
extern i32 iSaveCtr;
extern i32 lastAggZ[];
extern i32l lastPositionZ[];

#endif
