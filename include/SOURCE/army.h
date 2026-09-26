#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H

#include <Ints.h>
#include <vector>
#include <SOURCE/KB_TYPES.h>
#include <BASE/icon.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/combatTypes.h>

class sample;

enum class ArmyAttackTarget : i32 {
    ARMY_ATTACK_TARGET_ASSIGNED = 0,
    ARMY_ATTACK_TARGET_ENEMY    = 1,
    ARMY_ATTACK_TARGET_OCCUPIED = 2
};
using enum ArmyAttackTarget;

enum class ArmyDamagePenalty : i32 {
    ARMY_DAMAGE_PENALTY_NONE = 0,
    ARMY_DAMAGE_PENALTY_HALF = 2
};
using enum ArmyDamagePenalty;

typedef enum ArmyHexConstant {
    ARMY_ATTACK_HEX_COUNT = 2,
    ARMY_HEX_INVALID      = -1
} ArmyHexConstant;

#pragma pack(push, 1)
class army {
public:
    u8 m_animationState;
    u8 m_animationCycle;
    H2SteppedEnumStorage<ArmyAnimationSequence, char> m_pendingAnimationSequence;
    H2EnumStorage<ArmyAnimationSequence, i8> m_effectAnimationStart;
    H2EnumStorage<ArmyAnimationSequence, i8> m_effectAnimationEnd;
    i8 m_effectAnimationLength;
    ArmyDrawState m_drawState;
    struct SLimitData m_creatureLimits;
    struct SLimitData m_quantityLimits;
    struct SLimitData m_spriteLimits;
    struct SLimitData m_spellLimits;
    i32 m_standingAnimation;
    b32 m_showQuantity;
    H2EnumStorage<CombatSide, i32> m_targetSide;
    i32 m_targetIndex;
    CombatHexDirection m_attackDirection;
    i32 m_unknown5e;
    i32 m_moveTargetHex;
    b32 m_drawSpellEffect;
    i32 m_mirrorSourceIndex;
    i32 m_mirrorImageIndex;
    i32 m_roundCounter;
    H2EnumStorage<CreatureType, i32> m_monsterType;
    i32 m_hex;
    H2SteppedEnumStorage<ArmyAnimationSequence, i32> m_animationSequence;
    i32 m_animationFrame;
    H2EnumStorage<ArmyFacing, i32> m_facing;
    CombatHexDirection m_walkDirection;
    b32 m_facingChanged;
    i32 m_initialQuantity;
    i32 m_quantity;
    i32 m_lastTargetHex;
    i32 m_temporaryResurrectionQuantity;
    i32 m_hitPointsLost;
    i32 m_armyGroupSlot;
    ArmyDamagePenalty m_damagePenalty;
    i32 m_speed;
    i32 m_walkDuration;
    i32 m_luckOutcome;
    struct tag_monsterInfo m_monster;
    i16 m_unknownD4;
    b32 m_damagePending;
    b32 m_killPending;
    b32 m_deathPending;
    H2EnumStorage<SpellType, i32> m_spellEffect;
    H2EnumStorage<CombatSide, i32> m_side;
    i32 m_index;
    i32 m_lastAnimationTime;
    i32 m_morale;
    i32 m_luck;
    i32 m_spellEffectYOffset;
    i32 m_yOffset;
    i32 m_xOffset;
    i32 m_spellCount;
    u8 m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_COUNT)];
    b32 m_effectAnimationStarted;
    b32 m_drawEnabled;
    b32 m_hitByCreature;
    i8* m_palette;
    struct SMonFrameInfo m_frameInfo;
    class icon* m_creatureIcon;
    class icon* m_missileIcon;
    class sample* m_samples[H2EnumIndex(ARMY_SAMPLE_COUNT)];
    army(void);
    void WaitSample(ArmySampleType);
    void InitClean(void);
    void Init(CreatureType monsterType, i32 quantity, CombatSide side, i32 index, i32 hex, i32 unknown);
    void LoadResources(void);
    void FreeResources(void);
    void DrawToBuffer(i32 x, i32 y, i32 effectsOnly);
    void Wince(void);
    void Walk(CombatHexDirection direction, i32 finishStanding, i32 skipDrawing);
    void SpecialAttack(void);
    void DirDoAttack(CombatHexDirection direction);
    void DoHydraAttack(i32);
    void DoAttack(i32 retaliation);
    void SetChargingMoveAnimation(ChargingDirection direction);
    void RevertChargingMoveAnimation(void);
    void SetJumpingAnimation(void);
    void RevertJumpingAnimation(void);
    void ChargingDamage(const std::vector<i32>& affectedHexes);
    void ResetPath(void);
    i32 WalkTo(void);
    i32 WalkTo(i32 destination);
    i32 AttackTo(void);
    i32 AttackTo(i32 destination);
    void CheckLuck(void);
    void DamageEnemy(class army* target, i32* damageResult, i32* killedResult, i32 rangedAttack, i32 retaliation);
    i32 Damage(i32l damage, SpellType spell);
    void PowEffect(CombatEffectType effect, i32 resetLimits, i32 effectX, i32 effectY);
    u32l Strength(void);
    i32 LeaveNoBody(void);
    void ProcessDeath(i32 immediate);
    void SpellEffect(CombatEffectType effect, i32 effectFrameDelay, i32 animateCreature);
    void CancelSpellType(ArmySpellCancelType cancelType);
    void CancelIndividualSpell(ArmySpellInfluence influence);
    i32 SetSpellInfluence(ArmySpellInfluence influence, i32 rounds);
    void DecrementSpellRounds(void);
    void GoBerserk(void);
    void MoveAttack(i32 destination, i32 moveOnly);
    float SpellCastWorkChance(SpellType spell);
    i32 SpellCastWorks(SpellType spell);
    void DispelGood(void);
    void Cure(i32 amount);
    i32 MidX(void);
    i32 MidY(void);
    i32 TopY(void);
    i32 RightX(void);
    i32 LeftX(void);
    i32 OtherArmyAdjacent(CombatSide side, i32 index);
    i32 GetPowBaseY(void);
    i32 CanFit(i32 hex, i32 tryOtherSide, i32* fittingHex);
    i32 ValidFlight(i32 destination, ArmyPathTarget pathMode);
    i32 FlyTo(void);
    i32 FlyTo(i32 destination);
    i32 FindPath(i32 sourceHex, i32 targetHex, i32, i32 ignoreSpeed, ArmyPathTarget pathMode);
    i32 ValidPath(i32 targetHex, ArmyPathTarget pathMode);
    i32 GetMoveMask(i32 sourceHex);
    i32 GetAttackMask(i32 sourceHex, ArmyAttackTarget targetMode, i32 targetHex);
    i32 ValidMove(CombatHexDirection direction);
    i32 ValidMove(i32 sourceHex, CombatHexDirection direction);
    i32 ValidAttack(i32 sourceHex, CombatHexDirection direction, ArmyAttackTarget targetMode, i32 requiredTargetHex, i32* attackHex);
    i32 GetAdjacentCellIndex(i32 sourceHex, CombatHexDirection direction);
    bool IsCloseMove(i32 destination);
    void ArcJump(i32 fromHex, i32 toHex);
    void MoveTo(i32 destination);
    bool TargetOnStraightLine(i32 targetHex);
    CombatHexDirection GetStraightLineDirection(i32 targetHex);
    i32 GetStraightLineDistanceToHex(i32 targetHex);
    bool FlightThroughObstacles(i32 destination);
    bool IsEnemyCreatureHex(i32 hexIndex);
    void MoveAttackNonFlyer(i32 startHex, i32 attackMask);
    i32 ValidRange(i32 targetHex);
    CombatHexDirection GetBestDirection(i32 sourceHex, i32 targetHex, i32 blockedMask);
    i32 IsAlive(void) {
        return m_monsterType >= CREATURE_PEASANT && m_quantity > 0;
    }
};
#pragma pack(pop)

#define CLEAR_ARMY_TARGET(a) ((a).m_targetSide = COMBAT_SIDE_NONE, (a).m_targetIndex = -1)

#define ARMY_HAS_BERSERK_OR_HYPNOTIZE(a)                                                           \
    ((a).m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]                                       \
     || (a).m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
#define ARMY_HAS_INCAPACITATING_SPELL(a)                                                           \
    ((a).m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)]                                         \
     || (a).m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]                                   \
     || (a).m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)])
extern b32 bSecondAttack;
extern b32 gbGenieHalf;

#endif
