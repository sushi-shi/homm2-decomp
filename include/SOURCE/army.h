#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 57 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <_types.h>
#include <BASE/icon.h>
#include <SOURCE/ARMY.h>

class sample;

#pragma pack(push, 1)  // recovered layout is byte-packed
class army {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char   m_animationState;  // +0x00
    char _pad_0x1[0x1];
    char   m_animationDelay;  // +0x02
    char _pad_0x3[0x3];
    int    m_drawState;  // +0x06
    struct SLimitData m_creatureLimits;  // +0x0a
    struct SLimitData m_quantityLimits;  // +0x1a
    struct SLimitData m_spriteLimits;  // +0x2a
    struct SLimitData m_spellLimits;  // +0x3a
    char m_padding4A[4];
    int    m_showQuantity;  // +0x4e
    int    m_targetSide;  // +0x52
    int    m_targetIndex;  // +0x56
    int    m_attackDirection;  // +0x5a
    int    field_0x5e;  // +0x5e
    int    m_moveTargetHex;  // +0x62
    int    field_0x66;  // +0x66
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    int    field_0x72;  // +0x72
    int    m_monsterType;  // +0x76
    int    m_hex;  // +0x7a
    int    m_animationSequence;  // +0x7e
    int    m_animationFrame;  // +0x82
    int    m_facing;  // +0x86
    int    m_walkDirection;  // +0x8a
    int    m_facingChanged;  // +0x8e
    int    m_initialQuantity;  // +0x92
    int    m_quantity;  // +0x96
    int    m_lastTargetHex;  // +0x9a
    int    field_0x9e;  // +0x9e
    int    m_hitPointsLost;  // +0xa2
    int    field_0xa6;  // +0xa6
    int    m_damagePenalty;  // +0xaa
    int    m_speed;  // +0xae
    int    m_walkDuration;  // +0xb2
    int    m_luckOutcome;  // +0xb6
    struct tag_monsterInfo m_monster;  // +0xba
    short  field_0xd4;  // +0xd4
    int    m_animationValue1;  // +0xd6
    int    m_animationValue2;  // +0xda
    int    m_animationValue3;  // +0xde
    int    m_spellEffect;  // +0xe2
    int    m_side;  // +0xe6
    int    m_index;  // +0xea
    int    m_lastAnimationTime;  // +0xee
    int    m_morale;  // +0xf2
    int    m_luck;  // +0xf6
    int    field_0xfa;  // +0xfa
    int    m_yOffset;  // +0xfe
    int    m_xOffset;  // +0x102
    int    m_spellCount;  // +0x106
    unsigned char m_spellInfluence[ARMY_SPELL_INFLUENCE_COUNT];  // +0x10a
    char _pad_0x119[0x4];
    int    m_drawEnabled;  // +0x11d
    int    m_hitByCreature;  // +0x121
    signed char *m_palette;  // +0x125
    struct SMonFrameInfo m_frameInfo;  // +0x129
    class icon *m_creatureIcon;  // +0x45e
    class icon *m_missileIcon;  // +0x462
    class sample *m_samples[ARMY_SAMPLE_COUNT];  // +0x466
    // --- constructors ---
    army(void);
    // --- methods ---
    void WaitSample(int);
    void InitClean(void);
    void Init(int, int, int, int, int, int);
    void LoadResources(void);
    void FreeResources(void);
    void DrawToBuffer(int, int, int);
    void Wince(void);
    void Walk(int, int, int);
    void SpecialAttack(void);
    void DirDoAttack(int);
    void DoHydraAttack(int);
    void DoAttack(int);
    void ResetPath(void);
    int WalkTo(void);
    int WalkTo(int);
    int AttackTo(void);
    int AttackTo(int);
    void CheckLuck(void);
    void DamageEnemy(class army *, int *, int *, int, int);
    int Damage(long int, int);
    void PowEffect(int, int, int, int);
    unsigned long int Strength(void);
    int LeaveNoBody(void);
    void ProcessDeath(int);
    void SpellEffect(int, int, int);
    void CancelSpellType(int);
    void CancelIndividualSpell(int);
    int SetSpellInfluence(int, int);
    void DecrementSpellRounds(void);
    void GoBerserk(void);
    void MoveAttack(int, int);
    float SpellCastWorkChance(int);
    int SpellCastWorks(int);
    void DispelGood(void);
    void Cure(int);
    int MidX(void);
    int MidY(void);
    int TopY(void);
    int RightX(void);
    int LeftX(void);
    int OtherArmyAdjacent(int, int);
    int GetPowBaseY(void);
    int CanFit(int, int, int *);
    int ValidFlight(int, int);
    int FlyTo(void);
    int FlyTo(int);
    int FindPath(int, int, int, int, int);
    int ValidPath(int, int);
    int GetMoveMask(int);
    int GetAttackMask(int, int, int);
    int ValidMove(int);
    int ValidMove(int, int);
    int ValidAttack(int, int, int, int, int *);
    int GetAdjacentCellIndex(int, int);
    int ValidRange(int);
    int GetBestDirection(int, int, int);
    int IsAlive(void) { return m_monsterType >= 0 && m_quantity > 0; }
};
#pragma pack(pop)
SIZE(army, 0x482);
// ---- globals (declarations, RVA order) ----
extern int bSecondAttack;
extern int gbGenieHalf;

#endif // HOMM2_SOURCE_ARMY_H
