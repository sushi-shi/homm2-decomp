#include <va.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/SAMPLE_TYPES.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <string.h>

DATA(0x00520df4) static SSampleSourceFiles gSampleSourceFiles =
    {SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE};

// @early-stop: retail alignment artifact.
VA(0x004dad60, 0x181)
sample::sample(char* name, i32l channelType, i32l volume, i32l loopCount)
    : resource(RESOURCE_CATEGORY_SAMPLE, gpResourceManager->MakeId(name, 1), 1, 0) {
    i32 formatFlags;
    m_playbackData.channelType = channelType;
    m_playbackData.volume = volume;
    m_playbackData.loopCount = loopCount;
    formatFlags = IDX(SAMPLE_FORMAT_STEREO);

    char filename[32];
    strcpy(filename, name);
    _strrev(filename);

    for (i32 i = 0; i < 3; i++) {
        switch (filename[i]) {
            case '1':
                m_playbackData.sampleRate = IDX(SAMPLE_RATE_11025);
                break;
            case '2':
                m_playbackData.sampleRate = IDX(SAMPLE_RATE_22050);
                break;
            case '4':
                m_playbackData.sampleRate = IDX(SAMPLE_RATE_44100);
                break;
            case '6':
                m_playbackData.format = IDX(SAMPLE_FORMAT_16_BIT);
                break;
            case '8':
                m_playbackData.format = IDX(SAMPLE_FORMAT_8_BIT);
                break;
            case 'M':
            case 'm':
                formatFlags = 0;
                break;
        }
    }
    m_playbackData.format += formatFlags;

    u32l size = gpResourceManager->GetFileSize(m_id);
#line 57
    m_playbackData.data =
        static_cast<char*>(H2_ALLOC_AT(size, gSampleSourceFiles.sampleAllocation, 57));
    m_playbackData.size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_playbackData.data), size);
}

// @early-stop: delinker artifact.
VA(0x004daf40, 0x2c)
inline sample::~sample() {
#line 97
    H2_FREE_AT(m_playbackData.data, gSampleSourceFiles.sampleDestruction, 0x61);
    m_playbackData.data = 0;
    m_playbackData.size = 0;
    m_playbackData.volume = 0;
}

VA(0x004daf70, 0x72)
MIDIWrap::MIDIWrap(char* name) : resource(RESOURCE_CATEGORY_SAMPLE, gpResourceManager->MakeId(name, 1), 1, 0) {
    u32l size = gpResourceManager->GetFileSize(m_id);
#line 110
    m_data = static_cast<char*>(H2_ALLOC_AT(size, gSampleSourceFiles.midiAllocation, 110));
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

// @early-stop: delinker artifact.
VA(0x004db030, 0x28)
inline MIDIWrap::~MIDIWrap() {
#line 118
    H2_FREE_AT(m_data, gSampleSourceFiles.midiDestruction, 0x76);
    m_data = 0;
}



VTBL(sample, 0x004ebab4);
VTBL(MIDIWrap, 0x004ebab8);
