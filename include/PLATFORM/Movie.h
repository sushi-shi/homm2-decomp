#ifndef HOMM2_PLATFORM_MOVIE_H
#define HOMM2_PLATFORM_MOVIE_H

#include <Ints.h>

#include "Types.h"

namespace platform {

using MovieId = i32;

// Zero is no movie, so a handle reads as false the way the game expects.
inline constexpr MovieId kInvalidMovie = 0;

// What a movie hands back when it has something to show. The palette is only
// set on the frames that change it, which is what the game waits for before
// repainting the screen.
struct MovieFrame {
    i32 index = 0;
    i32 count = 0;
    Rect dirty;
    const u8* palette = nullptr;
};

// The game plays two movies at once - a scene and its companion - so these
// take a handle rather than there being one current movie.
MovieId MovieOpen(const char* retailPath, bool withSound);
void MovieClose(MovieId movie);

// Where the next frame should land, and how wide the destination is.
void MovieTarget(MovieId movie, void* buffer, i32 pitch, i32 height, i32 left, i32 top);

// Draws the pending frame into that target. False when there is nothing new.
bool MovieDraw(MovieId movie, MovieFrame& frame);

// Steps to the next frame, and says whether one is still due.
void MovieAdvance(MovieId movie);
bool MovieWaiting(MovieId movie);
bool MovieAtEnd(MovieId movie);

// Where playback stands, for the game's own pacing decisions.
i32 MovieFrameIndex(MovieId movie);
i32 MovieFrameCount(MovieId movie);

// The palette the current frame is drawn with, 256 RGB triples.
const u8* MoviePalette(MovieId movie);

Size MovieSize(MovieId movie);

}

#endif
