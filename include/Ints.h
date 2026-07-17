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

// Boolean-valued retail storage. MSVC 4.2 has no `bool`; retail flags are full
// ints (occasionally bytes). These aliases mark boolean intent without changing
// the ABI, and boolean fields must not be forced into enum domains. b32/b8
// state is written with true/false, defined here for the pre-bool compiler.
typedef i32 b32;
typedef i8 b8;
#ifndef __clang__
#define true 1
#define false 0
#endif

// Production keeps reconstructed semantic domains integer-mangled and ABI-neutral.
// The opt-in Clang audit sees scoped enums and rejects cross-domain data flow;
// `using enum` keeps the production unqualified-constant spelling compiling.
// The _T variants carry the storage type so field widths agree in both modes.
// HOMM2_ENUM_FLAGS declares the audit-only bitmask operators for a flag domain.
// HOMM2_ENUM_VALUES_* declare a named-number catalog (counts, sizes, geometry,
// thresholds): the audit keeps it an unscoped typed enum, so values still
// convert to integers but cross-domain comparisons diagnose. True semantic
// domains use HOMM2_ENUM_BEGIN and become scoped enums under the audit.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define HOMM2_ENUM_VALUES_BEGIN(name) typedef enum name {
#define HOMM2_ENUM_VALUES_END(name)                                                                \
    }                                                                                              \
    name;
#define HOMM2_ENUM_BEGIN(name) enum class name : i32 {
#define HOMM2_ENUM_END(name)                                                                       \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
#define HOMM2_ENUM_BEGIN_T(name, storage) enum class name : storage {
#define HOMM2_ENUM_END_T(name, storage)                                                            \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
#define HOMM2_ENUM_FLAGS(name)                                                                     \
    inline constexpr name operator|(name a, name b) {                                                        \
        return static_cast<name>(static_cast<i64>(a) | static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator&(name a, name b) {                                                        \
        return static_cast<name>(static_cast<i64>(a) & static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator^(name a, name b) {                                                        \
        return static_cast<name>(static_cast<i64>(a) ^ static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator~(name a) {                                                                \
        return static_cast<name>(~static_cast<i64>(a));                                            \
    }                                                                                              \
    inline constexpr name& operator|=(name& a, name b) {                                                     \
        return a = a | b;                                                                          \
    }                                                                                              \
    inline constexpr name& operator&=(name& a, name b) {                                                     \
        return a = a & b;                                                                          \
    }                                                                                              \
    inline constexpr name& operator^=(name& a, name b) {                                                     \
        return a = a ^ b;                                                                          \
    }
#else
#define HOMM2_ENUM_VALUES_BEGIN(name) enum {
#define HOMM2_ENUM_VALUES_END(name)                                                                \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define HOMM2_ENUM_BEGIN(name) enum {
#define HOMM2_ENUM_END(name)                                                                       \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define HOMM2_ENUM_BEGIN_T(name, storage) enum {
#define HOMM2_ENUM_END_T(name, storage)                                                            \
    }                                                                                              \
    ;                                                                                              \
    typedef storage name;
#define HOMM2_ENUM_FLAGS(name)
#endif

// Table lookup by semantic domain: IDX spells the value-as-index conversion at
// the site. Production expands to the bare value, so bytes cannot change.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define IDX(x) static_cast<i32>(x)
#else
#define IDX(x) (x)
#endif

// Flag-domain membership test usable in integer/boolean context. The flags
// argument and bit share one HOMM2_ENUM_FLAGS domain; production expands to
// the plain bitwise AND.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define HAS(flags, bit) IDX((flags) & (bit))
#else
#define HAS(flags, bit) ((flags) & (bit))
#endif

#endif // HOMM2_INTS_H
