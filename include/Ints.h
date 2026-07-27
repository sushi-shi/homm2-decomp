#ifndef HOMM2_INTS_H
#define HOMM2_INTS_H

// Modern compilers use real enum domains; retail MSVC 4.2 retains its integer ABI.
// Derive this from the required language support rather than a compiler name or build flag.
#if defined(__cplusplus) && __cplusplus >= 202002L
#define H2_STRICT_ENUMS 1
#else
#define H2_STRICT_ENUMS 0
#endif

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
#if !H2_STRICT_ENUMS
#define true 1
#define false 0
#endif

// Modern builds type-check domains; the retail MSVC build keeps the integer ABI.
#if H2_STRICT_ENUMS
#define H2_ENUM_BEGIN(name) typedef enum name {
#define H2_ENUM_END(name)                                                                          \
    }                                                                                              \
    name;
#define H2_ENUM_CLASS_BEGIN(name) enum class name : i32 {
#define H2_ENUM_CLASS_FORWARD(name) enum class name : i32
#define H2_ENUM_CLASS_FORWARD_SPLIT(name, storage) enum class name : storage
#define H2_ENUM_CLASS_END(name)                                                                    \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
#define H2_ENUM_CLASS_BEGIN_T(name, storage) enum class name : storage {
#define H2_ENUM_CLASS_END_T(name, storage)                                                         \
    }                                                                                              \
    ;                                                                                              \
    using enum name;
// Keep strict builds type-safe without changing the retail domain or field storage.
#define H2_ENUM_CLASS_BEGIN_SPLIT(name, storage) enum class name : storage {
#define H2_ENUM_CLASS_END_SPLIT(name, storage)                                                     \
    }                                                                                              \
    ;                                                                                              \
    using enum name;

// A retail domain can appear in fields of several widths. Keep the exact field
// representation while presenting the domain type to strict-build expressions.
template <typename Enum, typename Storage>
class H2EnumStorage {
public:
    H2EnumStorage() = default;
    constexpr H2EnumStorage(Enum value) : m_value(static_cast<Storage>(value)) {}
    constexpr H2EnumStorage(Storage value) : m_value(value) {}

    template <typename OtherStorage>
    constexpr H2EnumStorage(H2EnumStorage<Enum, OtherStorage> value)
        : m_value(static_cast<Storage>(static_cast<Enum>(value))) {}

    constexpr operator Enum() const { return static_cast<Enum>(m_value); }

    template <typename Integer>
        requires(__is_integral(Integer))
    explicit constexpr operator Integer() const {
        return static_cast<Integer>(static_cast<Enum>(m_value));
    }

    H2EnumStorage& operator=(Enum value) {
        m_value = static_cast<Storage>(value);
        return *this;
    }

    H2EnumStorage& operator=(Storage value) {
        m_value = value;
        return *this;
    }

    template <typename OtherStorage>
    H2EnumStorage& operator=(H2EnumStorage<Enum, OtherStorage> value) {
        m_value = static_cast<Storage>(static_cast<Enum>(value));
        return *this;
    }

    H2EnumStorage& operator|=(Enum value) {
        m_value = static_cast<Storage>(m_value | static_cast<Storage>(value));
        return *this;
    }

    H2EnumStorage& operator&=(Enum value) {
        m_value = static_cast<Storage>(m_value & static_cast<Storage>(value));
        return *this;
    }

    H2EnumStorage& operator^=(Enum value) {
        m_value = static_cast<Storage>(m_value ^ static_cast<Storage>(value));
        return *this;
    }

private:
    Storage m_value;
};

template <typename Enum, typename Storage>
constexpr bool operator==(H2EnumStorage<Enum, Storage> lhs, Enum rhs) {
    return static_cast<Enum>(lhs) == rhs;
}

template <typename Enum, typename Storage>
constexpr bool operator==(Enum lhs, H2EnumStorage<Enum, Storage> rhs) {
    return lhs == static_cast<Enum>(rhs);
}

template <typename Enum, typename Storage>
constexpr bool operator!=(H2EnumStorage<Enum, Storage> lhs, Enum rhs) {
    return !(lhs == rhs);
}

template <typename Enum, typename Storage>
constexpr bool operator!=(Enum lhs, H2EnumStorage<Enum, Storage> rhs) {
    return !(lhs == rhs);
}

template <typename Enum, typename LeftStorage, typename RightStorage>
constexpr bool operator==(
    H2EnumStorage<Enum, LeftStorage> lhs, H2EnumStorage<Enum, RightStorage> rhs
) {
    return static_cast<Enum>(lhs) == static_cast<Enum>(rhs);
}

template <typename Enum, typename LeftStorage, typename RightStorage>
constexpr bool operator!=(
    H2EnumStorage<Enum, LeftStorage> lhs, H2EnumStorage<Enum, RightStorage> rhs
) {
    return !(lhs == rhs);
}

template <typename Enum, typename Storage>
class H2SteppedEnumStorage {
public:
    H2SteppedEnumStorage() = default;
    constexpr H2SteppedEnumStorage(Enum value) : m_value(static_cast<Storage>(value)) {}

    constexpr operator Enum() const { return static_cast<Enum>(m_value); }

    template <typename Integer>
        requires(__is_integral(Integer))
    explicit constexpr operator Integer() const {
        return static_cast<Integer>(static_cast<Enum>(m_value));
    }

    H2SteppedEnumStorage& operator=(Enum value) {
        m_value = static_cast<Storage>(value);
        return *this;
    }

    H2SteppedEnumStorage& operator=(Storage value) {
        m_value = value;
        return *this;
    }

    H2SteppedEnumStorage& operator++() {
        ++m_value;
        return *this;
    }

    H2SteppedEnumStorage operator++(i32) {
        H2SteppedEnumStorage previous = *this;
        ++m_value;
        return previous;
    }

    H2SteppedEnumStorage& operator--() {
        --m_value;
        return *this;
    }

    H2SteppedEnumStorage operator--(i32) {
        H2SteppedEnumStorage previous = *this;
        --m_value;
        return previous;
    }

    H2SteppedEnumStorage& operator+=(i32 amount) {
        m_value = static_cast<Storage>(m_value + amount);
        return *this;
    }

    H2SteppedEnumStorage& operator-=(i32 amount) {
        m_value = static_cast<Storage>(m_value - amount);
        return *this;
    }

private:
    Storage m_value;
};

template <typename Enum, typename Storage>
constexpr i32 H2EnumIndex(H2EnumStorage<Enum, Storage> value) {
    return static_cast<i32>(static_cast<Enum>(value));
}

template <typename Enum, typename Storage>
constexpr i32 H2EnumIndex(H2SteppedEnumStorage<Enum, Storage> value) {
    return static_cast<i32>(static_cast<Enum>(value));
}

template <typename Value>
constexpr i32 H2EnumIndex(Value value) {
    return static_cast<i32>(value);
}

#define H2_ENUM_STORAGE(name, storage) H2EnumStorage<name, storage>
#define H2_ENUM_STORAGE_STEPPED(name, storage) H2SteppedEnumStorage<name, storage>
#define H2_ENUM_BITFIELD(name, storage) name
#define H2_ENUM_PARAM(name, storage) name
#define H2_ENUM_RETURN(name, storage) name
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
#define H2_ENUM_CLASS_FORWARD(name) typedef i32 name
#define H2_ENUM_CLASS_FORWARD_SPLIT(name, storage) typedef i32 name
#define H2_ENUM_CLASS_END(name)                                                                    \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define H2_ENUM_CLASS_BEGIN_T(name, storage) enum {
#define H2_ENUM_CLASS_END_T(name, storage)                                                         \
    }                                                                                              \
    ;                                                                                              \
    typedef storage name;
// The enum domain remains i32 in retail builds; each field retains its proven storage.
#define H2_ENUM_CLASS_BEGIN_SPLIT(name, storage) enum {
#define H2_ENUM_CLASS_END_SPLIT(name, storage)                                                     \
    }                                                                                              \
    ;                                                                                              \
    typedef i32 name;
#define H2_ENUM_STORAGE(name, storage) storage
#define H2_ENUM_STORAGE_STEPPED(name, storage) storage
#define H2_ENUM_BITFIELD(name, storage) storage
#define H2_ENUM_PARAM(name, storage) storage
#define H2_ENUM_RETURN(name, storage) storage
#define H2_ENUM_FLAGS(name)
#endif

// Preserve a retail integer assignment chain while allowing strict enum views
// of individual fields to receive the same value without cross-domain casts at
// the reconstructed call site.
#if H2_STRICT_ENUMS
#define H2_ENUM_ASSIGN_CHAIN_5(a, b, c, d, e, value)                                               \
    do {                                                                                           \
        (e) = static_cast<decltype(e)>(value);                                                     \
        (d) = static_cast<decltype(d)>(value);                                                     \
        (c) = static_cast<decltype(c)>(value);                                                     \
        (b) = static_cast<decltype(b)>(value);                                                     \
        (a) = static_cast<decltype(a)>(value);                                                     \
    } while (0)
#else
#define H2_ENUM_ASSIGN_CHAIN_5(a, b, c, d, e, value) ((a) = (b) = (c) = (d) = (e) = (value))
#endif

// Decode a semantic enum from a masked packed-storage field. Retail retains
// the original two-step /Od assignment shape.
#if H2_STRICT_ENUMS
#define H2_ENUM_DECODE_MASKED(type, target, raw, mask)                                             \
    do {                                                                                           \
        (target) = static_cast<type>((raw) & (mask));                                              \
    } while (0)
#else
#define H2_ENUM_DECODE_MASKED(type, target, raw, mask)                                             \
    do {                                                                                           \
        (target) = (raw);                                                                          \
        (target) &= (mask);                                                                        \
    } while (0)
#endif

// Table lookup by semantic domain: IDX spells the value-as-index conversion at
// the site. Production expands to the bare value, so bytes cannot change.
#if H2_STRICT_ENUMS
#define IDX(x) H2EnumIndex(x)
#else
#define IDX(x) (x)
#endif

// Flag-domain membership test usable in integer/boolean context. The flags
// argument and bit share one H2_ENUM_FLAGS domain; production expands to
// the plain bitwise AND.
#if H2_STRICT_ENUMS
#define HAS(flags, bit) (IDX((flags) & (bit)))
#else
#define HAS(flags, bit) ((flags) & (bit))
#endif

// Bit-index to mask: the domain value is a shift count. Production expands to
// the plain shift.
#if H2_STRICT_ENUMS
#define BIT(x) (1 << IDX(x))
#else
#define BIT(x) (1 << (x))
#endif

// Sequence domains step to adjacent ids (animation followthroughs). The audit
// defines the increment; production sees plain integer arithmetic.
#if H2_STRICT_ENUMS
#define H2_ENUM_STEPPED(name)                                                                      \
    inline constexpr name operator+(name a, i32 amount) {                                          \
        return static_cast<name>(static_cast<i64>(a) + amount);                                    \
    }                                                                                              \
    inline constexpr name operator-(name a, i32 amount) {                                          \
        return static_cast<name>(static_cast<i64>(a) - amount);                                    \
    }                                                                                              \
    inline constexpr name operator%(name a, i32 modulus) {                                         \
        return static_cast<name>(static_cast<i64>(a) % modulus);                                   \
    }                                                                                              \
    inline constexpr name operator%(name a, name modulus) {                                        \
        return static_cast<name>(static_cast<i64>(a) % static_cast<i64>(modulus));                  \
    }                                                                                              \
    inline constexpr name operator&(name a, i32 mask) {                                            \
        return static_cast<name>(static_cast<i64>(a) & mask);                                      \
    }                                                                                              \
    inline name& operator+=(name& a, i32 amount) {                                                 \
        return a = a + amount;                                                                     \
    }                                                                                              \
    inline name& operator-=(name& a, i32 amount) {                                                 \
        return a = a - amount;                                                                     \
    }                                                                                              \
    inline name& operator++(name& a) {                                                             \
        return a = static_cast<name>(static_cast<i64>(a) + 1);                                     \
    }                                                                                              \
    inline name operator++(name& a, int) {                                                         \
        name old = a;                                                                              \
        ++a;                                                                                       \
        return old;                                                                                \
    }                                                                                              \
    inline name& operator--(name& a) {                                                             \
        return a = static_cast<name>(static_cast<i64>(a) - 1);                                     \
    }                                                                                              \
    inline name operator--(name& a, int) {                                                         \
        name old = a;                                                                              \
        --a;                                                                                       \
        return old;                                                                                \
    }                                                                                              \
    inline name& operator%=(name& a, i32 modulus) {                                                \
        return a = a % modulus;                                                                    \
    }                                                                                              \
    inline name& operator%=(name& a, name modulus) {                                               \
        return a = a % modulus;                                                                    \
    }
#else
#define H2_ENUM_STEPPED(name)
#endif

#endif
