#ifndef SMACKH
#define SMACKH


#define SMACKVERSION "3.0g"

#include "rad.h"

RADDEFSTART


typedef struct SmackTag {
  u32 Version;
  u32 Width;
  u32 Height;
  u32 Frames;
  u32 MSPerFrame;
  u32 SmackerType;
  u32 LargestInTrack[7];
  u32 tablesize;
  u32 codesize;
  u32 absize;
  u32 detailsize;
  u32 typesize;
  u32 TrackType[7];
  u32 extra;
  u32 NewPalette;
  u8  Palette[772];
  u32 PalType;
  u32 FrameNum;
  u32 FrameSize;
  u32 SndSize;
  s32 LastRectx;
  s32 LastRecty;
  s32 LastRectw;
  s32 LastRecth;
  u32 OpenFlags;
  u32 LeftOfs;
  u32 TopOfs;
  u32 LargestFrameSize;
  u32 Highest1SecRate;
  u32 Highest1SecFrame;
  u32 ReadError;
  u32 addr32;
} Smack;

#define SmackHeaderSize(smk) ((((u8*)&((smk)->extra))-((u8*)(smk)))+4)


typedef struct SmackSumTag {
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
  u32 HighestMemAmount;
  u32 TotalExtraMemory;
  u32 HighestExtraUsed;
  u32 BitmapHandle;
  u32 SoundWindowProc;
} SmackSum;


#define SMACKNEEDPAN    0x00020L
#define SMACKNEEDVOLUME 0x00040L
#define SMACKFRAMERATE  0x00080L
#define SMACKLOADEXTRA  0x00100L
#define SMACKPRELOADALL 0x00200L
#define SMACKNOSKIP     0x00400L
#define SMACKSIMULATE   0x00800L
#define SMACKFILEHANDLE 0x01000L
#define SMACKTRACK1     0x02000L
#define SMACKTRACK2     0x04000L
#define SMACKTRACK3     0x08000L
#define SMACKTRACKS (SMACKTRACK1|SMACKTRACK2|SMACKTRACK3)

#define SMACKAUTOEXTRA 0xffffffffL


#define SMACKSURFACEFAST             0
#define SMACKSURFACESLOW             1
#define SMACKSURFACEDIRECT           2


RADEXPFUNC Smack PTR4* RADEXPLINK SmackOpen(const char PTR4* name, u32 flags, u32 extrabuf);
RADEXPFUNC void        RADEXPLINK SmackClose(Smack PTR4* smk);
RADEXPFUNC u32         RADEXPLINK SmackDoFrame(Smack PTR4* smk);
RADEXPFUNC void        RADEXPLINK SmackSummary(Smack PTR4* smk, SmackSum PTR4* sum);
RADEXPFUNC void        RADEXPLINK SmackNextFrame(Smack PTR4* smk);
RADEXPFUNC void        RADEXPLINK SmackToBuffer(Smack PTR4* smk, u32 left, u32 top, u32 Pitch,
                                                u32 destheight, const void PTR4* buf, u32 Flags);
RADEXPFUNC u32         RADEXPLINK SmackToBufferRect(Smack PTR4* smk, u32 SmackSurface);
RADEXPFUNC u32         RADEXPLINK SmackWait(Smack PTR4* smk);
RADEXPFUNC u8          RADEXPLINK SmackSoundUseMSS(void PTR4* dig);
RADEXPFUNC u8          RADEXPLINK SmackSoundUseDirectSound(void PTR4* lpds);

RADEXPFUNC void        RADEXPLINK SmackSimulate(u32 sim);
RADEXPFUNC void        RADEXPLINK SmackFrameRate(u32 forcerate);
RADEXPFUNC u32         RADEXPLINK SmackSoundOnOff(Smack PTR4* smk, u32 on);
RADEXPFUNC void        RADEXPLINK SmackColorTrans(Smack PTR4* smk, const void PTR4* trans);
RADEXPFUNC void        RADEXPLINK SmackColorRemap(Smack PTR4* smk, const void PTR4* remappal,
                                                  u32 numcolors, u32 paltype);
RADEXPFUNC u32         RADEXPLINK SmackGetTrackData(Smack PTR4* smk, void PTR4* dest, u32 trackflag);
RADEXPFUNC void        RADEXPLINK SmackGoto(Smack PTR4* smk, u32 frame);
RADEXPFUNC u32         RADEXPLINK SmackSoundInTrack(Smack PTR4* smk, u32 trackflags);
RADEXPFUNC void        RADEXPLINK SmackVolumePan(Smack PTR4* smk, u32 trackflag, u32 volume, u32 pan);
RADEXPFUNC void        RADEXPLINK SmackSoundCheck(void);

RADDEFEND

#endif
