#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class sample {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    char _pad_0x10[0x4];
    int    field_0x14;  // +0x14
    int    field_0x18;  // +0x18
    int    field_0x1c;  // +0x1c
    int    field_0x20;  // +0x20
    int    field_0x24;  // +0x24
    int    field_0x28;  // +0x28
    int    field_0x2c;  // +0x2c
    // --- constructors ---
    sample(char *, long int, long int, long int);
    virtual ~sample();
};
#pragma pack(pop)
SIZE(sample, 0x30);
#endif // HOMM2_BASE_SAMPLE_H
