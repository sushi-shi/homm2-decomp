#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class playerData {
public:
    signed char m_color;  // +0x00
    signed char m_heroCount;  // +0x01
    signed char m_currentHero;  // +0x02
    signed char m_heroLocatorPage;  // +0x03
    signed char m_heroIds[8];  // +0x04
    char m_unknown0c[2];  // +0x0c
    signed char m_unknown0e;  // +0x0e
    int m_unknown0f;  // +0x0f
    signed char m_cheatValue;  // +0x13
    char m_unusedSaveData[0x2c];  // +0x14
    signed char m_canDig;  // +0x40
    signed char m_unknown41;  // +0x41
    signed char m_unknown42;  // +0x42
    signed char m_daysLeft;  // +0x43
    signed char m_townCount;  // +0x44
    signed char m_currentTown;  // +0x45
    signed char m_townLocatorPage;  // +0x46
    signed char m_townIds[72];  // +0x47
    int m_resources[7];  // +0x8f
    signed char m_unknownab;  // +0xab
    signed char m_unknownac;  // +0xac
    char m_unknownad[6];  // +0xad
    char m_pad_0xb3[0x34];
    int m_secondaryResources[7];  // +0xe7
    char m_pad_0x103[0x18];
    // --- methods ---
    void Write(int);
    void Read(int);
    int NextHero(int);
    int HasMobileHero(void);
    int BuildingsOwned(int, int, int);
    int NumOfGivenArtifact(int);
    signed char CurrentHero(void) { return m_currentHero; }
    signed char CurrentTown(void) { return m_currentTown; }
    signed char TownId(int index) { return m_townIds[index]; }
};
#pragma pack(pop)
SIZE(playerData, 0x11b);
#endif // HOMM2_SOURCE_PLAYERDATA_H
