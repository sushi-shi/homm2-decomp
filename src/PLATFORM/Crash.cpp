#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#include <execinfo.h>
#include <fcntl.h>
#include <ucontext.h>
#include <unistd.h>
#endif

#include <PLATFORM/Platform.h>

namespace platform {
#if defined(__EMSCRIPTEN__) || defined(_WIN32)

void InstallCrashHandler(const char*) {}

#else
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

void WriteBacktrace(int fileDescriptor, int number, void* address, void* rawContext) {

    const char* name = SignalName(number);
    ::write(fileDescriptor, "\nhomm2 crashed: ", 16);
    ::write(fileDescriptor, name, std::strlen(name));
    ::write(fileDescriptor, "\n\n", 2);
    char fault[64];
    const int faultLength = std::snprintf(fault, sizeof(fault), "fault address: %p\n\n", address);
    ::write(fileDescriptor, fault, static_cast<std::size_t>(faultLength > 0 ? faultLength : 0));
#if defined(__i386__)
    ucontext_t* context = static_cast<ucontext_t*>(rawContext);
    char registers[160];
    const int registerLength = std::snprintf(
        registers,
        sizeof(registers),
        "EIP=%08lx EAX=%08lx EDX=%08lx EDI=%08lx\n\n",
        static_cast<unsigned long>(context->uc_mcontext.gregs[REG_EIP]),
        static_cast<unsigned long>(context->uc_mcontext.gregs[REG_EAX]),
        static_cast<unsigned long>(context->uc_mcontext.gregs[REG_EDX]),
        static_cast<unsigned long>(context->uc_mcontext.gregs[REG_EDI])
    );
    ::write(
        fileDescriptor,
        registers,
        static_cast<std::size_t>(registerLength > 0 ? registerLength : 0)
    );
#endif

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

void Handle(int number, siginfo_t* info, void* context) {
    void* address = info != nullptr ? info->si_addr : nullptr;
    WriteBacktrace(STDERR_FILENO, number, address, context);

    if (gReportPath[0] != '\0') {
        const int file = ::open(gReportPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (file >= 0) {
            WriteBacktrace(file, number, address, context);
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
        struct sigaction action {};
        action.sa_sigaction = Handle;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;
        sigaction(number, &action, nullptr);
    }
}

#endif
}
