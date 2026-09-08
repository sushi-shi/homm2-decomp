#include <BASE/AggDirectory.h>
#include <PLATFORM/Binary.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace {

class MemoryFiles final : public platform::IFileSystem {
public:
    std::vector<u8> bytes;
    i32 position = 0;
    i32 chunk = 1;
    i32 visibleSize = -1;

    std::string DataRoot() const override { return {}; }
    std::string UserRoot() const override { return {}; }
    std::string ProgramRoot() const override { return {}; }
    std::string LocaleDataRoot() const override { return {}; }
    std::string Resolve(const char*, platform::FileMode) const override { return {}; }
    std::vector<std::string> List(const char*) const override { return {}; }
    bool Exists(const char*) const override { return false; }
    i32 Open(const char*, platform::FileMode) override { return 0; }
    i32 OpenLocale(const char*) override { return -1; }
    void Close(i32) override {}
    i32 Write(i32, const void*, i32) override { return -1; }
    i32 Tell(i32) override { return position; }
    i32 Length(i32) override { return static_cast<i32>(bytes.size()); }
    i32 Seek(i32, i32 offset) override {
        if (offset < 0 || offset > Length(0))
            return -1;
        position = offset;
        return position;
    }
    i32 Read(i32, void* destination, i32 count) override {
        const i32 length = visibleSize >= 0 ? visibleSize : Length(0);
        const i32 available = std::max<i32>(0, length - position);
        const i32 transferred = std::min({count, available, chunk});
        if (transferred > 0) {
            std::memcpy(destination, bytes.data() + position, static_cast<std::size_t>(transferred));
            position += transferred;
        }
        return transferred;
    }
};

bool Expect(bool valid, const char* description) {
    if (!valid)
        std::fprintf(stderr, "AGG directory mismatch: %s\n", description);
    return valid;
}

MemoryFiles Archive() {
    MemoryFiles file;
    file.bytes.resize(64);
    file.bytes[0] = 2;
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 2, 0x1234abcd);
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 6, 26);
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 10, 4);
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 14, 0xff876543);
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 18, 30);
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 22, 4);
    std::memcpy(file.bytes.data() + 26, "ABCDEFGH", 8);
    return file;
}

}

int main(int argc, char** argv) {
    std::vector<resources::AggEntry> entries;
    std::string error;
    if (argc > 1) {
        for (int index = 1; index < argc; ++index) {
            std::ifstream input(argv[index], std::ios::binary);
            MemoryFiles file;
            file.bytes.assign(std::istreambuf_iterator<char>(input), {});
            file.chunk = 4096;
            if (!input || !resources::ReadAggDirectory(file, 0, entries, error)) {
                std::fprintf(stderr, "%s: %s\n", argv[index], error.c_str());
                return 1;
            }
            std::printf("%s: %zu members validated\n", argv[index], entries.size());
        }
        return 0;
    }

    bool valid = true;
    MemoryFiles file = Archive();
    valid &= Expect(resources::ReadAggDirectory(file, 0, entries, error) && entries.size() == 2,
                    "complete directory through one-byte reads");
    if (entries.size() != 2)
        return 1;
    valid &= Expect(entries[0].id == 0x1234abcd && entries[0].offset == 26 && entries[0].size == 4
        && entries[1].id == 0xff876543 && entries[1].offset == 30 && entries[1].size == 4,
        "little-endian metadata");
    const auto first = entries[0];
    std::array<char, 5> output{'?', '?', '?', '?', '?'};
    file.Seek(0, first.offset);
    valid &= Expect(!resources::ReadAggMember(file, 0, first, output.data(), 5)
        && file.position == first.offset && output[0] == '?', "reject crossing into next member");
    valid &= Expect(resources::ReadAggMember(file, 0, first, output.data(), 4)
        && std::memcmp(output.data(), "ABCD?", 5) == 0, "complete member payload");
    valid &= Expect(!resources::ReadAggMember(file, 0, first, output.data(), 1), "end of member");
    valid &= Expect(resources::ReadAggMember(file, 0, first, nullptr, 0), "empty read at end");
    file.Seek(0, first.offset - 1);
    valid &= Expect(!resources::ReadAggMember(file, 0, first, output.data(), 1), "before member");
    file.Seek(0, first.offset + 2);
    valid &= Expect(resources::ReadAggMember(file, 0, first, output.data(), 2)
        && output[0] == 'C' && output[1] == 'D', "partial member at restored offset");
    file.Seek(0, first.offset);
    valid &= Expect(!resources::ReadAggMember(file, 0, first, output.data(), 0xffffffff),
                    "oversized read");
    file.visibleSize = first.offset + 2;
    valid &= Expect(!resources::ReadAggMember(file, 0, first, output.data(), 4), "short payload");

    const auto reject = [&](MemoryFiles& malformed, const char* description) {
        const auto previous = entries;
        const bool rejected = !resources::ReadAggDirectory(malformed, 0, entries, error);
        return Expect(rejected && !error.empty() && entries.size() == previous.size()
            && entries[0].id == previous[0].id, description);
    };
    file = Archive(); file.bytes.resize(1);
    valid &= reject(file, "short count");
    file = Archive(); file.bytes[0] = 0;
    valid &= reject(file, "zero count");
    file = Archive(); file.bytes[0] = 255; file.bytes[1] = 255;
    valid &= reject(file, "count exceeds file");
    file = Archive(); file.visibleSize = 13;
    valid &= reject(file, "short directory transfer");
    file = Archive(); file.bytes.resize(30);
    valid &= reject(file, "missing filename table");
    file = Archive();
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 6, 1);
    valid &= reject(file, "member points into directory");
    file = Archive();
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 10, 9);
    valid &= reject(file, "member points into filename trailer");
    file = Archive();
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 10, 0xffffffff);
    valid &= reject(file, "member length overflow");
    file = Archive();
    platform::binary::WriteU32(file.bytes.data(), file.bytes.size(), 6, 0xffffffff);
    valid &= reject(file, "member offset overflow");
    return valid ? 0 : 1;
}
