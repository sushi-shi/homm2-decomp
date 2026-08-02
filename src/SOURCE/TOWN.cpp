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
VA(0x004a3ff0, 0x5a)
town::town(void) {
    m_type = FACTION_KNIGHT;
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_occupyingHeroId = TOWN_OCCUPYING_HERO_NONE;
    m_buildings = IDX(TOWN_BUILDING_TENT);
    m_buildState = 0;
    m_unknown1d = 0;
}

VA(0x004a404a, 0x3f)
i32 town::HasGarrison(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_army.m_creatureTypes[slot] != CREATURE_NONE)
            return 1;
    }
    return 0;
}

VA(0x004a4089, 0xde)
void town::GiveSpells(hero* targetHero) {
    hero* pupil;
    i32 stage;
    i32 slotN;

    if (targetHero == NULL && m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE)
        return;

    if (targetHero != NULL)
        pupil = targetHero;
    else
        pupil = gpGame->GetHero(m_occupyingHeroId);

    if (!pupil->HasArtifact(ARTIFACT_MAGIC_BOOK))
        return;
    if (!(m_buildings & IDX(TOWN_BUILDING_MAGE_GUILD)))
        return;

    for (stage = 0; stage < IDX(pupil->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
                                + TOWN_MAGE_GUILD_WISDOM_LEVEL_BONUS;
         ++stage) {
        for (slotN = 0; slotN < m_spellCounts[stage + TOWN_MAGE_GUILD_FIRST_LEVEL]; ++slotN) {
            pupil->AddSpell(m_spells[stage][slotN], pupil->Stats(HERO_PRIMARY_KNOWLEDGE));
        }
    }
}

VA(0x004a4167, 0xa5)
void town::XformToCastle(void) {
    i32 objectType;
    gpGame->ConvertObject(
        m_x + RANDOM_TOWN_LEFT,
        m_y + RANDOM_TOWN_TOP,
        m_x + RANDOM_TOWN_RIGHT,
        m_y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_OBJECT_TILESET,
        TOWN_CONVERT_SOURCE_FRAME,
        TOWN_CONVERT_ANY_FRAME,
        RANDOM_TOWN_OBJECT_TILESET,
        TOWN_CONVERT_OBJECT_NONE,
        MAP_OBJECT_CASTLE,
        MAP_OBJECT_CASTLE
    );
    gpGame->ConvertObject(
        m_x + RANDOM_TOWN_LEFT,
        m_y + RANDOM_TOWN_TOP,
        m_x + RANDOM_TOWN_RIGHT,
        m_y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_OVERLAY_TILESET,
        TOWN_CONVERT_SOURCE_FRAME,
        TOWN_CONVERT_ANY_FRAME,
        RANDOM_TOWN_OVERLAY_TILESET,
        TOWN_CONVERT_OBJECT_NONE,
        MAP_OBJECT_CASTLE,
        MAP_OBJECT_CASTLE
    );
}

VA(0x004a420c, 0xc4)
void town::View(i32 noFade) {
    bEnteringTown = 1;
    if (giHighMemBuffer + TOWN_VIEW_MEMORY_REQUIREMENT > TOWN_VIEW_HIGH_MEMORY_LIMIT)
        gAdvDisposeLevel = ADV_DISPOSE_FULL;
    else if (giHighMemBuffer + TOWN_VIEW_MEMORY_REQUIREMENT > TOWN_VIEW_LOW_MEMORY_LIMIT)
        gAdvDisposeLevel = ADV_DISPOSE_PARTIAL;

    townManager* manager = gpTownManager;
    manager->SetTown(this);
    if (!noFade)
        gpWindowManager->FadeScreen(FADE_OUT, TOWN_FADE_STEPS, NULL);
    gpExec->CallManager(gpTownManager);
    if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
        gpAdvManager->SetHeroContext(m_occupyingHeroId, 0);
    gAdvDisposeLevel = ADV_DISPOSE_NONE;
    bEnteringTown = 0;
}

VA(0x004a42d0, 0x129)
void town::Deallocate(void) {
    playerData* playerRecord = &gpGame->m_players[m_owner];
    i32 position = TOWN_ID_NONE;
    i32 i;

    for (i = 0; i < playerRecord->m_townCount; ++i) {
        if (playerRecord->m_townIds[i] == m_id)
            position = i;
    }
    for (i = position; i < playerRecord->m_townCount - 1; ++i)
        playerRecord->m_townIds[i] = playerRecord->m_townIds[i + 1];

    playerRecord->m_townIds[playerRecord->m_townCount - 1] = TOWN_ID_NONE;
    if (m_id == playerRecord->m_currentTown)
        playerRecord->m_currentTown = TOWN_ID_NONE;
    --playerRecord->m_townCount;
    if (playerRecord->m_townCount < TOWN_PLAYER_WINDOW_SIZE)
        playerRecord->m_townLocatorPage = 0;
    else if (playerRecord->m_townLocatorPage + TOWN_PLAYER_WINDOW_SIZE > playerRecord->m_townCount)
        playerRecord->m_townLocatorPage = playerRecord->m_townCount - TOWN_PLAYER_WINDOW_SIZE;

    gpGame->m_townOwners[m_id] = TOWN_OWNER_NONE;
    m_owner = TOWN_OWNER_NONE;
}

VA(0x004a43f9, 0x24c)
void town::BuildBuilding(H2_ENUM_PARAM(BuildingSlotType, i32) building) {
    i32 level;
    if (building == BUILDING_SLOT_MAGE_GUILD) {
        ++m_buildState;
        m_spellCounts[m_buildState] = gSpellLimits[m_buildState - TOWN_MAGE_GUILD_FIRST_LEVEL];
        if (m_type == FACTION_WIZARD && (m_buildings & IDX(TOWN_BUILDING_LIBRARY)))
            ++m_spellCounts[m_buildState];
        if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
            GiveSpells(NULL);
    }
    if (building == BUILDING_SLOT_SPECIAL && m_type == FACTION_WIZARD) {
        for (level = 0; level < m_buildState; ++level)
            ++m_spellCounts[level + TOWN_MAGE_GUILD_FIRST_LEVEL];
        if (m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE)
            GiveSpells(NULL);
    }

    m_buildings |= BIT(building);
    if (building == BUILDING_SLOT_UPGRADE_FIRST)
        m_buildings &= ~IDX(TOWN_BUILDING_DWELLING_2);
    if (building == BUILDING_SLOT_UPGRADE_SECOND)
        m_buildings &= ~IDX(TOWN_BUILDING_DWELLING_3);
    if (building == BUILDING_SLOT_UPGRADE_THIRD)
        m_buildings &= ~IDX(TOWN_BUILDING_DWELLING_4);
    if (building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE)
        m_buildings &= ~IDX(TOWN_BUILDING_DWELLING_5);
    if (building == BUILDING_SLOT_SPECIAL_TWENTY_NINE)
        m_buildings &= ~IDX(TOWN_BUILDING_DWELLING_6);
    if (building == BUILDING_SLOT_SPECIAL_THIRTY)
        m_buildings &= ~(IDX(TOWN_BUILDING_DWELLING_6) | IDX(TOWN_BUILDING_UPGRADED_DWELLING_6));

    if (building >= BUILDING_SLOT_DWELLING_FIRST && building <= BUILDING_SLOT_DWELLING_SIXTH) {
        m_garrison[IDX(building) - IDX(TOWN_OBJECT_DWELLING_1)] =
            gMonsterDatabase[IDX(
                gDwellingType[IDX(m_type)][IDX(building) - IDX(TOWN_OBJECT_DWELLING_1)]
            )]
                .growth;
    }
    if (building >= BUILDING_SLOT_UPGRADE_FIRST
        && building <= BUILDING_SLOT_SPECIAL_TWENTY_NINE) {
        m_garrison[IDX(building) - IDX(TOWN_OBJECT_DWELLING_1)] =
            m_garrison[IDX(building) - IDX(TOWN_OBJECT_DWELLING_6)];
    }
    if (building == BUILDING_SLOT_SPECIAL_THIRTY) {
        m_garrison[IDX(building) - IDX(TOWN_OBJECT_DWELLING_1)] =
            m_garrison[IDX(building) - IDX(TOWN_OBJECT_DWELLING_2)];
    }
    if (building == BUILDING_SLOT_CASTLE) {
        m_buildings &= ~IDX(TOWN_BUILDING_TENT);
        XformToCastle();
    }
    GiveSpells(NULL);
    BitSet(gpGame->m_knownTowns, m_id);
}

VA(0x004a4645, 0x1e)
i32 town::CanBuildDock(void) {
    return m_boatX != TOWN_DOCK_COORDINATE_NONE;
}

VA(0x004a4663, 0x9e)
void town::CalcNumLevelArchers(i32* numArchers, i32* mageGuildLevel) {
    *mageGuildLevel = m_buildState;
    *numArchers = 0;
    BuildingSlotType building;
    for (building = BUILDING_SLOT_DWELLING_FIRST; building <= BUILDING_SLOT_SPECIAL_THIRTY;
         ++building) {
        if (m_buildings & BIT(building))
            ++*numArchers;
    }
    for (building = BUILDING_SLOT_MAGE_GUILD; building <= BUILDING_SLOT_NEUTRAL_LAST;
         ++building) {
        if (m_buildings & BIT(building))
            ++*numArchers;
    }
}

i32 bEnteringTown = 0;
