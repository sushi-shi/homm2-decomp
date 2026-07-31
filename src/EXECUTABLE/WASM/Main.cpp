#include <PLATFORM/Entry.h>

// The page mounts IDBFS on /storage and syncs it in before the runtime starts
// the program, so there is nothing to set up here. Sleeps yield to the browser
// through ASYNCIFY, which is why the game can keep its own loop.
int main() {
    return platform::Run("");
}
