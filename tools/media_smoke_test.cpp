#include <PLATFORM/Platform.h>
#include <PLATFORM/Movie.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s <installed game directory>\n", argv[0]);
        return 2;
    }
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto state = std::filesystem::temp_directory_path()
        / ("homm2-media-smoke-" + std::to_string(nonce));
    if (setenv("HOMM2_DATA", argv[1], 1) != 0
        || setenv("XDG_DATA_HOME", state.c_str(), 1) != 0
        || setenv("SDL_VIDEODRIVER", "dummy", 1) != 0
        || setenv("SDL_AUDIODRIVER", "dummy", 1) != 0
        || !platform::Startup())
        return 1;

    platform::DisplayMode mode;
    mode.scale = 1;
    bool valid = platform::Video().Open(mode) && platform::Audio().Open();
    valid = valid && platform::Audio().PlayMusicTrack(2, 1)
        && platform::Audio().IsMusicPlaying();
    if (!valid)
        std::fputs("Ogg playback or SDL output failed\n", stderr);

    const platform::MovieId movie = platform::MovieOpen("DATA/IVYDES.SMK", true);
    if (movie == platform::kInvalidMovie) {
        std::fputs("Smacker opening failed\n", stderr);
        valid = false;
    } else {
        const platform::Size size = platform::MovieSize(movie);
        if (size.width <= 0 || size.height <= 0 || platform::MovieFrameCount(movie) <= 0
            || platform::Video().Pixels() == nullptr) {
            valid = false;
        } else {
            auto* pixels = platform::Video().Pixels();
            std::fill_n(pixels, mode.width * mode.height, 0xc7);
            platform::MovieTarget(movie, pixels, mode.width, mode.height, 0, 0);
            int decoded = 0;
            const int frames = std::min(platform::MovieFrameCount(movie), 8);
            for (; decoded < frames; ++decoded) {
                platform::MovieFrame frame;
                if (!platform::MovieDraw(movie, frame))
                    break;
                if (const u8* palette = platform::MoviePalette(movie)) {
                    std::array<platform::Color, 256> colors{};
                    for (std::size_t index = 0; index < colors.size(); ++index)
                        colors[index] = {palette[index * 3], palette[index * 3 + 1],
                                         palette[index * 3 + 2]};
                    platform::Video().SetPalette(colors.data(), 0, colors.size());
                }
                platform::Video().Present();
                platform::MovieAdvance(movie);
            }
            valid = valid && decoded == frames && decoded > 0
                && std::any_of(pixels, pixels + mode.width * mode.height,
                               [](u8 value) { return value != 0xc7; });
            std::printf("Smacker frames decoded and presented: %d\n", decoded);
        }
        platform::MovieClose(movie);
    }
    platform::Audio().StopMusic();
    platform::Shutdown();
    std::filesystem::remove_all(state);
    return valid ? 0 : 1;
}
