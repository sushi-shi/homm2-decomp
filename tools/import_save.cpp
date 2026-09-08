#include <IMPORT/LegacySave.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <optional>

namespace {

void Usage() {
    std::fprintf(stderr,
        "Usage: homm2-import-save --encoding windows-1251|windows-1252|utf-8 INPUT OUTPUT.h2s\n"
        "Imports a retail save without changing the original file.\n"
        "Legacy saves do not identify their text encoding; choose the original game's code page.\n");
}

int Fail(const std::string& error) {
    std::fprintf(stderr, "Import failed: %s\n", error.c_str());
    return 1;
}

}

#ifdef _WIN32
int wmain(int argc, wchar_t** argv) {
    const auto argumentAt = [&](int index) {
        const auto utf8 = std::filesystem::path(argv[index]).u8string();
        return std::string(utf8.begin(), utf8.end());
    };
#else
int main(int argc, char** argv) {
    const auto argumentAt = [&](int index) { return std::string(argv[index]); };
#endif
    std::optional<localization::TextEncoding> encoding;
    std::vector<std::filesystem::path> paths;
    for (int i = 1; i < argc; ++i) {
        const std::string argument = argumentAt(i);
        if (argument == "--help" || argument == "-h") { Usage(); return 0; }
        if (argument == "--encoding" && i + 1 < argc) {
            const std::string value = argumentAt(++i);
            if (value == "windows-1251") encoding = localization::TextEncoding::Windows1251;
            else if (value == "windows-1252") encoding = localization::TextEncoding::Windows1252;
            else if (value == "utf-8") encoding = localization::TextEncoding::Utf8;
            else return Fail("unknown text encoding");
        } else if (argument.starts_with('-')) {
            Usage();
            return 1;
        } else {
            paths.emplace_back(std::u8string(argument.begin(), argument.end()));
        }
    }
    if (!encoding || paths.size() != 2) { Usage(); return 1; }
    std::error_code fileError;
    const auto size = std::filesystem::file_size(paths[0], fileError);
    if (fileError) return Fail("cannot read the input file");
    if (size > savegame::MaximumFileBytes) return Fail("input exceeds the save size limit");
    std::ifstream input(paths[0], std::ios::binary);
    std::vector<u8> bytes(static_cast<std::size_t>(size));
    if (!input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
        return Fail("cannot read the complete input file");
    input.close();
    savegame::Snapshot state;
    std::string error;
    if (!legacy_save::Decode(bytes, *encoding, state, error)) return Fail(error);
    std::vector<u8> native;
    if (!savegame::Encode(state, native, error)) return Fail(error);

    if (std::filesystem::exists(paths[1], fileError) || fileError)
        return Fail("output already exists or cannot be inspected");
    auto temporary = paths[1];
    temporary += ".importing";
    if (std::filesystem::exists(temporary, fileError) || fileError)
        return Fail("temporary output already exists or cannot be inspected");
    std::ofstream output(temporary, std::ios::binary);
    if (!output) return Fail("cannot create output beside the destination");
    output.write(reinterpret_cast<const char*>(native.data()), static_cast<std::streamsize>(native.size()));
    output.close();
    if (!output) {
        std::filesystem::remove(temporary, fileError);
        return Fail("cannot write the complete output file");
    }
    std::filesystem::rename(temporary, paths[1], fileError);
    if (fileError) {
        std::filesystem::remove(temporary, fileError);
        return Fail("cannot install the output file");
    }
    std::puts("Imported native save. The original file is unchanged.");
    return 0;
}
