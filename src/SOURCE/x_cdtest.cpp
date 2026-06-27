// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\x_cdtest.obj   from: (directly linked into exe)
// functions: 6   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x00449f80, 0x3b)
// int CDTest_Init(void);

VA(0x00449fbb, 0x33)
// void CDTest_Cleanup(void);

VA(0x00449fee, 0x26)
// int CDTest_VerifyCDQuick(void);

VA(0x0044a014, 0x4d)
// int CDTest_VerifyCDThorough(void);

VA(0x0044a061, 0x8f)
// int CDTest_VerifyTrack(int);

VA(0x0044a0f0, 0x2ce)
// int CDTest_GenerateTable(char *);

// ---- data / globals / vtables ----
DATA(0x004f5188)  // struct sCDTest_Track_Data * cdTestTrackData
