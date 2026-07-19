#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H

#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <BASE/icon.h>
#include <SOURCE/ARMY.h>

class sample;

#pragma pack(push, 1)
class army {
public:
    char m_animationState;
    char m_animationCycle;
    char m_pendingAnimationSequence;
    i8 m_effectAnimationStart;
    i8 m_effectAnimationEnd;
    i8 m_effectAnimationLength;
    i32 m_drawState;
    struct SLimitData m_creatureLimits;
    struct SLimitData m_quantityLimits;
    struct SLimitData m_spriteLimits;
    struct SLimitData m_spellLimits;
    i32 m_standingAnimation;
    i32 m_showQuantity;
    i32 m_targetSide;
    i32 m_targetIndex;
    i32 m_attackDirection;
    i32 m_unknown5e;
    i32 m_moveTargetHex;
    i32 m_drawSpellEffect;
    i32 m_mirrorSourceIndex;
    i32 m_mirrorImageIndex;
    i32 m_roundCounter;
    CreatureType m_monsterType;
    i32 m_hex;
    ArmyAnimationSequence m_animationSequence;
    i32 m_animationFrame;
    i32 m_facing;
    i32 m_walkDirection;
    i32 m_facingChanged;
    i32 m_initialQuantity;
    i32 m_quantity;
    i32 m_lastTargetHex;
    i32 m_temporaryResurrectionQuantity;
    i32 m_hitPointsLost;
    i32 m_armyGroupSlot;
    i32 m_damagePenalty;
    i32 m_speed;
    i32 m_walkDuration;
    i32 m_luckOutcome;
    struct tag_monsterInfo m_monster;
    i16 m_unknownD4;
    i32 m_damagePending;
    i32 m_killPending;
    i32 m_deathPending;
    i32 m_spellEffect;
    i32 m_side;
    i32 m_index;
    i32 m_lastAnimationTime;
    i32 m_morale;
    i32 m_luck;
    i32 m_spellEffectYOffset;
    i32 m_yOffset;
    i32 m_xOffset;
    i32 m_spellCount;
    u8 m_spellInfluence[ARMY_SPELL_INFLUENCE_COUNT];
    i32 m_effectAnimationStarted;
    i32 m_drawEnabled;
    i32 m_hitByCreature;
    i8* m_palette;
    struct SMonFrameInfo m_frameInfo;
    class icon* m_creatureIcon;
    class icon* m_missileIcon;
    class sample* m_samples[IDX(ARMY_SAMPLE_COUNT)];
    army(void);
    void WaitSample(ArmySampleType);
    void InitClean(void);
    void Init(CreatureType, i32, i32, i32, i32, i32);
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
    i32 Damage(i32l, SpellType);
    void PowEffect(i32, i32, i32, i32);
    u32l Strength(void);
    i32 LeaveNoBody(void);
    void ProcessDeath(i32);
    void SpellEffect(i32, i32, i32);
    void CancelSpellType(ArmySpellCancelType);
    void CancelIndividualSpell(ArmySpellInfluence);
    i32 SetSpellInfluence(ArmySpellInfluence, i32);
    void DecrementSpellRounds(void);
    void GoBerserk(void);
    void MoveAttack(i32, i32);
    float SpellCastWorkChance(SpellType);
    i32 SpellCastWorks(SpellType);
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
        return m_monsterType >= CREATURE_PEASANT && m_quantity > 0;
    }
};
#pragma pack(pop)
SIZE(army, 0x482);
extern i32 bSecondAttack;
extern b32 gbGenieHalf;

#endif
