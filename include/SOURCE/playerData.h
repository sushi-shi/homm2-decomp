#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H

#include <va.h>
#include <SOURCE/GAME.h>

H2_ENUM_BEGIN(PlayerDataStorageConstant)
    PLAYER_HERO_CAPACITY         = 8,
    PLAYER_AVAILABLE_HERO_COUNT  = 2,
    PLAYER_UNUSED_SAVE_DATA_SIZE = 0x2c,
    PLAYER_BARRIER_STATE_SIZE    = 6,
    PLAYER_RUNTIME_TAIL_GAP_SIZE = 0x1c
H2_ENUM_END(PlayerDataStorageConstant)

#pragma pack(push, 1)
struct playerAttentionWeights {
    float gameWeightA;
    float gameRemainder;
    float gameWeightB;
    float buildingValue;
    float upgradeBase;
    float heroValue;
};
SIZE(playerAttentionWeights, 0x18);

class playerData {
public:
    i8 m_color;
    i8 m_heroCount;
    i8 m_currentHero;
    i8 m_heroLocatorPage;
    i8 m_heroIds[PLAYER_HERO_CAPACITY];
    i8 m_availableHeroIds[PLAYER_AVAILABLE_HERO_COUNT];
    i8 m_minimumHeroCount;
    i32 m_aiDifficulty;
    i8 m_cheatValue;
    char m_unusedSaveData[PLAYER_UNUSED_SAVE_DATA_SIZE];
    i8 m_ultimateArtifactHintChance;
    i8 m_ultimateArtifactHintX;
    i8 m_ultimateArtifactHintY;
    i8 m_daysLeft;
    i8 m_townCount;
    i8 m_currentTown;
    i8 m_townLocatorPage;
    i8 m_townIds[IDX(GAME_TOWN_COUNT)];
    i32 m_resources[IDX(RES_COUNT)];
    i8 m_evilInterface;
    i8 m_barrierTents;
    char m_unknownad[PLAYER_BARRIER_STATE_SIZE];
    playerAttentionWeights m_attentionWeights;
    char m_unknownCb[PLAYER_RUNTIME_TAIL_GAP_SIZE];
    i32 m_income[IDX(RES_COUNT)];
    i32 m_obeliskValue;
    i32 m_totalObeliskValue;
    i32 m_unexploredValue;
    float m_upgradeValueWeight;
    float m_artifactValue;
    float m_artifactPoolShare;
    void Write(i32);
    void Read(i32);
    i32 NextHero(i32);
    i32 HasMobileHero(void);
    i32 BuildingsOwned(i32, BuildingSlotType, i32);
    i32 NumOfGivenArtifact(ArtifactType);
    i8 CurrentHero(void) {
        return m_currentHero;
    }
    i8 CurrentTown(void) {
        return m_currentTown;
    }
    i8 Color(void) {
        return m_color;
    }
    i8 HeroCount(void) {
        return m_heroCount;
    }
    i8 TownCount(void) {
        return m_townCount;
    }
    i8 HeroId(i32 index) {
        return m_heroIds[index];
    }
    i8 TownId(i32 index) {
        return m_townIds[index];
    }
    i8 AvailableHeroId(i32 index) {
        return m_availableHeroIds[index];
    }
};
#pragma pack(pop)
SIZE(playerData, 0x11b);
extern playerData* gpCurPlayer;
#endif
