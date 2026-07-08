#ifndef HOMM2_BASE_SOUNDMANAGER_H
#define HOMM2_BASE_SOUNDMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 37 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
// forward declarations:
class sample;
struct _SAMPLE;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
struct SampleChannelStruct {
    int startChannel;
    int endChannel;
    int currentChannel;
};

class soundManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    m_digitalDriver;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    char _pad_0x42[0xe];
    int    m_midiFile;  // +0x50
    struct _SAMPLE *m_sampleHandles[14];  // +0x54  sample handles
    char _pad_0x8c[0x8];
    int    m_numSampleHandles;  // +0x94
    char _pad_0x98[0x40];             // 0x98..0xd8
    char   field_0xd8[0x14];          // 0xd8  per-channel volume byte
    struct _SAMPLE *field_0xec[14];   // 0xec  active sample per channel
    char _pad_0x124[0x8];             // 0x124..0x12c
    int    field_0x12c[14];           // 0x12c  sample address low per channel
    char _pad_0x164[0x8];             // 0x164..0x16c
    int    field_0x16c[14];           // 0x16c  sample address high per channel
    char _pad_0x1a4[0x3c8];           // 0x1a4..0x56c
    int    field_0x56c;  // +0x56c
    char _pad_0x570[0x4];
    int    field_0x574;  // +0x574
    char   m_currentTrack;  // +0x578
    char   m_pollRequested;  // +0x579
    char   field_0x57a;  // +0x57a
    char   field_0x57b;  // +0x57b
    char _pad_0x57c[0x14];
    long   field_0x590[0x3c];  // +0x590  saved music file positions per track
    int    m_fading;  // +0x680
    int    m_samplesReady;  // +0x684
    int    m_fadeSteps;  // +0x688
    int    m_fadeTargetTrack;  // +0x68c
    int    m_cdTrack;  // +0x690
    int    m_cdPlayFrame;  // +0x694
    short  m_auxDevice;  // +0x698
    int    m_cdReady;  // +0x69a
    int    field_0x69e;  // +0x69e
    int    m_cdStarted;  // +0x6a2
    int    field_0x6a6;  // +0x6a6
    int    m_pollTimer;  // +0x6aa
    // --- constructors ---
    soundManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void ValidatePreviousPosition(int);
    void CDStop(void);
    int CDIsPlaying(void);
    void CDStartup(void);
    void CDShutdown(void);
    void CDSetVolume(int, int);
    void CDPlay(int, int, int, int);
    void CDPoll(void);
    int ConvertVolume(int, int);
    void AllocateSampleHandles(void);
    struct _SAMPLE * StartSample(char *, char * *, short int, short int, int, int, long int);
    void StopAllSamples(int);
    void StopSample(struct _SAMPLE *);
    void ModifySample(struct _SAMPLE *, short int, long int);
    long int DigitalReport(struct _SAMPLE *, short int);
    void AdjustSoundVolumes(void);
    void AdjustMusicVolumes(void);
    void ForcePollSound(void);
    void SetMusicQuality(int);
    void PlayAmbientMusic(int, long int, int);
    void PollSound(void);
    void SwitchAmbientMusic(int);
    struct _SAMPLE * MemorySample(class sample *);
    void GetNumberCDDrives(void);
    void ServiceSound(void);
    int MusicPlaying(void);
    void MIDIStartup(void);
    void MIDIShutdown(void);
    void MIDIPlay(int);
    void MIDIStop(void);
    int MIDIIsPlaying(void);
    void MIDISetVolume(void);
    void MIDIPoll(void);
};
#pragma pack(pop)
SIZE(soundManager, 0x6ae);
// ---- globals (declarations, RVA order) ----
DATA(0x0051f018) extern char *digitalDriverNames[14];
DATA(0x0051f050) extern struct SampleChannelStruct *SCS;
DATA(0x0051f080) extern char CDPreviousPosition[60][15];
DATA(0x0051f404) extern int CDWaiting;
DATA(0x0051f408) extern int CDPlaying;
DATA(0x0051f40c) extern int iCalibrateLoop;
DATA(0x0051fec8) extern struct _MDI_DRIVER *hMDI;
DATA(0x0051fecc) extern int CurrentMidiFile;
DATA(0x0051fed0) extern unsigned char *bGotMidi;
DATA(0x0051ff0c) extern long lLastMIDIPollTickCount;
DATA(0x00534980) extern char *lpszReturnString;
DATA(0x00534a80) extern unsigned long nMCIError;
DATA(0x00534a88) extern short *iLastVolume;
DATA(0x00534ac8) extern char *CommandString;
DATA(0x00534cf0) extern class MIDIWrap *pMIDIWrap[60];
DATA(0x00534de0) extern struct _SEQUENCE *hSequence[60];

#endif // HOMM2_BASE_SOUNDMANAGER_H
