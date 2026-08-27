#ifndef HOMM2_BASE_SOUNDMANAGER_H
#define HOMM2_BASE_SOUNDMANAGER_H

#include <Ints.h>
#include <stdio.h>
#include "baseManager.h"

typedef enum MidiTrackConstant {
    MIDI_NO_TRACK    = -1,
    MIDI_TRACK_COUNT = 60
} MidiTrackConstant;

typedef enum SoundStorageConstant {
    SOUND_CHANNEL_VOLUME_CAPACITY = 0x14,
    DIGITAL_DRIVER_NAME_COUNT     = 14,
    SOUND_CHANNEL_TYPE_COUNT      = 4
} SoundStorageConstant;

enum class SoundVolumeConversionMode : i32 {
    SOUND_VOLUME_EFFECT = 100,
    SOUND_VOLUME_MUSIC  = 101
};
using enum SoundVolumeConversionMode;

enum class SoundSampleOperation : i16 {
    SOUND_SAMPLE_OPERATION_VOLUME        = 1,
    SOUND_SAMPLE_OPERATION_START         = 5,
    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME = 100,
    SOUND_SAMPLE_OPERATION_MUSIC_VOLUME  = 101
};
using enum SoundSampleOperation;

enum class SoundDigitalReportQuery : i16 {
    SOUND_DIGITAL_REPORT_VOLUME  = 1,
    SOUND_DIGITAL_REPORT_PLAYING = 4
};
using enum SoundDigitalReportQuery;

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

typedef enum SoundBackendKind {
    SOUND_BACKEND_AUDIERE = 0,
    SOUND_BACKEND_MILES   = 1,
    SOUND_BACKEND_NONE    = 2
} SoundBackendKind;

class soundManager H2_FINAL : public baseManager {
public:
    SoundBackendKind m_backend;
    SoundBackendKind m_savedBackend;
    struct _DIG_DRIVER* m_digitalDriver;
    audiere::AudioDevicePtr m_audiereDevice;
    i32 m_musicFadeTargetTrack;
    i32 m_musicFadeSteps;
    i32 m_musicTrack;
    soundManager(void);
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
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


extern bool gSoundDisabled;


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
