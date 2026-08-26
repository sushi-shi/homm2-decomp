#ifndef HOMM2_PLATFORM_SDL3_INTERNAL_H
#define HOMM2_PLATFORM_SDL3_INTERNAL_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <PLATFORM/Audio.h>
#include <PLATFORM/FileSystem.h>
#include <PLATFORM/Host.h>
#include <PLATFORM/Input.h>
#include <PLATFORM/Types.h>

namespace platform::sdl3 {

struct CopyRegion {
    int sourceX = 0;
    int sourceY = 0;
    int destinationX = 0;
    int destinationY = 0;
    int width = 0;
    int height = 0;
};

bool ClipCopyRegion(
    Rect source,
    Point destination,
    Size sourceBounds,
    Size destinationBounds,
    CopyRegion& clipped
);

class TextInputDepth {
public:
    bool Acquire();
    bool Release();
    bool Reset();
    unsigned Count() const { return m_count; }

private:
    unsigned m_count = 0;
};

struct DecodedAudio {
    std::vector<std::uint8_t> samples;
    int sampleRate = 0;
    int channels = 0;
};

bool DecodeAudioFile(const std::string& path, int requestedChannels, DecodedAudio& audio);
void CloseMovies();

class Video;

struct InputHostServices {
    std::unique_ptr<IInput> input;
    std::unique_ptr<IHost> host;
};

InputHostServices CreateInputHost(Video& video);
std::unique_ptr<IAudio> CreateAudio();
std::unique_ptr<IFileSystem> CreateFileSystem();

}

#endif
