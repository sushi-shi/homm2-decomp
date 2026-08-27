
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


typedef long i32l;
typedef unsigned long u32l;


typedef i32 b32;
typedef i8 b8;
typedef char bchar;

#define H2_FINAL final


template <typename T>
inline constexpr bool H2IsMaskLike = std::is_integral_v<T> || std::is_enum_v<T>;

template <typename T>
constexpr long long H2MaskValue(T value) {
    return static_cast<long long>(value);
}


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


    constexpr i64 value() const { return static_cast<i64>(m_value); }

private:
    Storage m_value;
};

template <typename Code, typename Storage>
class H2OpenCodeStorage {
public:
    H2OpenCodeStorage() = default;
    constexpr H2OpenCodeStorage(Code value)
        : m_value(static_cast<Storage>(static_cast<i32>(value))) {}
    constexpr H2OpenCodeStorage(Storage value) : m_value(value) {}

    template <typename OtherStorage>
    constexpr H2OpenCodeStorage(H2OpenCodeStorage<Code, OtherStorage> value)
        : m_value(static_cast<Storage>(value.value())) {}

    constexpr operator Code() const { return Code(static_cast<i32>(m_value)); }
    constexpr operator i32() const { return static_cast<i32>(m_value); }
    constexpr Storage value() const { return m_value; }

    H2OpenCodeStorage& operator=(Code value) {
        m_value = static_cast<Storage>(static_cast<i32>(value));
        return *this;
    }

    H2OpenCodeStorage& operator=(Storage value) {
        m_value = value;
        return *this;
    }

    template <typename OtherStorage>
    H2OpenCodeStorage& operator=(H2OpenCodeStorage<Code, OtherStorage> value) {
        m_value = static_cast<Storage>(value.value());
        return *this;
    }

private:
    Storage m_value;
};

template <typename Code, typename Storage>
constexpr bool operator==(H2OpenCodeStorage<Code, Storage> lhs, Code rhs) {
    return static_cast<i32>(lhs) == static_cast<i32>(rhs);
}

template <typename Code, typename Storage>
constexpr bool operator==(Code lhs, H2OpenCodeStorage<Code, Storage> rhs) {
    return rhs == lhs;
}

template <typename Code, typename Storage>
constexpr bool operator!=(H2OpenCodeStorage<Code, Storage> lhs, Code rhs) {
    return !(lhs == rhs);
}

template <typename Code, typename Storage>
constexpr bool operator!=(Code lhs, H2OpenCodeStorage<Code, Storage> rhs) {
    return !(lhs == rhs);
}

template <typename Code, typename LeftStorage, typename RightStorage>
constexpr bool operator==(
    H2OpenCodeStorage<Code, LeftStorage> lhs, H2OpenCodeStorage<Code, RightStorage> rhs
) {
    return static_cast<i32>(lhs) == static_cast<i32>(rhs);
}

template <typename Code, typename LeftStorage, typename RightStorage>
constexpr bool operator!=(
    H2OpenCodeStorage<Code, LeftStorage> lhs, H2OpenCodeStorage<Code, RightStorage> rhs
) {
    return !(lhs == rhs);
}

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


#define DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(op)                                                   \
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

DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(+)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(-)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(*)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(/)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(%)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(<)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(>)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(<=)
DEFINE_ENUM_STORAGE_INTEGER_OPERATOR(>=)

#undef DEFINE_ENUM_STORAGE_INTEGER_OPERATOR


#define DEFINE_ENUM_STORAGE_BITWISE_OPERATOR(op)                                                   \
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

DEFINE_ENUM_STORAGE_BITWISE_OPERATOR(&)
DEFINE_ENUM_STORAGE_BITWISE_OPERATOR(|)
DEFINE_ENUM_STORAGE_BITWISE_OPERATOR(^)

#undef DEFINE_ENUM_STORAGE_BITWISE_OPERATOR


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

#define H2EnumIndex(value) static_cast<i32>(value)


#define MAYBE_UNUSED_INLINE [[maybe_unused]] inline

#define ENABLE_ENUM_FLAGS(name)                                                                    \
    MAYBE_UNUSED_INLINE constexpr name operator|(name a, name b) {                                          \
        return static_cast<name>(static_cast<i32>(a) | static_cast<i32>(b));                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator&(name a, name b) {                                          \
        return static_cast<name>(static_cast<i32>(a) & static_cast<i32>(b));                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator^(name a, name b) {                                          \
        return static_cast<name>(static_cast<i32>(a) ^ static_cast<i32>(b));                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator~(name a) {                                                  \
        return static_cast<name>(~static_cast<i32>(a));                                            \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr bool operator!(name a) {                                                  \
        return !static_cast<i32>(a);                                                               \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator|=(name& a, name b) {                                                 \
        return a = a | b;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator&=(name& a, name b) {                                                 \
        return a = a & b;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator^=(name& a, name b) {                                                 \
        return a = a ^ b;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator+(name a, name b) {                                          \
        return static_cast<name>(static_cast<i32>(a) + static_cast<i32>(b));                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator-(name a, name b) {                                          \
        return static_cast<name>(static_cast<i32>(a) - static_cast<i32>(b));                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator+=(name& a, name b) {                                                 \
        return a = a + b;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator-=(name& a, name b) {                                                 \
        return a = a - b;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator&(name a, i32 mask) {                                        \
        return static_cast<name>(static_cast<i32>(a) & mask);                                      \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator&(i32 mask, name a) {                                        \
        return a & mask;                                                                           \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator|(name a, i32 mask) {                                        \
        return static_cast<name>(static_cast<i32>(a) | mask);                                      \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator|(i32 mask, name a) {                                        \
        return a | mask;                                                                           \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator&=(name& a, i32 mask) {                                               \
        return a = a & mask;                                                                       \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator|=(name& a, i32 mask) {                                               \
        return a = a | mask;                                                                       \
    }

#define ENABLE_ENUM_STEPS(name)                                                                    \
    MAYBE_UNUSED_INLINE constexpr name operator+(name a, i32 amount) {                                      \
        return static_cast<name>(static_cast<i32>(a) + amount);                                    \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator-(name a, i32 amount) {                                      \
        return static_cast<name>(static_cast<i32>(a) - amount);                                    \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr i32 operator-(name a, name b) {                                           \
        return static_cast<i32>(a) - static_cast<i32>(b);                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator%(name a, i32 modulus) {                                     \
        return static_cast<name>(static_cast<i32>(a) % modulus);                                   \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator%(name a, name modulus) {                                    \
        return static_cast<name>(static_cast<i32>(a) % static_cast<i32>(modulus));                 \
    }                                                                                              \
    MAYBE_UNUSED_INLINE constexpr name operator&(name a, i32 mask) {                                        \
        return static_cast<name>(static_cast<i32>(a) & mask);                                      \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator+=(name& a, i32 amount) {                                             \
        return a = a + amount;                                                                     \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator-=(name& a, i32 amount) {                                             \
        return a = a - amount;                                                                     \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator+=(name& a, name b) {                                                 \
        return a = a + static_cast<i32>(b);                                                        \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator-=(name& a, name b) {                                                 \
        return a = a - static_cast<i32>(b);                                                        \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator%=(name& a, i32 modulus) {                                            \
        return a = a % modulus;                                                                    \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator%=(name& a, name modulus) {                                           \
        return a = a % modulus;                                                                    \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator++(name& a) {                                                         \
        return a = a + 1;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name operator++(name& a, int) {                                                     \
        name old = a;                                                                              \
        a = a + 1;                                                                                 \
        return old;                                                                                \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name& operator--(name& a) {                                                         \
        return a = a - 1;                                                                          \
    }                                                                                              \
    MAYBE_UNUSED_INLINE name operator--(name& a, int) {                                                     \
        name old = a;                                                                              \
        a = a - 1;                                                                                 \
        return old;                                                                                \
    }

#define ENABLE_ENUM_INDEX_OFFSETS(name)                                                            \
    MAYBE_UNUSED_INLINE constexpr i32 operator+(name value, i32 offset) {                          \
        return static_cast<i32>(value) + offset;                                                   \
    }

#endif
