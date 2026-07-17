#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 57 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <BASE/icon.h>
#include <SOURCE/ARMY.h>

class sample;

#pragma pack(push, 1) // recovered layout is byte-packed
class army {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char m_animationState;              // +0x00
    char m_animationCycle;              // +0x01
    char m_pendingAnimationSequence;    // +0x02
    i8 m_effectAnimationStart;          // +0x03
    i8 m_effectAnimationEnd;            // +0x04
    i8 m_effectAnimationLength;         // +0x05
    i32 m_drawState;                    // +0x06
    struct SLimitData m_creatureLimits; // +0x0a
    struct SLimitData m_quantityLimits; // +0x1a
    struct SLimitData m_spriteLimits;   // +0x2a
    struct SLimitData m_spellLimits;    // +0x3a
    i32 m_standingAnimation;            // +0x4a
    i32 m_showQuantity;                 // +0x4e
    i32 m_targetSide;                   // +0x52
    i32 m_targetIndex;                  // +0x56
    i32 m_attackDirection;              // +0x5a
    i32 m_unknown5e; // +0x5e target-cluster int, only zeroed in the ctor; unnamed and unused in Ironfist (field_5E) too
    i32 m_moveTargetHex;                 // +0x62
    i32 m_drawSpellEffect;               // +0x66
    i32 m_mirrorSourceIndex;             // +0x6a
    i32 m_mirrorImageIndex;              // +0x6e
    i32 m_roundCounter;                  // +0x72
    i32 m_monsterType;                   // +0x76
    i32 m_hex;                           // +0x7a
    i32 m_animationSequence;             // +0x7e
    i32 m_animationFrame;                // +0x82
    i32 m_facing;                        // +0x86
    i32 m_walkDirection;                 // +0x8a
    i32 m_facingChanged;                 // +0x8e
    i32 m_initialQuantity;               // +0x92
    i32 m_quantity;                      // +0x96
    i32 m_lastTargetHex;                 // +0x9a
    i32 m_temporaryResurrectionQuantity; // +0x9e
    i32 m_hitPointsLost;                 // +0xa2
    i32 m_armyGroupSlot;                 // +0xa6
    i32 m_damagePenalty;                 // +0xaa
    i32 m_speed;                         // +0xae
    i32 m_walkDuration;                  // +0xb2
    i32 m_luckOutcome;                   // +0xb6
    struct tag_monsterInfo m_monster;    // +0xba
    i16 m_unknownD4; // +0xd4 set to 6 in Init, never read; unnamed in Ironfist (field_D4, same i16 width) too
    i32 m_damagePending;                             // +0xd6
    i32 m_killPending;                               // +0xda
    i32 m_deathPending;                              // +0xde
    i32 m_spellEffect;                               // +0xe2
    i32 m_side;                                      // +0xe6
    i32 m_index;                                     // +0xea
    i32 m_lastAnimationTime;                         // +0xee
    i32 m_morale;                                    // +0xf2
    i32 m_luck;                                      // +0xf6
    i32 m_spellEffectYOffset;                        // +0xfa
    i32 m_yOffset;                                   // +0xfe
    i32 m_xOffset;                                   // +0x102
    i32 m_spellCount;                                // +0x106
    u8 m_spellInfluence[ARMY_SPELL_INFLUENCE_COUNT]; // +0x10a
    i32 m_effectAnimationStarted;                    // +0x119
    i32 m_drawEnabled;                               // +0x11d
    i32 m_hitByCreature;                             // +0x121
    i8* m_palette;                                   // +0x125
    struct SMonFrameInfo m_frameInfo;                // +0x129
    class icon* m_creatureIcon;                      // +0x45e
    class icon* m_missileIcon;                       // +0x462
    class sample* m_samples[ARMY_SAMPLE_COUNT];      // +0x466
    // --- constructors ---
    army(void);
    // --- methods ---
    void WaitSample(i32);
    void InitClean(void);
    void Init(i32, i32, i32, i32, i32, i32);
    void LoadResources(void);
    void FreeResources(void);
    void DrawToBuffer(i32, i32, i32);
    void Wince(void);
    void Walk(i32, i32, i32);
    void SpecialAttack(void);
    void DirDoAttack(i32);
    void DoHydraAttack(i32);
    void DoAttack(i32);
    void ResetPath(void);
    i32 WalkTo(void);
    i32 WalkTo(i32);
    i32 AttackTo(void);
    i32 AttackTo(i32);
    void CheckLuck(void);
    void DamageEnemy(class army*, i32*, i32*, i32, i32);
    i32 Damage(i32l, i32);
    void PowEffect(i32, i32, i32, i32);
    u32l Strength(void);
    i32 LeaveNoBody(void);
    void ProcessDeath(i32);
    void SpellEffect(i32, i32, i32);
    void CancelSpellType(i32);
    void CancelIndividualSpell(i32);
    i32 SetSpellInfluence(i32, i32);
    void DecrementSpellRounds(void);
    void GoBerserk(void);
    void MoveAttack(i32, i32);
    float SpellCastWorkChance(i32);
    i32 SpellCastWorks(i32);
    void DispelGood(void);
    void Cure(i32);
    i32 MidX(void);
    i32 MidY(void);
    i32 TopY(void);
    i32 RightX(void);
    i32 LeftX(void);
    i32 OtherArmyAdjacent(i32, i32);
    i32 GetPowBaseY(void);
    i32 CanFit(i32, i32, i32*);
    i32 ValidFlight(i32, i32);
    i32 FlyTo(void);
    i32 FlyTo(i32);
    i32 FindPath(i32, i32, i32, i32, i32);
    i32 ValidPath(i32, i32);
    i32 GetMoveMask(i32);
    i32 GetAttackMask(i32, i32, i32);
    i32 ValidMove(i32);
    i32 ValidMove(i32, i32);
    i32 ValidAttack(i32, i32, i32, i32, i32*);
    i32 GetAdjacentCellIndex(i32, i32);
    i32 ValidRange(i32);
    i32 GetBestDirection(i32, i32, i32);
    i32 IsAlive(void) {
        return m_monsterType >= 0 && m_quantity > 0;
    }
};
#pragma pack(pop)
SIZE(army, 0x482);
// ---- globals (declarations, RVA order) ----
extern i32 bSecondAttack;
extern i32 gbGenieHalf;

#endif // HOMM2_SOURCE_ARMY_H
