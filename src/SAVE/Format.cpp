#include <SAVE/Format.h>

#include <algorithm>
#include <array>

namespace savegame {
namespace {

constexpr std::array<u8, 8> Magic = {'H', '2', 'S', 'A', 'V', 'E', '\r', '\n'};

u32 Crc32(std::span<const u8> bytes) {
    u32 crc = 0xffffffffu;
    for (u8 byte : bytes) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320u : 0);
    }
    return crc ^ 0xffffffffu;
}

}

std::vector<u8> WrapPayload(std::span<const u8> payload) {
    if (payload.size() > MaximumFileBytes - 20) throw FormatError("save exceeds the size limit");
    Writer writer;
    writer.Raw(Magic);
    writer.Word(FormatVersion);
    writer.Word(static_cast<u32>(payload.size()));
    writer.Word(Crc32(payload));
    writer.Raw(payload);
    return std::move(writer.bytes);
}

std::span<const u8> UnwrapPayload(std::span<const u8> file) {
    Reader reader(file);
    const auto magic = reader.Raw(Magic.size());
    if (!std::equal(magic.begin(), magic.end(), Magic.begin()))
        throw FormatError("not a native save; convert legacy saves with homm2-import-save");
    if (reader.Word() != FormatVersion) throw FormatError("unsupported native save version");
    const u32 count = reader.Word();
    const u32 checksum = reader.Word();
    const auto payload = reader.Raw(count);
    reader.Finish();
    if (Crc32(payload) != checksum) throw FormatError("save checksum mismatch");
    return payload;
}

}
