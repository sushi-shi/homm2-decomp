// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004a8530, 0x5adb)
void advManager::DoEvent(class mapCell *, int, int) {}

// @early-stop
// reloc-masked: all 0x9f7 code bytes identical; residual is delinked local-label naming
VA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell *cell, int x, int y)
{
    int erased_a = 0;
    int frame_k = ERASE_NO_FRAME;
    signed char isWide_d = 0;
    mapCell *cells_h[ERASE_NEIGHBOR_COUNT];
    mapCellExtra *extras_b[ERASE_NEIGHBOR_COUNT];
    int i_e;
    int cellX_b[2];
    int cellY_e[2];
    mapCell *currentCell_k;
    mapCellExtra *extra_i;
    volatile int changed_i;

    for (i_e = 0; i_e < ERASE_NEIGHBOR_COUNT; i_e++) {
        cells_h[i_e] = 0;
        extras_b[i_e] = 0;
    }

    erased_a = 1;
    if (cell->objTileset == ERASE_TILESET_11)
        frame_k = cell->objIndex - 1;
    if (cell->objTileset == ERASE_TILESET_63)
        frame_k = cell->objIndex - 1;
    if (cell->objTileset == ERASE_TILESET_62 && cell->objIndex == 9) {
        frame_k = 9;
        isWide_d = 1;
    }
    if (cell->objTileset == ERASE_TILESET_59 && cell->objIndex == 131)
        frame_k = 124;
    if (cell->objTileset == ERASE_TILESET_55 && cell->objIndex == 61)
        frame_k = 54;
    if (cell->objTileset == ERASE_TILESET_50 && cell->objIndex == 45)
        frame_k = 38;
    if (cell->objTileset == ERASE_TILESET_50 && cell->objIndex == 19)
        frame_k = 12;
    if (cell->objTileset == ERASE_TILESET_46) {
        switch (cell->objIndex) {
        case 1: frame_k = 0; break;
        case 3: frame_k = 2; break;
        case 5: frame_k = 4; break;
        case 7: frame_k = 6; break;
        case 9: frame_k = 8; break;
        case 11: frame_k = 10; break;
        case 13: frame_k = 12; break;
        case 15: frame_k = 14; break;
        case 19: frame_k = 18; break;
        }
    }

    for (i_e = 0; i_e < ERASE_NEIGHBOR_COUNT; i_e++) {
        if (isWide_d)
            frame_k--;
        else if (i_e > 0)
            break;

        if (frame_k != ERASE_NO_FRAME) {
        if (isWide_d) {
            if (frame_k > 6)
                cellX_b[0] = frame_k - 9 + x;
            else
                cellX_b[0] = frame_k - 6 + x;
            if (frame_k > 6)
                cellY_e[0] = y;
            else
                cellY_e[0] = y - 1;
        }
        else {
            cellX_b[0] = x - 1;
            cellY_e[0] = y;
        }

        if (cellX_b[0] >= 0) {
        if (cellY_e[0] >= 0) {
        cells_h[i_e] = gpGame->m_worldMap.Row(cellY_e[0]) + cellX_b[0];
        if (i_e > 1) {
            cells_h[i_e]->ovlTileset = 0;
            cells_h[i_e]->ovlIndex = ERASE_EMPTY_INDEX;
        }
        else if (cells_h[i_e]->objIndex != ERASE_EMPTY_INDEX) {
        if (cells_h[i_e]->objIndex == frame_k && cells_h[i_e]->objTileset == cell->objTileset) {
            cells_h[i_e]->objIndex = 0;
            cells_h[i_e]->objTileset = ERASE_CLEARED_TILESET;
            cells_h[i_e]->objFlag0 = 0;
        }

        if (cells_h[i_e]->extra && m_mapData->Extra(cells_h[i_e]->extra)->objIndex != ERASE_EMPTY_INDEX)
            extras_b[i_e] = m_mapData->Extra(cells_h[i_e]->extra);
        else
            extras_b[i_e] = 0;

        while (extras_b[i_e]) {
            if (extras_b[i_e]->objIndex == frame_k && extras_b[i_e]->objTileset == cell->objTileset) {
                extras_b[i_e]->objIndex = 0;
                extras_b[i_e]->objTileset = ERASE_CLEARED_TILESET;
                extras_b[i_e]->objFlag = 0;
            }

            if (extras_b[i_e]->index && m_mapData->Extra(extras_b[i_e]->index)->objIndex != ERASE_EMPTY_INDEX)
                extras_b[i_e] = m_mapData->Extra(extras_b[i_e]->index);
            else
                extras_b[i_e] = 0;
        }
        }
        }
        }
        }
    }

    cell->triggerType = 0;
    cell->objIndex = 0;
    cell->objTileset = ERASE_CLEARED_TILESET;
    cell->objFlag0 = 0;

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;
        if (currentCell_k->objTileset != ERASE_CLEARED_TILESET)
            continue;

        if (currentCell_k->extra && m_mapData->Extra(currentCell_k->extra)->objIndex != ERASE_EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->extra);
        else
            continue;

        if (extra_i->objTileset == ERASE_CLEARED_TILESET || extra_i->objIndex == ERASE_EMPTY_INDEX)
            continue;

            currentCell_k->objIndex = extra_i->objIndex;
            currentCell_k->objTileset = extra_i->objTileset;
            currentCell_k->objFlag0 = extra_i->objFlag;
            currentCell_k->w4a = extra_i->f4a;
            currentCell_k->w4b = extra_i->f4b;
            extra_i->objIndex = 0;
            extra_i->objTileset = ERASE_CLEARED_TILESET;
            extra_i->objFlag = 0;
    }

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;

        if (currentCell_k->objTileset != ERASE_CLEARED_TILESET &&
            currentCell_k->objIndex != ERASE_EMPTY_INDEX && !currentCell_k->w4b)
            goto cellDone;

        if (currentCell_k->extra && m_mapData->Extra(currentCell_k->extra)->objIndex != ERASE_EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->extra);
        else
            extra_i = 0;

        while (extra_i) {
            if (extra_i->objTileset != ERASE_CLEARED_TILESET &&
                extra_i->objIndex != ERASE_EMPTY_INDEX && !extra_i->f4b)
                goto cellDone;

            if (extra_i->index && m_mapData->Extra(extra_i->index)->objIndex != ERASE_EMPTY_INDEX)
                extra_i = m_mapData->Extra(extra_i->index);
            else
                extra_i = 0;
        }

        currentCell_k->field8 |= 0x80;
cellDone:
        changed_i = 0;
    }

    SendMapChange(ERASE_MAP_CHANGE_OBJECT, 0, x, y, ERASE_MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(mapOriginX + ERASE_ENVIRONMENT_BORDER,
                         mapOriginY + ERASE_ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}

VA(0x004aea02, 0x90)
void advManager::HeroSwap(class hero *, class hero *) {}

VA(0x004aea92, 0x12f)
int advManager::BarrierEvent(class mapCell *, class hero *) { return 0; }

VA(0x004aebc1, 0x99)
signed char StrEqNoCase(char *, char *) { return 0; }

VA(0x004aec5a, 0xde)
void advManager::PasswordEvent(class mapCell *, class hero *) {}

VA(0x004aed38, 0x6fe)
void advManager::GenericSiteEvent(class mapCell *, class hero *) {}

VA(0x004af436, 0x191)
void advManager::RecruitSiteEvent(class mapCell *, class hero *) {}

VA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(class hero *, int, short int *) {}

VA(0x004af652, 0x22a)
void advManager::JailEvent(class mapCell *, class hero *, int, int) {}

VA(0x004af87c, 0x1da)
void advManager::TownEvent(class mapCell *, int, int) {}

// @early-stop
// reloc-masked: all 0x516 code bytes identical; residual is delinked jump-table/local-label and empty-string symbol naming
VA(0x004afa56, 0x516)
void advManager::EventSound(int eventType, int eventData, struct SAMPLE2 *outSample)
{
    const int treasureSound_a = EVENT_SOUND_TREASURE;
    const int experienceSound_o = EVENT_SOUND_EXPERIENCE;
    const int moraleSound_e = EVENT_SOUND_MORALE;
    const int luckSound_g = EVENT_SOUND_LUCK;
    const int pickupSound_o = EVENT_SOUND_PICKUP;
    const int mineSound_c = EVENT_SOUND_MINE;
    char sampleFile_m[EVENT_SOUND_FILENAME_LENGTH];
    strcpy(sampleFile_m, "");
    int musicTrack_e = EVENT_SOUND_NONE;

    switch (eventType) {
    case MAP_EVENT_ARTESIAN_SPRING:
    case MAP_EVENT_FLOTSAM:
        musicTrack_e = pickupSound_o;
    case MAP_EVENT_ARCHER_HOUSE:
    case MAP_EVENT_GOBLIN_HUT:
    case MAP_EVENT_DWARF_COTTAGE:
    case MAP_EVENT_PEASANT_HUT:
    case MAP_EVENT_LOG_CABIN:
    case MAP_EVENT_TREE_CITY:
    case MAP_EVENT_RUINS:
    case MAP_EVENT_IDOL:
    case MAP_EVENT_SHRINE_SECOND_CIRCLE:
    case MAP_EVENT_WAGON:
    case MAP_EVENT_MAGELLAN_MAPS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_WATER_WHEEL:
    case MAP_EVENT_LEAN_TO:
        musicTrack_e = EVENT_SOUND_TRACK_40;
        break;
    case MAP_EVENT_SEA_CHEST:
    case MAP_EVENT_PYRAMID:
    case MAP_EVENT_CITY_OF_DEAD:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_BOAT:
        if (eventData == EVENT_SOUND_VARIANT_1)
            musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_SKELETON:
    case MAP_EVENT_MINE:
    case MAP_EVENT_MERMAID:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_DAEMON_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_36;
        break;
    case MAP_EVENT_SHIPWRECK:
    case MAP_EVENT_WATERING_HOLE:
    case MAP_EVENT_OBSERVATION_TOWER:
        musicTrack_e = EVENT_SOUND_TRACK_32;
        break;
    case MAP_EVENT_FAERIE_RING:
    case MAP_EVENT_FOUNTAIN:
    case MAP_EVENT_WITCH_DOCTOR_HUT:
        musicTrack_e = luckSound_g;
        break;
    case MAP_EVENT_BUOY:
    case MAP_EVENT_RESOURCE:
    case MAP_EVENT_HALFLING_HOLE:
    case MAP_EVENT_XANADU:
        musicTrack_e = moraleSound_e;
        break;
    case MAP_EVENT_GAZEBO:
    case MAP_EVENT_TEMPLE:
    case MAP_EVENT_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_37;
        break;
    case MAP_EVENT_ANCIENT_LAMP:
    case MAP_EVENT_TAR_PIT:
        musicTrack_e = EVENT_SOUND_TRACK_33;
        break;
    case MAP_EVENT_GRAVEYARD:
    case MAP_EVENT_DESERT_TENT:
    case MAP_EVENT_STANDING_STONES:
    case MAP_EVENT_DERELICT_SHIP:
    case MAP_EVENT_BOTTLE:
    case MAP_EVENT_REEFS:
        musicTrack_e = EVENT_SOUND_TRACK_39;
        break;
    case MAP_EVENT_LIGHTHOUSE:
    case MAP_EVENT_EXCAVATION:
    case MAP_EVENT_SPHINX:
    case MAP_EVENT_WITCH_HUT:
        musicTrack_e = EVENT_SOUND_TRACK_31;
        break;
    case MAP_EVENT_OASIS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_ABANDONED_MINE:
    case MAP_EVENT_TREE_OF_KNOWLEDGE:
    case MAP_EVENT_HILL_FORT:
    case MAP_EVENT_SHRINE_THIRD_CIRCLE:
        musicTrack_e = EVENT_SOUND_TRACK_38;
        break;
    case MAP_EVENT_STONE_LITHS:
    case MAP_EVENT_MERCENARY_CAMP:
        musicTrack_e = EVENT_SOUND_TRACK_34;
        break;
    case MAP_EVENT_HUT_OF_MAGI:
        musicTrack_e = EVENT_SOUND_TRACK_1;
        break;
    case MAP_EVENT_WHIRLPOOL:
    case MAP_EVENT_FORT:
    case MAP_EVENT_FREEMANS_FOUNDRY:
        musicTrack_e = EVENT_SOUND_TRACK_35;
        break;
    case MAP_EVENT_EXPANSION_DWELLING:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_EXPANSION_OBJECT:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_EXPANSION_ALTAR:
        switch (eventData) {
        case EVENT_SOUND_VARIANT_0:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_1:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_5:
            musicTrack_e = luckSound_g;
            break;
        case EVENT_SOUND_VARIANT_2:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_3:
            break;
        case EVENT_SOUND_VARIANT_6:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_4:
            musicTrack_e = experienceSound_o;
            break;
        default:
            ;
        }
        break;
    case MAP_EVENT_JAIL:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_EXPANSION_SITE:
        musicTrack_e = experienceSound_o;
        break;
    default:
        musicTrack_e = EVENT_SOUND_NONE;
        break;
    }

    if (musicTrack_e != EVENT_SOUND_NONE) {
    switch (musicTrack_e) {
    case EVENT_SOUND_TREASURE:
        strcpy(sampleFile_m, "treasure.82m");
        break;
    case EVENT_SOUND_EXPERIENCE:
        strcpy(sampleFile_m, "expernce.82m");
        break;
    case EVENT_SOUND_MORALE:
        strcpy(sampleFile_m, "goodmrle.82m");
        break;
    case EVENT_SOUND_LUCK:
        strcpy(sampleFile_m, "goodluck.82m");
        break;
    case EVENT_SOUND_PICKUP:
        strcpy(sampleFile_m, "pickup01.82m");
        break;
    case EVENT_SOUND_MINE:
        strcpy(sampleFile_m, "h2mine.82m");
        break;
    default:
        gpSoundManager->SwitchAmbientMusic(musicTrack_e);
        break;
    }

    if (strlen(sampleFile_m) > EVENT_SOUND_SAMPLE_NAME_EMPTY_LIMIT)
        *outSample = LoadPlaySample(sampleFile_m);
    }
}

VA(0x004aff6c, 0xc7)
void advManager::EventWindow(int, int, char *, int, int, int, int, int) {}

VA(0x004b0033, 0xb6)
int GiveArtifact(class hero *, int, int, signed char) { return 0; }

VA(0x004b00e9, 0x5e)
int advManager::GiveRandomArtifact(class hero *) { return 0; }

VA(0x004b0147, 0x67)
int advManager::GiveExperience(class hero *, int, int) { return 0; }

VA(0x004b01ae, 0x80)
void advManager::GiveResource(class hero *, int, int) {}

VA(0x004b022e, 0xbb)
void advManager::RecruitEvent(class hero *, int, class mapCell *) {}

VA(0x004b02e9, 0x261)
int advManager::SkeletonEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b054a, 0x29b)
int advManager::ZombieEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b07e5, 0x2f8)
int advManager::GhostEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b0add, 0x274)
void advManager::HouseEvent(class hero *, class mapCell *) {}

VA(0x004b0d51, 0x62f)
int advManager::CombatMonsterEvent(class hero *, int, int, class mapCell *, int, int, int, int, int, int, int, int, int, int, int) { return 0; }

VA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(class hero *, int, int) {}

VA(0x004b1973, 0x1dd)
void advManager::TransferArtifacts(class hero *, class hero *) {}

VA(0x004b1b50, 0x7f)
void advManager::HeroLoses(class hero *) {}

VA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(class hero *) {}

VA(0x004b1d01, 0x142)
void advManager::FizzleCenter(int) {}

VA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(class mapCell *, class hero *, int, int) {}

VA(0x004b4883, 0x65)
int advManager::BarrierAIEvent(class mapCell *, class hero *) { return 0; }

VA(0x004b48e8, 0x53)
void advManager::PasswordAIEvent(class mapCell *, class hero *) {}

VA(0x004b493b, 0x369)
void advManager::GenericSiteAIEvent(class mapCell *, class hero *) {}

VA(0x004b4ca4, 0x18f)
void advManager::RecruitSiteAIEvent(class mapCell *, class hero *) {}

VA(0x004b4e33, 0x1a2)
void advManager::JailAIEvent(class mapCell *, class hero *, int, int) {}

VA(0x004b4fd5, 0x82b)
void advManager::PlayerMonsterInteract(mapCell *cell, mapCell *combatCell, hero *eventHero,
                                       int *handled, int x, int y, int unused,
                                       int combatX, int combatY)
{
    static char *followerText =
        "{Followers}\n\nA group of %s with a desire for greater glory wish to join you. Do you accept? ";
    int monster_n;
    float strengthRatio_p;
    int combatResult_f;
    int forcedJoin_f;
    int joining;
    int monsterCount_n;
    int joiningCost_i;
    char offerText_g[MONSTER_OFFER_BUFFER_SIZE];

    unused = 0;
    gpMouseManager->ShowColorPointer();
    monster_n = cell->objIndex;
    forcedJoin_f = cell->w4hi & MONSTER_JOIN_FORCED;
    monsterCount_n = cell->w4hi & MONSTER_COUNT_MASK;
    strengthRatio_p = static_cast<float>(gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)) /
                      static_cast<float>(gMonsterDatabase[monster_n].fightValue * monsterCount_n);

    if (gbInCampaign &&
        ((gpGame->m_dwarfAlliance && (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)) ||
         (gpGame->m_ogreAlliance && (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)) ||
         (gpGame->m_dragonAlliance && (monster_n == MONSTER_GREEN_DRAGON || monster_n == MONSTER_RED_DRAGON ||
                                       monster_n == MONSTER_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monster_n)) {
            if (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)
                NormalDialog("The dwarves hail you, \"Any friend of Roland is a friend of ours.  You may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)
                NormalDialog("The ogres give you a grunt of recognition, \"Archibald's allies may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog("The dragons see you and call out.  \"Our alliance with Archibald compels us to join you.  Unfortunately you have no room.  A pity!\"  They quickly scatter.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            *handled = 1;
        }
        else {
            if (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)
                NormalDialog("The dwarves recognize their allies and gladly join your forces.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)
                NormalDialog("The ogres recognize you as the Dwarfbane and lumber over to join you.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog("The dragons, snarling and growling, agree to join forces with you, their 'Ally'.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_dwarfbane &&
        (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)) {
        NormalDialog("\"The Dwarfbane!!!!, run for your lives.\"", 1, -1, -1, -1, 0, -1, 0, -1, 0);
        *handled = 1;
        return;
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(0) &&
        (monster_n == MONSTER_ELF || monster_n == MONSTER_GRAND_ELF)) {
        *handled = 1;
        if (eventHero->m_army.CanJoin(monster_n)) {
            NormalDialog("As you approach the group of elves, their leader calls them all to attention.  He shouts to them, \"Who of you is brave enough to join this fearless ally of ours?\"  The group explodes with cheers as they run to join your ranks.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
        }
        else {
            NormalDialog("The elves stand at attention as you approach.  Their leader calls to you and says, \"Let us not impede your progress, ally!  Move on, and may victory be yours.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        return;
    }

    if (eventHero->m_army.CanJoin(monster_n) && strengthRatio_p > MONSTER_STRENGTH_JOIN &&
        !eventHero->HasArtifact(MONSTER_NO_JOIN_ARTIFACT) &&
        monster_n != MONSTER_GENIE && monster_n != MONSTER_EARTH_ELEMENTAL &&
        monster_n != MONSTER_AIR_ELEMENTAL && monster_n != MONSTER_FIRE_ELEMENTAL &&
        monster_n != MONSTER_WATER_ELEMENTAL) {
        if (forcedJoin_f) {
            sprintf(gText, followerText, gArmyNamesPlural[monster_n]);
            EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, monsterCount_n, -1);
                *handled = 1;
                return;
            }
            else {
                EventWindow(0x43, 1, "Insulted by your refusal of their offer, the monsters attack!",
                            -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
        else if (eventHero->m_diplomacy != MONSTER_DIPLOMACY_NONE) {
            if (eventHero->m_diplomacy == MONSTER_DIPLOMACY_EXPERT)
                joining = monsterCount_n;
            else if (eventHero->m_diplomacy == MONSTER_DIPLOMACY_ADVANCED)
                joining = monsterCount_n / 2;
            else
                joining = monsterCount_n / 4;
            if (!joining)
                joining = 1;

            joiningCost_i = gMonsterDatabase[monster_n].cost * monsterCount_n;
            if (joiningCost_i > gpGame->m_players[eventHero->m_owner].resources[RES_GOLD]) {
                if (strengthRatio_p > MONSTER_STRENGTH_FLEE)
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (monsterCount_n == 1) {
                sprintf(gText,
                        "The %s is swayed by your diplomatic tongue, and offers to join your army for the sum of %d gold.  Do you accept?",
                        gArmyNames[monster_n], joiningCost_i);
            }
            else {
                sprintf(gText,
                        "The creatures are swayed by your diplomatic tongue, and make you an offer:\n\n");
                if (monsterCount_n == joining)
                    sprintf(offerText_g,
                            "All %d of the %s will join your army for the sum of %d gold.  Do you accept?",
                            monsterCount_n, gArmyNamesPlural[monster_n], joiningCost_i);
                else
                    sprintf(offerText_g,
                            "%d of the %d %s will join your army, and the rest will leave you alone, for the sum of %d gold.  Do you accept?",
                            joining, monsterCount_n, gArmyNamesPlural[monster_n], joiningCost_i);
                strcat(gText, offerText_g);
            }

            NormalDialog(gText, 2, -1, -1, RES_GOLD, joiningCost_i, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, joining, -1);
                *handled = 1;
                gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= joiningCost_i;
                return;
            }
            else {
                EventWindow(0x43, 1, "Insulted by your refusal of their offer, the monsters attack!",
                            -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
    }

    if (strengthRatio_p > MONSTER_STRENGTH_FLEE) {
monstersFlee:
        sprintf(gText,
                "The %s, awed by the power of your forces, begin to scatter.  Do you wish to pursue and engage them?",
                gArmyNamesPlural[monster_n]);
        EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
            goto fightMonsters;
        *handled = 1;
        return;
    }

fightMonsters:
    combatResult_f = CombatMonsterEvent(eventHero, monster_n, monsterCount_n, combatCell, x, y, unused,
                                        combatX, combatY, -1, 0, 0, -1, 0, 0);
    if (combatResult_f == 0 || combatResult_f == -1)
        *handled = 1;
}

VA(0x004b5800, 0x440)
void advManager::ComputerMonsterInteract(class mapCell *, class hero *, int *) {}

VA(0x004b5c40, 0x1d0)
int advManager::DoNetCombat(char *) { return 0; }

VA(0x004b5e10, 0x64e)
int advManager::DoCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int) { return 0; }

VA(0x004b645e, 0x36f)
void advManager::SendHeroTownData(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int, int, int, int) {}

VA(0x004b67cd, 0x462)
void advManager::ReceiveHeroTownData(char *, int *, int *, int *, class hero * *, class armyGroup * *, class town * *, class hero * *, class armyGroup * *, int *, int *, int *, signed char *, signed char *, signed char *) {}

VA(0x004b6c2f, 0x254)
int advManager::AutoResolveCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int) { return 0; }

VA(0x004b6e83, 0xb8)
int RiddleStringsEqual(char *, char *) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x0051cc0c) int gbNoShowCombat;
