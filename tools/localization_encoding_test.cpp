#include <BASE/Utf8.h>
#include <SOURCE/PluralRules.h>

#include <cstdio>
#include <cstring>
#include <string>

int main() {
    const char* text = reinterpret_cast<const char*>(u8"AéЯ猫");
    const std::uint32_t expected[] = {'A', 0x00e9, 0x042f, 0x732b};
    std::size_t offset = 0;
    for (std::uint32_t codePoint : expected) {
        const utf8::Decoded decoded = utf8::Decode(text + offset);
        if (!decoded.valid || decoded.codePoint != codePoint) {
            std::fputs("UTF-8 decode mismatch\n", stderr);
            return 1;
        }
        const std::size_t next = utf8::Next(text, offset);
        if (utf8::Previous(text, next) != offset) {
            std::fputs("UTF-8 boundary mismatch\n", stderr);
            return 1;
        }
        offset = next;
    }
    if (text[offset] != '\0') {
        std::fputs("UTF-8 traversal mismatch\n", stderr);
        return 1;
    }
    if (!utf8::IsValid(text) || utf8::IsValid("\xff")) {
        std::fputs("UTF-8 validation mismatch\n", stderr);
        return 1;
    }

    char truncated[5];
    if (utf8::Copy(truncated, sizeof(truncated), "A\xd0\xaf\xd0\x91") != 3
        || std::strcmp(truncated, "A\xd0\xaf") != 0) {
        std::fputs("UTF-8 bounded copy mismatch\n", stderr);
        return 1;
    }

    char encoded[4];
    const std::size_t encodedLength = utf8::Encode(0x732b, encoded);
    if (encodedLength != 3 || std::memcmp(encoded, text + offset - 3, 3) != 0) {
        std::fputs("UTF-8 encode mismatch\n", stderr);
        return 1;
    }

    char russian[] = "\xd1\x91\xd0\xb6";
    char western[] = "\xc3\xa9lan";
    char polish[] = "\xc4\x85" "cki";
    if (!utf8::UppercaseFirst(russian) || std::strcmp(russian, "\xd0\x81\xd0\xb6") != 0
        || !utf8::UppercaseFirst(western) || std::strcmp(western, "\xc3\x89lan") != 0
        || !utf8::UppercaseFirst(polish)
        || std::strcmp(polish, "\xc4\x84" "cki") != 0
        || !utf8::EqualIgnoringCase("\xd0\x81\xd0\xb6", "\xd1\x91\xd0\x96")) {
        std::fputs("Unicode case conversion mismatch\n", stderr);
        return 1;
    }

    const char invalid[] = "\xc0X";
    const utf8::Decoded malformed = utf8::Decode(invalid);
    if (malformed.valid || malformed.length != 1 || malformed.codePoint != 0xfffd) {
        std::fputs("invalid UTF-8 handling mismatch\n", stderr);
        return 1;
    }

    localization::plural::Rules rules;
    std::string error;
    if (!localization::plural::Parse(
            "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : "
            "n%10>=2 && n%10<=4 && (n%100<12 || n%100>14) ? 1 : 2);",
            rules,
            error
        )
        || localization::plural::Select(rules, 1) != 0
        || localization::plural::Select(rules, 2) != 1
        || localization::plural::Select(rules, 5) != 2
        || localization::plural::Select(rules, 11) != 2
        || localization::plural::Select(rules, 22) != 1) {
        std::fputs("catalog plural-rule mismatch\n", stderr);
        return 1;
    }
    if (!localization::plural::Parse("nplurals=1; plural=0;", rules, error)
        || localization::plural::Select(rules, 500) != 0
        || localization::plural::Parse("nplurals=2; plural=n/0;", rules, error)) {
        std::fputs("catalog plural-rule validation mismatch\n", stderr);
        return 1;
    }
    if (!localization::plural::Parse(
            "nplurals=3; plural=(n==1 ? 0 : n%10>=2 && n%10<=4 && "
            "(n%100<10 || n%100>=20) ? 1 : 2);",
            rules,
            error
        )
        || localization::plural::Select(rules, 1) != 0
        || localization::plural::Select(rules, 3) != 1
        || localization::plural::Select(rules, 12) != 2
        || localization::plural::Select(rules, 23) != 1) {
        std::fputs("Polish plural-rule mismatch\n", stderr);
        return 1;
    }
    return 0;
}
