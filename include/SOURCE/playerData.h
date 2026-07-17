#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1) // recovered layout is byte-packed
struct playerAttentionWeights {
    float gameWeightA;   // +0x00
    float gameRemainder; // +0x04
    float gameWeightB;   // +0x08
    float buildingValue; // +0x0c
    float upgradeBase;   // +0x10
    float heroValue;     // +0x14
};
SIZE(playerAttentionWeights, 0x18);

class playerData {
public:
    i8 m_color;                      // +0x00
    i8 m_heroCount;                  // +0x01
    i8 m_currentHero;                // +0x02
    i8 m_heroLocatorPage;            // +0x03
    i8 m_heroIds[8];                 // +0x04
    i8 m_availableHeroIds[2];        // +0x0c
    i8 m_minimumHeroCount;           // +0x0e
    i32 m_aiDifficulty;              // +0x0f
    i8 m_cheatValue;                 // +0x13
    char m_unusedSaveData[0x2c];     // +0x14
    i8 m_ultimateArtifactHintChance; // +0x40
    i8 m_ultimateArtifactHintX;      // +0x41
    i8 m_ultimateArtifactHintY;      // +0x42
    i8 m_daysLeft;                   // +0x43
    i8 m_townCount;                  // +0x44
    i8 m_currentTown;                // +0x45
    i8 m_townLocatorPage;            // +0x46
    i8 m_townIds[72];                // +0x47
    i32 m_resources[7];              // +0x8f
    i8 m_evilInterface;              // +0xab
    i8 m_barrierTents;               // +0xac
    char m_unknownad
        [6]; // +0xad save-serialized in playerData::Write/Read but never otherwise read
    playerAttentionWeights m_attentionWeights; // +0xb3
    char m_unknownCb
        [0x1c]; // +0xcb never referenced; sits after the AI attention weights
    i32 m_income[7];            // +0xe7
    i32 m_obeliskValue;         // +0x103
    i32 m_totalObeliskValue;    // +0x107
    i32 m_unexploredValue;      // +0x10b
    float m_upgradeValueWeight; // +0x10f
    float m_artifactValue;      // +0x113
    float m_artifactPoolShare;  // +0x117
    // --- methods ---
    void Write(i32);
    void Read(i32);
    i32 NextHero(i32);
    i32 HasMobileHero(void);
    i32 BuildingsOwned(i32, BuildingSlotType, i32);
    i32 NumOfGivenArtifact(i32);
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
#endif // HOMM2_SOURCE_PLAYERDATA_H
