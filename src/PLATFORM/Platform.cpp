#include <PLATFORM/Platform.h>

#include <cstdio>
#include <cstdlib>

namespace platform {
namespace {

Backend* gBackend = nullptr;
bool gCleanupRegistered = false;

void CleanupAtExit() {
    Shutdown();
}

}

bool Startup() {
    if (gBackend != nullptr) {
        return true;
    }
    gBackend = CreateBackend();
    if (gBackend != nullptr && !gCleanupRegistered) {
        if (std::atexit(CleanupAtExit) != 0) {
            std::fprintf(stderr, "[homm2] unable to register platform cleanup\n");
        } else {
            gCleanupRegistered = true;
        }
    }
    return gBackend != nullptr;
}

void Shutdown() {
    if (gBackend == nullptr) {
        return;
    }
    DestroyBackend(gBackend);
    gBackend = nullptr;
}

Backend& Get() {

    if (gBackend == nullptr && !Startup()) {
        std::fprintf(stderr, "[homm2] platform failed to start\n");
        std::abort();
    }
    return *gBackend;
}

}
