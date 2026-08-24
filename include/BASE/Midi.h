#ifndef HOMM2_BASE_MIDI_H
#define HOMM2_BASE_MIDI_H

#include <Ints.h>


bool MIDIStartup(void);
void MIDIShutdown(void);
void MIDIPlay(i32& currentTrack, i32& fadeSteps, i32 track);
void MIDIStop(i32& currentTrack);
bool MIDIIsPlaying(void);
void MIDISetVolume(i32& fadeSteps);

#endif
