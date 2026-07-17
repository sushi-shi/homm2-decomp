#ifndef HOMM2_BASE_SOUNDMANAGER_H
#define HOMM2_BASE_SOUNDMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 37 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include <stdio.h>
#include "baseManager.h"

HOMM2_ENUM_VALUES_BEGIN(SoundVolumeConstant)
    SOUND_VOLUME_STEPS = 10,
    SOUND_SAMPLE_VOLUME_MAX = 0x40,
    SOUND_MIDI_VOLUME_MAX = 0x7f,
    SOUND_CD_VOLUME_SCALE_DIVISOR = 0x280,
    SOUND_CD_MUSIC_TRACK_FIRST = 8,
    SOUND_CD_MUSIC_TRACK_LAST = 15,
    SOUND_FADE_HOLD_STEPS = 10
HOMM2_ENUM_VALUES_END(SoundVolumeConstant)

HOMM2_ENUM_VALUES_BEGIN(MidiTrackConstant)
    MIDI_NO_TRACK = -1,
    MIDI_TRACK_COUNT = 60
HOMM2_ENUM_VALUES_END(MidiTrackConstant)

HOMM2_ENUM_BEGIN(MidiSequenceStatus)
    MIDI_SEQUENCE_PLAYING = 4
HOMM2_ENUM_END(MidiSequenceStatus)

HOMM2_ENUM_VALUES_BEGIN(MidiVolumeConstant)
    MIDI_VOLUME_HIGH_RANGE = 6,
    MIDI_VOLUME_FADE_SPLIT = 10,
    MIDI_VOLUME_LOW_RANGE = 11,
    MIDI_MAX_VOLUME = 127
HOMM2_ENUM_VALUES_END(MidiVolumeConstant)

HOMM2_ENUM_BEGIN(SoundVolumeConversionMode)
    SOUND_VOLUME_EFFECT = 100,
    SOUND_VOLUME_MUSIC = 101
HOMM2_ENUM_END(SoundVolumeConversionMode)

HOMM2_ENUM_BEGIN(SoundSampleOperation)
    SOUND_SAMPLE_OPERATION_VOLUME = 1,
    SOUND_SAMPLE_OPERATION_START = 5,
    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME = 100,
    SOUND_SAMPLE_OPERATION_MUSIC_VOLUME = 101
HOMM2_ENUM_END(SoundSampleOperation)

HOMM2_ENUM_BEGIN(SoundDigitalReportQuery)
    SOUND_DIGITAL_REPORT_VOLUME = 1,
    SOUND_DIGITAL_REPORT_PLAYING = 4
HOMM2_ENUM_END(SoundDigitalReportQuery)

HOMM2_ENUM_BEGIN(SoundSampleStatus)
    SOUND_SAMPLE_STATUS_DONE = 2,
    SOUND_SAMPLE_STATUS_PLAYING = 4
HOMM2_ENUM_END(SoundSampleStatus)
// forward declarations:
class sample;
struct _SAMPLE;
struct _DIG_DRIVER;
struct tag_message;

#pragma pack(push, 1) // recovered layout is byte-packed
struct SampleChannelStruct {
    i32 startChannel;
    i32 endChannel;
    i32 currentChannel;
};

class soundManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    struct _DIG_DRIVER* m_digitalDriver; // +0x36
    i32 field_0x3a;                      // +0x3a  constructor-initialized only; no retail reader
    i32 m_ready;                         // +0x3e
    char _pad_0x42[0xe];
    FILE* m_midiFile;                    // +0x50
    struct _SAMPLE* m_sampleHandles[14]; // +0x54  sample handles
    char _pad_0x8c[0x8];
    i32 m_numSampleHandles;               // +0x94
    char _pad_0x98[0x40];                 // 0x98..0xd8
    char m_channelVolumes[0x14];          // 0xd8  per-channel volume byte
    struct _SAMPLE* m_channelSamples[14]; // 0xec  active sample per channel
    char _pad_0x124[0x8];                 // 0x124..0x12c
    void* m_channelSampleData[14];        // 0x12c  sample data per channel
    char _pad_0x164[0x8];                 // 0x164..0x16c
    u32l m_channelSampleSizes[14];        // 0x16c  sample byte length per channel
    char _pad_0x1a4[0x3c8];               // 0x1a4..0x56c
    i32 field_0x56c;                      // +0x56c  constructor-initialized only; no retail reader
    char _pad_0x570[0x4];
    i32 field_0x574;      // +0x574  constructor-initialized only; no retail reader
    char m_currentTrack;  // +0x578
    char m_pollRequested; // +0x579
    char m_pollDue;       // +0x57a
    char m_pollToggle;    // +0x57b
    char _pad_0x57c[0x14];
    i32l m_savedTrackPositions[MIDI_TRACK_COUNT]; // +0x590  saved music file positions per track
    i32 m_fading;                                 // +0x680
    i32 m_samplesReady;                           // +0x684
    i32 m_fadeSteps;                              // +0x688
    i32 m_fadeTargetTrack;                        // +0x68c
    i32 m_cdTrack;                                // +0x690
    i32 m_cdPlayFrame;                            // +0x694
    i16 m_auxDevice;                              // +0x698
    i32 m_cdReady;                                // +0x69a
    i32 m_midiReady;                              // +0x69e
    i32 m_cdStarted;                              // +0x6a2
    i32 m_midiStarted;                            // +0x6a6
    i32 m_pollTimer;                              // +0x6aa
    // --- constructors ---
    soundManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void ValidatePreviousPosition(i32);
    void CDStop(void);
    i32 CDIsPlaying(void);
    void CDStartup(void);
    void CDShutdown(void);
    void CDSetVolume(i32, i32);
    void CDPlay(i32, i32, i32, i32);
    void CDPoll(void);
    i32 ConvertVolume(i32, i32);
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
// ---- globals (declarations, RVA order) ----
extern char* digitalDriverNames[14];
extern SampleChannelStruct SCS[4];
extern char CDPreviousPosition[60][15];
extern i32 CDWaiting;
extern i32 CDPlaying;
extern i32 iCalibrateLoop;
extern struct _MDI_DRIVER* hMDI;
extern i32 CurrentMidiFile;
extern u8 bGotMidi[MIDI_TRACK_COUNT];
extern i32l lLastMIDIPollTickCount;
extern char lpszReturnString[0x100];
extern u32l nMCIError;
extern i16 iLastVolume[0x20];
extern char CommandString[0x100];
extern class MIDIWrap* pMIDIWrap[MIDI_TRACK_COUNT];
extern struct _SEQUENCE* hSequence[MIDI_TRACK_COUNT];

#endif // HOMM2_BASE_SOUNDMANAGER_H
