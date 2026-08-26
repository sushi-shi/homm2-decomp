#include <SDL3/SDL.h>

#include <cstdio>
#include <memory>

#include <PLATFORM/Platform.h>

#include "Sdl3Internal.h"
#include "Sdl3Video.h"

namespace platform {
namespace {

struct Sdl3Backend {
    sdl3::Video video;
    sdl3::InputHostServices inputHost = sdl3::CreateInputHost(video);
    std::unique_ptr<IAudio> audio = sdl3::CreateAudio();
    std::unique_ptr<IFileSystem> files = sdl3::CreateFileSystem();
    Backend facade;

    Sdl3Backend() {
        facade.video = &video;
        facade.input = inputHost.input.get();
        facade.audio = audio.get();
        facade.host = inputHost.host.get();
        facade.files = files.get();
    }
};

Sdl3Backend* gConcrete = nullptr;

}

Backend* CreateBackend() {
    if (gConcrete != nullptr) {
        return &gConcrete->facade;
    }
    if (!SDL_Init(0)) {
        std::fprintf(stderr, "[homm2] SDL_Init: %s\n", SDL_GetError());
        return nullptr;
    }

    gConcrete = new Sdl3Backend();
    return &gConcrete->facade;
}

void DestroyBackend(Backend* backend) {
    if (backend == nullptr || gConcrete == nullptr || backend != &gConcrete->facade) {
        return;
    }
    sdl3::CloseMovies();
    delete gConcrete;
    gConcrete = nullptr;
    SDL_Quit();
}

}
