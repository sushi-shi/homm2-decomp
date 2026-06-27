// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\SAMPLE.OBJ   from: .\basewin.lib
// functions: 8   data: 2
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004dad60, 0x181)
// void sample::constructor(char *, long int, long int, long int);

RVA(0x004daef0, 0x41)
// void * sample::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004daef0, 0x41)
// void * sample::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004daf40, 0x2c)
// void sample::~destructor(void);

RVA(0x004daf70, 0x72)
// void MIDIWrap::constructor(char *);

RVA(0x004daff0, 0x3d)
// void * MIDIWrap::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004daff0, 0x3d)
// void * MIDIWrap::scalar_dtor(unsigned int);   // virtual [introduces virtual]

RVA(0x004db030, 0x28)
// void MIDIWrap::~destructor(void);

// ---- data / globals / vtables ----
DATA(0x004ebab4)  // const sample::vftable
DATA(0x004ebab8)  // const MIDIWrap::vftable

// ===== vtable MIDIWrap (root)  (1 slots) =====
//  [ 0] RVA(0x004daff0, 0x3d)  void * MIDIWrap::scalar_dtor(unsigned int)   <- introduces virtual

// ===== vtable sample (root)  (1 slots) =====
//  [ 0] RVA(0x004daef0, 0x41)  void * sample::scalar_dtor(unsigned int)   <- introduces virtual
