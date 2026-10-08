#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H

#include <H2/Ints.h>
#include <Domains.h>
#include <SOURCE/kbTypes.h>
#include <BASE/icon.h>
#include <SOURCE/combatTypes.h>

enum {
    ARMY_ANIMATION_NONE                  = -1,
    ARMY_ANIMATION_WALK_BEGIN            = 0,
    ARMY_ANIMATION_WALK_BEGIN_STANDING   = 1,
    ARMY_ANIMATION_WALK_MIDDLE           = 2,
    ARMY_ANIMATION_WALK_END              = 3,
    ARMY_ANIMATION_WALK_END_STANDING     = 4,
    ARMY_ANIMATION_WALK_STAND            = 5,
    ARMY_ANIMATION_WALK_SOURCE_COUNT     = 6,
    ARMY_ANIMATION_WALK                  = 6,
    ARMY_ANIMATION_STAND                 = 7,
    ARMY_ANIMATION_STANDING_FIRST        = 8,
    ARMY_ANIMATION_STANDING_LAST         = 12,
    ARMY_ANIMATION_STANDING_END          = 13,
    ARMY_ANIMATION_DEATH                 = 13,
    ARMY_ANIMATION_WINCE                 = 14,
    ARMY_ANIMATION_WINCE_RETURN          = 15,
    ARMY_ANIMATION_ATTACK_UP             = 16,
    ARMY_ANIMATION_ATTACK_UP_RETURN      = 17,
    ARMY_ANIMATION_BREATH_UP             = 18,
    ARMY_ANIMATION_BREATH_UP_RETURN      = 19,
    ARMY_ANIMATION_ATTACK_FORWARD        = 20,
    ARMY_ANIMATION_ATTACK_FORWARD_RETURN = 21,
    ARMY_ANIMATION_BREATH_FORWARD        = 22,
    ARMY_ANIMATION_BREATH_FORWARD_RETURN = 23,
    ARMY_ANIMATION_ATTACK_DOWN           = 24,
    ARMY_ANIMATION_ATTACK_DOWN_RETURN    = 25,
    ARMY_ANIMATION_BREATH_DOWN           = 26,
    ARMY_ANIMATION_BREATH_DOWN_RETURN    = 27,
    ARMY_ANIMATION_SHOOT_UP              = 28,
    ARMY_ANIMATION_SHOOT_UP_RETURN       = 29,
    ARMY_ANIMATION_SHOOT_FORWARD         = 30,
    ARMY_ANIMATION_SHOOT_FORWARD_RETURN  = 31,
    ARMY_ANIMATION_SHOOT_DOWN            = 32,
    ARMY_ANIMATION_SHOOT_DOWN_RETURN     = 33
};
typedef i32 ArmyAnimationSequence;

enum {
    ARMY_SAMPLE_MOVE,
    ARMY_SAMPLE_ATTACK,
    ARMY_SAMPLE_WINCE,
    ARMY_SAMPLE_SHOT,
    ARMY_SAMPLE_KILL,
    ARMY_SAMPLE_EXTRA_ONE,
    ARMY_SAMPLE_EXTRA_TWO,
    ARMY_SAMPLE_COUNT
};
typedef i32 ArmySampleType;
typedef enum ArmyCombatConstant {
    ARMY_SAMPLE_VOLUME                   = 64,
    ARMY_SAMPLE_CHANNEL                  = 3,
    ARMY_PRIMARY_SAMPLE_COUNT            = (ARMY_SAMPLE_KILL) + 1,
    ARMY_QUANTITY_TEXT_SIZE              = 12,
    ARMY_SPELL_EFFECT_ANIMATION_DURATION = 275,
    ARMY_MAGE_BOLT_DELAY                 = 115,
    ARMY_POW_EFFECT_DELAY                = 120,
    ARMY_ARCHMAGE_DISPEL_CHANCE          = 20,
    ARMY_PERCENT_MAX                     = 100
} ArmyCombatConstant;

enum {
    ARMY_SPELL_INFLUENCE_NONE          = -1,
    ARMY_SPELL_INFLUENCE_HASTE         = 0,
    ARMY_SPELL_INFLUENCE_SLOW          = 1,
    ARMY_SPELL_INFLUENCE_BLIND         = 2,
    ARMY_SPELL_INFLUENCE_BLESS         = 3,
    ARMY_SPELL_INFLUENCE_CURSE         = 4,
    ARMY_SPELL_INFLUENCE_BERSERK       = 5,
    ARMY_SPELL_INFLUENCE_PARALYZE      = 6,
    ARMY_SPELL_INFLUENCE_HYPNOTIZE     = 7,
    ARMY_SPELL_INFLUENCE_DRAGON_SLAYER = 8,
    ARMY_SPELL_INFLUENCE_BLOODLUST     = 9,
    ARMY_SPELL_INFLUENCE_SHIELD        = 10,
    ARMY_SPELL_INFLUENCE_PETRIFIED     = 11,
    ARMY_SPELL_INFLUENCE_ANTI_MAGIC    = 12,
    ARMY_SPELL_INFLUENCE_STONESKIN     = 13,
    ARMY_SPELL_INFLUENCE_STEELSKIN     = 14,
    ARMY_SPELL_INFLUENCE_COUNT         = 15
};
typedef i32 ArmySpellInfluence;
enum {
    ARMY_CANCEL_SPELLS_AFTER_MOVE   = 0,
    ARMY_CANCEL_SPELLS_AFTER_ATTACK = 1,
    ARMY_CANCEL_SPELLS_AFTER_DAMAGE = 2,
    ARMY_CANCEL_SPELLS_UNUSED       = 3
};
typedef i32 ArmySpellCancelType;
typedef enum ArmyAttackConstant {
    ARMY_COMBAT_TEXT_SIZE              = 800,
    ARMY_LUCK_ROLL_MAX                 = 24,
    ARMY_ATTACK_EFFECT_CHANCE          = 20,
    ARMY_ROYAL_MUMMY_EFFECT_CHANCE     = 30,
    ARMY_BREATH_ATTACK_SEQUENCE_OFFSET = 2,
    ARMY_NEAREST_DISTANCE_LIMIT        = 999999,
    ARMY_RETALIATION_DELAY             = 150,
    ARMY_SECOND_ATTACK_DELAY           = 100,
    ARMY_BAD_LUCK_EFFECT_DELAY         = 180,
    ARMY_ASCII_CASE_OFFSET             = 32,
    ARMY_DAMAGE_STAT_LIMIT             = 20,
    ARMY_MOAT_ATTACK_BONUS             = 3,
    ARMY_DRAGON_SLAYER_BONUS           = 5,
    ARMY_GENIE_HALF_ROLL_MAX           = 5,
    ARMY_GENIE_HALF_ROLL               = 2,
    ARMY_PATH_BLOCKED                  = 3
} ArmyAttackConstant;

typedef enum ArmyFrameConstant {
    ARMY_MISSILE_OFFSET_COUNT     = 3,
    ARMY_PROJECTILE_ANGLE_COUNT   = 12,
    ARMY_ANIMATION_SEQUENCE_COUNT = 34,
    ARMY_ANIMATION_FRAME_COUNT    = 16,
    ARMY_STANDING_CHANCE_COUNT    = 10
} ArmyFrameConstant;

#pragma pack(push, 1)
struct SMonFrameInfo {
    struct MissileOffset {
        i16 x;
        i16 y;
    };
    char unused00;
    i16 blindEffectX;
    i16 blindEffectY;
    i8 animationXOffsets[(ARMY_ANIMATION_WALK_SOURCE_COUNT)][ARMY_ANIMATION_FRAME_COUNT];
    i8 walkXOffsets[ARMY_ANIMATION_FRAME_COUNT];
    i8 standingAnimationCount;
    float standingAnimationChances[ARMY_STANDING_CHANCE_COUNT];
    i32 standStillDelay;
    i32 walkDuration;
    i32 attackDuration;
    i32 flightSpeed;
    struct MissileOffset missileOffsets[ARMY_MISSILE_OFFSET_COUNT];
    u8 projectileDirectionCount;
    float projectileAngles[ARMY_PROJECTILE_ANGLE_COUNT];
    i32 quantityX[(ARMY_FACING_COUNT)];
    i8 animationFrameCount[ARMY_ANIMATION_SEQUENCE_COUNT];
    i8 animationFrames[ARMY_ANIMATION_SEQUENCE_COUNT][ARMY_ANIMATION_FRAME_COUNT];
};
#pragma pack(pop)

class sample;

enum {
    ARMY_ATTACK_TARGET_ASSIGNED = 0,
    ARMY_ATTACK_TARGET_ENEMY    = 1,
    ARMY_ATTACK_TARGET_OCCUPIED = 2
};
typedef i32 ArmyAttackTarget;
enum {
    ARMY_DAMAGE_PENALTY_NONE = 0,
    ARMY_DAMAGE_PENALTY_HALF = 2
};
typedef i32 ArmyDamagePenalty;
typedef enum ArmyHexConstant {
    ARMY_ATTACK_HEX_COUNT = 2,
    ARMY_HEX_INVALID      = -1
} ArmyHexConstant;

typedef enum ArmyDisplayConstant {
    ARMY_QUANTITY_OVERRIDE_NONE = -1
} ArmyDisplayConstant;

#pragma pack(push, 1)
class army {
public:
    u8 m_attackPending;
    u8 m_shootingAnimationActive;
    char m_pendingAnimationSequence;
    i8 m_effectAnimationStart;
    i8 m_effectAnimationEnd;
    i8 m_effectAnimationLength;
    ArmyDrawState m_drawState;
    struct SLimitData m_creatureLimits;
    struct SLimitData m_quantityLimits;
    struct SLimitData m_spriteLimits;
    struct SLimitData m_spellLimits;
    i32 m_standingAnimation;
    b32 m_showQuantity;
    i32 m_targetSide;
    i32 m_targetIndex;
    CombatHexDirection m_attackDirection;
    i32 m_unused5e;
    i32 m_moveTargetHex;
    b32 m_drawSpellEffect;
    i32 m_mirrorSourceIndex;
    i32 m_mirrorImageIndex;
    i32 m_mirrorImageRoundsRemaining;
    i32 m_monsterType;
    i32 m_hex;
    i32 m_animationSequence;
    i32 m_animationFrame;
    i32 m_facing;
    CombatHexDirection m_walkDirection;
    b32 m_facingChanged;
    i32 m_initialQuantity;
    i32 m_quantity;
    i32 m_displayQuantityOverride;
    i32 m_temporaryResurrectionQuantity;
    i32 m_hitPointsLost;
    i32 m_armyGroupSlot;
    ArmyDamagePenalty m_damagePenalty;
    i32 m_speed;
    i32 m_walkDuration;
    i32 m_luckOutcome;
    struct tag_monsterInfo m_monster;
    i16 m_unusedD4;
    b32 m_damagePending;
    b32 m_killPending;
    b32 m_deathPending;
    i32 m_pendingAbilitySpell;
    i32 m_side;
    i32 m_index;
    i32 m_lastAnimationTime;
    i32 m_morale;
    i32 m_luck;
    i32 m_spellEffectYOffset;
    i32 m_yOffset;
    i32 m_xOffset;
    i32 m_spellCount;
    u8 m_spellInfluence[(ARMY_SPELL_INFLUENCE_COUNT)];
    b32 m_effectAnimationFinished;
    b32 m_drawEnabled;
    b32 m_hitByCreature;
    i8* m_yModify;
    struct SMonFrameInfo m_frameInfo;
    class icon* m_creatureIcon;
    class icon* m_missileIcon;
    class sample* m_samples[(ARMY_SAMPLE_COUNT)];
    army(void);
    void WaitSample(ArmySampleType sampleIndex);
    void InitClean(void);
    void Init(CreatureType monsterType, i32 quantity, CombatSide side, i32 index, i32 hex, i32 armyGroupSlot);
    void LoadResources(void);
    void FreeResources(void);
    void DrawToBuffer(i32 x, i32 y, i32 quantityOverlayOnly);
    void Wince(void);
    void Walk(CombatHexDirection direction, i32 finishStanding, i32 skipDrawing);
    void SpecialAttack(void);
    void DirDoAttack(CombatHexDirection direction);
    void DoHydraAttack(i32);
    void DoAttack(i32 retaliation);
    void ResetPath(void);
    i32 WalkTo(void);
    i32 WalkTo(i32 destination);
    i32 AttackTo(void);
    i32 AttackTo(i32 destination);
    void CheckLuck(void);
    void DamageEnemy(class army* target, i32* damageResult, i32* killedResult, i32 rangedAttack, i32 defenseModifier);
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
    i32 ValidRange(i32 targetHex);
    CombatHexDirection GetBestDirection(i32 sourceHex, i32 targetHex, i32 blockedMask);
    i32 IsAlive(void) {
        return m_monsterType >= CREATURE_PEASANT && m_quantity > 0;
    }
};
#pragma pack(pop)

#define CLEAR_ARMY_TARGET(a) ((a).m_targetSide = COMBAT_SIDE_NONE, (a).m_targetIndex = -1)

#define ARMY_HAS_BERSERK_OR_HYPNOTIZE(a)                                                           \
    ((a).m_spellInfluence[(ARMY_SPELL_INFLUENCE_BERSERK)]                                       \
     || (a).m_spellInfluence[(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
#define ARMY_HAS_INCAPACITATING_SPELL(a)                                                           \
    ((a).m_spellInfluence[(ARMY_SPELL_INFLUENCE_BLIND)]                                         \
     || (a).m_spellInfluence[(ARMY_SPELL_INFLUENCE_PARALYZE)]                                   \
     || (a).m_spellInfluence[(ARMY_SPELL_INFLUENCE_PETRIFIED)])
extern b32 bSecondAttack;
extern b32 gbGenieHalf;

extern SMonFrameInfo sViewArmyMonFrameInfo;

void BuildTempWalkSeq(struct SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing);
void ModifyFrameInfo(struct SMonFrameInfo* frameInfo, CreatureType monsterType);

#endif
