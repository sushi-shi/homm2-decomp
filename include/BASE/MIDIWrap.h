#ifndef HOMM2_BASE_MIDIWRAP_H
#define HOMM2_BASE_MIDIWRAP_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class MIDIWrap {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    int    field_0x10;  // +0x10
    // --- constructors ---
    MIDIWrap(char *);
    virtual ~MIDIWrap();
};
#pragma pack(pop)
SIZE(MIDIWrap, 0x14);
#endif // HOMM2_BASE_MIDIWRAP_H
