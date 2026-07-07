// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BITMAP.OBJ   from: .\basewin.lib
// functions: 14   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/bitmap.h>
VA(0x004cffc0, 0x2a)
bitmap::bitmap(void) {}

VA(0x004d0040, 0x53)
bitmap::bitmap(short int, short int, short int) {}

VA(0x004d00a0, 0x8f)
bitmap::bitmap(unsigned long int) {}

VA(0x004d0130, 0x2c)
bitmap::~bitmap() {}

VA(0x004d0160, 0xff)
void bitmap::DrawToBufferCareful(short int, short int) {}

VA(0x004d0260, 0x3c)
void bitmap::DrawToBuffer(short int, short int) {}

VA(0x004d02a0, 0x32)
void bitmap::DrawToScreen(short int, short int) {}

VA(0x004d02e0, 0x2d)
void bitmap::GrabScreen(short int, short int) {}

VA(0x004d0310, 0x26)
void bitmap::GrabBitmap(class bitmap *, short int, short int) {}

VA(0x004d0340, 0xf0)
void bitmap::GrabBitmapCareful(class bitmap *, short int, short int) {}

VA(0x004d0430, 0xcb)
void bitmap::CopyTo(class bitmap *, int, int, int, int, int, int) {}

VA(0x004d0500, 0x65)
void bitmap::CopyToCareful(class bitmap *, int, int, int, int, int, int) {}


// ===== vtable bitmap (root)  (1 slots) =====
//  [ 0] VA(0x004cfff0, 0x41)  void * bitmap::scalar_dtor(unsigned int)   <- introduces virtual
