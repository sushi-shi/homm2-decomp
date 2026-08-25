#include <SOURCE/Localization.h>
#include <SOURCE/PluralRules.h>

#include <BASE/Utf8.h>
#include <PLATFORM/Platform.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace localization {
namespace {

constexpr std::uint32_t kMoMagic = 0x950412de;
constexpr std::uint32_t kMoSwappedMagic = 0xde120495;
constexpr std::uint32_t kMoHeaderSize = 7 * sizeof(std::uint32_t);
constexpr char kContextSeparator = '\x04';

struct Message {
    std::vector<std::string> sourceForms;
    std::vector<std::string> forms;
};

struct EnglishMessage {
    const char* id;
    const char* singular;
    const char* plural;
};

constexpr EnglishMessage kEnglishMessages[] = {
#define HOMM2_MESSAGE(id, english) {id, english, nullptr},
#define HOMM2_PLURAL(id, singular, plural) {id, singular, plural},
#include <locales/messages.def>
#undef HOMM2_PLURAL
#undef HOMM2_MESSAGE
};

struct Catalog {
    std::map<std::string, Message> messages;
    std::string language;
    std::string charset;
    std::string fontProfile;
    std::string resourceEncoding;
    plural::Rules pluralRules;
};

enum class ResourceEncoding {
    Western,
    Cyrillic,
    Utf8,
};

struct LegacyBinding {
    char** slot;
    char* english;
};

Catalog gCatalog;
std::string gLanguage = "en";
FontProfile gFontProfile = FontProfile::Latin;
ResourceEncoding gResourceEncoding = ResourceEncoding::Western;
bool gHasCatalog = false;
std::set<std::string> gWarnedFormatIds;
std::set<std::string> gWarnedMissingIds;
std::vector<LegacyBinding> gLegacyBindings;

std::uint32_t Swap(std::uint32_t value) {
    return ((value & 0x000000ffU) << 24) | ((value & 0x0000ff00U) << 8)
        | ((value & 0x00ff0000U) >> 8) | ((value & 0xff000000U) >> 24);
}

void Log(platform::LogLevel level, const std::string& message) {
    platform::Host().Log(level, message.c_str());
}

std::string Lower(std::string text) {
    for (char& character : text) {
        character = static_cast<char>(
            std::tolower(static_cast<unsigned char>(character))
        );
    }
    return text;
}

std::string Trim(std::string text) {
    const auto space = [](unsigned char character) { return std::isspace(character) != 0; };
    text.erase(text.begin(), std::find_if_not(text.begin(), text.end(), space));
    text.erase(std::find_if_not(text.rbegin(), text.rend(), space).base(), text.end());
    return text;
}

std::string NormalizeLanguage(std::string language) {
    language = Trim(std::move(language));
    const std::size_t encoding = language.find('.');
    if (encoding != std::string::npos) {
        language.erase(encoding);
    }
    const std::size_t modifier = language.find('@');
    if (modifier != std::string::npos) {
        language.erase(modifier);
    }
    for (char& character : language) {
        if (character == '_') {
            character = '-';
        } else {
            character = static_cast<char>(
                std::tolower(static_cast<unsigned char>(character))
            );
        }
    }
    language.erase(
        std::remove_if(
            language.begin(),
            language.end(),
            [](unsigned char character) {
                return !(std::isalnum(character) || character == '-');
            }
        ),
        language.end()
    );
    return language;
}

std::string CommandLineLanguage(const char* commandLine) {
    std::istringstream arguments(commandLine != nullptr ? commandLine : "");
    std::string argument;
    constexpr const char* prefix = "--language=";
    while (arguments >> argument) {
        if (argument.rfind(prefix, 0) == 0) {
            return argument.substr(std::char_traits<char>::length(prefix));
        }
    }
    return std::string();
}

std::string ReadLanguageSetting() {
    const std::filesystem::path path =
        std::filesystem::path(platform::Files().UserRoot()) / "language.cfg";
    std::ifstream stream(path);
    std::string language;
    std::getline(stream, language);
    return language;
}

std::string HostLanguage() {
    const char* names[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    for (const char* name : names) {
        const char* value = std::getenv(name);
        if (value != nullptr && *value != '\0') {
            return value;
        }
    }
    return std::string();
}

std::vector<std::filesystem::path> CatalogDirectories() {
    std::vector<std::filesystem::path> directories;
    if (const char* configured = std::getenv("HOMM2_LOCALE_PATH");
        configured != nullptr && *configured != '\0') {
        directories.emplace_back(configured);
    }

    const std::filesystem::path program = platform::Files().ProgramRoot();
    directories.push_back(program / "lang");
    directories.push_back(program / ".." / "share" / "homm2" / "lang");
#ifdef __EMSCRIPTEN__
    directories.emplace_back("/lang");
#else
    std::error_code error;
    const std::filesystem::path working = std::filesystem::current_path(error);
    if (!error) {
        directories.push_back(working / "lang");
        directories.push_back(working / "locales");
    }
#endif
    return directories;
}

bool RangePresent(std::size_t size, std::uint32_t offset, std::uint32_t length) {
    return offset <= size && length <= size - offset;
}

bool ReadMo(const std::filesystem::path& path, Catalog& catalog, std::string& error) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        error = "cannot open catalog";
        return false;
    }
    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>()
    );
    if (bytes.size() < kMoHeaderSize) {
        error = "catalog header is truncated";
        return false;
    }

    const auto rawWord = [&bytes](std::size_t offset) {
        const auto* data = reinterpret_cast<const unsigned char*>(bytes.data() + offset);
        return static_cast<std::uint32_t>(data[0])
            | (static_cast<std::uint32_t>(data[1]) << 8)
            | (static_cast<std::uint32_t>(data[2]) << 16)
            | (static_cast<std::uint32_t>(data[3]) << 24);
    };

    const std::uint32_t magic = rawWord(0);
    if (magic != kMoMagic && magic != kMoSwappedMagic) {
        error = "catalog has invalid magic";
        return false;
    }
    const bool swapped = magic == kMoSwappedMagic;
    const auto word = [&rawWord, swapped](std::size_t offset) {
        const std::uint32_t value = rawWord(offset);
        return swapped ? Swap(value) : value;
    };

    const std::uint32_t revision = word(4);
    const std::uint32_t count = word(8);
    const std::uint32_t originalTable = word(12);
    const std::uint32_t translatedTable = word(16);
    if (revision > 1) {
        error = "catalog revision is unsupported";
        return false;
    }
    const std::uint64_t tableLength = static_cast<std::uint64_t>(count) * 8;
    if (tableLength > UINT32_MAX
        || !RangePresent(bytes.size(), originalTable, static_cast<std::uint32_t>(tableLength))
        || !RangePresent(
            bytes.size(), translatedTable, static_cast<std::uint32_t>(tableLength)
        )) {
        error = "catalog string table is invalid";
        return false;
    }

    catalog = Catalog();
    std::string metadata;
    for (std::uint32_t index = 0; index < count; ++index) {
        const std::size_t originalEntry =
            static_cast<std::size_t>(originalTable) + static_cast<std::size_t>(index) * 8;
        const std::size_t translatedEntry =
            static_cast<std::size_t>(translatedTable) + static_cast<std::size_t>(index) * 8;
        const std::uint32_t originalLength = word(originalEntry);
        const std::uint32_t originalOffset = word(originalEntry + 4);
        const std::uint32_t translatedLength = word(translatedEntry);
        const std::uint32_t translatedOffset = word(translatedEntry + 4);
        if (!RangePresent(bytes.size(), originalOffset, originalLength)
            || !RangePresent(bytes.size(), translatedOffset, translatedLength)) {
            error = "catalog contains an out-of-range string";
            return false;
        }

        std::string original(bytes.data() + originalOffset, originalLength);
        std::string translated(bytes.data() + translatedOffset, translatedLength);
        if (original.empty()) {
            metadata = std::move(translated);
            continue;
        }

        const std::size_t contextEnd = original.find(kContextSeparator);
        std::string id;
        if (contextEnd != std::string::npos) {
            id = original.substr(0, contextEnd);
        } else {
            const std::size_t singularEnd = original.find('\0');
            id = original.substr(0, singularEnd);
        }
        if (id.empty()) {
            continue;
        }

        Message message;
        const std::size_t sourceStart =
            contextEnd == std::string::npos ? 0 : contextEnd + 1;
        std::size_t start = sourceStart;
        do {
            const std::size_t end = original.find('\0', start);
            message.sourceForms.emplace_back(
                original.substr(start, end == std::string::npos ? end : end - start)
            );
            if (end == std::string::npos) {
                break;
            }
            start = end + 1;
        } while (start <= original.size());

        start = 0;
        do {
            const std::size_t end = translated.find('\0', start);
            message.forms.emplace_back(
                translated.substr(start, end == std::string::npos ? end : end - start)
            );
            if (end == std::string::npos) {
                break;
            }
            start = end + 1;
        } while (start <= translated.size());
        for (const std::string& form : message.sourceForms) {
            if (!utf8::IsValid(form.c_str())) {
                error = "catalog contains invalid UTF-8 source text";
                return false;
            }
        }
        for (const std::string& form : message.forms) {
            if (!utf8::IsValid(form.c_str())) {
                error = "catalog contains invalid UTF-8 translated text";
                return false;
            }
        }
        catalog.messages[std::move(id)] = std::move(message);
    }

    std::istringstream fields(metadata);
    if (!utf8::IsValid(metadata.c_str())) {
        error = "catalog metadata is not valid UTF-8";
        return false;
    }
    std::string line;
    while (std::getline(fields, line)) {
        const std::size_t colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        const std::string name = Lower(Trim(line.substr(0, colon)));
        const std::string value = Trim(line.substr(colon + 1));
        if (name == "language") {
            catalog.language = NormalizeLanguage(value);
        } else if (name == "content-type") {
            const std::string lowered = Lower(value);
            const std::size_t marker = lowered.find("charset=");
            if (marker != std::string::npos) {
                catalog.charset = Trim(value.substr(marker + sizeof("charset=") - 1));
            }
        } else if (name == "x-homm2-font-profile") {
            catalog.fontProfile = Lower(value);
        } else if (name == "x-homm2-resource-encoding") {
            catalog.resourceEncoding = Lower(value);
        } else if (name == "plural-forms") {
            std::string pluralError;
            if (!plural::Parse(value, catalog.pluralRules, pluralError)) {
                error = pluralError;
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> LanguageFallbacks(const std::string& requested) {
    std::vector<std::string> result;
    if (!requested.empty()) {
        result.push_back(requested);
        const std::size_t separator = requested.find('-');
        if (separator != std::string::npos) {
            result.push_back(requested.substr(0, separator));
        }
    }
    return result;
}

bool LoadCatalog(const std::string& requested) {
    for (const std::string& language : LanguageFallbacks(requested)) {
        if (language == "en") {
            return false;
        }
        for (const std::filesystem::path& directory : CatalogDirectories()) {
            const std::filesystem::path path = directory / (language + ".mo");
            std::error_code filesystemError;
            if (!std::filesystem::is_regular_file(path, filesystemError)) {
                continue;
            }

            Catalog loaded;
            std::string error;
            if (!ReadMo(path, loaded, error)) {
                Log(
                    platform::LogLevel::Warning,
                    "localization: " + path.string() + ": " + error
                );
                continue;
            }

            const std::string charset = Lower(loaded.charset);
            if (charset != "utf-8" && charset != "utf8") {
                Log(
                    platform::LogLevel::Warning,
                    "localization: " + path.string() + " uses non-UTF-8 charset '"
                        + loaded.charset + "'"
                );
                continue;
            }

            bool pluralFormsComplete = true;
            for (const auto& [id, message] : loaded.messages) {
                if (message.sourceForms.size() > 1
                    && message.forms.size() < loaded.pluralRules.formCount) {
                    Log(
                        platform::LogLevel::Warning,
                        "localization: " + path.string() + " has only "
                            + std::to_string(message.forms.size()) + " plural forms for '"
                            + id + "' but declares "
                            + std::to_string(loaded.pluralRules.formCount)
                    );
                    pluralFormsComplete = false;
                    break;
                }
            }
            if (!pluralFormsComplete) {
                continue;
            }

            FontProfile fontProfile = FontProfile::Latin;
            if (loaded.fontProfile == "buka-cyrillic") {
                fontProfile = FontProfile::BukaCyrillic;
            } else if (!loaded.fontProfile.empty() && loaded.fontProfile != "latin") {
                Log(
                    platform::LogLevel::Warning,
                    "localization: " + path.string() + " uses unsupported font profile '"
                        + loaded.fontProfile + "'"
                );
                continue;
            }

            ResourceEncoding resourceEncoding = ResourceEncoding::Western;
            if (loaded.resourceEncoding == "windows-1251"
                || loaded.resourceEncoding == "cp1251") {
                resourceEncoding = ResourceEncoding::Cyrillic;
            } else if (loaded.resourceEncoding == "utf-8"
                       || loaded.resourceEncoding == "utf8") {
                resourceEncoding = ResourceEncoding::Utf8;
            } else if (!loaded.resourceEncoding.empty()
                       && loaded.resourceEncoding != "windows-1252"
                       && loaded.resourceEncoding != "cp1252") {
                Log(
                    platform::LogLevel::Warning,
                    "localization: " + path.string()
                        + " uses unsupported original-resource encoding '"
                        + loaded.resourceEncoding + "'"
                );
                continue;
            }

            gCatalog = std::move(loaded);
            gLanguage = !gCatalog.language.empty() ? gCatalog.language : language;
            gFontProfile = fontProfile;
            gResourceEncoding = resourceEncoding;
            gHasCatalog = true;
            Log(
                platform::LogLevel::Info,
                "localization: loaded " + gLanguage + " from " + path.string()
            );
            return true;
        }
    }
    return false;
}

std::string FormatSignature(const char* text) {
    std::string signature;
    if (text == nullptr) {
        return signature;
    }
    for (std::size_t index = 0; text[index] != '\0'; ++index) {
        if (text[index] != '%') {
            continue;
        }
        // A percentage in prose ("150% damage") is not a printf field. A
        // numeric conversion never needs the space after a percent sign when
        // the sign itself directly follows a digit.
        if (index > 0 && std::isdigit(static_cast<unsigned char>(text[index - 1]))
            && text[index + 1] == ' ') {
            continue;
        }
        ++index;
        if (text[index] == '%') {
            continue;
        }
        constexpr const char* conversions = "diuoxXfFeEgGaAcspn";
        while (text[index] != '\0') {
            if (std::strchr(conversions, text[index]) != nullptr) {
                signature.push_back(text[index]);
                break;
            }
            ++index;
        }
    }
    return signature;
}

bool FormatCompatible(const char* id, const char* english, const char* translated) {
    if (FormatSignature(english) == FormatSignature(translated)) {
        return true;
    }
    const std::string key = id != nullptr ? id : "";
    if (gWarnedFormatIds.insert(key).second) {
        Log(
            platform::LogLevel::Warning,
            "localization: placeholder mismatch for '" + key + "'; using English"
        );
    }
    return false;
}

bool SourceCompatible(const char* id, const Message& message, const EnglishMessage& base) {
    const bool singularMatches = !message.sourceForms.empty()
        && message.sourceForms[0] == base.singular;
    const bool pluralMatches = base.plural == nullptr
        || (message.sourceForms.size() > 1 && message.sourceForms[1] == base.plural);
    if (singularMatches && pluralMatches) {
        return true;
    }
    const std::string key = id != nullptr ? id : "";
    if (gWarnedFormatIds.insert("source:" + key).second) {
        Log(
            platform::LogLevel::Warning,
            "localization: stale English source for '" + key + "'; using English"
        );
    }
    return false;
}

std::size_t PluralIndex(std::uint32_t count, std::size_t formCount) {
    const std::size_t index = plural::Select(gCatalog.pluralRules, count);
    return formCount == 0 ? 0 : std::min(index, formCount - 1);
}

const Message* Find(const char* id) {
    if (!gHasCatalog || id == nullptr) {
        return nullptr;
    }
    const auto found = gCatalog.messages.find(id);
    return found == gCatalog.messages.end() ? nullptr : &found->second;
}

const EnglishMessage* FindEnglish(const char* id) {
    if (id != nullptr) {
        for (const EnglishMessage& message : kEnglishMessages) {
            if (std::strcmp(message.id, id) == 0) {
                return &message;
            }
        }
    }
    const std::string key = id != nullptr ? id : "";
    if (gWarnedMissingIds.insert(key).second) {
        Log(
            platform::LogLevel::Warning,
            "localization: unknown message id '" + key + "'"
        );
    }
    return nullptr;
}

void RestoreLegacyTables() {
    for (const LegacyBinding& binding : gLegacyBindings) {
        *binding.slot = binding.english;
    }
    gLegacyBindings.clear();
}

}

void Initialize(const char* commandLine) {
    RestoreLegacyTables();
    gCatalog = Catalog();
    gLanguage = "en";
    gFontProfile = FontProfile::Latin;
    gResourceEncoding = ResourceEncoding::Western;
    gHasCatalog = false;
    gWarnedFormatIds.clear();
    gWarnedMissingIds.clear();

    std::string requested = CommandLineLanguage(commandLine);
    bool explicitSelection = !requested.empty();
    if (requested.empty()) {
        if (const char* environment = std::getenv("HOMM2_LANGUAGE");
            environment != nullptr && *environment != '\0') {
            requested = environment;
            explicitSelection = true;
        }
    }
    if (requested.empty()) {
        requested = ReadLanguageSetting();
        explicitSelection = !requested.empty();
    }
    if (requested.empty()) {
        requested = HostLanguage();
    }
    requested = NormalizeLanguage(std::move(requested));
    if (requested.empty() || requested == "c" || requested == "posix"
        || requested == "en" || requested.rfind("en-", 0) == 0) {
        return;
    }

    if (LoadCatalog(requested)) {
        LocalizeLegacyTables();
    } else if (explicitSelection) {
        Log(
            platform::LogLevel::Warning,
            "localization: no compatible catalog for '" + requested + "'; using English"
        );
    }
}

const char* Language() { return gLanguage.c_str(); }

FontProfile ActiveFontProfile() { return gFontProfile; }

bool HasCatalog() { return gHasCatalog; }

void UseEnglish(const char* reason) {
    if (!gHasCatalog) {
        return;
    }
    Log(
        platform::LogLevel::Warning,
        std::string("localization: ") + (reason != nullptr ? reason : "catalog rejected")
            + "; using English"
    );
    RestoreLegacyTables();
    gCatalog = Catalog();
    gLanguage = "en";
    gFontProfile = FontProfile::Latin;
    gResourceEncoding = ResourceEncoding::Western;
    gHasCatalog = false;
}

std::size_t ApplyLegacyTable(const char* idPrefix, char** values, std::size_t count) {
    if (!gHasCatalog || idPrefix == nullptr || values == nullptr) {
        return 0;
    }
    std::size_t applied = 0;
    for (std::size_t index = 0; index < count; ++index) {
        const std::string id = std::string(idPrefix) + "." + std::to_string(index);
        applied += ApplyLegacyString(id.c_str(), values[index]);
    }
    return applied;
}

std::size_t ApplyLegacyString(const char* id, char*& value) {
    if (!gHasCatalog || id == nullptr || value == nullptr || *value == '\0') {
        return 0;
    }
    char* english = value;
    const Message* message = Find(id);
    if (message == nullptr || message->sourceForms.empty()
        || message->sourceForms[0] != english || message->forms.empty()
        || message->forms[0].empty()) {
        return 0;
    }
    const char* translated = message->forms[0].c_str();
    // Gettext requires translations to preserve a source string's leading
    // newline. A legacy widget label may need that line back for a longer
    // translation; "\n\b" is the catalog escape for suppressing it at bind time.
    if (translated[0] == '\n' && translated[1] == '\b') {
        translated += 2;
    }
    if (!FormatCompatible(id, english, translated)) {
        return 0;
    }
    gLegacyBindings.push_back({&value, english});
    value = const_cast<char*>(translated);
    return 1;
}

const char* Tr(const char* id) {
    const EnglishMessage* base = FindEnglish(id);
    if (base == nullptr) {
        return id != nullptr ? id : "";
    }
    const char* english = base->singular;
    const Message* message = Find(id);
    if (message == nullptr || !SourceCompatible(id, *message, *base)
        || message->forms.empty() || message->forms[0].empty()) {
        return english;
    }
    const char* translated = message->forms[0].c_str();
    return FormatCompatible(id, english, translated) ? translated : english;
}

const char* TrPlural(
    const char* id,
    std::uint32_t count
) {
    const EnglishMessage* base = FindEnglish(id);
    if (base == nullptr) {
        return id != nullptr ? id : "";
    }
    const char* englishSingular = base->singular;
    const char* englishPlural = base->plural != nullptr ? base->plural : base->singular;
    const Message* message = Find(id);
    const char* english = count == 1 ? englishSingular : englishPlural;
    if (message == nullptr || !SourceCompatible(id, *message, *base)
        || message->forms.empty()) {
        return english;
    }
    const std::string& form = message->forms[PluralIndex(count, message->forms.size())];
    if (form.empty()) {
        return english;
    }
    return FormatCompatible(id, english, form.c_str()) ? form.c_str() : english;
}

namespace {

std::uint32_t DecodeWestern(unsigned char byte) {
    if (byte < 0x80 || byte >= 0xa0) {
        return byte;
    }
    constexpr std::uint32_t table[] = {
        0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
        0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017d, 0xfffd,
        0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
        0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178,
    };
    return table[byte - 0x80];
}

std::uint32_t DecodeCyrillic(unsigned char byte) {
    if (byte < 0x80) {
        return byte;
    }
    if (byte >= 0xc0) {
        return 0x0410 + byte - 0xc0;
    }
    constexpr std::uint32_t table[] = {
        0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
        0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
        0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
        0xfffd, 0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
        0x00a0, 0x040e, 0x045e, 0x0408, 0x00a4, 0x0490, 0x00a6, 0x00a7,
        0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x0407,
        0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5, 0x00b6, 0x00b7,
        0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455, 0x0457,
    };
    return table[byte - 0x80];
}

} // namespace

std::string DecodeResourceText(const char* text) {
    if (text == nullptr) {
        return std::string();
    }
    if (gResourceEncoding == ResourceEncoding::Utf8) {
        return text;
    }

    std::string result;
    result.reserve(std::strlen(text) * 2);
    for (const auto* byte = reinterpret_cast<const unsigned char*>(text);
         *byte != 0; ++byte) {
        const std::uint32_t codePoint = gResourceEncoding == ResourceEncoding::Cyrillic
            ? DecodeCyrillic(*byte)
            : DecodeWestern(*byte);
        char encoded[4];
        const std::size_t length = utf8::Encode(codePoint, encoded);
        result.append(encoded, length);
    }
    return result;
}

std::string DecodeExternalText(const char* text) {
    if (text == nullptr || utf8::IsValid(text)) {
        return text != nullptr ? text : "";
    }
    return DecodeResourceText(text);
}

}
