#ifndef HOMM2_BASE_MUSICFLAGS_H
#define HOMM2_BASE_MUSICFLAGS_H

#include <Ints.h>

// Buka-added music-path state: Midi publishes these directly on startup and
// shutdown, so they are module globals rather than MusicFlags-private state.
// The feature is not yet semantically named; these spellings are provisional
// reconstruction claims.
extern u8 gMusicFlagA;
extern u8 gMusicFlagB;
extern u8 gMusicFeatureEnabled;

u8 GetMusicFlagA(void);
u8 GetMusicFlagB(void);
u8 MusicFlagsActive(void);

#endif
