#include <PLATFORM/Video.h>
#include <PLATFORM/FileSystem.h>

#include <sstream>
#include <string>

namespace platform {
namespace {
constexpr const char* kPath = "HEROES2.DISPLAY";
const char* ScalingName(Scaling scaling) {
    switch (scaling) {
    case Scaling::Nearest: return "nearest";
    case Scaling::Linear: return "linear";
    case Scaling::Integer: return "integer";
    }
    return nullptr;
}
}

bool ReadDisplaySettings(IFileSystem& files, DisplaySettings& settings) {
    const i32 file = files.Open(kPath, FileMode::Read);
    if (file < 0) return false;
    char bytes[256] = {};
    const i32 length = files.Length(file);
    const bool read = length > 0 && length < static_cast<i32>(sizeof(bytes))
        && ReadExact(files, file, bytes, length);
    files.Close(file);
    if (!read) return false;
    std::istringstream input(std::string(bytes, static_cast<std::size_t>(length)));
    std::string magic, fullKey, scaleKey, scale, syncKey, extra;
    int version = 0, full = -1, sync = -1;
    if (!(input >> magic >> version >> fullKey >> full >> scaleKey >> scale >> syncKey >> sync)
        || input >> extra || magic != "H2DISPLAY" || version != 1
        || fullKey != "fullscreen" || scaleKey != "scaling" || syncKey != "vsync"
        || (full != 0 && full != 1) || (sync != 0 && sync != 1)) return false;
    DisplaySettings decoded;
    if (scale == "nearest") decoded.scaling = Scaling::Nearest;
    else if (scale == "linear") decoded.scaling = Scaling::Linear;
    else if (scale == "integer") decoded.scaling = Scaling::Integer;
    else return false;
    decoded.fullscreen = full != 0;
    decoded.vsync = sync != 0;
    settings = decoded;
    return true;
}

bool WriteDisplaySettings(IFileSystem& files, const DisplaySettings& settings) {
    const char* scale = ScalingName(settings.scaling);
    if (scale == nullptr) return false;
    const std::string text = std::string("H2DISPLAY 1\nfullscreen ")
        + (settings.fullscreen ? "1" : "0") + "\nscaling " + scale
        + "\nvsync " + (settings.vsync ? "1" : "0") + "\n";
    const i32 file = files.Open(kPath, FileMode::Write);
    if (file < 0) return false;
    const bool written = WriteExact(files, file, text.data(), static_cast<i32>(text.size()));
    files.Close(file);
    return written;
}
}
