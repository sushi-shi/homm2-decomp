#include <windows.h>

#include <string>

#include <PLATFORM/WIN32/Application.h>

#include <PLATFORM/Platform.h>

#include "State.h"

int main(int argc, char** argv) {

    std::string commandLine;
    for (int i = 1; i < argc; ++i) {
        if (!commandLine.empty()) {
            commandLine.push_back(' ');
        }
        commandLine += argv[i];
    }

    if (!platform::Startup()) {
        return 1;
    }

    const std::string report = platform::Files().UserRoot() + "/crash.txt";
    platform::InstallCrashHandler(report.c_str());

    const int result = WinMain(nullptr, nullptr, commandLine.data(), SW_SHOWNORMAL);

    platform::Shutdown();
    return result;
}
