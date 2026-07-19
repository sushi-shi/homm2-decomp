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

#ifndef NULL
#define NULL 0
#endif

// Boolean-int aliases for the pre-bool compiler.
typedef i32 b32;
typedef i8 b8;
#ifndef __clang__
#define true 1
#define false 0
#endif

// Strict Clang builds type-check domains; production keeps the integer ABI.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define H2_ENUM_BEGIN(name) typedef enum name {
#define H2_ENUM_END(name)                                                                          \
    }                                                                                              \
    name;
#define H2_ENUM_CLASS_BEGIN(name) enum class name : i32 {
#define H2_ENUM_CLASS_END(name)                                                                    \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
#define H2_ENUM_CLASS_BEGIN_T(name, storage) enum class name : storage {
#define H2_ENUM_CLASS_END_T(name, storage)                                                         \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
#define H2_ENUM_FLAGS(name)                                                                        \
    inline constexpr name operator|(name a, name b) {                                              \
        return static_cast<name>(static_cast<i64>(a) | static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator&(name a, name b) {                                              \
        return static_cast<name>(static_cast<i64>(a) & static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator^(name a, name b) {                                              \
        return static_cast<name>(static_cast<i64>(a) ^ static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name operator~(name a) {                                                      \
        return static_cast<name>(~static_cast<i64>(a));                                            \
    }                                                                                              \
    inline constexpr name& operator|=(name& a, name b) {                                           \
        return a = a | b;                                                                          \
    }                                                                                              \
    inline constexpr name& operator&=(name& a, name b) {                                           \
        return a = a & b;                                                                          \
    }                                                                                              \
    inline constexpr name& operator^=(name& a, name b) {                                           \
        return a = a ^ b;                                                                          \
    }                                                                                              \
    inline constexpr name operator-(name a, name b) {                                              \
        return static_cast<name>(static_cast<i64>(a) - static_cast<i64>(b));                       \
    }                                                                                              \
    inline constexpr name& operator-=(name& a, name b) {                                           \
        return a = a - b;                                                                          \
    }                                                                                              \
    inline constexpr bool operator!(name a) {                                                      \
        return !static_cast<i64>(a);                                                               \
    }
#else
#define H2_ENUM_BEGIN(name) enum {
#define H2_ENUM_END(name)                                                                          \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define H2_ENUM_CLASS_BEGIN(name) enum {
#define H2_ENUM_CLASS_END(name)                                                                    \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define H2_ENUM_CLASS_BEGIN_T(name, storage) enum {
#define H2_ENUM_CLASS_END_T(name, storage)                                                         \
    }                                                                                              \
    ;                                                                                              \
    typedef storage name;
#define H2_ENUM_FLAGS(name)
#endif

// Table lookup by semantic domain: IDX spells the value-as-index conversion at
// the site. Production expands to the bare value, so bytes cannot change.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define IDX(x) static_cast<i32>(x)
#else
#define IDX(x) (x)
#endif

// Flag-domain membership test usable in integer/boolean context. The flags
// argument and bit share one H2_ENUM_FLAGS domain; production expands to
// the plain bitwise AND.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define HAS(flags, bit) (IDX((flags) & (bit)))
#else
#define HAS(flags, bit) ((flags) & (bit))
#endif

// Bit-index to mask: the domain value is a shift count. Production expands to
// the plain shift.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define BIT(x) (1 << IDX(x))
#else
#define BIT(x) (1 << (x))
#endif

// Sequence domains step to adjacent ids (animation followthroughs). The audit
// defines the increment; production sees plain integer arithmetic.
#ifdef HOMM2_STRICT_ENUM_TYPES
#define H2_ENUM_STEPPED(name)                                                                      \
    inline name& operator++(name& a) {                                                             \
        return a = static_cast<name>(static_cast<i64>(a) + 1);                                     \
    }                                                                                              \
    inline name operator++(name& a, int) {                                                         \
        name old = a;                                                                              \
        ++a;                                                                                       \
        return old;                                                                                \
    }
#else
#define H2_ENUM_STEPPED(name)
#endif

#endif
