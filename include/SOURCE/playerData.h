#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H

#include <va.h>
#include <SOURCE/GAME.h>

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
    i8 m_heroIds[8];
    i8 m_availableHeroIds[2];
    i8 m_minimumHeroCount;
    i32 m_aiDifficulty;
    i8 m_cheatValue;
    char m_unusedSaveData[0x2c];
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
    char m_unknownad[6];
    playerAttentionWeights m_attentionWeights;
    char m_unknownCb[0x1c];
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
