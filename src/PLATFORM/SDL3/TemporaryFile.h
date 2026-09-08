#ifndef HOMM2_PLATFORM_SDL3_TEMPORARYFILE_H
#define HOMM2_PLATFORM_SDL3_TEMPORARYFILE_H

#include <SDL3/SDL.h>
#include <string>

namespace platform::sdl3 {

// Returns a write-only SDL stream owning the exclusively created native file.
SDL_IOStream* OpenTemporarySibling(const std::string& destination, std::string& temporary);
bool FlushFileData(SDL_IOStream* stream);
bool RemoveTemporaryFile(const std::string& temporary);
bool ReplaceTemporaryFile(const std::string& temporary, const std::string& destination);

}

#endif
