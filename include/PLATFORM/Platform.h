#ifndef HOMM2_PLATFORM_PLATFORM_H
#define HOMM2_PLATFORM_PLATFORM_H

#include "Audio.h"
#include "FileSystem.h"
#include "Host.h"
#include "Input.h"
#include "Types.h"
#include "Video.h"

namespace platform {

struct Backend {
    IVideo* video = nullptr;
    IInput* input = nullptr;
    IAudio* audio = nullptr;
    IHost* host = nullptr;
    IFileSystem* files = nullptr;
};

void InstallCrashHandler(const char* reportPath);

Backend* CreateBackend();
void DestroyBackend(Backend* backend);

bool Startup();
void Shutdown();
Backend& Get();

inline IVideo& Video() { return *Get().video; }
inline IInput& Input() { return *Get().input; }
inline IAudio& Audio() { return *Get().audio; }
inline IHost& Host() { return *Get().host; }
inline IFileSystem& Files() { return *Get().files; }

}

#endif
