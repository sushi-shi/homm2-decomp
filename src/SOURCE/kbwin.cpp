#include <Ints.h>
#include <SOURCE/kbwin.h>
#include <PLATFORM/Runtime.h>

i32l KBTickCount(void) {
    return platform::Ticks();
}
