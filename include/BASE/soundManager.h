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
    SOUND_SAMPLE_HANDLE_CAPACITY  = 14,
    SOUND_CHANNEL_VOLUME_CAPACITY = 0x14,
    DIGITAL_DRIVER_NAME_COUNT     = 14,
    SOUND_CHANNEL_TYPE_COUNT      = 4,
    CD_POSITION_CAPACITY          = 15,
    MCI_RESULT_CAPACITY           = 0x100,
    SAVED_SAMPLE_VOLUME_CAPACITY  = 0x20,
    MCI_COMMAND_CAPACITY          = 0x100
H2_ENUM_END(SoundStorageConstant)

H2_ENUM_CLASS_BEGIN(SoundVolumeConversionMode)
    SOUND_VOLUME_EFFECT = 100,
    SOUND_VOLUME_MUSIC  = 101
H2_ENUM_CLASS_END(SoundVolumeConversionMode)

H2_ENUM_CLASS_BEGIN(SoundSampleOperation)
    SOUND_SAMPLE_OPERATION_VOLUME        = 1,
    SOUND_SAMPLE_OPERATION_START         = 5,
    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME = 100,
    SOUND_SAMPLE_OPERATION_MUSIC_VOLUME  = 101
H2_ENUM_CLASS_END(SoundSampleOperation)

H2_ENUM_CLASS_BEGIN(SoundDigitalReportQuery)
    SOUND_DIGITAL_REPORT_VOLUME  = 1,
    SOUND_DIGITAL_REPORT_PLAYING = 4
H2_ENUM_CLASS_END(SoundDigitalReportQuery)

class sample;
struct _SAMPLE;
struct _DIG_DRIVER;
struct tag_message;

#pragma pack(push, 1)
struct SampleChannelStruct {
    i32 startChannel;
    i32 endChannel;
    i32 currentChannel;
};

class soundManager : public baseManager {
public:
    struct _DIG_DRIVER* m_digitalDriver;
    i32 field_0x3a;
    i32 m_ready;
    char _pad_0x42[0xe];
    FILE* m_midiFile;
    struct _SAMPLE* m_sampleHandles[SOUND_SAMPLE_HANDLE_CAPACITY];
    char _pad_0x8c[0x8];
    i32 m_numSampleHandles;
    char _pad_0x98[0x40];
    char m_channelVolumes[SOUND_CHANNEL_VOLUME_CAPACITY];
    struct _SAMPLE* m_channelSamples[SOUND_SAMPLE_HANDLE_CAPACITY];
    char _pad_0x124[0x8];
    void* m_channelSampleData[SOUND_SAMPLE_HANDLE_CAPACITY];
    char _pad_0x164[0x8];
    u32l m_channelSampleSizes[SOUND_SAMPLE_HANDLE_CAPACITY];
    char _pad_0x1a4[0x3c8];
    i32 field_0x56c;
    char _pad_0x570[0x4];
    i32 field_0x574;
    char m_currentTrack;
    char m_pollRequested;
    char m_pollDue;
    char m_pollToggle;
    char _pad_0x57c[0x14];
    i32l m_savedTrackPositions[MIDI_TRACK_COUNT];
    i32 m_fading;
    i32 m_samplesReady;
    i32 m_fadeSteps;
    i32 m_fadeTargetTrack;
    i32 m_cdTrack;
    i32 m_cdPlayFrame;
    i16 m_auxDevice;
    i32 m_cdReady;
    i32 m_midiReady;
    i32 m_cdStarted;
    i32 m_midiStarted;
    i32 m_pollTimer;
    soundManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void ValidatePreviousPosition(i32);
    void CDStop(void);
    i32 CDIsPlaying(void);
    void CDStartup(void);
    void CDShutdown(void);
    void CDSetVolume(i32, i32);
    void CDPlay(i32, i32, i32, i32);
    void CDPoll(void);
    i32 ConvertVolume(i32, SoundVolumeConversionMode);
    void AllocateSampleHandles(void);
    struct _SAMPLE* StartSample(char*, char**, i16, i16, i32, i32, i32l);
    void StopAllSamples(i32);
    void StopSample(struct _SAMPLE*);
    void ModifySample(struct _SAMPLE*, i16, i32l);
    i32l DigitalReport(struct _SAMPLE*, i16);
    void AdjustSoundVolumes(void);
    void AdjustMusicVolumes(void);
    void ForcePollSound(void);
    void SetMusicQuality(i32);
    void PlayAmbientMusic(i32, i32l, i32);
    void PollSound(void);
    void SwitchAmbientMusic(i32);
    struct _SAMPLE* MemorySample(class sample*);
    void GetNumberCDDrives(void);
    void ServiceSound(void);
    i32 MusicPlaying(void);
    void MIDIStartup(void);
    void MIDIShutdown(void);
    void MIDIPlay(i32);
    __declspec(dllexport) inline void MIDIStop(void);
    __declspec(dllexport) inline i32 MIDIIsPlaying(void);
    __declspec(dllexport) inline void MIDISetVolume(void);
    void MIDIPoll(void);
};
#pragma pack(pop)
SIZE(soundManager, 0x6ae);
extern char* digitalDriverNames[DIGITAL_DRIVER_NAME_COUNT];
extern SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT];
extern char CDPreviousPosition[MIDI_TRACK_COUNT][CD_POSITION_CAPACITY];
extern i32 CDWaiting;
extern i32 CDPlaying;
extern i32 iCalibrateLoop;
extern struct _MDI_DRIVER* hMDI;
extern i32 CurrentMidiFile;
extern u8 bGotMidi[MIDI_TRACK_COUNT];
extern i32l lLastMIDIPollTickCount;
extern char lpszReturnString[MCI_RESULT_CAPACITY];
extern u32l nMCIError;
extern i16 iLastVolume[SAVED_SAMPLE_VOLUME_CAPACITY];
extern char CommandString[MCI_COMMAND_CAPACITY];
extern class MIDIWrap* pMIDIWrap[MIDI_TRACK_COUNT];
extern struct _SEQUENCE* hSequence[MIDI_TRACK_COUNT];

#endif
