#include <IRONFIST/creatures.h>

#include <cstdlib>
#include <array>
#include <cstring>
#include <string>
#include <vector>

#include <tinyxml2.h>

#include <BASE/Utf8.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/paths.h>
#include <SOURCE/Localization.h>

namespace ironfist {

std::vector<CreatureType> RandomizableCreatures;

static i32 CreatureCount;
i32 CreatureRandomBounds[KB_CREATURE_TABLE_CAPACITY][2];
i32 CreatureSecondaryCosts[KB_CREATURE_TABLE_CAPACITY][SECONDARY_RESOURCE_COUNT];
static char* cMonFilenameOwned[KB_CREATURE_TABLE_CAPACITY];
static char* cArmyFrameFileNamesOwned[KB_CREATURE_TABLE_CAPACITY];
static char* gArmyNamesOwned[KB_CREATURE_TABLE_CAPACITY];
static char* gArmyNamesPluralOwned[KB_CREATURE_TABLE_CAPACITY];
static char* cArmyProjectileFileNames[KB_CREATURE_TABLE_CAPACITY];
static bool creatureStringsOwned = false;

struct attributeNameTableEntry {
    const char* name;
    MonsterFlags flag;
};

static attributeNameTableEntry creatureAttributeNameTable[5] = {
    { "two-hex",        MONSTER_FLAGS_WIDE          },
    { "flies",          MONSTER_FLAGS_FLYING        },
    { "shoots",         MONSTER_FLAGS_SHOOTER       },
    { "two-hex-attack", MONSTER_FLAGS_BREATH_ATTACK },
    { "undead",         MONSTER_FLAGS_UNDEAD        }
};

struct SecondaryResourceNameTableEntry {
    const char* name;
    ResourceType resource;
};

static SecondaryResourceNameTableEntry SecondaryResourceNameTable[] = {
    { "wood",    RES_WOOD    },
    { "mercury", RES_MERCURY },
    { "ore",     RES_ORE     },
    { "sulfur",  RES_SULFUR  },
    { "crystal", RES_CRYSTAL },
    { "gems",    RES_GEMS    }
};

struct ExtendedAttributeName {
    const char* name;
    CreatureAttribute attribute;
};

static constexpr std::array<ExtendedAttributeName, H2EnumIndex(CreatureAttribute::Count)>
    ExtendedAttributeNames = { {
        { "strike-and-return", CreatureAttribute::StrikeAndReturn },
        { "plasma-blast", CreatureAttribute::PlasmaBlast },
        { "teleporter", CreatureAttribute::Teleporter },
        { "astral-dodge", CreatureAttribute::AstralDodge },
        { "shadow-mark", CreatureAttribute::ShadowMark },
        { "jumper", CreatureAttribute::Jumper },
        { "charger", CreatureAttribute::Charger }
    } };

static std::array<std::vector<bool>, H2EnumIndex(CreatureAttribute::Count)>
    creatureAttributeTable;

static void ResetCreatureAttributes() {
    for (std::vector<bool>& attributes : creatureAttributeTable) {
        attributes.assign(KB_CREATURE_TABLE_CAPACITY, false);
    }
}

static void GrantCreatureAttribute(i32 id, const std::string& name) {
    for (const ExtendedAttributeName& entry : ExtendedAttributeNames) {
        if (entry.name == name) {
            creatureAttributeTable[H2EnumIndex(entry.attribute)][id] = true;
            return;
        }
    }

    DisplayError(
        "Attempted to grant attribute that does not exist. Check creatures.xml .", name
    );
    exit(1);
}

bool HasCreatureAttribute(CreatureType creature, CreatureAttribute attribute) {
    const i32 id = H2EnumIndex(creature);
    if (id < 0 || id >= KB_CREATURE_TABLE_CAPACITY)
        return false;
    return creatureAttributeTable[H2EnumIndex(attribute)][id];
}

const char* GetCreatureName(i32 id) {
    return gArmyNames[id];
}

const char* GetCreaturePluralName(i32 id) {
    return gArmyNamesPlural[id];
}

static char* QueryAttributeCopy(tinyxml2::XMLElement* el, const char* attribute) {
    const char* text = NULL;
    el->QueryStringAttribute(attribute, &text);
    if (text == NULL) {
        text = "invalid name";
    }
    return strdup(text);
}

static char* LocalizeCreatureName(i32 id, const char* table, char* value) {
    std::string messageId = std::string("table.") + table + "." + std::to_string(id);
    std::string translated = localization::TranslateExternal(messageId.c_str(), value);
    if (translated != value) {
        free(value);
        return strdup(translated.c_str());
    }
    return value;
}

static const char* QueryTextAttribute(tinyxml2::XMLElement* el, const char* attribute) {
    const char* text = NULL;
    el->QueryStringAttribute(attribute, &text);
    return text != NULL ? text : "invalid name";
}

static void ReadCreatureData(tinyxml2::XMLNode* root) {
    CreatureCount = 0;

    for (tinyxml2::XMLNode* creature = root->FirstChild(); creature;
         creature = creature->NextSibling()) {
        tinyxml2::XMLElement* crElem = creature->ToElement();
        const i32 id = crElem->IntAttribute("id");
        if (id < 0 || id >= KB_CREATURE_TABLE_CAPACITY) {
            continue;
        }

        free(cMonFilenameOwned[id]);
        cMonFilenameOwned[id] = QueryAttributeCopy(crElem, "icn");
        cMonFilename[id] = cMonFilenameOwned[id];
        free(cArmyFrameFileNamesOwned[id]);
        cArmyFrameFileNamesOwned[id] = QueryAttributeCopy(crElem, "frm");
        cArmyFrameFileNames[id] = cArmyFrameFileNamesOwned[id];
        free(gArmyNamesOwned[id]);
        gArmyNamesOwned[id] = QueryAttributeCopy(crElem, "name-singular");
        gArmyNamesOwned[id] = LocalizeCreatureName(id, "gArmyNames", gArmyNamesOwned[id]);
        gArmyNames[id] = gArmyNamesOwned[id];
        free(gArmyNamesPluralOwned[id]);
        gArmyNamesPluralOwned[id] = QueryAttributeCopy(crElem, "name-plural");
        gArmyNamesPluralOwned[id] =
            LocalizeCreatureName(id, "gArmyNamesPlural", gArmyNamesPluralOwned[id]);
        gArmyNamesPlural[id] = gArmyNamesPluralOwned[id];
        free(cArmyProjectileFileNames[id]);
        cArmyProjectileFileNames[id] = QueryAttributeCopy(crElem, "projectile");

        i32 minDamage = 0;
        i32 maxDamage = 0;
        i32 creatureFlags = 0;

        for (i32 i = 0; i < SECONDARY_RESOURCE_COUNT; i++) {
            CreatureSecondaryCosts[id][i] = 0;
        }

        for (tinyxml2::XMLNode* property = crElem->FirstChild(); property;
             property = property->NextSibling()) {
            tinyxml2::XMLElement* propElem = property->ToElement();
            std::string name = propElem->Name();
            if (name == "damage") {
                minDamage = propElem->IntAttribute("minimum");
                maxDamage = propElem->IntAttribute("maximum");
            } else if (name == "random-spawn") {
                CreatureRandomBounds[id][0] = propElem->IntAttribute("minimum");
                CreatureRandomBounds[id][1] = propElem->IntAttribute("maximum");
            } else if (name == "creature-attribute") {
                bool attributeFound = false;
                const char* attrName = QueryTextAttribute(propElem, "name");
                for (i32 k = 0; k < static_cast<i32>((sizeof(creatureAttributeNameTable)
                                          / sizeof(creatureAttributeNameTable[0])));
                     k++) {
                    if (!strcmp(creatureAttributeNameTable[k].name, attrName)) {
                        creatureFlags |= H2EnumIndex(creatureAttributeNameTable[k].flag);
                        attributeFound = true;
                        break;
                    }
                }

                if (!attributeFound) {
                    // Ironfist-only attribute; using general attribute engine
                    GrantCreatureAttribute(id, QueryTextAttribute(propElem, "name"));
                }
            } else if (name == "secondary-cost") {
                bool customedSecondaryCost = false;
                for (i32 k = 0;
                     k < static_cast<i32>((sizeof(SecondaryResourceNameTable)
                               / sizeof(SecondaryResourceNameTable[0])));
                     k++) {
                    if (std::string(SecondaryResourceNameTable[k].name)
                        == std::string(QueryTextAttribute(propElem, "resource"))) {
                        if (!customedSecondaryCost) {
                            CreatureSecondaryCosts
                                [id][H2EnumIndex(SecondaryResourceNameTable[k].resource)] =
                                    propElem->IntAttribute("cost");
                            customedSecondaryCost = true;
                        } else {
                            std::string errorMessage =
                                "Error loading creatures.xml.\nToo many secondary resource "
                                "costs for creature "
                                + std::string(gArmyNames[id]) + "!";
                            DisplayError(errorMessage, "Startup Error");
                            exit(1);
                        }
                    }
                }
            }
        }

        if (id > CreatureCount)
            CreatureCount = id;

        tag_monsterInfo info = {};
        info.cost = static_cast<i16>(crElem->IntAttribute("cost"));
        info.fightValue = crElem->IntAttribute("fight-value");
        info.iconIndex = static_cast<i8>(crElem->IntAttribute("fight-value-aux"));
        info.growth = static_cast<i8>(crElem->IntAttribute("growth"));
        info.hitPoints = static_cast<u16>(crElem->IntAttribute("hp"));
        info.race = FactionTypeFromCode(crElem->IntAttribute("faction"));
        info.speed = static_cast<i8>(crElem->IntAttribute("speed"));
        info.attack = static_cast<i8>(crElem->IntAttribute("attack"));
        info.defense = static_cast<i8>(crElem->IntAttribute("defense"));
        info.damageMin = static_cast<i8>(minDamage);
        info.damageMax = static_cast<i8>(maxDamage);
        info.shots = static_cast<i8>(crElem->IntAttribute("shots"));
        utf8::Copy(
            info.spriteName,
            sizeof(info.spriteName),
            QueryTextAttribute(crElem, "short-name")
        );
        info.flags.all = MonsterFlagsFromCode(creatureFlags);
        gMonsterDatabase[id] = info;
    }
    CreatureCount++;
}

StartingArmyRange
    StartingArmyBounds[KB_FACTION_TABLE_CAPACITY][HERO_ARMY_TIER_COUNT] = {
    { { CREATURE_PEASANT, 30, 50 },  { CREATURE_ARCHER, 3, 5 }   },
    { { CREATURE_GOBLIN, 15, 25 },   { CREATURE_ORC, 3, 5 }      },
    { { CREATURE_SPRITE, 10, 20 },   { CREATURE_DWARF, 2, 4 }    },
    { { CREATURE_CENTAUR, 6, 10 },   { CREATURE_GARGOYLE, 2, 4 } },
    { { CREATURE_HALFLING, 6, 10 },  { CREATURE_BOAR, 2, 4 }     },
    { { CREATURE_SKELETON, 6, 10 },  { CREATURE_ZOMBIE, 2, 4 }   },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_NONE, 0, 0 }, { CREATURE_NONE, 0, 0 } },
    { { CREATURE_CYBER_KOBOLD_SPEARMAN, 8, 12 },
      { CREATURE_CYBER_PLASMA_BERSERKER, 3, 6 } }
};

CreatureType
    NeutralTownCreatures[KB_FACTION_TABLE_CAPACITY][NEUTRAL_TOWN_TIER_COUNT] = {
    { CREATURE_PEASANT, CREATURE_ARCHER, CREATURE_PIKEMAN, CREATURE_SWORDSMAN,
      CREATURE_CAVALRY },
    { CREATURE_GOBLIN, CREATURE_ORC, CREATURE_WOLF, CREATURE_OGRE, CREATURE_TROLL },
    { CREATURE_SPRITE, CREATURE_DWARF, CREATURE_ELF, CREATURE_DRUID, CREATURE_UNICORN },
    { CREATURE_CENTAUR, CREATURE_GARGOYLE, CREATURE_GRIFFIN, CREATURE_MINOTAUR,
      CREATURE_HYDRA },
    { CREATURE_HALFLING, CREATURE_BOAR, CREATURE_IRON_GOLEM, CREATURE_ROC, CREATURE_MAGE },
    { CREATURE_SKELETON, CREATURE_ZOMBIE, CREATURE_MUMMY, CREATURE_VAMPIRE, CREATURE_LICH },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE, CREATURE_NONE },
    { CREATURE_CYBER_KOBOLD_SPEARMAN, CREATURE_CYBER_PLASMA_BERSERKER,
      CREATURE_CYBER_PLASMA_LANCER, CREATURE_CYBER_INDIGO_PANTHER,
      CREATURE_CYBER_SHADOW_ASSASSIN }
};

std::vector<FactionType> PlayableFactions = {
    FACTION_KNIGHT,   FACTION_BARBARIAN,   FACTION_SORCERESS,           FACTION_WARLOCK,
    FACTION_WIZARD,   FACTION_NECROMANCER, FACTION_CYBORG
};

void LoadCreatures() {
    if (creatureStringsOwned) {
        UnloadCreatures();
    } else {
        // The retail tables begin with static string literals. The XML becomes
        // the authority after startup, so discard those pointers without
        // attempting to free them.
        memset(cMonFilename, 0, sizeof(cMonFilename));
        memset(cArmyFrameFileNames, 0, sizeof(cArmyFrameFileNames));
        memset(gArmyNames, 0, sizeof(gArmyNames));
        memset(gArmyNamesPlural, 0, sizeof(gArmyNamesPlural));
        memset(cMonFilenameOwned, 0, sizeof(cMonFilenameOwned));
        memset(cArmyFrameFileNamesOwned, 0, sizeof(cArmyFrameFileNamesOwned));
        memset(gArmyNamesOwned, 0, sizeof(gArmyNamesOwned));
        memset(gArmyNamesPluralOwned, 0, sizeof(gArmyNamesPluralOwned));
        memset(cArmyProjectileFileNames, 0, sizeof(cArmyProjectileFileNames));
    }

    ResetCreatureAttributes();
    RandomizableCreatures.clear();

    tinyxml2::XMLDocument doc(true);
    tinyxml2::XMLError err = doc.LoadFile(ResolveDataPath("DATA/creatures.xml").c_str());
    if (err) {
        std::string message = "Could not load XML. " + std::string(doc.ErrorStr());
        DisplayError(message, "Ironfist data");
        exit(1);
    }
    ReadCreatureData(doc.FirstChild());
    creatureStringsOwned = true;

    for (i32 i = 0; i <= MAX_BASE_CREATURE; i++)
            RandomizableCreatures.push_back(CreatureTypeFromOrdinal(i));
    for (i32 i = MIN_EXTENDED_CREATURE; i <= MAX_EXTENDED_CREATURE; i++)
            RandomizableCreatures.push_back(CreatureTypeFromOrdinal(i));
}

void UnloadCreatures() {
    if (!creatureStringsOwned) {
        return;
    }

    for (i32 i = 0; i < KB_CREATURE_TABLE_CAPACITY; i++) {
        std::free(cMonFilenameOwned[i]);
        std::free(cArmyFrameFileNamesOwned[i]);
        std::free(gArmyNamesOwned[i]);
        std::free(gArmyNamesPluralOwned[i]);
        std::free(cArmyProjectileFileNames[i]);

        cMonFilenameOwned[i] = NULL;
        cArmyFrameFileNamesOwned[i] = NULL;
        gArmyNamesOwned[i] = NULL;
        gArmyNamesPluralOwned[i] = NULL;
        cMonFilename[i] = NULL;
        cArmyFrameFileNames[i] = NULL;
        gArmyNames[i] = NULL;
        gArmyNamesPlural[i] = NULL;
        cArmyProjectileFileNames[i] = NULL;
    }

    RandomizableCreatures.clear();
    creatureStringsOwned = false;
}

} // namespace ironfist
