#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>
#include <BASE/sampleData.h>

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
    SamplePlaybackData m_playbackData;  // +0x10..+0x2f
    // --- constructors ---
    sample(char *, i32l, i32l, i32l);
    // Inline lets the compiler fold the body into ??_G; dllexport also retains the
    // standalone ??1 body required by retail.
    __declspec(dllexport) virtual inline ~sample() OVERRIDE;
};
#pragma pack(pop)
SIZE(sample, 0x30);
#endif // HOMM2_BASE_SAMPLE_H
