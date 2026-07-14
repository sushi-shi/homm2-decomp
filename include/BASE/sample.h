#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>

typedef enum sampleAudioFormat {
    SAMPLE_FORMAT_8_BIT = 0,
    SAMPLE_FORMAT_16_BIT = 1,
    SAMPLE_FORMAT_STEREO = 2
} sampleAudioFormat;

typedef enum samplePlaybackRate {
    SAMPLE_RATE_11025 = 11025,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_44100 = 44100
} samplePlaybackRate;

#pragma pack(push, 1)  // recovered layout is byte-packed
class sample : public resource {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    struct _SAMPLE *m_activeSample;  // +0x10  active AIL sample
    char   *m_data;  // +0x14  sample data buffer
    int    m_size;  // +0x18
    int    m_channelType;  // +0x1c
    int    m_sampleRate;  // +0x20
    int    m_format;  // +0x24
    int    m_volume;  // +0x28
    int    m_loopCount;  // +0x2c
    // --- constructors ---
    sample(char *, long int, long int, long int);
    // Inline lets the compiler fold the body into ??_G; dllexport also retains the
    // standalone ??1 body required by retail.
    __declspec(dllexport) virtual inline ~sample() OVERRIDE;
};
#pragma pack(pop)
SIZE(sample, 0x30);
#endif // HOMM2_BASE_SAMPLE_H
