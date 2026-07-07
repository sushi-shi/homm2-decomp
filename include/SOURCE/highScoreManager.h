#ifndef HOMM2_SOURCE_HIGHSCOREMANAGER_H
#define HOMM2_SOURCE_HIGHSCOREMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class highScoreManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    char _pad_0x36[0x28];
    char   field_0x5e;  // +0x5e
    int    field_0x5f;  // +0x5f
    // --- constructors ---
    highScoreManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#pragma pack(pop)
SIZE(highScoreManager, 0x63);
#endif // HOMM2_SOURCE_HIGHSCOREMANAGER_H
