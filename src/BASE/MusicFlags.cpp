#include <H2/Ints.h>
#include <BASE/MusicFlags.h>

u8 gMidiOpenFailed = 1;

u8 MidiReady(void) {
    return gMidiReady;
}

u8 MidiUnavailable(void) {
    b32 active;
    if (gMidiOpenFailed && gMidiStarted)
        active = true;
    else
        active = false;
    return active;
}

u8 MidiStarted(void) {
    return gMidiStarted;
}
