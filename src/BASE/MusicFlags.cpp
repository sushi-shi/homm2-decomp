#include <va.h>
#include <BASE/MusicFlags.h>

// Buka-added music-path state: Midi, soundmgr, and ADVMGR read these
// through the getters below. The feature is not yet semantically named;
// these spellings are provisional reconstruction claims.
static u8 gMusicFlagA;
static u8 gMusicFlagB;
static u8 gMusicFeatureEnabled = 1;

VA(0x004c5770, 0xa)
u8 GetMusicFlagA(void) {
    return gMusicFlagA;
}

VA(0x004c5780, 0x32)
u8 MusicFlagsActive(void) {
    i32 active;
    if (gMusicFeatureEnabled && gMusicFlagB)
        active = 1;
    else
        active = 0;
    return active;
}
