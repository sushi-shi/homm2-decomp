#ifndef HOMM2_BASE_MUSICFLAGS_H
#define HOMM2_BASE_MUSICFLAGS_H

#include <H2/Ints.h>


extern u8 gMidiReady;
extern u8 gMidiStarted;
extern u8 gMidiOpenFailed;

u8 MidiReady(void);
u8 MidiStarted(void);
u8 MidiUnavailable(void);

#endif
