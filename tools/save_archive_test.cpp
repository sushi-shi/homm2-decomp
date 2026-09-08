#include <SAVE/Format.h>

#include <algorithm>
#include <array>
#include <cstdio>

namespace {

template <typename Function>
bool Rejects(Function function) {
    try { function(); } catch (const savegame::FormatError&) { return true; }
    return false;
}

}

int main() {
    savegame::Writer writer;
    writer.Fields(i8{-1}, u16{0x1234}, 1.0f);
    const std::array<u8, 12> expected = {
        0xff, 0xff, 0xff, 0xff, 0x34, 0x12, 0, 0, 0, 0, 0x80, 0x3f,
    };
    if (!std::equal(writer.bytes.begin(), writer.bytes.end(), expected.begin(), expected.end())) return 1;
    writer.Text("Александр");
    const std::vector<u8> file = savegame::WrapPayload(writer.bytes);
    savegame::Reader reader(savegame::UnwrapPayload(file));
    i16 signedValue{};
    u32 unsignedValue{};
    float realValue{};
    reader.Fields(signedValue, unsignedValue, realValue);
    std::string name;
    reader.Text(name);
    reader.Finish();
    if (signedValue != -1 || unsignedValue != 0x1234 || realValue != 1.0f || name != "Александр") return 2;

    for (std::size_t length = 0; length < file.size(); ++length) {
        if (!Rejects([&] { savegame::UnwrapPayload(std::span(file).first(length)); })) return 3;
    }
    auto broken = file;
    broken.back() ^= 1;
    if (!Rejects([&] { savegame::UnwrapPayload(broken); })) return 4;
    broken = file;
    broken[8] = 2;
    if (!Rejects([&] { savegame::UnwrapPayload(broken); })) return 5;
    broken = file;
    broken.push_back(0);
    if (!Rejects([&] { savegame::UnwrapPayload(broken); })) return 6;

    savegame::Writer tooLarge;
    tooLarge.Word(256);
    if (!Rejects([&] { savegame::Reader input(tooLarge.bytes); u8 value{}; input.Field(value); })) return 7;
    if (!Rejects([&] { savegame::Writer output; output.Text("\xff"); })) return 8;
    if (!Rejects([&] {
            savegame::Reader input(writer.bytes);
            input.Raw(12);
            char narrow[13];
            input.Text(narrow);
        })) return 9;
    std::puts("native archive encoding, UTF-8 and corruption checks passed");
    return 0;
}
