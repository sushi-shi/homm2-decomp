// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\SAMPLE.OBJ   from: .\basewin.lib
// functions: 8   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/Misc.h>
VA(0x004dad60, 0x181)
sample::sample(char *, long int, long int, long int) {}

VA(0x004daf40, 0x2c)
sample::~sample()
{
    BaseFree(field_0x14, __FILE__, __LINE__);
    field_0x14 = 0;
    field_0x18 = 0;
    field_0x28 = 0;
}

VA(0x004daf70, 0x72)
MIDIWrap::MIDIWrap(char *) {}

VA(0x004db030, 0x28)
MIDIWrap::~MIDIWrap()
{
    BaseFree(field_0x10, __FILE__, __LINE__);
    field_0x10 = 0;
}


// ===== vtable MIDIWrap (root)  (1 slots) =====
//  [ 0] VA(0x004daff0, 0x3d)  void * MIDIWrap::scalar_dtor(unsigned int)   <- introduces virtual

// ===== vtable sample (root)  (1 slots) =====
//  [ 0] VA(0x004daef0, 0x41)  void * sample::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(sample, 0x004ebab4);
VTBL(MIDIWrap, 0x004ebab8);
