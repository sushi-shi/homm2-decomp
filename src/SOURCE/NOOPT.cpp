#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>

static b32 TickDeadlinePending(i32l deadline, i32l current) {
    u32l remaining = static_cast<u32l>(deadline)
        - static_cast<u32l>(current);
    return remaining != 0 && remaining < 0x80000000UL;
}

void DelayTil(i32* endTime) {
    while (TickDeadlinePending(*endTime, KBTickCount())) {
        Process1WindowsMessage();
        PollSound();
    }
}

void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

void DelayTilMilli(i32l endTime) {
    while (TickDeadlinePending(endTime, KBTickCount())) {
        Process1WindowsMessage();
        PollSound();
    }
}
