#ifndef HOMM2_PLATFORM_WIN32_SMACK_H
#define HOMM2_PLATFORM_WIN32_SMACK_H

#include "rad.h"

#define SMACKLOADEXTRA 0x00100L
#define SMACKAUTOEXTRA 0x00200L
#define SMACKPRELOADALL 0x00400L
#define SMACKNEEDVOLUME 0x00800L
#define SMACKTRACK1 0x02000L
#define SMACKTRACK2 0x04000L
#define SMACKTRACK3 0x08000L

#define SMACKSURFACEFAST 0
#define SMACKSURFACESLOW 1

struct Smack {
    u32 Version;
    u32 Width;
    u32 Height;
    u32 Frames;
    u32 MSPerFrame;
    u32 FrameNum;
    u32 FrameSize;
    u32 SndSize;
    u32 LargestInTrack;
    u32 tablesize;
    u32 codesize;
    u32 absize;
    u32 detailsize;
    u32 typesize;
    u32 TreesSize;
    u32 ExtraSize;
    u32 NewPalette;
    u8 Palette[772];
    u32 PalType;
    u32 LastRectx;
    u32 LastRecty;
    u32 LastRectw;
    u32 LastRecth;
    u32 OpenFlags;
    u32 LeftOfs;
    u32 TopOfs;
    u32 ReadError;
    void* Reserved;
};

struct SmackSum {
    u32 TotalTime;
    u32 MS100PerFrame;
    u32 TotalOpenTime;
    u32 TotalFrames;
    u32 SkippedFrames;
    u32 SoundSkips;
    u32 TotalBlitTime;
    u32 TotalReadTime;
    u32 TotalDecompTime;
    u32 TotalBackReadTime;
    u32 TotalReadSpeed;
    u32 SlowestFrameTime;
    u32 Slowest2FrameTime;
    u32 SlowestFrameNum;
    u32 Slowest2FrameNum;
    u32 AverageFrameSize;
    u32 Highest1SecRate;
    u32 Highest1SecFrame;
    u32 HighestMemAmount;
    u32 TotalExtraMemory;
    u32 HighestExtraUsed;
};

Smack* SmackOpen(const char* name, u32 flags, u32 extraBuffer);
void SmackClose(Smack* smack);
u32 SmackDoFrame(Smack* smack);
void SmackNextFrame(Smack* smack);
u32 SmackWait(Smack* smack);
void SmackSummary(Smack* smack, SmackSum* summary);
void SmackToBuffer(Smack* smack, u32 left, u32 top, u32 pitch, u32 destHeight,
                   void* buffer, u32 flags);
u32 SmackToBufferRect(Smack* smack, u32 surface);
u8 SmackSoundUseMSS(void* dig);
u8 SmackSoundUseDirectSound(void* lpds);

#endif
