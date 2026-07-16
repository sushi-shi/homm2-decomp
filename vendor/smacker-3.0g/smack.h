#ifndef SMACKH
#define SMACKH
/*==========================================================================*/
/*  smack.h  --  Smacker Video Technology, Win32 SDK (smackw32.DLL).         */
/*                                                                           */
/*  Copyright (C) 1994-96 RAD Game Tools, Inc.                               */
/*                                                                           */
/*  Version 3.0g -- the release HEROES2W.EXE (Heroes of Might and Magic II   */
/*  "The Price of Loyalty", New World Computing, 1996) links against.        */
/*                                                                           */
/*  PROVENANCE / version pin:                                                */
/*    The retail EXE imports smackw32.DLL BY ORDINAL only (10 ordinals: 14,  */
/*    18, 19, 20, 21, 23, 28, 32, 33, 38).  The ordinal->name table below is */
/*    taken from the period smackw32.DLL of the same retail lineage: its     */
/*    embedded build string reads "*** Smacker Version: 3.0g ***" with       */
/*    LegalCopyright "Copyright (C) 1994-96 RAD Game Tools, Inc." (this DLL   */
/*    carries no VS_VERSION resource; the later GOG re-release ships a 3.0r   */
/*    DLL whose ordinals are RENUMBERED and do NOT fit this EXE).  The 3.0g   */
/*    39-export table maps our imports as:                                   */
/*        14 SmackOpen@12   18 SmackClose@4   19 SmackDoFrame@4              */
/*        20 SmackSummary@8 21 SmackNextFrame@4  23 SmackToBuffer@28         */
/*        28 SmackToBufferRect@8  32 SmackWait@4  33 SmackSoundUseMSS@4      */
/*        38 SmackSoundUseDirectSound@4                                      */
/*    Ordinal 38 is disambiguated (vs an alternate 38==SmackSoundUseWin@0 in */
/*    the earlier 38-export tables) by the retail call site at .text:4013C4, */
/*    which pushes exactly ONE argument before `call [ord 38]` -> the @4     */
/*    (1-arg) SmackSoundUseDirectSound, confirming the SmackSoundUseDW-bump  */
/*    39-export layout of Smacker 3.0g.                                      */
/*                                                                           */
/*    This header is RECONSTRUCTED: real prototypes matching the exported    */
/*    @N stdcall byte-counts, plus the version-specific Smack/SmackSum       */
/*    structs recovered from the 3.0g implementation.                       */
/*    The Win32-GDI-dependent SmackBuffer.. / SmackBlit.. families (ordinals */
/*    1..13, and 34/35/36/37/39) are NOT imported by the retail EXE and are  */
/*    intentionally omitted so this header stays free of <windows.h>.        */
/*==========================================================================*/

#define SMACKVERSION "3.0g"

#include "rad.h"

RADDEFSTART

/*--------------------------------------------------------------------------*/
/*  The Smacker file/instance struct (a "Smack").  Stable RAD 3.x layout.    */
/*--------------------------------------------------------------------------*/
typedef struct SmackTag {
  u32 Version;           /* SMK2 only right now                             */
  u32 Width;             /* Width (1 based, 640 for example)                */
  u32 Height;            /* Height (1 based, 480 for example)               */
  u32 Frames;            /* Number of frames (1 based, 100 = 100 frames)    */
  u32 MSPerFrame;        /* Frame Rate                                      */
  u32 SmackerType;       /* bit 0 set = ring frame                          */
  u32 LargestInTrack[7]; /* Largest single size for each track              */
  u32 tablesize;         /* Size of the init tables                         */
  u32 codesize;          /* Compression info                                */
  u32 absize;            /* ditto                                           */
  u32 detailsize;        /* ditto                                           */
  u32 typesize;          /* ditto                                           */
  u32 TrackType[7];      /* hi byte 0x80-Comp 0x40-PCM 0x20-16bit 0x10-ster */
  u32 extra;             /* extra value (should be zero)                    */
  u32 NewPalette;        /* set to one if the palette changed               */
  u8  Palette[772];      /* palette data                                    */
  u32 PalType;           /* type of palette                                 */
  u32 FrameNum;          /* Frame Number to be displayed                    */
  u32 FrameSize;         /* The current frame's size in bytes               */
  u32 SndSize;           /* The current frame sound tracks' size in bytes   */
  s32 LastRectx;         /* Rect set in from SmackToBufferRect (X coord)    */
  s32 LastRecty;         /* Rect set in from SmackToBufferRect (Y coord)    */
  s32 LastRectw;         /* Rect set in from SmackToBufferRect (Width)      */
  s32 LastRecth;         /* Rect set in from SmackToBufferRect (Height)     */
  u32 OpenFlags;         /* flags used on open                              */
  u32 LeftOfs;           /* Left Offset used in SmackTo                     */
  u32 TopOfs;            /* Top Offset used in SmackTo                      */
  u32 LargestFrameSize;  /* Largest frame size                              */
  u32 Highest1SecRate;   /* Highest 1 sec data rate                         */
  u32 Highest1SecFrame;  /* Highest 1 sec data rate starting frame          */
  u32 ReadError;         /* Set to non-zero if a read error has occurred    */
  u32 addr32;            /* translated address for 16 bit interface         */
} Smack;

#define SmackHeaderSize(smk) ((((u8*)&((smk)->extra))-((u8*)(smk)))+4)

/*--------------------------------------------------------------------------*/
/*  Playback statistics, filled by SmackSummary().                          */
/*--------------------------------------------------------------------------*/
typedef struct SmackSumTag {
  u32 TotalTime;         /* total time                                      */
  u32 MS100PerFrame;     /* MS*100 per frame (100000/x = Frames/Sec)        */
  u32 TotalOpenTime;     /* Time to open and prepare for decompression      */
  u32 TotalFrames;       /* Total Frames displayed                          */
  u32 SkippedFrames;     /* Total number of skipped frames                  */
  u32 SoundSkips;        /* Total number of sound skips                     */
  u32 TotalBlitTime;     /* Total time spent blitting                       */
  u32 TotalReadTime;     /* Total time spent reading                        */
  u32 TotalDecompTime;   /* Total time spent decompressing                  */
  u32 TotalBackReadTime; /* Total time spent reading in background          */
  u32 TotalReadSpeed;    /* Total io speed (bytes/second)                   */
  u32 SlowestFrameTime;  /* Slowest single frame time                       */
  u32 Slowest2FrameTime; /* Second slowest single frame time                */
  u32 SlowestFrameNum;   /* Slowest single frame number                     */
  u32 Slowest2FrameNum;  /* Second slowest single frame number              */
  u32 AverageFrameSize;  /* Average size of the frame                       */
  u32 HighestMemAmount;  /* Highest amount of memory allocated              */
  u32 TotalExtraMemory;  /* Total extra memory allocated                    */
  u32 HighestExtraUsed;  /* Highest extra memory actually used              */
  u32 BitmapHandle;      /* GDI bitmap handle retained by this 3.0g build   */
  u32 SoundWindowProc;   /* previous sound-window procedure                 */
} SmackSum;

/*=======================  SmackOpen() flags  ==============================*/
#define SMACKNEEDPAN    0x00020L /* Will be setting the pan                 */
#define SMACKNEEDVOLUME 0x00040L /* Will be setting the volume              */
#define SMACKFRAMERATE  0x00080L /* Override fr (call SmackFrameRate first) */
#define SMACKLOADEXTRA  0x00100L /* Load the extra buffer during SmackOpen  */
#define SMACKPRELOADALL 0x00200L /* Preload the entire animation            */
#define SMACKNOSKIP     0x00400L /* Don't skip frames if falling behind     */
#define SMACKSIMULATE   0x00800L /* Simulate the speed (call SmackSim first)*/
#define SMACKFILEHANDLE 0x01000L /* Use when passing in a file handle       */
#define SMACKTRACK1     0x02000L /* Play audio track 1                      */
#define SMACKTRACK2     0x04000L /* Play audio track 2                      */
#define SMACKTRACK3     0x08000L /* Play audio track 3                      */
#define SMACKTRACKS (SMACKTRACK1|SMACKTRACK2|SMACKTRACK3)

#define SMACKAUTOEXTRA 0xffffffffL /* NOT A FLAG! - Use as extrabuf param   */

/*======================  SmackToBufferRect() modes  ======================*/
#define SMACKSURFACEFAST             0
#define SMACKSURFACESLOW             1
#define SMACKSURFACEDIRECT           2

/*==========================================================================*/
/*  Core API.  Every function the retail EXE imports is declared here; the   */
/*  companion playback/sound calls that take only Smack.. /SmackSum.. args   */
/*  are included for completeness.  (*) = imported by HEROES2W.EXE.          */
/*==========================================================================*/

RADEXPFUNC Smack PTR4* RADEXPLINK SmackOpen(const char PTR4* name, u32 flags, u32 extrabuf);            /* (*) ord 14 */
RADEXPFUNC void        RADEXPLINK SmackClose(Smack PTR4* smk);                                          /* (*) ord 18 */
RADEXPFUNC u32         RADEXPLINK SmackDoFrame(Smack PTR4* smk);                                        /* (*) ord 19 */
RADEXPFUNC void        RADEXPLINK SmackSummary(Smack PTR4* smk, SmackSum PTR4* sum);                    /* (*) ord 20 */
RADEXPFUNC void        RADEXPLINK SmackNextFrame(Smack PTR4* smk);                                      /* (*) ord 21 */
RADEXPFUNC void        RADEXPLINK SmackToBuffer(Smack PTR4* smk, u32 left, u32 top, u32 Pitch,
                                                u32 destheight, const void PTR4* buf, u32 Flags);       /* (*) ord 23 */
RADEXPFUNC u32         RADEXPLINK SmackToBufferRect(Smack PTR4* smk, u32 SmackSurface);                 /* (*) ord 28 */
RADEXPFUNC u32         RADEXPLINK SmackWait(Smack PTR4* smk);                                           /* (*) ord 32 */
RADEXPFUNC u8          RADEXPLINK SmackSoundUseMSS(void PTR4* dig);                                     /* (*) ord 33 */
RADEXPFUNC u8          RADEXPLINK SmackSoundUseDirectSound(void PTR4* lpds);                            /* (*) ord 38 */

RADEXPFUNC void        RADEXPLINK SmackSimulate(u32 sim);                                               /*     ord 15 */
RADEXPFUNC void        RADEXPLINK SmackFrameRate(u32 forcerate);                                        /*     ord 16 */
RADEXPFUNC u32         RADEXPLINK SmackSoundOnOff(Smack PTR4* smk, u32 on);                             /*     ord 17 */
RADEXPFUNC void        RADEXPLINK SmackColorTrans(Smack PTR4* smk, const void PTR4* trans);             /*     ord 24 */
RADEXPFUNC void        RADEXPLINK SmackColorRemap(Smack PTR4* smk, const void PTR4* remappal,
                                                  u32 numcolors, u32 paltype);                          /*     ord 25 */
RADEXPFUNC u32         RADEXPLINK SmackGetTrackData(Smack PTR4* smk, void PTR4* dest, u32 trackflag);   /*     ord 26 */
RADEXPFUNC void        RADEXPLINK SmackGoto(Smack PTR4* smk, u32 frame);                                /*     ord 27 */
RADEXPFUNC u32         RADEXPLINK SmackSoundInTrack(Smack PTR4* smk, u32 trackflags);                   /*     ord 29 */
RADEXPFUNC void        RADEXPLINK SmackVolumePan(Smack PTR4* smk, u32 trackflag, u32 volume, u32 pan);  /*     ord 30 */
RADEXPFUNC void        RADEXPLINK SmackSoundCheck(void);                                                /*     ord 31 */

RADDEFEND

#endif /* SMACKH */
