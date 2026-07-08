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
class resourceManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    resource *field_0x36;  // +0x36  resource-list head
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    int    field_0x42[2];  // +0x42  per-aggregate file descriptors
    void  *field_0x4a[2];  // +0x4a  per-aggregate header buffers
    int    field_0x52[2];  // +0x52
    int    field_0x5a;  // +0x5a
    char _pad_0x5e[0x4];
    char   field_0x62;  // +0x62
    char _pad_0x63[0x3b];
    int    field_0x9e;  // +0x9e
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
DATA(0x0051e99c) extern int iSaveCtr;
DATA(0x005331e8) extern int lastAggZ[10];
DATA(0x00533210) extern long lastPositionZ[12];

#endif // HOMM2_BASE_RESOURCEMANAGER_H
