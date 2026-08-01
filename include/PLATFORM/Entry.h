#ifndef HOMM2_PLATFORM_ENTRY_H
#define HOMM2_PLATFORM_ENTRY_H

#include <Ints.h>

namespace platform {

// What every executable does once its own platform is ready: bring the
// backend up, run the game, take it down again.
i32 Run(const char* commandLine);

}

#endif
