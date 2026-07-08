// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534c60+ block (distinct from IconToBitmap's 0x534c20 block).
static int gFlipSkip;
static unsigned int gFlipRun;
static int gFlipX0;
static unsigned int gFlipCnt;
static unsigned int gFlipCnt2;
static int gFlipY;
static IconEntry *gFlipEntry;
static int gFlipX;
static unsigned char *gFlipSrc;
static unsigned char *gFlipDimPal;
static unsigned char *gFlipDimDst;
static int gFlipClipB;
static int gFlipRow;
static unsigned int gFlipDimLen;
static unsigned char gFlipColor;
static int gFlipClipR;
static unsigned char *gFlipDst;
static int gFlipXEnd;

VA(0x004d1ba0, 0x4f1)
// void FlipIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
//                       int param_6, int param_7, int param_8, int param_9, int param_10, int param_11);

