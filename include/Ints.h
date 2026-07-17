// Ints.h - fixed-width integer aliases.
//
// Widths are this target's: 32-bit Win32 / MSVC 4.2, where `long` == `int` == 4
// bytes and `unsigned long` == `unsigned int` == 4 bytes. `i32`/`u32` preserve
// int-backed retail declarations. `i32l`/`u32l` preserve the smaller set of
// long-backed declarations whose distinct C++ type identity affects mangling and
// overload resolution. A future port may map both families to native 32-bit types.
//
// NOTE: the SDK's own aliases (BOOL/DWORD/WORD/BYTE/UINT/INT/LONG/...) are left
// as-is in our sources - they pin our externs to the real Win32 signatures.
// These aliases are ONLY for our raw int/long/unsigned/short/signed|unsigned
// char. Plain `char` (text) stays `char`.
#ifndef HOMM2_INTS_H
#define HOMM2_INTS_H

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

#endif // HOMM2_INTS_H
