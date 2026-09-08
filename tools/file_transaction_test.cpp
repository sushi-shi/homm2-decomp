#include <PLATFORM/FileTransaction.h>
#include <PLATFORM/Platform.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;

namespace {

bool Expect(bool valid, const char* description) {
    if (!valid) std::fprintf(stderr, "file transaction mismatch: %s\n", description);
    return valid;
}

std::string Read(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

class Faults final : public platform::IFileSystem {
public:
    explicit Faults(platform::IFileSystem& files) : real(files) {}
    std::string DataRoot() const override { return real.DataRoot(); }
    std::string UserRoot() const override { return real.UserRoot(); }
    std::string ProgramRoot() const override { return real.ProgramRoot(); }
    std::string LocaleDataRoot() const override { return real.LocaleDataRoot(); }
    std::string Resolve(const char* path, platform::FileMode mode) const override {
        return real.Resolve(path, mode);
    }
    std::vector<std::string> List(const char* path) const override { return real.List(path); }
    bool Exists(const char* path) const override { return real.Exists(path); }
    i32 Open(const char* path, platform::FileMode mode) override { return real.Open(path, mode); }
    i32 OpenLocale(const char* path) override { return real.OpenLocale(path); }
    void Close(i32 file) override { real.Close(file); }
    bool CloseChecked(i32 file) override {
        const bool closed = real.CloseChecked(file);
        return !failClose && closed;
    }
    i32 CreateTemporarySibling(const std::string& path, std::string& temporary) override {
        if (failOpen) return -1;
        return real.CreateTemporarySibling(path, temporary);
    }
    bool Flush(i32 file) override { return !failFlush && real.Flush(file); }
    bool ReplaceFile(const std::string& temporary, const std::string& path) override {
        ++replacements;
        return !failReplace && real.ReplaceFile(temporary, path);
    }
    bool RemoveTemporary(const std::string& path) override { return real.RemoveTemporary(path); }
    i32 Read(i32 file, void* bytes, i32 count) override { return real.Read(file, bytes, count); }
    i32 Write(i32 file, const void* bytes, i32 count) override {
        if (written >= failAfter) return 0;
        const i32 result = real.Write(file, bytes, std::min({count, chunk, failAfter - written}));
        if (result > 0) written += result;
        return result;
    }
    i32 Seek(i32 file, i32 offset) override { return real.Seek(file, offset); }
    i32 Tell(i32 file) override { return real.Tell(file); }
    i32 Length(i32 file) override { return real.Length(file); }

    platform::IFileSystem& real;
    bool failOpen = false, failFlush = false, failClose = false, failReplace = false;
    i32 failAfter = 1000000, chunk = 7, written = 0;
    i32 replacements = 0;
};

bool NoTemporaryFiles(const std::filesystem::path& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().filename().string().starts_with(".homm2-save-")) return false;
    }
    return true;
}

bool Rejections(const std::filesystem::path& destination) {
    const std::string original = "previous complete save";
    std::ofstream(destination, std::ios::binary) << original;
    const std::string payload(1024, 'S');
    bool valid = true;
    for (i32 position : {0, 1, 7, 31, 63, 64, 255, 511, 1023}) {
        Faults faults(platform::Files());
        faults.failAfter = position;
        {
            platform::FileTransaction transaction(faults, "GAMES/CASE.GM1");
            valid &= Expect(transaction.Handle() >= 0, "temporary creation");
            valid &= Expect(!transaction.Write(payload.data(), static_cast<i32>(payload.size())),
                            "injected partial/zero-progress write fails");
            valid &= Expect(Read(destination) == original, "old save unchanged during failed write");
            valid &= Expect(!transaction.Commit(), "failed writer cannot commit");
        }
        valid &= Expect(faults.replacements == 0, "failed writes never reach replacement");
        valid &= Expect(NoTemporaryFiles(destination.parent_path()), "failed write cleanup");
    }
    for (int failure = 0; failure < 4; ++failure) {
        Faults faults(platform::Files());
        faults.failOpen = failure == 0;
        faults.failFlush = failure == 1;
        faults.failClose = failure == 2;
        faults.failReplace = failure == 3;
        {
            platform::FileTransaction transaction(faults, "GAMES/CASE.GM1");
            if (!faults.failOpen)
                valid &= Expect(transaction.Write(payload.data(), static_cast<i32>(payload.size())),
                                "complete temporary before commit failure");
            valid &= Expect(!transaction.Commit(), "injected create/flush/close/rename failure");
        }
        valid &= Expect(Read(destination) == original, "commit failure preserves old save");
        valid &= Expect(NoTemporaryFiles(destination.parent_path()), "commit failure cleanup");
        valid &= Expect(faults.replacements == (faults.failReplace ? 1 : 0), "commit stage ordering");
    }
    {
        platform::FileTransaction abandoned(platform::Files(), "GAMES/CASE.GM1");
        valid &= Expect(abandoned.Write(payload.data(), 12), "abandoned partial write");
    }
    valid &= Expect(Read(destination) == original && NoTemporaryFiles(destination.parent_path()),
                    "uncommitted destructor preserves old save and removes temporary");
    return valid;
}

bool Replacements(const std::filesystem::path& destination) {
    bool valid = true;
    const std::string original = Read(destination);
    Faults faults(platform::Files());
    const std::string payload = "new complete save with chunked writes";
    platform::FileTransaction first(faults, "GAMES/CASE.GM1");
    platform::FileTransaction second(platform::Files(), "GAMES/CASE.GM1");
    valid &= Expect(first.Handle() >= 0 && second.Handle() >= 0 && first.Handle() != second.Handle(),
                    "concurrent transactions get distinct exclusive files");
    valid &= Expect(first.Write(payload.data(), 10), "first record");
    valid &= Expect(first.Write(payload.data() + 10, static_cast<i32>(payload.size()) - 10), "second record");
    valid &= Expect(Read(destination) == original, "old save remains visible until commit");
    valid &= Expect(first.Commit(), "successful checked commit");
    valid &= Expect(!first.Commit(), "transaction commits only once");
    valid &= Expect(Read(destination) == payload, "complete new save replaces old contents");
    valid &= Expect(!std::filesystem::exists(destination.parent_path() / "CASE.GM1"),
                    "preserves existing case-resolved destination spelling");
    const std::string secondPayload = "another complete save";
    valid &= Expect(second.Write(secondPayload.data(), static_cast<i32>(secondPayload.size())) && second.Commit(),
                    "second transaction can replace the completed first save");
    valid &= Expect(Read(destination) == secondPayload && NoTemporaryFiles(destination.parent_path()),
                    "successful commits leave no temporary files");
    const i32 file = platform::FileOpen("GAMES/CASE.GM1", platform::FileMode::Read);
    std::string loaded(secondPayload.size(), '\0');
    valid &= Expect(file >= 0 && platform::FileReadExact(file, loaded.data(), static_cast<i32>(loaded.size())),
                    "load committed file through production filesystem");
    platform::FileClose(file);
    valid &= Expect(loaded == secondPayload, "filesystem save/read round trip");
    platform::FileTransaction fresh(platform::Files(), "GAMES/new-\xd0\xaf.GM1");
    valid &= Expect(fresh.Write(payload.data(), static_cast<i32>(payload.size())) && fresh.Commit(),
                    "first save with UTF-8 filename");
    const auto freshPath = platform::Files().Resolve("GAMES/new-\xd0\xaf.GM1", platform::FileMode::Read);
    valid &= Expect(Read(freshPath) == payload, "UTF-8 filename round trip");
    return valid;
}

bool Interrupted(const char* executable, const std::filesystem::path& destination) {
    bool valid = true;
    const std::string previous = Read(destination);
    for (const char* position : {"1", "127", "1024"}) {
        std::vector<char*> arguments{const_cast<char*>(executable),
            const_cast<char*>("--interrupt"), const_cast<char*>(position), nullptr};
        pid_t child;
        const int spawned = ::posix_spawnp(&child, executable, nullptr, nullptr,
                                           arguments.data(), environ);
        int status = 0;
        valid &= Expect(spawned == 0 && ::waitpid(child, &status, 0) == child
                        && WIFEXITED(status) && WEXITSTATUS(status) == 0,
                        "interrupted writer subprocess");
        valid &= Expect(Read(destination) == previous, "process exit before commit preserves old save");
    }
    return valid;
}

}

int main(int argc, char** argv) {
    if (argc == 3 && std::string(argv[1]) == "--interrupt") {
        if (!platform::Startup()) return 1;
        platform::FileTransaction transaction(platform::Files(), "GAMES/CASE.GM1");
        const std::string payload(1024, 'X');
        const i32 count = std::atoi(argv[2]);
        if (count < 0 || count > 1024 || !transaction.Write(payload.data(), count)
            || !platform::Files().Flush(transaction.Handle())) return 1;
        std::_Exit(0); // Deliberately bypass close, destructors and commit.
    }
    const auto root = std::filesystem::temp_directory_path()
        / ("homm2-file-transaction-test-" + std::to_string(::getpid()));
    std::filesystem::create_directories(root / "DATA");
    if (setenv("HOMM2_DATA", root.c_str(), 1) != 0
        || setenv("XDG_DATA_HOME", (root / "state").c_str(), 1) != 0
        || !platform::Startup()) return 1;
    const auto destination = std::filesystem::path(platform::Files().UserRoot()) / "GAMES/case.gm1";
    bool valid = Rejections(destination);
    valid &= Replacements(destination);
    valid &= Interrupted(argv[0], destination);
    platform::Shutdown();
    std::filesystem::remove_all(root);
    return valid ? 0 : 1;
}
