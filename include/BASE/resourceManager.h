#ifndef HOMM2_BASE_RESOURCEMANAGER_H
#define HOMM2_BASE_RESOURCEMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 31 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
// forward declarations:
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

#pragma pack(push, 1)  // recovered layout is byte-packed
struct aggEntry {  // one .agg directory record (0xc bytes)
    u32l id;
    i32l offset;
    u32l size;
};

typedef enum ResourceManagerConstant {
    RESOURCE_MANAGER_INVALID_FILE = -1,
    RESOURCE_MANAGER_SUCCESS = 0,
    RESOURCE_MANAGER_ERROR = 3,
    RESOURCE_MANAGER_AGGREGATE_LIMIT = 2,
    RESOURCE_MANAGER_ENTRY_BYTES = 0xc,
    RESOURCE_MANAGER_READ13_BYTES = 0xd,
    RESOURCE_MANAGER_EVIL_TRANSLATION_COUNT = 37,
    RESOURCE_MANAGER_MESSAGE_MASK = 0x80,
    RESOURCE_MANAGER_BACKDROP_ROW_BYTES = 640,
    RESOURCE_MANAGER_BINARY_OPEN_MODE = 0x8000
} ResourceManagerConstant;

class resourceManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    resource *m_resourceListHead;  // +0x36  resource-list head
    i32    m_numAggregates;  // +0x3a
    i32    m_curAggregate;  // +0x3e
    i32    m_aggregateFd[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x42  per-aggregate file descriptors
    aggEntry *m_aggregateDir[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x4a  per-aggregate directory
    i32    m_aggregateEntryCount[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x52
    i32    m_expunging;  // +0x5a
    i32    m_reserved;  // +0x5e  unreferenced/reserved state
    char   m_lastFileName[0x3c];  // +0x62
    i32    m_lastFileId;  // +0x9e
    // --- constructors ---
    resourceManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void GetBackdrop(char *, class bitmap *, i32);
    void GetBackdropAtLoc(char *, class bitmap *, i32, i32, i32);
    class palette * GetPalette(char *);
    class bitmap * GetBitmap(char *);
    class icon * GetIcon(char *);
    class icon * GetIcon(u32l);
    class tileset * GetTileset(char *);
    class mouse * GetMouse(char *);
    class font * GetFont(char *);
    class sample * GetSample(char *);
    class MIDIWrap * GetMIDIWrap(char *);
    void Dispose(class resource *);
    void AddResource(class resource *);
    void Expunge(void);
    class resource * Query(u32l);
    void RemoveResource(class resource *);
    i32 LoadAggregateHeader(char *);
    void PointToFile(u32l);
    u32l GetFileSize(u32l);
    void SavePosition(void);
    void RestorePosition(void);
    i8 ReadByte(void);
    i16 ReadWord(void);
    i32l ReadLong(void);
    u32l MakeId(char *, i32);
    void Read13(i8 *);
    void ReadBlock(i8 *, u32l);
};
#pragma pack(pop)
SIZE(resourceManager, 0xa2);
// ---- globals (declarations, RVA order) ----
extern i32 iSaveCtr;
extern i32 lastAggZ[10];
extern i32l lastPositionZ[10];

#endif // HOMM2_BASE_RESOURCEMANAGER_H
