#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
struct playerAttentionWeights {
    float gameWeightA;    // +0x00
    float gameRemainder;  // +0x04
    float gameWeightB;    // +0x08
    float buildingValue;  // +0x0c
    float upgradeBase;    // +0x10
    float heroValue;      // +0x14
};
SIZE(playerAttentionWeights, 0x18);

class playerData {
public:
    signed char m_color;  // +0x00
    signed char m_heroCount;  // +0x01
    signed char m_currentHero;  // +0x02
    signed char m_heroLocatorPage;  // +0x03
    signed char m_heroIds[8];  // +0x04
    signed char m_availableHeroIds[2];  // +0x0c
    signed char m_minimumHeroCount;  // +0x0e
    int m_aiDifficulty;  // +0x0f
    signed char m_cheatValue;  // +0x13
    char m_unusedSaveData[0x2c];  // +0x14
    signed char m_ultimateArtifactHintChance;  // +0x40
    signed char m_ultimateArtifactHintX;  // +0x41
    signed char m_ultimateArtifactHintY;  // +0x42
    signed char m_daysLeft;  // +0x43
    signed char m_townCount;  // +0x44
    signed char m_currentTown;  // +0x45
    signed char m_townLocatorPage;  // +0x46
    signed char m_townIds[72];  // +0x47
    int m_resources[7];  // +0x8f
    signed char m_evilInterface;  // +0xab
    signed char m_barrierTents;  // +0xac
    char m_unknownad[6];  // +0xad
    playerAttentionWeights m_attentionWeights;  // +0xb3
    char m_unknownCb[0x1c];  // +0xcb
    int m_income[7];  // +0xe7
    int m_obeliskValue;  // +0x103
    int m_totalObeliskValue;  // +0x107
    int m_unexploredValue;  // +0x10b
    float m_upgradeValueWeight;  // +0x10f
    float m_artifactValue;  // +0x113
    float m_artifactPoolShare;  // +0x117
    // --- methods ---
    void Write(int);
    void Read(int);
    int NextHero(int);
    int HasMobileHero(void);
    int BuildingsOwned(int, int, int);
    int NumOfGivenArtifact(int);
    signed char CurrentHero(void) { return m_currentHero; }
    signed char CurrentTown(void) { return m_currentTown; }
    signed char Color(void) { return m_color; }
    signed char HeroCount(void) { return m_heroCount; }
    signed char TownCount(void) { return m_townCount; }
    signed char HeroId(int index) { return m_heroIds[index]; }
    signed char TownId(int index) { return m_townIds[index]; }
    signed char AvailableHeroId(int index) { return m_availableHeroIds[index]; }
};
#pragma pack(pop)
SIZE(playerData, 0x11b);
extern playerData *gpCurPlayer;
#endif // HOMM2_SOURCE_PLAYERDATA_H
