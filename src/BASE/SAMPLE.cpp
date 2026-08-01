#include <Ints.h>
#include <PLATFORM/Strings.h>
#include <BASE/sample.h>
#include <BASE/SAMPLE_TYPES.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <string.h>

typedef enum SampleConstant {
    FILENAME_CAPACITY    = 32,
    FORMAT_SUFFIX_LENGTH = 3
} SampleConstant;

static SSampleSourceFiles gSampleSourceFiles =
    {SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE, SAMPLE_SOURCE_FILE};

sample::sample(char* name, i32l channelType, i32l volume, i32l loopCount)
    : resource(
        RESOURCE_CATEGORY_SAMPLE,
        gpResourceManager->MakeId(name, 1),
        RESOURCE_REFERENCE_INITIAL,
        NULL
    ) {
    SampleAudioFormat formatFlags;
    m_playbackData.channelType = channelType;
    m_playbackData.volume = volume;
    m_playbackData.loopCount = loopCount;
    formatFlags = FORMAT_STEREO;

    char filename[FILENAME_CAPACITY];
    strcpy(filename, name);
    platform::Reverse(filename);

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
                m_playbackData.format = FORMAT_16_BIT;
                break;
            case '8':
                m_playbackData.format = FORMAT_8_BIT;
                break;
            case 'M':
            case 'm':
                formatFlags = FORMAT_MONO;
                break;
        }
    }
    m_playbackData.format |= formatFlags;

    u32l size = gpResourceManager->GetFileSize(m_id);
    m_playbackData.data =
        static_cast<char*>(H2_ALLOC(size));
    m_playbackData.size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_playbackData.data), size);
}

inline sample::~sample() {
    H2_FREE(m_playbackData.data);
    m_playbackData.data = NULL;
    m_playbackData.size = 0;
    m_playbackData.volume = 0;
}
