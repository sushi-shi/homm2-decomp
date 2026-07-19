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
struct aggEntry { // one .agg directory record (0xc bytes)
    u32l id;
    i32l offset;
    u32l size;
};

H2_ENUM_BEGIN(ResourceManagerLayoutConstant)
    RESOURCE_MANAGER_AGGREGATE_LIMIT   = 2,
    RESOURCE_MANAGER_READ13_BYTES      = 0xd,
    RESOURCE_MANAGER_FILENAME_CAPACITY = 0x3c
H2_ENUM_END(ResourceManagerLayoutConstant)

class resourceManager : public baseManager {
public:
    resource* m_resourceListHead;
    i32 m_numAggregates;
    i32 m_curAggregate;
    i32 m_aggregateFd[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    aggEntry* m_aggregateDir[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    i32 m_aggregateEntryCount[RESOURCE_MANAGER_AGGREGATE_LIMIT];
    i32 m_expunging;
    i32 m_reserved;
    char m_lastFileName[RESOURCE_MANAGER_FILENAME_CAPACITY];
    i32 m_lastFileId;
    resourceManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void GetBackdrop(char*, class bitmap*, i32);
    void GetBackdropAtLoc(char*, class bitmap*, i32, i32, i32);
    class palette* GetPalette(char*);
    class bitmap* GetBitmap(char*);
    class icon* GetIcon(char*);
    class icon* GetIcon(u32l);
    class tileset* GetTileset(char*);
    class mouse* GetMouse(char*);
    class font* GetFont(char*);
    class sample* GetSample(char*);
    class MIDIWrap* GetMIDIWrap(char*);
    void Dispose(class resource*);
    void AddResource(class resource*);
    void Expunge(void);
    class resource* Query(u32l);
    void RemoveResource(class resource*);
    i32 LoadAggregateHeader(char*);
    void PointToFile(u32l);
    u32l GetFileSize(u32l);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(char*, i32);
    void Read13(i8*);
    void ReadBlock(i8*, u32l);
};
#pragma pack(pop)
SIZE(resourceManager, 0xa2);
extern i32 iSaveCtr;
extern i32 lastAggZ[];
extern i32l lastPositionZ[];

#endif
