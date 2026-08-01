#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>

void DelayTil(i32* endTime) {
    while (KBTickCount() < *endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}

void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

void DelayTilMilli(i32l endTime) {
    while (KBTickCount() < endTime) {
        Process1WindowsMessage();
        PollSound();
    }
}
