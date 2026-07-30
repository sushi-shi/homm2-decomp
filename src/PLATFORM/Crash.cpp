#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <PLATFORM/Platform.h>

namespace platform {
namespace {

char gReportPath[512] = {};

const char* SignalName(int number) {
    switch (number) {
    case SIGSEGV: return "SIGSEGV (invalid memory access)";
    case SIGBUS: return "SIGBUS (bad address)";
    case SIGFPE: return "SIGFPE (arithmetic fault)";
    case SIGILL: return "SIGILL (illegal instruction)";
    case SIGABRT: return "SIGABRT (abort)";
    default: return "unknown signal";
    }
}

void WriteBacktrace(int fileDescriptor, int number) {

    const char* name = SignalName(number);
    ::write(fileDescriptor, "\nhomm2 crashed: ", 16);
    ::write(fileDescriptor, name, std::strlen(name));
    ::write(fileDescriptor, "\n\n", 2);

    void* frames[64];
    const int count = ::backtrace(frames, 64);

    const int skip = count > 2 ? 2 : 0;
    ::backtrace_symbols_fd(frames + skip, count - skip, fileDescriptor);

    ::write(fileDescriptor, "\nraw addresses for addr2line -Cfie <binary>:\n", 44);
    for (int i = skip; i < count; ++i) {
        char text[32];
        const int length =
            std::snprintf(text, sizeof(text), "%p%s", frames[i], i + 1 < count ? " " : "\n");
        ::write(fileDescriptor, text, static_cast<std::size_t>(length > 0 ? length : 0));
    }
}

void Handle(int number) {
    WriteBacktrace(STDERR_FILENO, number);

    if (gReportPath[0] != '\0') {
        const int file = ::open(gReportPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (file >= 0) {
            WriteBacktrace(file, number);
            ::close(file);
            ::write(STDERR_FILENO, "\nreport written to ", 19);
            ::write(STDERR_FILENO, gReportPath, std::strlen(gReportPath));
            ::write(STDERR_FILENO, "\n", 1);
        }
    }

    std::signal(number, SIG_DFL);
    ::raise(number);
}

}

void InstallCrashHandler(const char* reportPath) {
    if (reportPath != nullptr) {
        std::strncpy(gReportPath, reportPath, sizeof(gReportPath) - 1);
    }
    for (const int number : {SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT}) {
        std::signal(number, Handle);
    }
}

}
