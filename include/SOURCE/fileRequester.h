#ifndef HOMM2_SOURCE_FILEREQUESTER_H
#define HOMM2_SOURCE_FILEREQUESTER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class fileRequester : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    int    field_0x42;  // +0x42
    char _pad_0x46[0x258];
    int    field_0x29e;  // +0x29e
    int    field_0x2a2;  // +0x2a2
    int    field_0x2a6;  // +0x2a6
    int    field_0x2aa;  // +0x2aa
    char _pad_0x2ae[0x5];
    char   field_0x2b3;  // +0x2b3
    char _pad_0x2b4[0x15e];
    int    field_0x412;  // +0x412
    int    field_0x416;  // +0x416
    int    field_0x41a;  // +0x41a
    int    field_0x41e;  // +0x41e
    char _pad_0x422[0x9];
    int    field_0x42b;  // +0x42b
    // --- constructors ---
    fileRequester(int, int, int, char *, char *, char *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    int InitializeFiles(char *, char *, int);
    int MapExistsForFilter(int);
    void SetupFiles(void);
    void CleanUpData(void);
    void SetOK(int);
    void DoKnob(void);
    void Update(int);
    char * GetFilename(void);
};
#pragma pack(pop)
SIZE(fileRequester, 0x42f);
#endif // HOMM2_SOURCE_FILEREQUESTER_H
