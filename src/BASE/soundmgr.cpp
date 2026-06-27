// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\soundmgr.obj   from: .\basewin.lib
// functions: 34   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004cb630, 0x68)
// void HandleMCIError(int, char *);

VA(0x004cb6a0, 0xc7)
// void soundManager::ValidatePreviousPosition(int);

VA(0x004cb770, 0x13c)
// void soundManager::CDStop(void);

VA(0x004cb8b0, 0xb3)
// int soundManager::CDIsPlaying(void);

VA(0x004cb970, 0xf3)
// void soundManager::CDStartup(void);

VA(0x004cba70, 0xd5)
// void soundManager::CDShutdown(void);

VA(0x004cbb50, 0xe5)
// void soundManager::CDSetVolume(int, int);

VA(0x004cbc40, 0x473)
// void soundManager::CDPlay(int, int, int, int);

VA(0x004cc0c0, 0xf1)
// void soundManager::CDPoll(void);

VA(0x004cc1c0, 0xdd)
// int soundManager::ConvertVolume(int, int);

VA(0x004cc2a0, 0x5e)
// void SetReady2Poll(unsigned long int);

VA(0x004cc300, 0x26)
// void UpdateTimers(unsigned long int);

VA(0x004cc330, 0xd7)
// void soundManager::constructor(void);

VA(0x004cc410, 0x14a)
// struct _DIG_DRIVER * WAVE_init_driver(unsigned long int, unsigned short int, unsigned short int, unsigned short int);

VA(0x004cc560, 0x3a8)
// int soundManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

VA(0x004cc910, 0x91)
// void soundManager::AllocateSampleHandles(void);

VA(0x004cc9b0, 0x96)
// void soundManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

VA(0x004cca50, 0x1a)
// int soundManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

VA(0x004cca70, 0x1a)
// struct _SAMPLE * soundManager::StartSample(char *, char * *, short int, short int, int, int, long int);

VA(0x004cca90, 0x126)
// void soundManager::StopAllSamples(int);

VA(0x004ccbc0, 0xb1)
// void soundManager::StopSample(struct _SAMPLE *);

VA(0x004ccc80, 0x202)
// void soundManager::ModifySample(struct _SAMPLE *, short int, long int);

VA(0x004cce90, 0xa3)
// long int soundManager::DigitalReport(struct _SAMPLE *, short int);

VA(0x004ccf40, 0xe8)
// void soundManager::AdjustSoundVolumes(void);

VA(0x004cd030, 0xee)
// void soundManager::AdjustMusicVolumes(void);

VA(0x004cd120, 0x3a)
// void soundManager::ForcePollSound(void);

VA(0x004cd160, 0xe3)
// void soundManager::SetMusicQuality(int);

VA(0x004cd250, 0xc5)
// void soundManager::PlayAmbientMusic(int, long int, int);

VA(0x004cd320, 0x38f)
// void soundManager::PollSound(void);

VA(0x004cd6b0, 0x138)
// void soundManager::SwitchAmbientMusic(int);

VA(0x004cd7f0, 0x28f)
// struct _SAMPLE * soundManager::MemorySample(class sample *);

VA(0x004cda80, 0x16)
// void soundManager::GetNumberCDDrives(void);

VA(0x004cdaa0, 0x2e)
// void soundManager::ServiceSound(void);

VA(0x004cdad0, 0x7f)
// int soundManager::MusicPlaying(void);

// ---- data / globals / vtables ----
DATA(0x004eba20)  // const soundManager::vftable
DATA(0x0051f018)  // char * * digitalDriverNames
DATA(0x0051f050)  // struct SampleChannelStruct * SCS
DATA(0x0051f080)  // char (*)[15] CDPreviousPosition
DATA(0x0051f404)  // int CDWaiting
DATA(0x0051f408)  // int CDPlaying
DATA(0x0051f40c)  // int iCalibrateLoop
DATA(0x00534980)  // char * lpszReturnString
DATA(0x00534a80)  // unsigned long int nMCIError
DATA(0x00534a88)  // short int * iLastVolume
DATA(0x00534ac8)  // char * CommandString

// ===== vtable soundManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004cc560, 0x3a8)  int soundManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cc9b0, 0x96)  void soundManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cca50, 0x1a)  int soundManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
