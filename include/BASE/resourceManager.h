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
    unsigned long id;
    long offset;
    unsigned long size;
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
    int    m_numAggregates;  // +0x3a
    int    m_curAggregate;  // +0x3e
    int    m_aggregateFd[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x42  per-aggregate file descriptors
    aggEntry *m_aggregateDir[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x4a  per-aggregate directory
    int    m_aggregateEntryCount[RESOURCE_MANAGER_AGGREGATE_LIMIT];  // +0x52
    int    m_expunging;  // +0x5a
    int    m_reserved;  // +0x5e  unreferenced/reserved state
    char   m_lastFileName[0x3c];  // +0x62
    int    m_lastFileId;  // +0x9e
    // --- constructors ---
    resourceManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void GetBackdrop(char *, class bitmap *, int);
    void GetBackdropAtLoc(char *, class bitmap *, int, int, int);
    class palette * GetPalette(char *);
    class bitmap * GetBitmap(char *);
    class icon * GetIcon(char *);
    class icon * GetIcon(unsigned long int);
    class tileset * GetTileset(char *);
    class mouse * GetMouse(char *);
    class font * GetFont(char *);
    class sample * GetSample(char *);
    class MIDIWrap * GetMIDIWrap(char *);
    void Dispose(class resource *);
    void AddResource(class resource *);
    void Expunge(void);
    class resource * Query(unsigned long int);
    void RemoveResource(class resource *);
    int LoadAggregateHeader(char *);
    void PointToFile(unsigned long int);
    unsigned long int GetFileSize(unsigned long int);
    void SavePosition(void);
    void RestorePosition(void);
    signed char ReadByte(void);
    short int ReadWord(void);
    long int ReadLong(void);
    unsigned long int MakeId(char *, int);
    void Read13(signed char *);
    void ReadBlock(signed char *, unsigned long int);
};
#pragma pack(pop)
SIZE(resourceManager, 0xa2);
// ---- globals (declarations, RVA order) ----
extern int iSaveCtr;
extern int lastAggZ[10];
extern long lastPositionZ[10];

#endif // HOMM2_BASE_RESOURCEMANAGER_H
