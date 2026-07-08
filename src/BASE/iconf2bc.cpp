// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x5380c0+ block.
static unsigned char *gFCDst;
static unsigned int gFCDimLen;
static int gFCClipR;
static unsigned char gFCColor;
static int gFCX;
static int gFCSkip;
static int gFCY;
static unsigned char *gFCDimPal;
static unsigned int gFCCnt2;
static int gFCXEnd;
static unsigned int gFCCnt;
static IconEntry *gFCEntry;
static unsigned char *gFCSrc;
static unsigned char *gFCDimDst;
static int gFCX0;
static int gFCClipB;
static int gFCRow;
static unsigned int gFCRun;

VA(0x004d9790, 0x54d)
// void FlipIconToBitmapColorTable(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
//                                 int param_5, int param_6, int param_7, int param_8, int param_9,
//                                 int param_10, int param_11, unsigned char *param_12);

