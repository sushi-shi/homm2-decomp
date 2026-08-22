#include <va.h>
#include <BASE/MusicFlags.h>

DATA(0x0051f550) u8 gMusicFeatureEnabled = 1;

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

VA(0x004c57c0, 0xa)
u8 GetMusicFlagB(void) {
    return gMusicFlagB;
}
