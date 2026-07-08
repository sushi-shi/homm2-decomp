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
class soundManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    char _pad_0x42[0xe];
    int    field_0x50;  // +0x50
    struct _SAMPLE *field_0x54[14];  // +0x54  sample handles
    char _pad_0x8c[0x8];
    int    field_0x94;  // +0x94
    char _pad_0x98[0x4d4];
    int    field_0x56c;  // +0x56c
    char _pad_0x570[0x4];
    int    field_0x574;  // +0x574
    char   field_0x578;  // +0x578
    char   field_0x579;  // +0x579
    char   field_0x57a;  // +0x57a
    char   field_0x57b;  // +0x57b
    char _pad_0x57c[0x104];
    int    field_0x680;  // +0x680
    int    field_0x684;  // +0x684
    int    field_0x688;  // +0x688
    int    field_0x68c;  // +0x68c
    int    field_0x690;  // +0x690
    int    field_0x694;  // +0x694
    short  field_0x698;  // +0x698
    int    field_0x69a;  // +0x69a
    int    field_0x69e;  // +0x69e
    int    field_0x6a2;  // +0x6a2
    int    field_0x6a6;  // +0x6a6
    int    field_0x6aa;  // +0x6aa
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
