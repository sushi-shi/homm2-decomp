// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWN.OBJ   from: (directly linked into exe)
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/BITS.h>
#include <BASE/executive.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
VA(0x00432c00, 0x65)
town::town(void)
{
    m_type = FACTION_KNIGHT;
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_occupyingHeroId = TOWN_OCCUPYING_HERO_NONE;
    m_buildings = TOWN_BUILDING_TENT;
    m_buildState = 0;
    m_unknown1d = 0;
}

VA(0x00432c65, 0x54)
i32 town::HasGarrison(void)
{
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_army.m_creatureTypes[slot] != ARMY_GROUP_EMPTY_SLOT)
            return 1;
    }
    return 0;
}

VA(0x00432cb9, 0x111)
void town::GiveSpells(hero *targetHero)
{
    hero *activeHero;
    i32 level;
    i32 slot;

    if (targetHero == 0 && m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE)
        return;

    if (targetHero != 0)
        activeHero = targetHero;
    else
        activeHero = gpGame->GetHero(m_occupyingHeroId);

    if (!activeHero->HasArtifact(ARTIFACT_MAGIC_BOOK))
        return;
    if (!(m_buildings & TOWN_BUILDING_MAGE_GUILD))
        return;

    for (level = 0;
         level < activeHero->m_secondarySkills[HERO_SKILL_WISDOM]
                     + TOWN_MAGE_GUILD_WISDOM_LEVEL_BONUS;
         ++level) {
        for (slot = 0; slot < m_spellCounts[level + TOWN_MAGE_GUILD_FIRST_LEVEL]; ++slot) {
            activeHero->AddSpell(m_spells[level][slot],
                                 activeHero->Stats(HERO_PRIMARY_KNOWLEDGE));
        }
    }
}

VA(0x00432dca, 0xaa)
void town::XformToCastle(void)
{
    i32 objectType;
    gpGame->ConvertObject(m_x + RANDOM_TOWN_LEFT, m_y + RANDOM_TOWN_TOP,
                          m_x + RANDOM_TOWN_RIGHT, m_y + RANDOM_TOWN_BOTTOM,
                          RANDOM_TOWN_OBJECT_TILESET, TOWN_CONVERT_SOURCE_FRAME,
                          TOWN_CONVERT_ANY_FRAME, RANDOM_TOWN_OBJECT_TILESET,
                          TOWN_CONVERT_OBJECT_NONE,
                          RANDOM_TOWN_OBJECT_TILESET, RANDOM_TOWN_OBJECT_TILESET);
    gpGame->ConvertObject(m_x + RANDOM_TOWN_LEFT, m_y + RANDOM_TOWN_TOP,
                          m_x + RANDOM_TOWN_RIGHT, m_y + RANDOM_TOWN_BOTTOM,
                          RANDOM_TOWN_OVERLAY_TILESET, TOWN_CONVERT_SOURCE_FRAME,
                          TOWN_CONVERT_ANY_FRAME, RANDOM_TOWN_OVERLAY_TILESET,
                          TOWN_CONVERT_OBJECT_NONE,
                          RANDOM_TOWN_OBJECT_TILESET, RANDOM_TOWN_OBJECT_TILESET);
}

VA(0x00432e74, 0xe0)
void town::View(i32 noFade)
{
    bEnteringTown = 1;
    if (giHighMemBuffer + TOWN_VIEW_MEMORY_REQUIREMENT > TOWN_VIEW_HIGH_MEMORY_LIMIT)
        gAdvDisposeLevel = TOWN_DISPOSE_FULL;
    else if (giHighMemBuffer + TOWN_VIEW_MEMORY_REQUIREMENT > TOWN_VIEW_LOW_MEMORY_LIMIT)
        gAdvDisposeLevel = TOWN_DISPOSE_PARTIAL;

    townManager *manager = gpTownManager;
    manager->SetTown(this);
    if (!noFade)
        gpWindowManager->FadeScreen(1, TOWN_FADE_STEPS, 0);
    gpExec->CallManager(gpTownManager);
    if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
        gpAdvManager->SetHeroContext(m_occupyingHeroId, 0);
    gAdvDisposeLevel = TOWN_DISPOSE_NONE;
    bEnteringTown = 0;
}

VA(0x00432f54, 0x14d)
void town::Deallocate(void)
{
    playerData *playerRecord = &gpGame->m_players[m_owner];
    i32 position = TOWN_ID_NONE;
    i32 i;

    for (i = 0; i < playerRecord->m_townCount; ++i) {
        if (playerRecord->m_townIds[i] == m_id)
            position = i;
    }
    for (i = position; i < playerRecord->m_townCount - 1; ++i)
        playerRecord->m_townIds[i] = playerRecord->m_townIds[i + 1];

    playerRecord->m_townIds[playerRecord->m_townCount - 1] = TOWN_ID_NONE;
    if (OD_STEER(m_id) == playerRecord->m_currentTown)
        playerRecord->m_currentTown = TOWN_ID_NONE;
    --playerRecord->m_townCount;
    if (playerRecord->m_townCount < TOWN_PLAYER_WINDOW_SIZE)
        playerRecord->m_townLocatorPage = 0;
    else if (playerRecord->m_townLocatorPage + TOWN_PLAYER_WINDOW_SIZE > playerRecord->m_townCount)
        playerRecord->m_townLocatorPage = playerRecord->m_townCount - TOWN_PLAYER_WINDOW_SIZE;

    gpGame->m_townOwners[m_id] = TOWN_OWNER_NONE;
    m_owner = TOWN_OWNER_NONE;
}

// @semantic: Current TOWN.cpp/header epoch: the 0x8 frame, CFG, all 155
// instructions, and all nine ordered external relocations align. Raw bytes
// differ only at +0xb7 and +0xba: the two MOV stack displacements swap eax/ecx
// for the discarded-result spell-count increment. The retained prefix form is
// clearest; ten non-improving variants exhausted symmetric-subscript, postfix,
// commutative_order, and seven identifier_rename spellings. The retained
// source-hash maximum is 100%; revisit only after a relevant TOWN source/TU/header
// or comparison epoch alters MSVC register selection.
VA(0x004330a1, 0x23e)
void town::BuildBuilding(i32 building)
{
    i32 level;
    if (building == TOWN_OBJECT_MAGE_GUILD) {
        ++m_buildState;
        m_spellCounts[m_buildState] = gSpellLimits[m_buildState - TOWN_MAGE_GUILD_FIRST_LEVEL];
        if (m_type == FACTION_WIZARD && (m_buildings & TOWN_BUILDING_LIBRARY))
            ++m_spellCounts[m_buildState];
        if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
            GiveSpells(0);
    }
    if (building == TOWN_OBJECT_SPECIAL_BUILDING && m_type == FACTION_WIZARD) {
        for (level = 0; level < m_buildState; ++level)
            ++m_spellCounts[level + TOWN_MAGE_GUILD_FIRST_LEVEL];
        if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
            GiveSpells(0);
    }

    m_buildings |= 1L << building;
    if (building == TOWN_OBJECT_UPGRADED_DWELLING_2)
        m_buildings &= ~TOWN_BUILDING_DWELLING_2;
    if (building == TOWN_OBJECT_UPGRADED_DWELLING_3)
        m_buildings &= ~TOWN_BUILDING_DWELLING_3;
    if (building == TOWN_OBJECT_UPGRADED_DWELLING_4)
        m_buildings &= ~TOWN_BUILDING_DWELLING_4;
    if (building == TOWN_OBJECT_UPGRADED_DWELLING_5)
        m_buildings &= ~TOWN_BUILDING_DWELLING_5;
    if (building == TOWN_OBJECT_UPGRADED_DWELLING_6)
        m_buildings &= ~TOWN_BUILDING_DWELLING_6;
    if (building == TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6)
        m_buildings &= ~(TOWN_BUILDING_DWELLING_6 | TOWN_BUILDING_UPGRADED_DWELLING_6);

    if (building >= TOWN_OBJECT_DWELLING_1 && building <= TOWN_OBJECT_DWELLING_6) {
        m_garrison[building - TOWN_OBJECT_DWELLING_1] =
            gMonsterDatabase[gDwellingType[m_type][building - TOWN_OBJECT_DWELLING_1]].growth;
    }
    if (building >= TOWN_OBJECT_UPGRADED_DWELLING_2
        && building <= TOWN_OBJECT_UPGRADED_DWELLING_6) {
        m_garrison[building - TOWN_OBJECT_DWELLING_1] =
            m_garrison[building - TOWN_OBJECT_DWELLING_6];
    }
    if (building == TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6) {
        m_garrison[building - TOWN_OBJECT_DWELLING_1] =
            m_garrison[building - TOWN_OBJECT_DWELLING_2];
    }
    if (building == TOWN_OBJECT_CASTLE) {
        m_buildings &= ~TOWN_BUILDING_TENT;
        XformToCastle();
    }
    GiveSpells(0);
    BitSet(gpGame->m_knownTowns, m_id);
}

VA(0x004332df, 0x36)
i32 town::CanBuildDock(void)
{
    return m_boatX != TOWN_DOCK_COORDINATE_NONE;
}

VA(0x00433315, 0x9f)
void town::CalcNumLevelArchers(i32 *numArchers, i32 *mageGuildLevel)
{
    *mageGuildLevel = m_buildState;
    *numArchers = 0;
    i32 building;
    for (building = TOWN_OBJECT_DWELLING_1;
         building <= TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6; ++building) {
        if (m_buildings & (1L << building))
            ++*numArchers;
    }
    for (building = TOWN_OBJECT_MAGE_GUILD;
         building <= TOWN_COMMAND_LAST_NEUTRAL_BUILDING; ++building) {
        if (m_buildings & (1L << building))
            ++*numArchers;
    }
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f11b0) i32 bEnteringTown = 0;
