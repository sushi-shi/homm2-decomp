#ifndef HOMM2_BASE_INPUTMANAGER_H
#define HOMM2_BASE_INPUTMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
#include <_carcass_types.h>
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class inputManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    tag_message m_eventRing[64];  // +0x36  event ring (64 x 0x1c)
    int    m_readIndex;  // +0x736
    int    m_writeIndex;  // +0x73a
    int    field_0x73e;  // +0x73e
    int    field_0x742;  // +0x742
    int    field_0x746;  // +0x746
    int    field_0x74a;  // +0x74a
    short  m_keyState[128];  // +0x74e  scan-code to ASCII/extended-key table
    int    field_0x84e;  // +0x84e
    int    field_0x852;  // +0x852
    int    field_0x856;  // +0x856
    int    m_field_0x85a;  // +0x85a
    int    field_0x85e;  // +0x85e
    int    field_0x862;  // +0x862
    int    field_0x866;  // +0x866
    // --- constructors ---
    inputManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Flush(void);
    struct tag_message GetEvent(void);
    struct tag_message PeekEvent(void);
    void SetMouseCoords(int, int);
    void SetKeyCodeType(int);
    void AsciiConvert(struct tag_message &);
    void MakeScanCodeTable(void);
    void ForceMouseMove(void);
};
#pragma pack(pop)
SIZE(inputManager, 0x86a);
// ---- globals (declarations, RVA order) ----
extern int iCurSwapPalette;
extern int bLastMouseOffscreen;
extern int bLastOnscreenMouseColor;
extern int bInCheckChangeCursor;
extern int iLastBWOnScreenCheck;

#endif // HOMM2_BASE_INPUTMANAGER_H
