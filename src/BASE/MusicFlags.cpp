#include <Ints.h>
#include <BASE/MusicFlags.h>

u8 gMusicFeatureEnabled = 1;

u8 GetMusicFlagA(void) {
    return gMusicFlagA;
}

u8 MusicFlagsActive(void) {
    b32 active;
    if (gMusicFeatureEnabled && gMusicFlagB)
        active = true;
    else
        active = false;
    return active;
}

u8 GetMusicFlagB(void) {
    return gMusicFlagB;
}
