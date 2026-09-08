#ifndef HOMM2_SAVE_ARCHIVE_H
#define HOMM2_SAVE_ARCHIVE_H

#include <BASE/Utf8.h>
#include <Ints.h>

#include <bit>
#include <cstring>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace savegame {

inline constexpr std::size_t MaximumFileBytes = 64 * 1024 * 1024;

class FormatError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Writer {
public:
    static constexpr bool Reading = false;
    std::vector<u8> bytes;

    void Raw(std::span<const u8> data) {
        if (data.size() > MaximumFileBytes - bytes.size())
            throw FormatError("save exceeds the size limit");
        bytes.insert(bytes.end(), data.begin(), data.end());
    }

    void Word(u32 value) {
        const u8 data[] = {static_cast<u8>(value), static_cast<u8>(value >> 8),
                           static_cast<u8>(value >> 16), static_cast<u8>(value >> 24)};
        Raw(data);
    }

    // Native integer fields always occupy four little-endian bytes. Neither
    // the runtime field's width nor compiler packing changes the file layout.
    template <typename T> requires std::is_integral_v<T>
    void Field(const T& value) {
        if constexpr (std::is_signed_v<T> && sizeof(T) > sizeof(i32)) {
            if (value < std::numeric_limits<i32>::min() || value > std::numeric_limits<i32>::max())
                throw FormatError("integer is outside the native save range");
        } else if constexpr (!std::is_signed_v<T> && sizeof(T) > sizeof(u32)) {
            if (value > std::numeric_limits<u32>::max())
                throw FormatError("integer is outside the native save range");
        }
        Word(static_cast<u32>(value));
    }

    void Field(const float& value) { Word(std::bit_cast<u32>(value)); }

    template <typename Enum, typename Storage>
    void Field(const H2EnumStorage<Enum, Storage>& value) { Field(static_cast<Storage>(value.value())); }
    template <typename Enum, typename Storage>
    void Field(const H2SteppedEnumStorage<Enum, Storage>& value) { Field(static_cast<Storage>(value)); }
    template <typename Code, typename Storage>
    void Field(const H2OpenCodeStorage<Code, Storage>& value) { Field(value.value()); }

    template <typename T, std::size_t N>
    void Field(const T (&values)[N]) {
        for (const T& value : values) Field(value);
    }

    template <typename... T>
    void Fields(const T&... values) { (Field(values), ...); }

    template <typename Enum, typename Decoder>
    void EnumField(const Enum& value, Decoder) { Field(static_cast<std::underlying_type_t<Enum>>(value)); }

    void Text(const std::string& value) {
        if (value.size() > 1024 * 1024) throw FormatError("text exceeds the save format limit");
        if (value.find('\0') != std::string::npos || !utf8::IsValid(value.c_str()))
            throw FormatError("save text is not valid UTF-8");
        Word(static_cast<u32>(value.size()));
        Raw({reinterpret_cast<const u8*>(value.data()), value.size()});
    }

    template <std::size_t N>
    void Text(const char (&value)[N]) {
        const char* end = static_cast<const char*>(std::memchr(value, 0, N));
        if (end == nullptr) throw FormatError("unterminated runtime text");
        Text(std::string(value, end));
    }
};

class Reader {
public:
    static constexpr bool Reading = true;

    explicit Reader(std::span<const u8> bytes) : m_bytes(bytes) {
        if (bytes.size() > MaximumFileBytes) throw FormatError("save exceeds the size limit");
    }

    std::size_t Remaining() const { return m_bytes.size() - m_position; }

    std::span<const u8> Raw(std::size_t count) {
        if (count > Remaining()) throw FormatError("truncated save");
        const auto result = m_bytes.subspan(m_position, count);
        m_position += count;
        return result;
    }

    u32 Word() {
        const auto data = Raw(4);
        return static_cast<u32>(data[0]) | (static_cast<u32>(data[1]) << 8)
            | (static_cast<u32>(data[2]) << 16) | (static_cast<u32>(data[3]) << 24);
    }

    template <typename T> requires std::is_integral_v<T>
    void Field(T& value) {
        const u32 word = Word();
        if constexpr (std::is_signed_v<T>) {
            const i32 signedWord = std::bit_cast<i32>(word);
            if constexpr (sizeof(T) < sizeof(i32)) {
                if (signedWord < std::numeric_limits<T>::min() || signedWord > std::numeric_limits<T>::max())
                    throw FormatError("integer does not fit the runtime field");
            }
            value = static_cast<T>(signedWord);
        } else {
            if constexpr (sizeof(T) < sizeof(u32)) {
                if (word > std::numeric_limits<T>::max())
                    throw FormatError("integer does not fit the runtime field");
            }
            value = static_cast<T>(word);
        }
    }

    void Field(float& value) { value = std::bit_cast<float>(Word()); }

    template <typename Enum, typename Storage>
    void Field(H2EnumStorage<Enum, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }
    template <typename Enum, typename Storage>
    void Field(H2SteppedEnumStorage<Enum, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }
    template <typename Code, typename Storage>
    void Field(H2OpenCodeStorage<Code, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }

    template <typename T, std::size_t N>
    void Field(T (&values)[N]) {
        for (std::size_t index = 0; index < N; ++index) {
            if constexpr (std::is_array_v<T>) {
                Field(values[index]);
            } else {
                T value{};
                Field(value);
                values[index] = value;
            }
        }
    }

    template <typename... T>
    void Fields(T&... values) { (Field(values), ...); }

    template <typename Enum, typename Decoder>
    void EnumField(Enum& value, Decoder decode) {
        std::underlying_type_t<Enum> code{};
        Field(code);
        value = decode(code);
    }

    void Text(std::string& value, std::size_t maximum = 1024 * 1024) {
        const u32 count = Word();
        if (count > maximum) throw FormatError("text exceeds the save format limit");
        const auto data = Raw(count);
        value.assign(reinterpret_cast<const char*>(data.data()), data.size());
        if (value.find('\0') != std::string::npos || !utf8::IsValid(value.c_str()))
            throw FormatError("save text is not valid UTF-8");
    }

    template <std::size_t N>
    void Text(char (&value)[N]) {
        std::string text;
        Text(text, N - 1);
        std::memcpy(value, text.c_str(), text.size() + 1);
    }

    void Finish() const {
        if (Remaining() != 0) throw FormatError("unexpected data after save payload");
    }

private:
    std::span<const u8> m_bytes;
    std::size_t m_position = 0;
};

}

#endif
