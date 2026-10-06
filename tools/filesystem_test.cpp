#include <PLATFORM/FileSystem.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace {

bool Expect(const std::string& actual, const char* expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(
        stderr,
        "filesystem mismatch: %s: got '%s', expected '%s'\n",
        description,
        actual.c_str(),
        expected
    );
    return false;
}

bool Expect(bool actual, bool expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(stderr, "filesystem mismatch: %s\n", description);
    return false;
}

class ChunkedFileSystem final : public platform::IFileSystem {
public:
    explicit ChunkedFileSystem(std::vector<unsigned char> input, i32 chunk)
        : m_input(std::move(input)), m_chunk(chunk) {}

    std::string DataRoot() const override { return {}; }
    std::string UserRoot() const override { return {}; }
    std::string ProgramRoot() const override { return {}; }
    std::string LocaleDataRoot() const override { return {}; }
    std::string Resolve(const char*, platform::FileMode) const override { return {}; }
    std::vector<std::string> List(const char*) const override { return {}; }
    bool Exists(const char*) const override { return false; }
    i32 Open(const char*, platform::FileMode) override { return 1; }
    i32 OpenLocale(const char*) override { return -1; }
    void Close(i32) override {}

    i32 Read(i32, void* buffer, i32 count) override {
        if (m_readOffset >= m_input.size()) {
            return 0;
        }
        const i32 available = static_cast<i32>(m_input.size() - m_readOffset);
        const i32 transferred = std::min({count, m_chunk, available});
        std::memcpy(buffer, m_input.data() + m_readOffset, static_cast<std::size_t>(transferred));
        m_readOffset += static_cast<std::size_t>(transferred);
        return transferred;
    }

    i32 Write(i32, const void* buffer, i32 count) override {
        const i32 transferred = std::min(count, m_chunk);
        const auto* bytes = static_cast<const unsigned char*>(buffer);
        m_output.insert(m_output.end(), bytes, bytes + transferred);
        return transferred;
    }

    i32 Seek(i32, i32) override { return -1; }
    i32 Tell(i32) override { return -1; }
    i32 Length(i32) override { return static_cast<i32>(m_input.size()); }

    const std::vector<unsigned char>& Output() const { return m_output; }

private:
    std::vector<unsigned char> m_input;
    std::vector<unsigned char> m_output;
    std::size_t m_readOffset = 0;
    i32 m_chunk;
};

#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

bool ExpectConfigured(const char* value, const char* expected, const char* description) {
    return Expect(platform::ConfiguredDirectory(value), expected, description);
}

bool CheckConfiguredDirectories() {
    bool valid = true;
    valid &= ExpectConfigured(nullptr, "", "unset directory");
    valid &= ExpectConfigured("", "", "empty directory");
    valid &= ExpectConfigured("  ", "", "blank directory");
    valid &= ExpectConfigured("\"\"", "", "quoted empty directory");
    valid &= ExpectConfigured("/games/homm2", "/games/homm2", "plain directory");
    valid &= ExpectConfigured("\"/games/homm 2\"", "/games/homm 2", "cmd keeps quotes in set");
    valid &= ExpectConfigured(" \"/games/homm2\" ", "/games/homm2", "blanks around quotes");
    valid &= ExpectConfigured("/games/homm2/", "/games/homm2", "trailing separator");
    valid &= ExpectConfigured("/", "/", "root survives");
    valid &= ExpectConfigured("\"/games/homm2", "\"/games/homm2", "unbalanced quote is kept");
#ifdef _WIN32
    valid &= ExpectConfigured(
        "\"C:\\Games\\Heroes of Might and Magic II\"",
        "C:\\Games\\Heroes of Might and Magic II",
        "quoted drive path"
    );
    valid &= ExpectConfigured("C:\\Games\\Heroes II\\", "C:\\Games\\Heroes II", "drive path trailing separator");
    valid &= ExpectConfigured("C:\\", "C:\\", "drive root survives");
#endif
    return valid;
}

bool CheckHostPaths() {
    bool valid = true;
    const std::string name = "Герои II/DATA";
    valid &= Expect(platform::HostString(platform::HostPath(name)) == name
            || platform::HostString(platform::HostPath(name)) == "Герои II\\DATA",
        true, "UTF-8 host path round trip");
    valid &= Expect(
        platform::HostPath("Герои").u8string() == u8"Герои",
        true,
        "UTF-8 host path is not read in the ANSI code page"
    );
    return valid;
}

}

int main() {
    bool valid = true;
    valid &= Expect(
        platform::ResolveIn("/game", ".\\GAMES\\AUTOSAVE.GIC"),
        "/game" SEPARATOR "GAMES" SEPARATOR "AUTOSAVE.GIC",
        "retail separators"
    );
    valid &= Expect(
        platform::ResolveIn("/game", "DATA/../MAPS/TEST.MP2"),
        "/game" SEPARATOR "MAPS" SEPARATOR "TEST.MP2",
        "parent component inside root"
    );
    valid &= Expect(
        platform::ResolveIn("/game", "../../outside"),
        "/game" SEPARATOR "outside",
        "parent component cannot escape root"
    );
    valid &= Expect(
        platform::ResolveIn("/game/", "C:\\DATA\\HEROES2.AGG"),
        "/game/DATA" SEPARATOR "HEROES2.AGG",
        "drive letter and trailing root separator"
    );
#ifdef _WIN32
    valid &= Expect(
        platform::ResolveIn("C:\\Games\\Heroes II\\", ".\\DATA\\heroes2x.agg"),
        "C:\\Games\\Heroes II\\DATA\\heroes2x.agg",
        "Windows root with a trailing backslash"
    );
#else
    valid &= Expect(platform::IsUserState("HEROES2.CFG"), true, "preferences are user state");
    valid &= Expect(
        platform::IsUserState(".\\GAMES\\AUTOSAVE.GIC"),
        true,
        "saved games are user state"
    );
    valid &= Expect(
        platform::IsUserState("DATA\\HEROES2.AGG"),
        false,
        "installation aggregate is not user state"
    );
    valid &= Expect(
        platform::IsUserState("DATA\\H2CABCD.NW"),
        false,
        "removed compressor scratch names are not user state"
    );
    valid &= Expect(
        platform::IsUserState("GAMES/../DATA/HEROES2.AGG"),
        false,
        "state classification normalizes parent components"
    );
#endif
    valid &= CheckConfiguredDirectories();
    valid &= CheckHostPaths();

    const std::string stamp =
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const std::filesystem::path caseRoot = std::filesystem::temp_directory_path()
        / platform::HostPath("homm2-filesystem-тест-" + stamp);
    const std::string caseText = platform::HostString(caseRoot);
    std::error_code fileError;
    std::filesystem::remove_all(caseRoot, fileError);
    std::filesystem::create_directories(caseRoot / "DATA", fileError);
    valid &= Expect(!fileError, true, "case fixture creation");
    std::ofstream(caseRoot / "DATA" / "HEROES2.AGG").put('x');

    valid &= Expect(platform::HoldsGameData(caseText), true, "uppercase installation holds game data");
    valid &= Expect(
        platform::FindGameData({"", caseText + SEPARATOR "missing", caseText}),
        caseText.c_str(),
        "first candidate holding game data wins"
    );
    valid &= Expect(
        platform::FindGameData({caseText + SEPARATOR "missing"}),
        "",
        "no candidate holds game data"
    );
    valid &= Expect(
        std::filesystem::exists(
            platform::HostPath(platform::ResolveIn(caseText, ".\\data\\heroes2.agg"))
        ),
        true,
        "lowercase request finds uppercase archive below a non-ASCII root"
    );

#ifndef _WIN32
    // Windows folders are case-insensitive, so a collision cannot be made there.
    std::filesystem::create_directories(caseRoot / "data", fileError);
    valid &= Expect(
        platform::ResolveIn(caseText, "DaTa/HEROES2.AGG"),
        "",
        "ambiguous case-folded path fails deterministically"
    );
    std::filesystem::remove_all(caseRoot / "data", fileError);
    valid &= Expect(
        platform::ResolveIn(caseText, "data/HEROES2.AGG"),
        (caseRoot / "DATA" / "HEROES2.AGG").string().c_str(),
        "single case-folded path uses existing spelling"
    );
    const std::filesystem::path blockedRoot = caseRoot / "blocked";
    std::ofstream(blockedRoot).put('x');
    valid &= Expect(
        platform::PrepareUserState(blockedRoot.string()),
        false,
        "unwritable state root reports failure"
    );
#endif
    std::filesystem::remove_all(caseRoot, fileError);

    ChunkedFileSystem chunked({1, 2, 3, 4, 5}, 2);
    unsigned char input[5] = {};
    valid &= Expect(platform::ReadExact(chunked, 1, input, 5), true, "chunked exact read");
    valid &= Expect(
        std::vector<unsigned char>(input, input + 5) == std::vector<unsigned char>({1, 2, 3, 4, 5}),
        true,
        "chunked read contents"
    );
    valid &= Expect(platform::ReadExact(chunked, 1, input, 1), false, "short read fails");

    const unsigned char output[] = {6, 7, 8, 9, 10};
    valid &= Expect(platform::WriteExact(chunked, 1, output, 5), true, "chunked exact write");
    valid &= Expect(
        chunked.Output() == std::vector<unsigned char>({6, 7, 8, 9, 10}),
        true,
        "chunked write contents"
    );
    valid &= Expect(platform::ReadExact(chunked, 1, nullptr, 1), false, "null exact read");
    valid &= Expect(platform::WriteExact(chunked, 1, nullptr, 1), false, "null exact write");
    valid &= Expect(platform::ReadExact(chunked, 1, nullptr, 0), true, "empty exact read");
    return valid ? 0 : 1;
}
