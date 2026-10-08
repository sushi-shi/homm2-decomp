#ifndef HOMM2_BASE_MUSICFLAGS_H
#define HOMM2_BASE_MUSICFLAGS_H

#include <H2/Ints.h>

// Buka-added MIDI driver state: Midi publishes these directly on startup and
// shutdown, so they are module globals rather than MusicFlags-private state.
// gMidiReady: AIL_midiOutOpen succeeded; gMidiStarted: MIDIStartup has run;
// gMidiOpenFailed: the open failed. MidiUnavailable() is started and failed,
// which makes MIDIStartup skip retrying and the sound manager fall back.
extern u8 gMidiReady;
extern u8 gMidiStarted;
extern u8 gMidiOpenFailed;

u8 MidiReady(void);
u8 MidiStarted(void);
u8 MidiUnavailable(void);

#endif // HOMM2_BASE_MUSICFLAGS_H
