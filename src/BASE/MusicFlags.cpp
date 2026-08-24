#include <Ints.h>
#include <BASE/MusicFlags.h>

u8 gMusicFeatureEnabled = 1;

u8 GetMusicFlagA(void) {
    return gMusicFlagA;
}

u8 MusicFlagsActive(void) {
    i32 active;
    if (gMusicFeatureEnabled && gMusicFlagB)
        active = 1;
    else
        active = 0;
    return active;
}

u8 GetMusicFlagB(void) {
    return gMusicFlagB;
}
