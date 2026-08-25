#include <IRONFIST/creatures.h>

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <tinyxml2.h>

#include <IRONFIST/dialog.h>
#include <IRONFIST/paths.h>
#include <SOURCE/Localization.h>

std::vector<CreatureType> CREATURES_RANDOMIZABLE;

i32 giNumCreatures;
i32 gMonRandBound[KB_CREATURE_TABLE_CAPACITY][2];
i32 gMonSecondaryResourceCost[KB_CREATURE_TABLE_CAPACITY][IRONFIST_SECONDARY_RESOURCE_COUNT];
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

std::vector<std::string> ironfistAttributeNames = {
    STRIKE_AND_RETURN, PLASMA_BLAST, TELEPORTER, ASTRAL_DODGE, SHADOW_MARK, JUMPER, CHARGER
};
static std::vector<std::vector<i32>> ironfistAttributeTable;

static void ResetCreatureAttributes() {
    ironfistAttributeTable.clear();
    for (i32 j = 0; j < static_cast<i32>(ironfistAttributeNames.size()); j++) {
        ironfistAttributeTable.push_back(std::vector<i32>(KB_CREATURE_TABLE_CAPACITY, 0));
    }
}

void GrantCreatureAttribute(i32 id, std::string name) {
    for (i32 i = 0; i < static_cast<i32>(ironfistAttributeNames.size()); i++) {
        if (ironfistAttributeNames[i] == name) {
            ironfistAttributeTable[i][id] = 1;
            return;
        }
    }

    DisplayError(
        "Attempted to grant attribute that does not exist. Check creatures.xml .", name
    );
    exit(1);
}

i32 CreatureHasAttribute(i32 id, const std::string& name) {
    if (id < 0 || id >= KB_CREATURE_TABLE_CAPACITY)
        return 0;
    for (i32 i = 0; i < static_cast<i32>(ironfistAttributeNames.size()); i++) {
        if (ironfistAttributeNames[i] == name) {
            return ironfistAttributeTable[i][id];
        }
    }
    return 0;
}

char* GetCreatureName(i32 id) {
    return gArmyNames[id];
}

char* GetCreaturePluralName(i32 id) {
    return gArmyNamesPlural[id];
}

i32 GetNumCreatures() {
    return giNumCreatures;
}

void Ironfist_GetMonsterCost(i32 monster, i32* const costs) {
    for (i32 i = 0; i < IRONFIST_SECONDARY_RESOURCE_COUNT; i++) {
        costs[i] = gMonSecondaryResourceCost[monster][i];
    }

    costs[H2EnumIndex(RES_GOLD)] = gMonsterDatabase[monster].cost;
}

static char* QueryAttributeCopy(tinyxml2::XMLElement* el, const char* attribute, char** dest) {
    const char* text = NULL;
    el->QueryStringAttribute(attribute, &text);
    if (text == NULL) {
        text = "invalid name";
    }
    if (*dest) {
        free(*dest);
    }
    *dest = strdup(text);
    return *dest;
}

static void LocalizeCreatureName(i32 id, const char* table, char** value) {
    std::string messageId = std::string("table.") + table + "." + std::to_string(id);
    std::string translated = localization::TranslateExternal(messageId.c_str(), *value);
    if (translated != *value) {
        free(*value);
        *value = strdup(translated.c_str());
    }
}

static const char* QueryTextAttribute(tinyxml2::XMLElement* el, const char* attribute) {
    const char* text = NULL;
    el->QueryStringAttribute(attribute, &text);
    return text != NULL ? text : "invalid name";
}

static void ReadCreatureData(tinyxml2::XMLNode* root) {
    giNumCreatures = 0;

    for (tinyxml2::XMLNode* creature = root->FirstChild(); creature;
         creature = creature->NextSibling()) {
        tinyxml2::XMLElement* crElem = creature->ToElement();
        const i32 id = crElem->IntAttribute("id");
        if (id < 0 || id >= KB_CREATURE_TABLE_CAPACITY) {
            continue;
        }

        QueryAttributeCopy(crElem, "icn", &cMonFilename[id]);
        QueryAttributeCopy(crElem, "frm", &cArmyFrameFileNames[id]);
        QueryAttributeCopy(crElem, "name-singular", &gArmyNames[id]);
        QueryAttributeCopy(crElem, "name-plural", &gArmyNamesPlural[id]);
        LocalizeCreatureName(id, "gArmyNames", &gArmyNames[id]);
        LocalizeCreatureName(id, "gArmyNamesPlural", &gArmyNamesPlural[id]);
        QueryAttributeCopy(crElem, "projectile", &cArmyProjectileFileNames[id]);

        i32 minDamage = 0;
        i32 maxDamage = 0;
        i32 creatureFlags = 0;

        for (i32 i = 0; i < IRONFIST_SECONDARY_RESOURCE_COUNT; i++) {
            gMonSecondaryResourceCost[id][i] = 0;
        }

        for (tinyxml2::XMLNode* property = crElem->FirstChild(); property;
             property = property->NextSibling()) {
            tinyxml2::XMLElement* propElem = property->ToElement();
            std::string name = propElem->Name();
            if (name == "damage") {
                minDamage = propElem->IntAttribute("minimum");
                maxDamage = propElem->IntAttribute("maximum");
            } else if (name == "random-spawn") {
                gMonRandBound[id][0] = propElem->IntAttribute("minimum");
                gMonRandBound[id][1] = propElem->IntAttribute("maximum");
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
                            gMonSecondaryResourceCost
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

        if (id > giNumCreatures)
            giNumCreatures = id;

        tag_monsterInfo info = {};
        info.cost = static_cast<i16>(crElem->IntAttribute("cost"));
        info.fightValue = crElem->IntAttribute("fight-value");
        info.iconIndex = static_cast<i8>(crElem->IntAttribute("fight-value-aux"));
        info.growth = static_cast<i8>(crElem->IntAttribute("growth"));
        info.hitPoints = static_cast<u16>(crElem->IntAttribute("hp"));
        info.race = static_cast<FactionType>(crElem->IntAttribute("faction"));
        info.speed = static_cast<i8>(crElem->IntAttribute("speed"));
        info.attack = static_cast<i8>(crElem->IntAttribute("attack"));
        info.defense = static_cast<i8>(crElem->IntAttribute("defense"));
        info.damageMin = static_cast<i8>(minDamage);
        info.damageMax = static_cast<i8>(maxDamage);
        info.shots = static_cast<i8>(crElem->IntAttribute("shots"));
        strncpy(info.spriteName, QueryTextAttribute(crElem, "short-name"),
                MONSTER_SPRITE_NAME_SIZE - 1);
        info.flags.all = static_cast<MonsterFlags>(creatureFlags);
        gMonsterDatabase[id] = info;
    }
    giNumCreatures++;
}

randomHeroCreatureInfo
    randomHeroArmyBounds[IRONFIST_FACTION_TABLE_COUNT][IRONFIST_HERO_ARMY_TIER_COUNT] = {
    { { CREATURE_PEASANT, 30, 50 },  { CREATURE_ARCHER, 3, 5 }   },
    { { CREATURE_GOBLIN, 15, 25 },   { CREATURE_ORC, 3, 5 }      },
    { { CREATURE_SPRITE, 10, 20 },   { CREATURE_DWARF, 2, 4 }    },
    { { CREATURE_CENTAUR, 6, 10 },   { CREATURE_GARGOYLE, 2, 4 } },
    { { CREATURE_HALFLING, 6, 10 },  { CREATURE_BOAR, 2, 4 }     },
    { { CREATURE_SKELETON, 6, 10 },  { CREATURE_ZOMBIE, 2, 4 }   },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 },
      { static_cast<CreatureType>(CREATURE_INVALID_ID), 0, 0 } },
    { { static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID), 8, 12 },
      { static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID + 1), 3, 6 } }
};

CreatureType
    neutralTownCreatureTypes[IRONFIST_FACTION_TABLE_COUNT][IRONFIST_NEUTRAL_TOWN_TIER_COUNT] = {
    { CREATURE_PEASANT, CREATURE_ARCHER, CREATURE_PIKEMAN, CREATURE_SWORDSMAN,
      CREATURE_CAVALRY },
    { CREATURE_GOBLIN, CREATURE_ORC, CREATURE_WOLF, CREATURE_OGRE, CREATURE_TROLL },
    { CREATURE_SPRITE, CREATURE_DWARF, CREATURE_ELF, CREATURE_DRUID, CREATURE_UNICORN },
    { CREATURE_CENTAUR, CREATURE_GARGOYLE, CREATURE_GRIFFIN, CREATURE_MINOTAUR,
      CREATURE_HYDRA },
    { CREATURE_HALFLING, CREATURE_BOAR, CREATURE_IRON_GOLEM, CREATURE_ROC, CREATURE_MAGE },
    { CREATURE_SKELETON, CREATURE_ZOMBIE, CREATURE_MUMMY, CREATURE_VAMPIRE, CREATURE_LICH },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID), static_cast<CreatureType>(CREATURE_INVALID_ID),
      static_cast<CreatureType>(CREATURE_INVALID_ID) },
    { static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID),
      static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID + 1),
      static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID + 2),
      static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID + 3),
      static_cast<CreatureType>(CREATURE_CYBER_KOBOLD_SPEARMAN_ID + 4) }
};

std::vector<FactionType> FACTIONS_ACTUAL = {
    FACTION_KNIGHT,   FACTION_BARBARIAN,   FACTION_SORCERESS,           FACTION_WARLOCK,
    FACTION_WIZARD,   FACTION_NECROMANCER, static_cast<FactionType>(12)
};

std::vector<std::string> FACTIONS_ACTUAL_NAMES = {
    "Knight", "Barbarian", "Sorceress", "Warlock", "Wizard", "Necromancer", "Cyborg"
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
        memset(cArmyProjectileFileNames, 0, sizeof(cArmyProjectileFileNames));
    }

    ResetCreatureAttributes();
    CREATURES_RANDOMIZABLE.clear();

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
        CREATURES_RANDOMIZABLE.push_back(static_cast<CreatureType>(i));
    for (i32 i = MIN_IRONFIST_CREATURE; i <= MAX_IRONFIST_CREATURE; i++)
        CREATURES_RANDOMIZABLE.push_back(static_cast<CreatureType>(i));
}

void UnloadCreatures() {
    if (!creatureStringsOwned) {
        return;
    }

    for (i32 i = 0; i < KB_CREATURE_TABLE_CAPACITY; i++) {
        std::free(cMonFilename[i]);
        std::free(cArmyFrameFileNames[i]);
        std::free(gArmyNames[i]);
        std::free(gArmyNamesPlural[i]);
        std::free(cArmyProjectileFileNames[i]);

        cMonFilename[i] = NULL;
        cArmyFrameFileNames[i] = NULL;
        gArmyNames[i] = NULL;
        gArmyNamesPlural[i] = NULL;
        cArmyProjectileFileNames[i] = NULL;
    }

    CREATURES_RANDOMIZABLE.clear();
    creatureStringsOwned = false;
}
