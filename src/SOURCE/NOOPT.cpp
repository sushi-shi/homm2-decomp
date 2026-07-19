#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>

VA(0x00435200, 0x35)
void DelayTil(i32* endTime) {
    while (KBTickCount() < *endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}

VA(0x00435235, 0x25)
void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

VA(0x0043525a, 0x33)
void DelayTilMilli(i32l endTime) {
    while (KBTickCount() < endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}
