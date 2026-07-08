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
    tag_message field_0x36[64];  // +0x36  event ring (64 x 0x1c)
    int    field_0x736;  // +0x736
    int    field_0x73a;  // +0x73a
    int    field_0x73e;  // +0x73e
    int    field_0x742;  // +0x742
    int    field_0x746;  // +0x746
    int    field_0x74a;  // +0x74a
    short  field_0x74e;  // +0x74e
    short  field_0x750;  // +0x750
    short  field_0x752;  // +0x752
    short  field_0x754;  // +0x754
    short  field_0x756;  // +0x756
    short  field_0x758;  // +0x758
    short  field_0x75a;  // +0x75a
    short  field_0x75c;  // +0x75c
    short  field_0x75e;  // +0x75e
    short  field_0x760;  // +0x760
    short  field_0x762;  // +0x762
    short  field_0x764;  // +0x764
    short  field_0x766;  // +0x766
    short  field_0x768;  // +0x768
    short  field_0x76a;  // +0x76a
    short  field_0x76c;  // +0x76c
    short  field_0x76e;  // +0x76e
    short  field_0x770;  // +0x770
    short  field_0x772;  // +0x772
    short  field_0x774;  // +0x774
    short  field_0x776;  // +0x776
    short  field_0x778;  // +0x778
    short  field_0x77a;  // +0x77a
    short  field_0x77c;  // +0x77c
    short  field_0x77e;  // +0x77e
    short  field_0x780;  // +0x780
    short  field_0x782;  // +0x782
    short  field_0x784;  // +0x784
    short  field_0x786;  // +0x786
    short  field_0x788;  // +0x788
    short  field_0x78a;  // +0x78a
    short  field_0x78c;  // +0x78c
    short  field_0x78e;  // +0x78e
    short  field_0x790;  // +0x790
    short  field_0x792;  // +0x792
    short  field_0x794;  // +0x794
    short  field_0x796;  // +0x796
    short  field_0x798;  // +0x798
    short  field_0x79a;  // +0x79a
    short  field_0x79c;  // +0x79c
    short  field_0x79e;  // +0x79e
    short  field_0x7a0;  // +0x7a0
    short  field_0x7a2;  // +0x7a2
    short  field_0x7a4;  // +0x7a4
    short  field_0x7a6;  // +0x7a6
    short  field_0x7a8;  // +0x7a8
    short  field_0x7aa;  // +0x7aa
    short  field_0x7ac;  // +0x7ac
    short  field_0x7ae;  // +0x7ae
    short  field_0x7b0;  // +0x7b0
    short  field_0x7b2;  // +0x7b2
    short  field_0x7b4;  // +0x7b4
    short  field_0x7b6;  // +0x7b6
    short  field_0x7b8;  // +0x7b8
    short  field_0x7ba;  // +0x7ba
    short  field_0x7bc;  // +0x7bc
    short  field_0x7be;  // +0x7be
    short  field_0x7c0;  // +0x7c0
    short  field_0x7c2;  // +0x7c2
    short  field_0x7c4;  // +0x7c4
    short  field_0x7c6;  // +0x7c6
    short  field_0x7c8;  // +0x7c8
    short  field_0x7ca;  // +0x7ca
    short  field_0x7cc;  // +0x7cc
    short  field_0x7ce;  // +0x7ce
    short  field_0x7d0;  // +0x7d0
    short  field_0x7d2;  // +0x7d2
    short  field_0x7d4;  // +0x7d4
    short  field_0x7d6;  // +0x7d6
    short  field_0x7d8;  // +0x7d8
    short  field_0x7da;  // +0x7da
    short  field_0x7dc;  // +0x7dc
    short  field_0x7de;  // +0x7de
    short  field_0x7e0;  // +0x7e0
    short  field_0x7e2;  // +0x7e2
    short  field_0x7e4;  // +0x7e4
    short  field_0x7e6;  // +0x7e6
    short  field_0x7e8;  // +0x7e8
    short  field_0x7ea;  // +0x7ea
    short  field_0x7ec;  // +0x7ec
    short  field_0x7ee;  // +0x7ee
    short  field_0x7f0;  // +0x7f0
    short  field_0x7f2;  // +0x7f2
    short  field_0x7f4;  // +0x7f4
    short  field_0x7f6;  // +0x7f6
    short  field_0x7f8;  // +0x7f8
    short  field_0x7fa;  // +0x7fa
    short  field_0x7fc;  // +0x7fc
    short  field_0x7fe;  // +0x7fe
    char _pad_0x800[0x4e];
    int    field_0x84e;  // +0x84e
    int    field_0x852;  // +0x852
    int    field_0x856;  // +0x856
    char _pad_0x85a[0x4];
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
DATA(0x0051f980) extern int iCurSwapPalette;
DATA(0x0051f984) extern int bLastMouseOffscreen;
DATA(0x0051f988) extern int bLastOnscreenMouseColor;
DATA(0x0051f98c) extern int bInCheckChangeCursor;
DATA(0x00534bc8) extern int iLastBWOnScreenCheck;

#endif // HOMM2_BASE_INPUTMANAGER_H
