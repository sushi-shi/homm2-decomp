// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icon2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
#include <string.h>
int gIcRow;
int gIcPitch;
unsigned char gIcColor;
unsigned char * gIcDimPal;
unsigned int gIcRun;
unsigned int gIcCnt;
unsigned char * gIcSrc;
unsigned char * gIcDimDst;
int gIcClipR;
int gIcClipB;
int gIcX0;
unsigned int gIcDimLen;
int gIcY;
int gIcX;
unsigned char * gIcEntry;
unsigned int gIcCnt2;

VA(0x004d0570, 0x4ed)
// void IconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
//                   int clip, int clipX, int clipY, int clipW, int clipH, int color);


