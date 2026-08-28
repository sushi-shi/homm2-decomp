#include <PLATFORM/Runtime.h>

#include <cstdio>

namespace {

bool Expect(bool actual, bool expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(stderr, "tick deadline mismatch: %s\n", description);
    return false;
}

}

int main() {
    bool valid = true;
    valid &= Expect(platform::TickDeadlinePending(150, 100), true, "ordinary future");
    valid &= Expect(platform::TickDeadlinePending(100, 150), false, "ordinary past");
    valid &= Expect(platform::TickDeadlinePending(100, 100), false, "equal");
    valid &= Expect(
        platform::TickDeadlinePending(0x80000020UL, 0x7ffffff0UL),
        true,
        "signed boundary future"
    );
    valid &= Expect(
        platform::TickDeadlinePending(0x7ffffff0UL, 0x80000020UL),
        false,
        "signed boundary past"
    );
    valid &= Expect(
        platform::TickDeadlinePending(0x00000020UL, 0xfffffff0UL),
        true,
        "counter wrap future"
    );
    valid &= Expect(
        platform::TickDeadlinePending(0xfffffff0UL, 0x00000020UL),
        false,
        "counter wrap past"
    );
    valid &= Expect(platform::TickDeadlineExpired(100, 150), true, "ordinary expired");
    valid &= Expect(platform::TickDeadlineExpired(150, 100), false, "ordinary unexpired");
    valid &= Expect(platform::TickDeadlineExpired(100, 100), false, "equal not expired");
    valid &= Expect(
        platform::TickDeadlineExpired(0x7ffffff0UL, 0x80000020UL),
        true,
        "signed boundary expired"
    );
    valid &= Expect(
        platform::TickDeadlineExpired(0xfffffff0UL, 0x00000020UL),
        true,
        "counter wrap expired"
    );
    return valid ? 0 : 1;
}
