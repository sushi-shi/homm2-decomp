// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x5381b8+ block.
static int gFDX0;
static unsigned int gFDCnt;
static int gFDXEnd;
static unsigned char *gFDDst;
static unsigned int gFDCnt2;
static int gFDRow;
static IconEntry *gFDEntry;
static int gFDClipR;
static int gFDX;
static int gFDClipB;
static unsigned char *gFDSrc;
static int gFDY;
static unsigned int gFDRun;

VA(0x004daa20, 0x23b)
// void FlipDimIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
//                          int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
//                          int param_11);

