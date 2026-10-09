// Ints.h - fixed-width integer aliases.
//
// Widths are this target's: 32-bit Win32, where `long` == `int` == 4 bytes.
// i32l/u32l spell `long`/`unsigned long` where the retail type is long (the
// distinction is part of a decorated name). The SDK's own aliases
// (BOOL/DWORD/WORD/BYTE/...) stay as they are in our sources: they pin our
// externs to the real Win32 signatures.
#ifndef HOMM2_H2_INTS_H
#define HOMM2_H2_INTS_H

typedef signed char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long i32l;
typedef unsigned long u32l;
typedef __int64 i64;
typedef unsigned __int64 u64;

// Boolean-int aliases for retail's integer Boolean storage, results and
// parameters. VC6 has a real `bool` with real `true`/`false` keywords, and the
// distinction is byte-visible: an int-valued `c ? true : false` materialises a
// 32-bit temp (`xor reg,reg` before the `setcc`), a bool-valued one
// materialises a byte.
typedef i32 b32;
typedef i8 b8;
// Boolean storage whose proven retail C++ type is plain char. Unlike b8, this
// preserves decorated global-symbol identity as well as byte width.
typedef char bchar;

#endif // HOMM2_H2_INTS_H
