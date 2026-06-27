// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\MOUSEMGR.OBJ   from: .\basewin.lib
// functions: 17   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004c9270, 0xd9)
// void mouseManager::constructor(void);

VA(0x004c9350, 0x94)
// int mouseManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

VA(0x004c93f0, 0xed)
// void mouseManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

VA(0x004c94e0, 0x5)
// int mouseManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

VA(0x004c94f0, 0x135)
// void mouseManager::SetPointer(char *, int, int);

VA(0x004c9630, 0x405)
// void mouseManager::SetPointer(int);

VA(0x004c9a40, 0x47a)
// void mouseManager::NewUpdate(int);

VA(0x004c9ec0, 0x56)
// void mouseManager::MouseCoords(int &, int &);

VA(0x004c9f20, 0xa2)
// void mouseManager::SaveAndDraw(void);

VA(0x004c9fd0, 0x29)
// void mouseManager::RestoreUnderlying(void);

VA(0x004ca000, 0x2c)
// void mouseManager::ReallyHidePointer(void);

VA(0x004ca030, 0xb9)
// void mouseManager::ReallyShowPointer(void);

VA(0x004ca0f0, 0x1a)
// void mouseManager::HideColorPointer(void);

VA(0x004ca110, 0x98)
// void mouseManager::ShowColorPointer(void);

VA(0x004ca1b0, 0xc)
// int mouseManager::IsVis(void);

VA(0x004ca1c0, 0x66)
// void mouseManager::CheckUpdateMousePos(void);

VA(0x004ca230, 0x191)
// void mouseManager::SetColorMice(int);

// ---- data / globals / vtables ----
DATA(0x004eba00)  // const mouseManager::vftable
DATA(0x0051ebc8)  // int * iMouseOffset
DATA(0x0051ebd8)  // signed char (*)[2] iMouseSize
DATA(0x0051ec98)  // signed char (*)[2] iHotSpot
DATA(0x0051ed58)  // int gbInSetPointer
DATA(0x0051ed5c)  // int bInNewMouseUpdate
DATA(0x00533240)  // struct tagBITMAP * bmpAndMask
DATA(0x00533b58)  // void * * hMouseCursor
DATA(0x00533cd8)  // void * * cAndBits
DATA(0x00533e58)  // void * * cColorBits
DATA(0x00533fe0)  // struct _ICONINFO * IconInfo
DATA(0x00534760)  // void * * hbmpAndMask

// ===== vtable mouseManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c9350, 0x94)  int mouseManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c93f0, 0xed)  void mouseManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c94e0, 0x5)  int mouseManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
