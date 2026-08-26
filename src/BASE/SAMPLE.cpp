#include <Ints.h>
#include <PLATFORM/Strings.h>
#include <BASE/sample.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <string.h>

typedef enum SampleConstant {
    FILENAME_CAPACITY    = 32,
    FORMAT_SUFFIX_LENGTH = 3
} SampleConstant;

sample::sample(const char* name)
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
    m_playbackData.activeSample = 0;
    m_playbackData.channelType = 0;

    char fileName[FILENAME_CAPACITY];
    strcpy(fileName, name);
    platform::Reverse(fileName);

    for (i32 i = 0; i < FORMAT_SUFFIX_LENGTH; i++) {
        switch (fileName[i]) {
            case '8':
                m_playbackData.sampleFormat = FORMAT_8_BIT;
                break;
            case '6':
                m_playbackData.sampleFormat = FORMAT_16_BIT;
                break;
            case '1':
                m_playbackData.sampleRate = RATE_11025;
                break;
            case '2':
                m_playbackData.sampleRate = RATE_22050;
                break;
            case '4':
                m_playbackData.sampleRate = RATE_44100;
                break;
            case 'M':
            case 'm':
                m_playbackData.stereo = 0;
                break;
        }
    }

    u32l size = gpResourceManager->GetFileSize(m_id);
    m_playbackData.data =
        static_cast<char*>(H2_ALLOC(size));
    m_playbackData.size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_playbackData.data), size);
}

inline sample::~sample() {
    if (gpSoundManager != NULL)
        gpSoundManager->StopSample(this);
    H2_FREE(m_playbackData.data);
    memset(&m_playbackData, 0, sizeof(m_playbackData));
}
