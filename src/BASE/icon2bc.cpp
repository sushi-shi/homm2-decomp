// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <string.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534ca8+ block.
static int gCTPitch;
static unsigned int gCTCnt;
static int gCTX;
static int gCTY;
static IconEntry *gCTEntry;
static int gCTRow;
static unsigned char *gCTDimPal;
static int gCTClipB;
static int gCTClipR;
static unsigned char *gCTSrcCopy;
static unsigned int gCTCnt2;
static unsigned char *gCTDst;
static unsigned char *gCTSrc;
static unsigned int gCTDimLen;
static int gCTX0;
static unsigned char gCTColor;
static unsigned int gCTRun;

VA(0x004d32a0, 0x5af)
// void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y,
//                             int frame, int clip, int clipX, int clipY, int clipW, int clipH,
//                             int color, unsigned char *colorTable, int dimGate);

