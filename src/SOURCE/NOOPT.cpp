// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\NOOPT.OBJ   from: (directly linked into exe)
// functions: 3   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>

VA(0x00435200, 0x35)
void DelayTil(i32 *endTime)
{
    while (KBTickCount() < *endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}

VA(0x00435235, 0x25)
void DelayMilli(i32l delay)
{
    DelayTilMilli(KBTickCount() + delay);
}

VA(0x0043525a, 0x33)
void DelayTilMilli(i32l endTime)
{
    while (KBTickCount() < endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}
