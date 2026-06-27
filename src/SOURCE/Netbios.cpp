// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Netbios.obj   from: (directly linked into exe)
// functions: 5   data: 6
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004132f0, 0x155)
// signed char InitNetHost(void);

RVA(0x00413445, 0x1ab)
// signed char InitNetGuest(void);

RVA(0x004135f0, 0x5f)
// signed char WaitForHost(void);

RVA(0x0041364f, 0xe9)
// signed char WaitForGuest(void);

RVA(0x00413738, 0x1ba)
// int nbnet_init(void);

// ---- data / globals / vtables ----
DATA(0x004ee5c8)  // signed char iInitNetHostStatus
DATA(0x004ee5cc)  // signed char iInitNetGuestStatus
DATA(0x004ee5d0)  // int iNameRetryCount
DATA(0x004ee614)  // signed char iWaitForHostStatus
DATA(0x004ee658)  // signed char iWaitForGuestStatus
DATA(0x004ee65c)  // int iLastBroadcastTime
