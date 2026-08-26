#include <IRONFIST/paths.h>

#include <strings.h>

#include <filesystem>
#include <system_error>

#include <PLATFORM/Platform.h>

namespace ironfist {

std::string ResolveDataPath(const std::string& relative) {
    namespace fs = std::filesystem;

    fs::path resolved = platform::Files().DataRoot();
    fs::path remainder(relative);
    for (const fs::path& component : remainder) {
        fs::path candidate = resolved / component;
        if (!fs::exists(candidate)) {
            std::error_code listError;
            for (const fs::directory_entry& entry : fs::directory_iterator(resolved, listError)) {
                const std::string name = entry.path().filename().string();
                if (strcasecmp(name.c_str(), component.string().c_str()) == 0) {
                    candidate = entry.path();
                    break;
                }
            }
        }
        resolved = candidate;
    }
    return resolved.string();
}

} // namespace ironfist
