#include <va.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/SAMPLE_TYPES.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <string.h>

H2_ENUM_BEGIN(SampleConstant)
    FILENAME_CAPACITY    = 32,
    FORMAT_SUFFIX_LENGTH = 3
H2_ENUM_END(SampleConstant)

static SSampleSourceFiles gSampleSourceFiles =
    {SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE};

VA(0x004ce250, 0x1b7)
sample::sample(char* name, i32l, i32l, i32l)
    : resource(
        RESOURCE_CATEGORY_SAMPLE,
        gpResourceManager->MakeId(name, 1),
        RESOURCE_REFERENCE_INITIAL,
        NULL
    ) {
    m_playbackData.volume = 0x7f;
    m_playbackData.loopCount = 0;
    m_playbackData.stereo = 1;
    m_playbackData.sampleFormat = FORMAT_16_BIT;
    m_playbackData.sampleRate = RATE_44100;
    m_playbackData.activeSample = NULL;
    m_playbackData.channelType = 0;

    char filename[FILENAME_CAPACITY];
    strcpy(filename, name);
    _strrev(filename);

    for (i32 i = 0; i < FORMAT_SUFFIX_LENGTH; i++) {
        switch (filename[i]) {
            case '1':
                m_playbackData.sampleRate = RATE_11025;
                break;
            case '2':
                m_playbackData.sampleRate = RATE_22050;
                break;
            case '4':
                m_playbackData.sampleRate = RATE_44100;
                break;
            case '6':
                m_playbackData.sampleFormat = FORMAT_16_BIT;
                break;
            case '8':
                m_playbackData.sampleFormat = FORMAT_8_BIT;
                break;
            case 'M':
            case 'm':
                m_playbackData.stereo = 0;
                break;
        }
    }

    u32l size = gpResourceManager->GetFileSize(m_id);
#line 57
    m_playbackData.data =
        static_cast<char*>(H2_ALLOC(size));
    m_playbackData.size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_playbackData.data), size);
}

VA(0x004ce490, 0x8b)
inline sample::~sample() {
    if (gpSoundManager != NULL)
        gpSoundManager->StopSample(this);
#line 97
    H2_FREE(m_playbackData.data);
    memset(&m_playbackData, 0, sizeof(m_playbackData));
}

VA(0x004ce520, 0xbd)
MIDIWrap::MIDIWrap(char* name)
    : resource(
        RESOURCE_CATEGORY_SAMPLE,
        gpResourceManager->MakeId(name, 1),
        RESOURCE_REFERENCE_INITIAL,
        NULL
    ) {
    u32l size = gpResourceManager->GetFileSize(m_id);
#line 110
    m_data = static_cast<char*>(H2_ALLOC(size));
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_data), size);
}

VA(0x004ce610, 0x3d)
inline MIDIWrap::~MIDIWrap() {
#line 118
    H2_FREE(m_data);
    m_data = NULL;
}
