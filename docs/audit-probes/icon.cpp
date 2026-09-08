// Diagnostic for the 2026-09-08 audit, not a passing regression test.
// Uses the existing parity shim and production Icon2b.cpp.
#include <BASE/IconEntry.h>
#include <BASE/IconDraw.h>
#include <cstring>

extern "C" void h2cxx_icon_to_bitmap(
    unsigned char*, unsigned char*, int, int, int, int, int,
    int, int, int, int, int);

int main(int argc, char**) {
    const bool complete = argc > 1;
    auto* body = new u8[sizeof(IconEntry) + (complete ? 1 : 0)]{};
    IconEntry entry{};
    entry.w = entry.h = 1;
    entry.srcOffset = sizeof(IconEntry);
    std::memcpy(body, &entry, sizeof(entry));
    if (complete) body[sizeof(IconEntry)] = 0x80;
    u8 pixels[1]{};
    h2cxx_icon_to_bitmap(body, pixels, 1, 0, 0, 0,
                        static_cast<int>(ICON_DRAW_CLIP), 0, 0, 1, 1, 0);
    delete[] body;
}
