#ifndef HOMM2_PLATFORM_STRINGS_H
#define HOMM2_PLATFORM_STRINGS_H

#include <Ints.h>

namespace platform {

// The game grew up on the Microsoft C runtime, which had these and the
// standard one does not.
i32 CompareIgnoringCase(const char* left, const char* right);
i32 CompareIgnoringCase(const char* left, const char* right, u32 count);
char* Reverse(char* text);

}

#endif
