// Fixed-width integer aliases, and the typed storage used by enum fields.
//
// This is the clean-tree replacement for the matching tree's Ints.h. That
// version had to serve two masters: MSVC 4.2, where every enum domain must
// lower as a plain integer, and a strict audit build that type-checks them.
// It carried both behind a wall of H2_ENUM_* macros.
//
// The clean tree is generated in the strict configuration, so the macros are
// gone -- each domain is spelled as a real `enum class` at its point of
// declaration. What remains here is the part that is genuinely load-bearing:
// a storage wrapper that lets a struct field keep its retail width while still
// presenting the enum type to expressions. That matters because the game reads
// these structures straight out of .AGG archives and save files, so a field
// declared u8 must stay one byte.
#ifndef HOMM2_INTS_H
#define HOMM2_INTS_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

typedef std::int8_t i8;
typedef std::uint8_t u8;
typedef std::int16_t i16;
typedef std::uint16_t u16;
typedef std::int32_t i32;
typedef std::uint32_t u32;
typedef std::int64_t i64;
typedef std::uint64_t u64;

// `long` is a distinct type from `int` for overload resolution and name
// mangling even where both are 32 bits, so the places where retail's choice of
// `long` is load-bearing keep the built-in spelling.
typedef long i32l;
typedef unsigned long u32l;

// Integer-valued booleans. Retail predates `bool`, and these appear as struct
// fields whose width is part of the on-disk layout.
typedef i32 b32;
typedef i8 b8;

// Calling conventions remain part of the ABI on 32-bit x86. Keep source-owned
// callback and function-pointer types compatible with Win32 APIs, and preserve
// the stack contract of hand-written naked assembly. They collapse naturally
// on targets whose ABI has only one calling convention.
#if defined(_MSC_VER)
#define H2_CDECL __cdecl
#define H2_STDCALL __stdcall
#define H2_FASTCALL __fastcall
#define H2_PASCAL __pascal
#elif defined(__GNUC__) && defined(__i386__)
#define H2_CDECL __attribute__((cdecl))
#define H2_STDCALL __attribute__((stdcall))
#define H2_FASTCALL __attribute__((fastcall))
#define H2_PASCAL __attribute__((stdcall))
#else
#define H2_CDECL
#define H2_STDCALL
#define H2_FASTCALL
#define H2_PASCAL
#endif

// Values that can act as a mask or operand alongside a stored domain: plain
// integers, and other enum domains. Retail encodes several fields as one
// domain masked by constants belonging to a neighbouring domain -- a map
// object's type masked with the encoding constants sharing its byte -- so
// restricting these to integers alone would reject the game's own code.
template <typename T>
inline constexpr bool H2IsMaskLike = std::is_integral_v<T> || std::is_enum_v<T>;

template <typename T>
constexpr long long H2MaskValue(T value) {
    return static_cast<long long>(value);
}

// An enum-typed value stored in `Storage` bytes.
//
// This exists because the same domain is stored at different widths in
// different structures -- CreatureType appears as i8, u8, i16 and i32. Of the
// 67 wrapped domains, 43 use a single width and could be plain
// `enum class X : <width>` fields with no wrapper at all.
//
// Collapsing them needs an audit that has not been done: which structures are
// ever read or written as raw bytes -- from .AGG, from a .MP2 map, from a save,
// or over the network -- and therefore cannot change size or offsets? Note also
// that signedness carries meaning: CreatureType defines CREATURE_NONE = -1, so
// it can never become u8. See docs/clean-source.md, "the field-width audit".
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

    // Explicit on purpose. Making this implicit lets a stored domain convert to
    // an integer in any context, which was measured to break 15 more
    // translation units than it fixed: every expression that could go through
    // either the domain or an integer became ambiguous.
    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
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

    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
    H2EnumStorage& operator|=(Integer mask) {
        m_value = static_cast<Storage>(m_value | mask);
        return *this;
    }

    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
    H2EnumStorage& operator&=(Integer mask) {
        m_value = static_cast<Storage>(m_value & mask);
        return *this;
    }

    // Several domains are counters as much as they are domains -- a skill level
    // accumulates bonuses -- so they are stepped in place.
    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
    H2EnumStorage& operator+=(Integer amount) {
        m_value = static_cast<Storage>(m_value + H2MaskValue(amount));
        return *this;
    }

    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
    H2EnumStorage& operator-=(Integer amount) {
        m_value = static_cast<Storage>(m_value - H2MaskValue(amount));
        return *this;
    }

    H2EnumStorage& operator++() {
        ++m_value;
        return *this;
    }

    H2EnumStorage operator++(int) {
        H2EnumStorage previous = *this;
        ++m_value;
        return previous;
    }

    // Domains that double as small integers (side indices, counts) are compared
    // and combined with plain integers at a handful of sites.
    constexpr i64 value() const { return static_cast<i64>(m_value); }

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

// Arithmetic and ordering against mask-like values. Several domains index tables or
// step through adjacent ids, so these sites compare and offset directly.
#define H2_ENUM_STORAGE_INTEGER_OP(op)                                                             \
    template <typename Enum, typename Storage, typename Other,                                     \
              typename = std::enable_if_t<H2IsMaskLike<Other>>>                                    \
    constexpr auto operator op(H2EnumStorage<Enum, Storage> lhs, Other rhs) {                      \
        return lhs.value() op H2MaskValue(rhs);                                                    \
    }                                                                                              \
    template <typename Enum, typename Storage, typename Other,                                     \
              typename = std::enable_if_t<H2IsMaskLike<Other>>>                                    \
    constexpr auto operator op(Other lhs, H2EnumStorage<Enum, Storage> rhs) {                      \
        return H2MaskValue(lhs) op rhs.value();                                                    \
    }

H2_ENUM_STORAGE_INTEGER_OP(+)
H2_ENUM_STORAGE_INTEGER_OP(-)
H2_ENUM_STORAGE_INTEGER_OP(*)
H2_ENUM_STORAGE_INTEGER_OP(/)
H2_ENUM_STORAGE_INTEGER_OP(%)
H2_ENUM_STORAGE_INTEGER_OP(<)
H2_ENUM_STORAGE_INTEGER_OP(>)
H2_ENUM_STORAGE_INTEGER_OP(<=)
H2_ENUM_STORAGE_INTEGER_OP(>=)

#undef H2_ENUM_STORAGE_INTEGER_OP

// Bitwise combination of a stored domain with anything mask-like.
//
// The result is a value of the domain, which is what most sites want. The one
// exception is combining with the field's own storage type -- retail writes
// `i16 flags = m_flags | someI16` -- so that exact case gets its own overload
// returning the storage type. Partial ordering prefers it, and being explicit
// about the two cases beats a proxy that converts to either: that was tried,
// and made overload resolution ambiguous wherever the result met another
// operator.
#define H2_ENUM_STORAGE_BITWISE_OP(op)                                                             \
    template <typename Enum, typename Storage, typename Other,                                     \
              typename = std::enable_if_t<H2IsMaskLike<Other>>>                                    \
    constexpr Enum operator op(H2EnumStorage<Enum, Storage> lhs, Other rhs) {                      \
        return static_cast<Enum>(lhs.value() op H2MaskValue(rhs));                                 \
    }                                                                                              \
    template <typename Enum, typename Storage, typename Other,                                     \
              typename = std::enable_if_t<H2IsMaskLike<Other>>>                                    \
    constexpr Enum operator op(Other lhs, H2EnumStorage<Enum, Storage> rhs) {                      \
        return static_cast<Enum>(H2MaskValue(lhs) op rhs.value());                                 \
    }                                                                                              \
    template <typename Enum, typename Storage>                                                     \
    constexpr Storage operator op(H2EnumStorage<Enum, Storage> lhs, Storage rhs) {                 \
        return static_cast<Storage>(lhs.value() op static_cast<long long>(rhs));                   \
    }

H2_ENUM_STORAGE_BITWISE_OP(&)
H2_ENUM_STORAGE_BITWISE_OP(|)
H2_ENUM_STORAGE_BITWISE_OP(^)

#undef H2_ENUM_STORAGE_BITWISE_OP

// As above, for domains that are also stepped through with ++/--.
template <typename Enum, typename Storage>
class H2SteppedEnumStorage {
public:
    H2SteppedEnumStorage() = default;
    constexpr H2SteppedEnumStorage(Enum value) : m_value(static_cast<Storage>(value)) {}

    constexpr operator Enum() const { return static_cast<Enum>(m_value); }

    template <typename Integer, typename = std::enable_if_t<H2IsMaskLike<Integer>>>
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

    H2SteppedEnumStorage& operator++() { ++m_value; return *this; }
    H2SteppedEnumStorage operator++(int) {
        H2SteppedEnumStorage previous = *this;
        ++m_value;
        return previous;
    }

    H2SteppedEnumStorage& operator--() { --m_value; return *this; }
    H2SteppedEnumStorage operator--(int) {
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

// Table lookup by domain value: spells the value-as-index conversion at the
// site, for all three of the shapes a domain can arrive in.
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

#endif
