#include <SOURCE/Localization.h>

#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Platform.h>

#include <iterator>
#include <string>

// KB.h preserves four original unsized extern declarations. Complete them in
// this translation unit so the migration registry can derive their bounds.
extern char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT];
extern char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT];
extern char* gDwellingNames[H2EnumIndex(FACTION_COUNT)][KB_DWELLING_TYPE_COUNT];
extern char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT];
extern char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT];
extern char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT];
extern SWinSetup gWinSetup[KB_WIN_SETUP_COUNT];
extern char* cOutOfMemory;
extern char* xNecromancerShrine;
extern char* xNecromancerShrineDesc;

namespace localization {

template<std::size_t Count>
static std::size_t ApplyLegacyArray(const char* idPrefix, char* (&values)[Count]) {
    return ApplyLegacyTable(idPrefix, values, Count);
}

template<std::size_t Rows, std::size_t Columns>
static std::size_t ApplyLegacyArray(
    const char* idPrefix,
    char* (&values)[Rows][Columns]
) {
    return ApplyLegacyTable(idPrefix, &values[0][0], Rows * Columns);
}

void LocalizeLegacyTables() {
    std::size_t applied = 0;
#define HOMM2_LEGACY_TABLE(path, name) \
    applied += ApplyLegacyArray("table." #name, name);
#define HOMM2_LEGACY_MEMBER_TABLE(path, name, member, memberIndex) \
    for (std::size_t index = 0; index < std::size(name); ++index) { \
        const std::string id = "table." #name "." + std::to_string(index); \
        applied += ApplyLegacyString(id.c_str(), name[index].member); \
    }
#include <locales/legacy_tables.def>
#undef HOMM2_LEGACY_MEMBER_TABLE
#undef HOMM2_LEGACY_TABLE
    applied += ApplyLegacyString("system.memory.requirement", cOutOfMemory);
    applied += ApplyLegacyString("site.necromancer_shrine.name", xNecromancerShrine);
    applied += ApplyLegacyString(
        "site.necromancer_shrine.description",
        xNecromancerShrineDesc
    );
    platform::Host().Log(
        platform::LogLevel::Info,
        ("localization: applied " + std::to_string(applied) + " legacy table entries").c_str()
    );
}

}
