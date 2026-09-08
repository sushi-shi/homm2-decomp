#include <SOURCE/SaveNames.h>

namespace save_names {
namespace {

bool IsNameByte(char value) {
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z')
        || (value >= '0' && value <= '9') || value == '_';
}

}

LegacyFilename ToLegacyFilename(std::string_view name) {
    LegacyFilename result{};
    name = name.substr(0, name.find('\0'));
    const std::size_t dot = name.find_last_of('.');
    const std::string_view stem = name.substr(0, dot);
    std::size_t written = 0;
    for (char value : stem) {
        if (!IsNameByte(value))
            continue;
        if (value >= 'a' && value <= 'z')
            value = static_cast<char>(value - 'a' + 'A');
        result[written++] = value;
        if (written == 8)
            break;
    }

    if (dot != std::string_view::npos) {
        result[written++] = '.';
        std::size_t extensionLength = 0;
        for (char value : name.substr(dot + 1)) {
            if (!IsNameByte(value))
                continue;
            result[written++] = value;
            if (++extensionLength == 3)
                break;
        }
    }
    return result;
}

}
