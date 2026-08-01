#include <PLATFORM/Entry.h>

#include <stdlib.h>
#include <string.h>

#include <Ints.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/wingraph.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>

namespace {

enum CommandLineConstant {
    // What retail took from the command line, short of the buffer it kept.
    COMMAND_LINE_LIMIT = 0x3c
};

// The window, the game, and everything the game wants standing before it runs.
i32 RunGame(const char* commandLine) {
    memset(gcCommandLine, 0, sizeof(gcCommandLine));
    strncpy(gcCommandLine, commandLine, COMMAND_LINE_LIMIT);
    if (EarlySetup() == 0)
        return 0;

    srand(static_cast<u32>(platform::Ticks()));
    InitGraphics();

    oldmain();

    ShutDown(NULL);
    return 0;
}

}

namespace platform {

i32 Run(const char* commandLine) {
    if (!Startup()) {
        return 1;
    }

    const i32 result = RunGame(commandLine != nullptr ? commandLine : "");

    Shutdown();
    return result;
}

}
