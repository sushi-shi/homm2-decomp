#include <match.h>
#include <BASE/MusicFlags.h>

DATA(0x0051f550) u8 gMidiOpenFailed = 1;

VA(0x004c5770, 0xa)
u8 MidiReady(void) {
    return gMidiReady;
}

VA(0x004c5780, 0x32)
u8 MidiUnavailable(void) {
    b32 active;
    if (gMidiOpenFailed && gMidiStarted)
        active = true;
    else
        active = false;
    return active;
}

VA(0x004c57c0, 0xa)
u8 MidiStarted(void) {
    return gMidiStarted;
}
