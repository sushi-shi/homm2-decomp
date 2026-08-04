#ifndef HOMM2_BASE_MIDI_H
#define HOMM2_BASE_MIDI_H

#include <va.h>

// The MIDI backend mirrors the Audiere one in soundBackends.h: free functions
// that take the sound manager's music-track and fade-step members by
// reference instead of reading them off a manager instance.
bool MIDIStartup(void);
void MIDIShutdown(void);
void MIDIPlay(i32& currentTrack, i32& fadeSteps, i32 track);
void MIDIStop(i32& currentTrack);
bool MIDIIsPlaying(void);
void MIDISetVolume(i32& fadeSteps);
void MIDIPoll(void);

#endif
