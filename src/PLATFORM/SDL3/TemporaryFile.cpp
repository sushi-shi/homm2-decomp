#include "TemporaryFile.h"

#include <atomic>
#include <cerrno>
#include <cstdio>
#include <filesystem>
#include <limits>

#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace platform::sdl3 {
namespace {

std::filesystem::path Utf8Path(const std::string& text) {
    return std::filesystem::path(std::u8string_view(
        reinterpret_cast<const char8_t*>(text.data()), text.size()));
}

int OpenExclusive(const std::string& path) {
#ifdef _WIN32
    return ::_wopen(Utf8Path(path).c_str(),
                   _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
    return ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
#endif
}

bool CloseDescriptor(int descriptor) {
#ifdef _WIN32
    return ::_close(descriptor) == 0;
#else
    return ::close(descriptor) == 0;
#endif
}

FILE* StreamFromDescriptor(int descriptor) {
#ifdef _WIN32
    return ::_fdopen(descriptor, "wb");
#else
    return ::fdopen(descriptor, "wb");
#endif
}

std::size_t SDLCALL Write(void* context, const void* bytes, std::size_t count,
                          SDL_IOStatus* status) {
    const auto written = std::fwrite(bytes, 1, count, static_cast<FILE*>(context));
    if (written < count) *status = SDL_IO_STATUS_ERROR;
    return written;
}

bool SDLCALL Flush(void* context, SDL_IOStatus* status) {
    if (std::fflush(static_cast<FILE*>(context)) == 0) return true;
    *status = SDL_IO_STATUS_ERROR;
    return false;
}

bool SDLCALL Close(void* context) { return std::fclose(static_cast<FILE*>(context)) == 0; }

}

SDL_IOStream* OpenTemporarySibling(const std::string& destination, std::string& temporary) {
    static std::atomic<unsigned> sequence{0};
    const auto parent = Utf8Path(destination).parent_path();
    temporary.clear();
    for (unsigned attempt = 0; attempt < 128; ++attempt) {
        const auto name = ".homm2-save-" + std::to_string(SDL_GetTicksNS())
            + "-" + std::to_string(sequence.fetch_add(1)) + ".tmp";
        const auto candidate = parent / name;
        const auto utf8 = candidate.u8string();
        const std::string path(reinterpret_cast<const char*>(utf8.data()), utf8.size());
        const int descriptor = OpenExclusive(path);
        if (descriptor < 0) {
            if (errno == EEXIST) continue;
            return nullptr;
        }
        FILE* file = StreamFromDescriptor(descriptor);
        if (file == nullptr) {
            CloseDescriptor(descriptor);
            std::error_code error;
            std::filesystem::remove(candidate, error);
            return nullptr;
        }
        SDL_IOStreamInterface interface;
        SDL_INIT_INTERFACE(&interface);
        interface.write = Write;
        interface.flush = Flush;
        interface.close = Close;
        SDL_IOStream* stream = SDL_OpenIO(&interface, file);
        if (stream == nullptr) {
            std::fclose(file);
            std::error_code error;
            std::filesystem::remove(candidate, error);
            return nullptr;
        }
        SDL_SetNumberProperty(SDL_GetIOProperties(stream),
                              SDL_PROP_IOSTREAM_FILE_DESCRIPTOR_NUMBER, descriptor);
        temporary = path;
        return stream;
    }
    return nullptr;
}

bool FlushFileData(SDL_IOStream* stream) {
    if (!SDL_FlushIO(stream)) return false;
#ifdef __EMSCRIPTEN__
    // This commits to the virtual filesystem. IDBFS persistence is asynchronous.
    return true;
#else
    const Sint64 descriptor = SDL_GetNumberProperty(SDL_GetIOProperties(stream),
        SDL_PROP_IOSTREAM_FILE_DESCRIPTOR_NUMBER, -1);
    if (descriptor < 0 || descriptor > std::numeric_limits<int>::max()) return false;
#ifdef _WIN32
    return ::_commit(static_cast<int>(descriptor)) == 0;
#else
    return ::fsync(static_cast<int>(descriptor)) == 0;
#endif
#endif
}

bool RemoveTemporaryFile(const std::string& temporary) {
    std::error_code error;
    return std::filesystem::remove(Utf8Path(temporary), error);
}

bool ReplaceTemporaryFile(const std::string& temporary, const std::string& destination) {
    const auto source = Utf8Path(temporary);
    const auto target = Utf8Path(destination);
    if (source.parent_path() != target.parent_path() || source == target) return false;
#ifdef _WIN32
    return ::MoveFileExW(source.c_str(), target.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    if (::rename(temporary.c_str(), destination.c_str()) != 0) return false;
#ifndef __EMSCRIPTEN__
    const int directory = ::open(target.parent_path().c_str(), O_RDONLY | O_DIRECTORY);
    const bool synced = directory >= 0 && ::fsync(directory) == 0;
    if (directory >= 0) ::close(directory);
    // Replacement has already succeeded. A directory-sync failure cannot be
    // reported as a failed replacement (the new complete save is now visible).
    if (!synced) std::fprintf(stderr, "[homm2] save replaced, but directory sync failed\n");
#endif
    return true;
#endif
}

}
