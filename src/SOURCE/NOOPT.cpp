#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <PLATFORM/Runtime.h>

void DelayTil(i32* endTime) {
    while (platform::TickDeadlinePending(*endTime, platform::Ticks())) {
        platform::PumpEvents();
        PollSound();
    }
}

void DelayMilli(i32l delay) {
    DelayTilMilli(platform::Ticks() + delay);
}

void DelayTilMilli(i32l endTime) {
    while (platform::TickDeadlinePending(endTime, platform::Ticks())) {
        platform::PumpEvents();
        PollSound();
    }
}
