// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
VA(0x004a8530, 0x5adb)
void advManager::DoEvent(class mapCell *, int, int) {}

VA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell *, int, int) {}

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
void advManager::PlayerMonsterInteract(class mapCell *, class mapCell *, class hero *, int *, int, int, int, int, int) {}

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
