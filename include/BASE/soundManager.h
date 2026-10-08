#ifndef HOMM2_BASE_SOUNDMANAGER_H
#define HOMM2_BASE_SOUNDMANAGER_H

#include <H2/Ints.h>
#include <Domains.h>
#include <PLATFORM/Audio.h>
#include <stdio.h>
#include <BASE/baseManager.h>

typedef enum MidiTrackConstant {
    MIDI_NO_TRACK    = -1,
    MIDI_TRACK_COUNT = 60
} MidiTrackConstant;


typedef enum MusicTrack {
    MUSIC_TRACK_BATTLE_FIRST     = 2,
    MUSIC_TRACK_BATTLE_LAST      = 4,
    MUSIC_TRACK_SORCERESS_TOWN   = 5,
    MUSIC_TRACK_WARLOCK_TOWN     = 6,
    MUSIC_TRACK_NECROMANCER_TOWN = 7,
    MUSIC_TRACK_KNIGHT_TOWN      = 8,
    MUSIC_TRACK_BARBARIAN_TOWN   = 9,
    MUSIC_TRACK_WIZARD_TOWN      = 10,
    MUSIC_TRACK_LAVA             = 11,
    MUSIC_TRACK_WASTELAND        = 12,
    MUSIC_TRACK_DESERT           = 13,
    MUSIC_TRACK_SNOW             = 14,
    MUSIC_TRACK_SWAMP            = 15,
    MUSIC_TRACK_WATER            = 16,
    MUSIC_TRACK_DIRT             = 17,
    MUSIC_TRACK_GRASS            = 18,
    MUSIC_TRACK_LOST_GAME        = 19,
    MUSIC_TRACK_NEW_WEEK         = 20,
    MUSIC_TRACK_NEW_MONTH        = 21,
    MUSIC_TRACK_CAMPAIGN_EVIL    = 22,
    MUSIC_TRACK_CAMPAIGN_GOOD    = 24,
    MUSIC_TRACK_AI_TURN          = 28,
    MUSIC_TRACK_BATTLE_VICTORY   = 29,
    MUSIC_TRACK_BATTLE_LOSS      = 30,
    MUSIC_TRACK_MAIN_MENU        = 42,
    MUSIC_TRACK_HIGH_SCORE       = 43,
    // Ironfist's Cyborg town theme.
    MUSIC_TRACK_CYBORG_TOWN      = 44
} MusicTrack;


typedef enum SoundVolumeScale {
    SOUND_VOLUME_FULL = 127
} SoundVolumeScale;

typedef enum SoundStorageConstant {
    SOUND_CHANNEL_TYPE_COUNT = 4
} SoundStorageConstant;

enum class SoundVolumeConversionMode : i32 {
    SOUND_VOLUME_EFFECT = 100,
    SOUND_VOLUME_MUSIC  = 101
};
using enum SoundVolumeConversionMode;

class sample;
struct tag_message;

#pragma pack(push, 1)
struct SampleChannelStruct {
    i32 startChannel;
    i32 endChannel;
    i32 currentChannel;
};

typedef enum SoundBackendKind {
    SOUND_BACKEND_AUDIO_CD   = 0,
    SOUND_BACKEND_AUDIO_MIDI = 1,
    SOUND_BACKEND_NONE       = 2
} SoundBackendKind;

class soundManager : public baseManager {
public:
    SoundBackendKind m_backend;
    SoundBackendKind m_savedBackend;
    i32 m_musicFadeTargetTrack;
    i32 m_musicFadeSteps;
    i32 m_musicTrack;
    soundManager(void);
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    bool StartupCdBackend(void);
    void ShutdownSoundBackends(void);
    bool StartupMidiBackend(void);
    void SaveBackend(void);
    void RestoreBackend(void);
    i32 ConvertVolume(i32 volume, SoundVolumeConversionMode soundType);
    void StopAllSamples(i32 stopMusic);
    void StopSample(class sample* sampleResource);
    void ModifySample(class sample* sampleResource, i32 volume);
    bool DigitalReport(class sample* sampleResource);
    void AdjustSoundVolumes(void);
    void AdjustMusicVolumes(void);
    void SetMusicQuality(i32 musicSource);
    void PlayAmbientMusic(i32 track);
    void PollSound(void);
    void SwitchAmbientMusic(i32 track);
    void MemorySample(class sample* sampleResource);
    void ServiceSound(void);
    i32 MusicPlaying(void);
};
#pragma pack(pop)


extern bool gSoundDisabled;


extern bool gbSoundEnabled;

inline bool IsCdBackend(const soundManager* manager) {
    return manager->m_backend == SOUND_BACKEND_AUDIO_CD;
}

inline bool IsMidiBackend(const soundManager* manager) {
    return manager->m_backend == SOUND_BACKEND_AUDIO_MIDI;
}

inline bool IsSoundBackendActive(const soundManager* manager) {
    return IsCdBackend(manager) || IsMidiBackend(manager);
}


inline void soundManager::SaveBackend(void) {
    m_savedBackend = m_backend;
}

inline void soundManager::RestoreBackend(void) {
    if (m_backend == m_savedBackend)
        return;
    if (m_backend != SOUND_BACKEND_NONE)
        ShutdownSoundBackends();
    if (m_savedBackend == SOUND_BACKEND_AUDIO_MIDI)
        StartupMidiBackend();
    else if (m_savedBackend == SOUND_BACKEND_AUDIO_CD)
        StartupCdBackend();
}

extern SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT];
extern i32 CurrentMidiFile;
extern u8 bGotMidi[MIDI_TRACK_COUNT];

#endif
