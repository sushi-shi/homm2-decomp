#ifndef HOMM2_PLATFORM_HOST_H
#define HOMM2_PLATFORM_HOST_H

#include "Types.h"

namespace platform {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
};

class IHost {
public:
    virtual ~IHost() = default;

    virtual std::uint32_t Ticks() const = 0;

    virtual void Sleep(std::uint32_t milliseconds) = 0;

    virtual void Yield() = 0;

    virtual bool ShouldQuit() const = 0;
    virtual void RequestQuit() = 0;

    virtual void Log(LogLevel level, const char* message) = 0;

    virtual void ShowMessage(const char* title, const char* message) = 0;
};

}

#endif
