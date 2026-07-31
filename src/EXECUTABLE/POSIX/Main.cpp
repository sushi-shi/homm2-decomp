#include <PLATFORM/Entry.h>

#include <PLATFORM/Platform.h>

#include <string>

int main(int argc, char** argv) {
    std::string commandLine;
    for (int argument = 1; argument < argc; ++argument) {
        if (!commandLine.empty()) {
            commandLine.push_back(' ');
        }
        commandLine += argv[argument];
    }

    // The backend has to be up before the report path is known.
    if (!platform::Startup()) {
        return 1;
    }
    const std::string report = platform::Files().UserRoot() + "/crash.txt";
    platform::InstallCrashHandler(report.c_str());

    return platform::Run(commandLine.c_str());
}
