#ifndef HOMM2_BASE_SOUNDMANAGER_H
#define HOMM2_BASE_SOUNDMANAGER_H

#include <va.h>
#include <stdio.h>
#include "baseManager.h"

H2_ENUM_BEGIN(MidiTrackConstant)
    MIDI_NO_TRACK    = -1,
    MIDI_TRACK_COUNT = 60
H2_ENUM_END(MidiTrackConstant)

H2_ENUM_BEGIN(SoundStorageConstant)
    SOUND_CHANNEL_VOLUME_CAPACITY = 0x14,
    DIGITAL_DRIVER_NAME_COUNT     = 14,
    SOUND_CHANNEL_TYPE_COUNT      = 4
H2_ENUM_END(SoundStorageConstant)

H2_ENUM_CLASS_BEGIN(SoundVolumeConversionMode)
    SOUND_VOLUME_EFFECT = 100,
    SOUND_VOLUME_MUSIC  = 101
H2_ENUM_CLASS_END(SoundVolumeConversionMode)

H2_ENUM_CLASS_BEGIN_T(SoundSampleOperation, i16)
    SOUND_SAMPLE_OPERATION_VOLUME        = 1,
    SOUND_SAMPLE_OPERATION_START         = 5,
    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME = 100,
    SOUND_SAMPLE_OPERATION_MUSIC_VOLUME  = 101
H2_ENUM_CLASS_END_T(SoundSampleOperation, i16)

H2_ENUM_CLASS_BEGIN_T(SoundDigitalReportQuery, i16)
    SOUND_DIGITAL_REPORT_VOLUME  = 1,
    SOUND_DIGITAL_REPORT_PLAYING = 4
H2_ENUM_CLASS_END_T(SoundDigitalReportQuery, i16)

class sample;
struct _SAMPLE;
struct _DIG_DRIVER;
struct tag_message;
#include <audiere.h>

#pragma pack(push, 1)
struct SampleChannelStruct {
    i32 startChannel;
    i32 endChannel;
    i32 currentChannel;
};

H2_ENUM_BEGIN(SoundBackendKind)
    SOUND_BACKEND_AUDIERE = 0,
    SOUND_BACKEND_MILES   = 1,
    SOUND_BACKEND_NONE    = 2
H2_ENUM_END(SoundBackendKind)

class soundManager : public baseManager {
public:
    SoundBackendKind m_backend;
    SoundBackendKind m_savedBackend;
    struct _DIG_DRIVER* m_digitalDriver;
    audiere::AudioDevicePtr m_audiereDevice;
    i32 m_musicFadeTargetTrack;
    i32 m_musicFadeSteps;
    i32 m_musicTrack;
    soundManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message&) OVERRIDE;
    bool CDStartup(void);
    void ShutdownSoundBackends(void);
    bool StartupMilesBackend(void);
    void SaveBackend(void);
    void RestoreBackend(void);
    i32 ConvertVolume(i32, SoundVolumeConversionMode);
    float ConvertVolumeFloat(i32, SoundVolumeConversionMode);
    void StopAllSamples(i32);
    void StopSample(class sample*);
    void ModifySample(class sample*, i32);
    bool DigitalReport(class sample*);
    void AdjustSoundVolumes(void);
    void AdjustMusicVolumes(void);
    void SetMusicQuality(i32);
    void PlayAmbientMusic(i32);
    void PollSound(void);
    void SwitchAmbientMusic(i32);
    void MemorySample(class sample*);
    void ServiceSound(void);
    i32 MusicPlaying(void);
};
#pragma pack(pop)
SIZE(soundManager, 0x52);

// Set when the CD-ROM check turns the audio path off; the two backend
// startups refuse to run while it is set. Distinct from KB's gbNoSound.
extern bool gSoundDisabled;

// Set once a backend has come up and cleared on shutdown; the adventure and
// game layers save/restore it around forced ambient-music switches, so it is
// module state rather than a soundManager member.
extern bool gSoundBackendsReady;

inline bool IsAudiereBackend(const soundManager* manager) {
    return manager->m_backend == SOUND_BACKEND_AUDIERE && manager->m_audiereDevice != NULL;
}

inline bool IsMilesBackend(const soundManager* manager) {
    return manager->m_backend == SOUND_BACKEND_MILES && manager->m_digitalDriver != NULL;
}

inline bool IsSoundBackendActive(const soundManager* manager) {
    return IsAudiereBackend(manager) || IsMilesBackend(manager);
}

// Smacker playback hands the Miles digital driver to the movie player, so the
// backend that was live when a movie started is remembered here and brought
// back up afterwards.
inline void soundManager::SaveBackend(void) {
    m_savedBackend = m_backend;
}

inline void soundManager::RestoreBackend(void) {
    if (m_backend == m_savedBackend)
        return;
    if (m_backend != SOUND_BACKEND_NONE)
        ShutdownSoundBackends();
    if (m_savedBackend == SOUND_BACKEND_MILES)
        StartupMilesBackend();
    else if (m_savedBackend == SOUND_BACKEND_AUDIERE)
        CDStartup();
}

extern SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT];
extern struct _MDI_DRIVER* hMDI;
extern i32 CurrentMidiFile;
extern u8 bGotMidi[MIDI_TRACK_COUNT];
extern struct _SEQUENCE* hSequence[MIDI_TRACK_COUNT];
extern class MIDIWrap* pMIDIWrap[MIDI_TRACK_COUNT];

#endif
