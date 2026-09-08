#include <BASE/AggDirectory.h>
#include <PLATFORM/Binary.h>

#include <array>
#include <limits>

namespace resources {

bool ReadAggDirectory(platform::IFileSystem& files, i32 file,
                      std::vector<AggEntry>& entries, std::string& error) {
    error.clear();
    const auto reject = [&](const char* reason) {
        error = reason;
        return false;
    };
    const i32 length = files.Length(file);
    std::array<u8, 2> countBytes{};
    if (length < 2 || files.Seek(file, 0) != 0
        || !platform::ReadExact(files, file, countBytes.data(), countBytes.size()))
        return reject("missing entry count");
    u16 count = 0;
    platform::binary::ReadU16(countBytes.data(), countBytes.size(), 0, count);
    constexpr u32 recordSize = 12;
    constexpr u32 nameSize = 15;
    const u32 directoryEnd = 2 + static_cast<u32>(count) * recordSize;
    const u32 nameBytes = static_cast<u32>(count) * nameSize;
    if (count == 0 || directoryEnd + nameBytes > static_cast<u32>(length))
        return reject("directory or filename table exceeds the file");
    const u32 payloadEnd = static_cast<u32>(length) - nameBytes;

    std::vector<AggEntry> parsed;
    parsed.reserve(count);
    for (u32 index = 0; index < count; ++index) {
        std::array<u8, recordSize> bytes{};
        if (!platform::ReadExact(files, file, bytes.data(), bytes.size()))
            return reject("truncated entry table");
        AggEntry entry{};
        u32 offset = 0;
        platform::binary::ReadU32(bytes.data(), bytes.size(), 0, entry.id);
        platform::binary::ReadU32(bytes.data(), bytes.size(), 4, offset);
        platform::binary::ReadU32(bytes.data(), bytes.size(), 8, entry.size);
        if (offset < directoryEnd || offset > payloadEnd || entry.size > payloadEnd - offset)
            return reject("member extends outside the payload area");
        entry.offset = static_cast<i32>(offset);
        parsed.push_back(entry);
    }
    entries.swap(parsed);
    return true;
}

bool ReadAggMember(platform::IFileSystem& files, i32 file, const AggEntry& entry,
                   void* destination, u32 size) {
    const i32 position = files.Tell(file);
    if (entry.offset < 0 || position < entry.offset
        || size > static_cast<u32>(std::numeric_limits<i32>::max()))
        return false;
    const u32 consumed = static_cast<u32>(position - entry.offset);
    if (consumed > entry.size || size > entry.size - consumed)
        return false;
    return platform::ReadExact(files, file, destination, static_cast<i32>(size));
}

}
