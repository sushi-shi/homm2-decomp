#ifndef HOMM2_SOURCE_PLURAL_RULES_H
#define HOMM2_SOURCE_PLURAL_RULES_H

#include <cstddef>
#include <cstdint>
#include <string>

namespace localization::plural {

struct Rules {
    std::size_t formCount = 2;
    std::string expression = "n != 1";
};

// Parses the standard gettext Plural-Forms header. Keeping this independent
// of the language tag lets new catalogs bring their own grammar rules.
bool Parse(const std::string& header, Rules& rules, std::string& error);
std::size_t Select(const Rules& rules, std::uint32_t count);

}

#endif
