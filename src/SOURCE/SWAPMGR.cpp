// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SWAPMGR.OBJ   from: (directly linked into exe)
// functions: 13   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004543c0, 0x84)
// void swapManager::constructor(void);

RVA(0x00454444, 0x3e)
// void swapManager::constructor(class hero *, class hero *);

RVA(0x00454482, 0x50)
// void swapManager::Reset(void);

RVA(0x004544d2, 0x30)
// int swapManager::DrawSwapWin(void);

RVA(0x00454502, 0x3bc)
// int swapManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

RVA(0x004548be, 0x12d)
// void swapManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

RVA(0x004549eb, 0x1f8)
// void swapManager::DrawSelector(void);

RVA(0x00454be3, 0xaf0)
// int swapManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

RVA(0x004556d3, 0xa3)
// void swapManager::ViewMon(void);

RVA(0x00455776, 0x255)
// void swapManager::SwapArtifacts(void);

RVA(0x004559cb, 0x177)
// void swapManager::SwapMons(void);

RVA(0x00455b42, 0x47b)
// void swapManager::Update(void);

RVA(0x00455fbd, 0x388)
// void swapManager::SplitMons(void);

// ---- data / globals / vtables ----
DATA(0x004eb650)  // const swapManager::vftable

// ===== vtable swapManager : public baseManager  (3 slots) =====
//  [ 0] RVA(0x00454502, 0x3bc)  int swapManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] RVA(0x004548be, 0x12d)  void swapManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] RVA(0x00454be3, 0xaf0)  int swapManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
