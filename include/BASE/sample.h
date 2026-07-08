#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class sample : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    struct _SAMPLE *m_activeSample;  // +0x10  active AIL sample
    char   *m_data;  // +0x14  sample data buffer
    int    m_size;  // +0x18
    int    field_0x1c;  // +0x1c
    int    m_sampleRate;  // +0x20
    int    m_format;  // +0x24
    int    field_0x28;  // +0x28
    int    field_0x2c;  // +0x2c
    // --- constructors ---
    sample(char *, long int, long int, long int);
    virtual ~sample();
};
#pragma pack(pop)
SIZE(sample, 0x30);
#endif // HOMM2_BASE_SAMPLE_H
