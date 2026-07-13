#ifndef HOMM2_SOURCE_HERO_H
#define HOMM2_SOURCE_HERO_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 34 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>
// forward declarations:
class town;

typedef enum HeroSecondarySkill {
    HERO_SKILL_PATHFINDING = 0,
    HERO_SKILL_ARCHERY = 3,
    HERO_SKILL_DIPLOMACY = 4,
    HERO_SKILL_MYSTICISM = 5,
    HERO_SKILL_LEADERSHIP = 6,
    HERO_SKILL_WISDOM = 7,
    HERO_SKILL_BALLISTICS = 10,
    HERO_SKILL_NECROMANCY = 12
} HeroSecondarySkill;

#pragma pack(push, 1)  // recovered layout is byte-packed
class hero {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  m_spellPoints;  // +0x00
    char   m_id;  // +0x02
    char   m_owner;  // +0x03
    short  field_0x4;  // +0x04
    char _pad_0x6[0x1];
    short  field_0x7;  // +0x07
    char _pad_0x9[0x1];
    char m_name[13];  // +0x0a
    unsigned char m_cursorType;  // +0x17
    unsigned char m_unknown18;  // +0x18
    int    m_x;  // +0x19
    int    m_y;  // +0x1d
    int    m_destinationX;  // +0x21
    int    m_destinationY;  // +0x25
    unsigned char m_boatId;  // +0x29
    char m_unknown2a;  // +0x2a
    signed char m_unknown2b;  // +0x2b
    unsigned char m_direction;  // +0x2c
    short  m_locationType;  // +0x2d
    short  m_occupiedTown;  // +0x2f
    int m_mobility;  // +0x31
    int m_remainingMobility;  // +0x35
    int m_experience;  // +0x39
    short m_level;  // +0x3d
    signed char m_attack;  // +0x3f
    signed char m_defense;  // +0x40
    signed char m_spellPower;  // +0x41
    signed char m_knowledge;  // +0x42
    char _pad_0x43[0x1];
    signed char m_morale;  // +0x44
    signed char m_luck;  // +0x45
    char _pad_0x46[0x1];
    unsigned int m_gazeboVisits;  // +0x47
    unsigned int m_fortVisits;  // +0x4b
    unsigned int m_witchDoctorVisits;  // +0x4f
    unsigned int m_mercenaryCampVisits;  // +0x53
    unsigned int m_standingStoneVisits;  // +0x57
    unsigned int m_treeKnowledgeVisits;  // +0x5b
    unsigned int m_xanaduVisits;  // +0x5f
    char   field_0x63;  // +0x63
    char   field_0x64;  // +0x64
    class armyGroup m_army;  // +0x65
    signed char m_secondarySkills[14];  // +0x74
    signed char m_secondarySkillOrder[14];  // +0x82
    int m_secondarySkillCount;  // +0x90
    char _pad_0x94[0x41];
    signed char m_artifacts[14];  // +0xd5
    unsigned int m_eventFlags;  // +0xe3
    char m_unknownE7;  // +0xe7
    float m_aiFightValue;  // +0xe8
    char m_pad_0xec[0xe];
    // --- constructors ---
    hero(void);
    // --- methods ---
    void Read(int, signed char);
    void Write(int, signed char);
    void GetArmyStrengths(unsigned long int * const);
    int HasArtifact(int);
    int CalcMobility(void);
    int HasSpell(int);
    int GetNthSpell(int, int);
    int GetNumSpells(int);
    void UseSpell(int);
    void AddSpell(int, int);
    void HeroScreenUpdate(void);
    void UpdateArmies(void);
    void ViewStat(int, int);
    void ViewArtifact(int, int, int);
    int Dismiss(void);
    void Deallocate(int);
    int GetExperience(int);
    int GetLevel(int);
    void ApplyBattleWinTemps(void);
    void ApplyBattleLossTemps(void);
    void CheckLevel(void);
    int NumArtifacts(void);
    void SetSS(int, int);
    int TakeSS(int, int);
    int GiveSS(int, int);
    int CreatureTypeCount(int);
    void UpgradeCreatures(int, int);
    int GetNthSS(int);
    class town * GetOccupiedTown(void);
    signed char Stats(int);
    signed char GetSSLevel(int);
    void DoSSLevelDialog(int, int);
    void CheckAnduranPieces(int);
};
#pragma pack(pop)
SIZE(hero, 250);
// ---- globals (declarations, RVA order) ----
extern class hero *gpHVHero;
extern class heroWindow *gheroWin;
extern short *gMinExpForLevel;
extern int iOrigHeroViewID;
extern int gbNoDismiss;

#endif // HOMM2_SOURCE_HERO_H
