#include <PLATFORM/Platform.h>

#include <csignal>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

int main() {
    const std::filesystem::path report = std::filesystem::temp_directory_path()
        / ("homm2-crash-handler-" + std::to_string(::getpid()) + ".txt");
    std::error_code error;
    std::filesystem::remove(report, error);

    const pid_t child = ::fork();
    if (child == 0) {
        platform::InstallCrashHandler(report.c_str());
        ::raise(SIGABRT);
        _exit(1);
    }
    if (child < 0) {
        return 1;
    }

    int status = 0;
    if (::waitpid(child, &status, 0) != child
        || !WIFSIGNALED(status)
        || WTERMSIG(status) != SIGABRT) {
        return 1;
    }

    std::ifstream input(report);
    const std::string contents {
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };
    std::filesystem::remove(report, error);
    return contents.find("SIGABRT (abort)") != std::string::npos
            && contents.find("fault address: 0x") != std::string::npos
        ? 0
        : 1;
}
