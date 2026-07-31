#include <windows.h>

#include <set>
#include <string>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace platform::win32 {
namespace {

std::set<void*> gGdiObjects;

}

void TrackGdiObject(void* object) { gGdiObjects.insert(object); }

bool ForgetGdiObject(void* object) { return gGdiObjects.erase(object) != 0; }

std::string ResolvePath(const char* retailPath, FileMode mode) {
    return platform::Files().Resolve(retailPath, mode);
}

}
