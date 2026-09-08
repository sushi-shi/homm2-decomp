#include <SOURCE/MapRecords.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <BASE/Misc.h>
#include <PLATFORM/Platform.h>
#include <cstring>
#include <cstdlib>
#include <string>

namespace {

[[noreturn]] void InvalidExtra(const char* error) {
    platform::Host().Log(platform::LogLevel::Error,
        (std::string("Invalid map extra: ") + error).c_str());
    ShutDown(localization::Tr("system.file.read_error"));
    std::abort();
}

map_records::Record Extra(i32 index) {
    if (index <= 0 || index >= iMaxMapExtra || ppMapExtra == nullptr
        || pwSizeOfMapExtra == nullptr || ppMapExtra[index] == nullptr
        || pwSizeOfMapExtra[index] <= 0)
        InvalidExtra("missing record");
    return {static_cast<const u8*>(ppMapExtra[index]),
            static_cast<std::size_t>(pwSizeOfMapExtra[index])};
}

}

void* MapExtraRecord(i32 index, map_records::Kind kind) {
    if (const char* error = map_records::RecordError(Extra(index), kind)) InvalidExtra(error);
    return ppMapExtra[index];
}

std::string_view MapExtraText(i32 index, std::size_t offset) {
    const auto record = Extra(index);
    if (offset >= record.size()
        || std::memchr(record.data() + offset, 0, record.size() - offset) == nullptr)
        InvalidExtra("unterminated text");
    return map_records::Text(record, offset);
}

std::string_view MapExtraText(const void* record, std::size_t offset) {
    for (i32 index = 1; index < iMaxMapExtra; ++index) {
        if (ppMapExtra[index] == record) return MapExtraText(index, offset);
    }
    InvalidExtra("text record is not in the loaded table");
}
