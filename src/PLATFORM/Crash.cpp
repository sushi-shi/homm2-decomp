#include <csignal>
#include <cstddef>
#include <cstdint>

#if !defined(__EMSCRIPTEN__) && !defined(_WIN32)
#include <cerrno>
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
std::size_t gReportPathLength = 0;

void WriteAll(int fileDescriptor, const char* data, std::size_t size) {
    while (size != 0) {
        const ssize_t written = ::write(fileDescriptor, data, size);
        if (written > 0) {
            data += written;
            size -= static_cast<std::size_t>(written);
        } else if (written < 0 && errno == EINTR) {
            continue;
        } else {
            break;
        }
    }
}

struct SignalDescription {
    const char* text;
    std::size_t size;
};

SignalDescription SignalName(int number) {
    switch (number) {
    case SIGSEGV: return {"SIGSEGV (invalid memory access)", 31};
    case SIGBUS: return {"SIGBUS (bad address)", 20};
    case SIGFPE: return {"SIGFPE (arithmetic fault)", 25};
    case SIGILL: return {"SIGILL (illegal instruction)", 28};
    case SIGABRT: return {"SIGABRT (abort)", 15};
    default: return {"unknown signal", 14};
    }
}

template <std::size_t Size>
void WriteLiteral(int fileDescriptor, const char (&text)[Size]) {
    WriteAll(fileDescriptor, text, Size - 1);
}

void WriteHex(int fileDescriptor, std::uintptr_t value) {
    char text[2 + sizeof(value) * 2];
    text[0] = '0';
    text[1] = 'x';
    for (std::size_t index = sizeof(value) * 2; index != 0; --index) {
        const unsigned digit = static_cast<unsigned>(value & 0xfU);
        text[index + 1] = static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10);
        value >>= 4;
    }
    WriteAll(fileDescriptor, text, sizeof(text));
}

void WriteReport(int fileDescriptor, int number, void* address, void* rawContext) {

    const SignalDescription name = SignalName(number);
    WriteLiteral(fileDescriptor, "\nhomm2 crashed: ");
    WriteAll(fileDescriptor, name.text, name.size);
    WriteLiteral(fileDescriptor, "\n\nfault address: ");
    WriteHex(fileDescriptor, reinterpret_cast<std::uintptr_t>(address));
    WriteLiteral(fileDescriptor, "\n");
#if defined(__i386__)
    const ucontext_t* context = static_cast<const ucontext_t*>(rawContext);
    if (context != nullptr) {
        WriteLiteral(fileDescriptor, "EIP=");
        WriteHex(fileDescriptor, static_cast<std::uintptr_t>(context->uc_mcontext.gregs[REG_EIP]));
        WriteLiteral(fileDescriptor, " EAX=");
        WriteHex(fileDescriptor, static_cast<std::uintptr_t>(context->uc_mcontext.gregs[REG_EAX]));
        WriteLiteral(fileDescriptor, " EDX=");
        WriteHex(fileDescriptor, static_cast<std::uintptr_t>(context->uc_mcontext.gregs[REG_EDX]));
        WriteLiteral(fileDescriptor, " EDI=");
        WriteHex(fileDescriptor, static_cast<std::uintptr_t>(context->uc_mcontext.gregs[REG_EDI]));
        WriteLiteral(fileDescriptor, "\n");
    }
#else
    static_cast<void>(rawContext);
#endif
}

void Handle(int number, siginfo_t* info, void* context) {
    void* address = info != nullptr ? info->si_addr : nullptr;
    WriteReport(STDERR_FILENO, number, address, context);

    if (gReportPath[0] != '\0') {
        const int file = ::open(gReportPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (file >= 0) {
            WriteReport(file, number, address, context);
            ::close(file);
            WriteLiteral(STDERR_FILENO, "report written to ");
            WriteAll(STDERR_FILENO, gReportPath, gReportPathLength);
            WriteLiteral(STDERR_FILENO, "\n");
        }
    }

    // SA_RESETHAND and SA_NODEFER make this second delivery take the default
    // action without calling libc diagnostics from a corrupted process.
    ::kill(::getpid(), number);
    _exit(128 + number);
}

}

void InstallCrashHandler(const char* reportPath) {
    gReportPathLength = 0;
    gReportPath[0] = '\0';
    if (reportPath != nullptr) {
        while (gReportPathLength + 1 < sizeof(gReportPath)
               && reportPath[gReportPathLength] != '\0') {
            gReportPath[gReportPathLength] = reportPath[gReportPathLength];
            ++gReportPathLength;
        }
        gReportPath[gReportPathLength] = '\0';
    }
    for (const int number : {SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT}) {
        struct sigaction action {};
        action.sa_sigaction = Handle;
        sigemptyset(&action.sa_mask);
        action.sa_flags = static_cast<int>(
            static_cast<unsigned int>(SA_SIGINFO)
            | static_cast<unsigned int>(SA_RESETHAND)
            | static_cast<unsigned int>(SA_NODEFER)
        );
        sigaction(number, &action, nullptr);
    }
}

#endif
}
