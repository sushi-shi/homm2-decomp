#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>

VA(0x00479b00, 0x23)
void DelayTil(i32* endTime) {
    while (*endTime > KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
    }
}

VA(0x00479b23, 0x1a)
void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

VA(0x00479b3d, 0x21)
void DelayTilMilli(i32l endTime) {
    while (KBTickCount() < endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}
