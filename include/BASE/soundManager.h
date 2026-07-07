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

class soundManager : public baseManager {
public:
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
#endif // HOMM2_BASE_SOUNDMANAGER_H
