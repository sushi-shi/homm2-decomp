#include <PLATFORM/Entry.h>

#include <PLATFORM/Platform.h>

namespace platform {

i32 Run(const char* commandLine) {
    if (!Startup()) {
        return 1;
    }

    const i32 result = RunApplication(commandLine != nullptr ? commandLine : "");

    Shutdown();
    return result;
}

}
