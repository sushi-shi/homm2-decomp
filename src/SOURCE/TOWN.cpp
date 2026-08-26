#include <Ints.h>
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
town::town(void) {
    m_type = FACTION_KNIGHT;
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_occupyingHeroId = TOWN_OCCUPYING_HERO_NONE;
    m_buildings = H2EnumIndex(TOWN_BUILDING_TENT);
    m_buildState = 0;
    m_unknown1d = 0;
}

i32 town::HasGarrison(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_army.m_creatureTypes[slot] != CREATURE_NONE)
            return 1;
    }
    return 0;
}

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
    if (!(m_buildings & H2EnumIndex(TOWN_BUILDING_MAGE_GUILD)))
        return;

    for (stage = 0; stage < H2EnumIndex(pupil->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
                                + TOWN_MAGE_GUILD_WISDOM_LEVEL_BONUS;
         ++stage) {
        for (slotN = 0; slotN < m_spellCounts[stage + TOWN_MAGE_GUILD_FIRST_LEVEL]; ++slotN) {
            pupil->AddSpell(m_spells[stage][slotN], pupil->Stats(HERO_PRIMARY_KNOWLEDGE));
        }
    }
}

void town::XformToCastle(void) {

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

void town::Deallocate(void) {
    playerData* pp = &gpGame->m_players[m_owner];
    i32 pos = TOWN_ID_NONE;
    i32 i;

    for (i = 0; i < pp->m_townCount; ++i) {
        if (pp->m_townIds[i] == m_id)
            pos = i;
    }
    for (i = pos; i < pp->m_townCount - 1; ++i)
        pp->m_townIds[i] = pp->m_townIds[i + 1];

    pp->m_townIds[pp->m_townCount - 1] = TOWN_ID_NONE;
    if (pp->m_currentTown == m_id)
        pp->m_currentTown = TOWN_ID_NONE;
    --pp->m_townCount;
    if (pp->m_townCount < TOWN_PLAYER_WINDOW_SIZE)
        pp->m_townLocatorPage = 0;
    else if (pp->m_townLocatorPage + TOWN_PLAYER_WINDOW_SIZE > pp->m_townCount)
        pp->m_townLocatorPage = pp->m_townCount - TOWN_PLAYER_WINDOW_SIZE;

    gpGame->m_townOwners[m_id] = TOWN_OWNER_NONE;
    m_owner = TOWN_OWNER_NONE;
}

void town::BuildBuilding(BuildingSlotType building) {
    i32 level;
    if (building == BUILDING_SLOT_MAGE_GUILD) {
        ++m_buildState;
        m_spellCounts[m_buildState] = gSpellLimits[m_buildState - TOWN_MAGE_GUILD_FIRST_LEVEL];
        if (m_type == FACTION_WIZARD && (m_buildings & H2EnumIndex(TOWN_BUILDING_LIBRARY)))
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

    m_buildings |= (1 << H2EnumIndex(building));
    if (building == BUILDING_SLOT_UPGRADE_FIRST)
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_DWELLING_2);
    if (building == BUILDING_SLOT_UPGRADE_SECOND)
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_DWELLING_3);
    if (building == BUILDING_SLOT_UPGRADE_THIRD)
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_DWELLING_4);
    if (building == BUILDING_SLOT_NECROMANCER_MAGE_PREREQUISITE)
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_DWELLING_5);
    if (building == BUILDING_SLOT_SPECIAL_TWENTY_NINE)
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_DWELLING_6);
    if (building == BUILDING_SLOT_SPECIAL_THIRTY)
        m_buildings &= ~(H2EnumIndex(TOWN_BUILDING_DWELLING_6) | H2EnumIndex(TOWN_BUILDING_UPGRADED_DWELLING_6));

    if (building >= BUILDING_SLOT_DWELLING_FIRST && building <= BUILDING_SLOT_DWELLING_SIXTH) {
        m_garrison[H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1)] =
            gMonsterDatabase[H2EnumIndex(gDwellingType[H2EnumIndex(m_type)][H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1)])]
                .growth;
    }
    if (building >= BUILDING_SLOT_UPGRADE_FIRST
        && building <= BUILDING_SLOT_SPECIAL_TWENTY_NINE) {
        m_garrison[H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1)] =
            m_garrison[H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_6)];
    }
    if (building == BUILDING_SLOT_SPECIAL_THIRTY) {
        m_garrison[H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_1)] =
            m_garrison[H2EnumIndex(building) - H2EnumIndex(TOWN_OBJECT_DWELLING_2)];
    }
    if (building == BUILDING_SLOT_CASTLE) {
        m_buildings &= ~H2EnumIndex(TOWN_BUILDING_TENT);
        XformToCastle();
    }
    GiveSpells(NULL);
    H2BitSet(gpGame->m_knownTowns, m_id);
}

i32 town::CanBuildDock(void) {
    return m_boatX != TOWN_DOCK_COORDINATE_NONE;
}

void town::CalcNumLevelArchers(i32* numArchers, i32* mageGuildLevel) {
    *mageGuildLevel = m_buildState;
    *numArchers = 0;
    BuildingSlotType building;
    for (building = BUILDING_SLOT_DWELLING_FIRST; building <= BUILDING_SLOT_SPECIAL_THIRTY;
         ++building) {
        if (m_buildings & (1 << H2EnumIndex(building)))
            ++*numArchers;
    }
    for (building = BUILDING_SLOT_MAGE_GUILD; building <= BUILDING_SLOT_NEUTRAL_LAST;
         ++building) {
        if (m_buildings & (1 << H2EnumIndex(building)))
            ++*numArchers;
    }
}

i32 bEnteringTown = 0;
