#include <Ints.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/state.h>
#include <IRONFIST/hooks.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Utf8.h>
#include <BASE/soundManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>


#define ARMY_HASTE_WALK_DURATION_SCALE 0.65
#define ARMY_SLOW_WALK_DURATION_SCALE 1.5
#define ARMY_PROJECTILE_PI \
    3.14159
#define ARMY_SHOOT_UP_MIN_ANGLE 25.0f
#define ARMY_SHOOT_FORWARD_MIN_ANGLE -25.0f
#define ARMY_SPELL_CHANCE_NONE 0.0f
#define ARMY_SPELL_CHANCE_DWARF 0.75f
#define ARMY_SPELL_CHANCE_ALWAYS 1.0f
#define ARMY_SPELL_CHANCE_PERCENT 100.0f
#define ARMY_RANGER_ATTACK_DURATION_SCALE 0.78
#define ARMY_DURATION_BASE_SCALE 1.0
#define ARMY_ATTACK_DURATION_SPEED_SCALE 0.08
#define ARMY_WALK_DURATION_SPEED_SCALE                                             \
    0.12

// Ironfist movement state threading through the attack path: whether the
// mover ended adjacent to where it started (teleporters), whether the move
// is an attack, and the charger's flight state.
bool gCloseMove;
bool gMoveAttack;
bool gChargePathDamage;
bool gCharging;

namespace {

typedef enum ArmySpellStatConstant {
    HASTE_SPEED_BONUS       = 2,
    BLOODLUST_ATTACK_BONUS  = 3,
    STONESKIN_DEFENSE_BONUS = 3,
    STEELSKIN_DEFENSE_BONUS = 5
} ArmySpellStatConstant;

typedef enum ArmyDeathConstant {
    CORPSE_LIMIT           = 14,
    DEATH_RANDOM_MAX       = 100,
    DEATH_PRIMARY_CHANCE   = 60,
    DEATH_SECONDARY_CHANCE = 80
} ArmyDeathConstant;

typedef enum SpellEffectConstant {
    NO_POW_EFFECT_COORDINATE = -1,
    EFFECT_MINIMUM_Y         = 999
} SpellEffectConstant;

typedef enum ArmySpellChanceConstant {
    RANDOM_SPELL_ROLL_MAX           = 99,
    RESURRECT_POWER_PER_SPELL_POWER = 50,
    HYPNOTIZE_HIT_POINTS_PER_POWER  = 25,
    ARTIFACT_POWER_MULTIPLIER       = 2,
    CURE_HIT_POINTS_PER_POWER       = 5,
    WIDE_CREATURE_HALF_WIDTH        = 22,
    CONTROL_EFFECT_Y_OFFSET         = 5
} ArmySpellChanceConstant;

typedef enum InitializationConstant {
    INITIAL_UNKNOWN_D4 = 6
} InitializationConstant;

typedef enum DrawingConstant {
    SPELL_EFFECT_COLOR                  = 237,
    SELECTED_CREATURE_COLOR             = 236,
    WIDE_NEIGHBOR_HEX_OFFSET            = 2,
    WIDE_RIGHT_QUANTITY_X_OFFSET        = 53,
    WIDE_LEFT_QUANTITY_X_OFFSET         = 73,
    RIGHT_QUANTITY_X_OFFSET             = 9,
    LEFT_QUANTITY_X_OFFSET              = 29,
    RIGHT_QUANTITY_Y_OFFSET             = 11,
    LEFT_QUANTITY_Y_OFFSET              = 23,
    QUANTITY_STATUS_FRAME               = 10,
    SPELL_STATUS_FRAME                  = 11,
    EFFECT_STATUS_MIXED_FRAME           = 3,
    EFFECT_STATUS_BAD_FRAME             = 4,
    EFFECT_STATUS_DEFAULT_FRAME         = 2,
    EFFECT_STATUS_FRAME_OFFSET          = 10,
    QUANTITY_TEXT_Y_OFFSET              = 2,
    QUANTITY_TEXT_WIDTH                 = 20,
    QUANTITY_TEXT_HEIGHT                = 12,
    WINCE_SPELL_X_OFFSET                = 4,
    ELEMENTAL_STORM_FACING_SCALE        = 2,
    DRAWBRIDGE_WIDE_EXIT_HEX            = 60,
    PROJECTILE_TARGET_Y_OFFSET          = 17,
    POW_EFFECT_DWARF_OVERLAP_ADJUSTMENT = 2
} DrawingConstant;

enum class ProjectileAttackDirection : i32 {
    PROJECTILE_ATTACK_UP      = 0,
    PROJECTILE_ATTACK_FORWARD = 1,
    PROJECTILE_ATTACK_DOWN    = 2
};
using enum ProjectileAttackDirection;

typedef enum ProjectileConstant {
    HALF_TURN_DEGREES                = 180,
    VERTICAL_ANGLE                   = 90,
    DEFAULT_MISSILE_HALF_WIDTH       = 25,
    DEFAULT_MISSILE_HALF_HEIGHT      = 25,
    DEFAULT_MISSILE_SPACING          = 31,
    DEFAULT_MISSILE_DELAY            = 25,
    LICH_MISSILE_SPACING             = 26,
    LICH_MISSILE_DELAY               = 7,
    LICH_MISSILE_HALF_WIDTH          = 10,
    LICH_MISSILE_HALF_HEIGHT         = 10,
    MAGE_BOLT_START_WIDTH            = 5,
    MAGE_BOLT_END_WIDTH              = 4,
    MAGE_BOLT_ANGLE_DISTANCE_DIVISOR = 15,
    MAGE_BOLT_ANGLE_DISTANCE_BASE    = 15,
    MAGE_BOLT_FRAME_DELAY            = 10,
    MISSILE_DIAMETER_MULTIPLIER      = 2,
    LICH_SPLASH_DIRECTION_COUNT      = H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST) + 1,
    LICH_SPLASH_CENTER_DIRECTION     = H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST)
} ProjectileConstant;

typedef enum DamageConstant {
    GENIE_QUANTITY_DIVISOR = 2,
    DAMAGE_DISPLAY_DIVISOR = 5,
    SLOW_SPEED_DIVISOR     = 2
} DamageConstant;

enum class BerserkMaskIndex : i32 {
    BERSERK_MASK_SIDE_ZERO    = 0,
    BERSERK_MASK_SIDE_ONE     = 1,
    BERSERK_MASK_ATTACK       = 2,
    BERSERK_MASK_UNUSED       = 3,
    BERSERK_MASK_TARGET_FOUND = 4,
    BERSERK_MASK_COUNT        = 5
};
using enum BerserkMaskIndex;

#define PROJECTILE_HALF_TURN_DEGREES_FLOAT 180.0
#define PROJECTILE_DIRECTION_MIDPOINT_DIVISOR 2.0f
#define DAMAGE_DOUBLE_MULTIPLIER 2.0f
#define DAMAGE_HALF_DIVISOR 2.0f
#define DAMAGE_ROUNDING_OFFSET 0.5
#define DAMAGE_SHADOW_MARK_MULTIPLIER 1.5f
#define DAMAGE_CHARGE_PATH_MULTIPLIER 0.5f
#define DAMAGE_CHARGE_MULTIPLIER 1.25f
#define DAMAGE_TELEPORT_MULTIPLIER 1.25f

// Sprite frames for the Cyber creature ability animations.
enum ExtendedArmyFrame {
    ARMY_FORCE_SHIELD_ICON_X_OFFSET = 2,
    ARMY_FORCE_SHIELD_ICON_Y_OFFSET = -28,
    ARMY_FORCE_SHIELD_TEXT_Y_OFFSET = -24,
    ARMY_FORCE_SHIELD_ICON_FRAME    = 10,
    ARMY_DODGE_ANIMATION_LENGTH  = 7,
    ARMY_DODGE_FIRST_FRAME       = 34,
    ARMY_DODGE_WINCE_FRAME       = 50,
    ARMY_CHARGE_FRAME_FORWARD    = 17,
    ARMY_CHARGE_FRAME_UP         = 96,
    ARMY_CHARGE_FRAME_DOWN       = 97,
    ARMY_CHARGE_WALK_FIRST_FRAME = 46,
    ARMY_CHARGE_STAND_FRAME      = 69,
    ARMY_WALK_FRAME_COUNT        = 8
};

// The Shadow Assassin's wince plays the astral slip once per dodge, then
// the plain wince returns.
static void ApplyAstralDodgeWince(army* target) {
    if (target->m_monsterType != CREATURE_CYBER_SHADOW_ASSASSIN) {
        return;
    }
    if (ironfist::state::Get().combat.stack.abilityNowAnimating[target][ironfist::CreatureAttribute::AstralDodge]) {
        target->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)] =
            ARMY_DODGE_ANIMATION_LENGTH;
        for (i32 frame = 0; frame < ARMY_DODGE_ANIMATION_LENGTH; frame++) {
            target->m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WINCE)][frame] =
                static_cast<i8>(ARMY_DODGE_FIRST_FRAME + frame);
        }
        ironfist::state::Get().combat.stack.abilityNowAnimating[target][ironfist::CreatureAttribute::AstralDodge] = false;
    } else {
        target->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)] = 1;
        target->m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WINCE)][0] =
            ARMY_DODGE_WINCE_FRAME;
    }
}


void FormatCombatDamage(
    char* output,
    const char* attackerName,
    bool attackerIsPlural,
    i32 damage,
    i32 killed,
    const char* victimName
) {
    if (killed > 0) {
        if (victimName != NULL) {
            sprintf(
                output,
                localization::TrPlural(
                    attackerIsPlural ? "combat.attack.plural.damage_with_kills"
                                     : "combat.attack.singular.damage_with_kills",
                    killed
                ),
                attackerName,
                damage,
                killed,
                victimName
            );
        } else {
            sprintf(
                output,
                localization::TrPlural(
                    attackerIsPlural ? "combat.attack.plural.damage_with_generic_kills"
                                     : "combat.attack.singular.damage_with_generic_kills",
                    killed
                ),
                attackerName,
                damage,
                killed
            );
        }
    } else {
        sprintf(
            output,
            localization::Tr(
                attackerIsPlural ? "combat.attack.plural.damage"
                                 : "combat.attack.singular.damage"
            ),
            attackerName,
            damage
        );
    }
}

}

army::army(void) {
    H2SteppedEnumStorage<ArmySampleType, i32> sampleType;

    m_creatureIcon = NULL;
    m_hex = 0;
    for (sampleType = ARMY_SAMPLE_MOVE; sampleType < ARMY_SAMPLE_COUNT; sampleType++) {
        m_samples[H2EnumIndex(sampleType)] = NULL;
    }
    m_drawEnabled = 1;
    m_targetSide = COMBAT_SIDE_NONE;
    m_targetIndex = -1;
    m_attackDirection = COMBAT_DIRECTION_INVALID;
    m_unknown5e = 0;
    m_moveTargetHex = 0;
    m_palette = NULL;
    m_showQuantity = 1;
    m_yOffset = 0;
    m_xOffset = 0;
}

void army::WaitSample(ArmySampleType sampleIndex) {
    return;
}

void army::InitClean(void) {
    H2SteppedEnumStorage<ArmySampleType, i32> sampleType;

    for (sampleType = ARMY_SAMPLE_MOVE; sampleType < ARMY_SAMPLE_COUNT; sampleType++) {
        m_samples[H2EnumIndex(sampleType)] = NULL;
    }
    m_roundCounter = -1;
    m_spellCount = 0;
    memset(m_spellInfluence, 0, sizeof(m_spellInfluence));
    m_lastAnimationTime = platform::Ticks();
    m_drawEnabled = 1;
    m_creatureIcon = NULL;
    m_drawSpellEffect = 0;
    m_spellEffect = SPELL_NONE;
    m_mirrorSourceIndex = -1;
    m_mirrorImageIndex = -1;
    m_armyGroupSlot = -1;
    m_lastTargetHex = -1;
    for (auto& attribute : ironfist::CreatureAttributes) {
        if (ironfist::HasCreatureAttribute(m_monsterType, attribute))
            ironfist::state::Get().combat.stack.abilityCounter[this][attribute] = 1;
    }
}

void army::Init(
    CreatureType monsterType, i32 quantity, CombatSide side, i32 index, i32 hex, i32 unknown
) {
    hero* commander;
    i32 rearHex;

    InitClean();
    m_monsterType = monsterType;
    m_drawState = ARMY_DRAW_NORMAL;
    m_monster = gMonsterDatabase[H2EnumIndex(monsterType)];
    m_unknownD4 = INITIAL_UNKNOWN_D4;
    commander = gpCombatManager->m_heroes[H2EnumIndex(side)];
    if (commander) {
        m_monster.attack += commander->Stats(HERO_PRIMARY_ATTACK);
        m_monster.defense += commander->Stats(HERO_PRIMARY_DEFENSE);
    }
    m_facing = side == COMBAT_ATTACKER_SIDE ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    m_luckOutcome = 0;
    m_targetSide = COMBAT_SIDE_NONE;
    m_targetIndex = -1;
    m_attackDirection = COMBAT_DIRECTION_INVALID;
    m_speed = m_monster.speed;
    m_quantity = quantity;
    m_initialQuantity = quantity;
    m_temporaryResurrectionQuantity = 0;
    m_animationState = 0;
    m_hitPointsLost = 0;
    m_damagePenalty = ARMY_DAMAGE_PENALTY_NONE;
    m_killPending = 0;
    m_deathPending = 0;
    m_damagePending = 0;
    m_side = side;
    m_index = index;
    m_morale = gpCombatManager->m_armyGroups[H2EnumIndex(m_side)]->GetMorale(
        gpCombatManager->m_heroes[H2EnumIndex(m_side)],
        gpCombatManager->m_combatTowns[H2EnumIndex(m_side)],
        gpCombatManager->m_armyGroups[H2EnumIndex(OppositeCombatSide(m_side))]
    );
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
        || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
        || (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_NO_MORALE)))) {
        m_morale = 0;
    }
    m_luck = gpGame->GetLuck(
        gpCombatManager->m_heroes[H2EnumIndex(m_side)],
        this,
        gpCombatManager->m_combatTowns[H2EnumIndex(m_side)]
    );
    m_hex = hex;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = m_index;
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        rearHex = m_hex + (m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
        gpCombatManager->m_hexCells[rearHex].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[rearHex].m_occupantIndex = m_index;
        gpCombatManager->m_hexCells[rearHex].m_occupantFrame =
            rearHex >= m_hex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
        gpCombatManager->m_hexCells[m_hex].m_occupantFrame =
            rearHex < m_hex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
    }
    m_armyGroupSlot = unknown;
    for (auto& attribute : ironfist::CreatureAttributes) {
        if (ironfist::HasCreatureAttribute(m_monsterType, attribute))
            ironfist::state::Get().combat.stack.abilityCounter[this][attribute] = 1;
    }
}

void army::LoadResources(void) {
    i32 i;

    if (gbNoShowCombat) {
        return;
    }

    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[H2EnumIndex(m_monsterType)], 1)
    );
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(&m_frameInfo), sizeof(m_frameInfo));
    ModifyFrameInfo(&m_frameInfo, m_monsterType);
    m_walkDuration = m_frameInfo.walkDuration;

    sprintf(gText, "%smove.82M", m_monster.spriteName);
    m_samples[H2EnumIndex(ARMY_SAMPLE_MOVE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%sattk.82M", m_monster.spriteName);
    m_samples[H2EnumIndex(ARMY_SAMPLE_ATTACK)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%swnce.82M", m_monster.spriteName);
    m_samples[H2EnumIndex(ARMY_SAMPLE_WINCE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%skill.82M", m_monster.spriteName);
    m_samples[H2EnumIndex(ARMY_SAMPLE_KILL)] = gpResourceManager->GetSample(gText);

    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER)))) {
        sprintf(gText, "%sshot.82M", m_monster.spriteName);
        m_samples[H2EnumIndex(ARMY_SAMPLE_SHOT)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_VAMPIRE || m_monsterType == CREATURE_VAMPIRE_LORD) {
        sprintf(gText, "%sext1.82M", m_monster.spriteName);
        m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
        sprintf(gText, "%sext2.82M", m_monster.spriteName);
        m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_TWO)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        sprintf(gText, "%sexpl.82M", m_monster.spriteName);
        m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
    }

    m_creatureIcon = gpResourceManager->GetIcon(cMonFilename[H2EnumIndex(m_monsterType)]);
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER)))) {
        if (m_monsterType == CREATURE_GIANT || m_monsterType == CREATURE_TITAN) {
            sprintf(gText, "titanmsl.icn");
        } else if (m_monsterType == CREATURE_HALFLING) {
            sprintf(gText, "halflmsl.icn");
        } else if (m_monsterType == CREATURE_ARCHER || m_monsterType == CREATURE_RANGER) {
            sprintf(gText, "arch_msl.icn");
        } else if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
            sprintf(gText, "lich_msl.icn");
        } else if (m_monsterType == CREATURE_ORC || m_monsterType == CREATURE_ORC_CHIEF) {
            sprintf(gText, "orc__msl.icn");
        } else if (m_monsterType == CREATURE_DRUID || m_monsterType == CREATURE_GREATER_DRUID) {
            sprintf(gText, "druidmsl.icn");
        } else if (m_monsterType == CREATURE_TROLL || m_monsterType == CREATURE_WAR_TROLL) {
            sprintf(gText, "trollmsl.icn");
        } else {
            sprintf(gText, "elf__msl.icn");
        }
        m_missileIcon = gpResourceManager->GetIcon(gText);
    } else {
        m_samples[H2EnumIndex(ARMY_SAMPLE_SHOT)] = NULL;
        m_missileIcon = NULL;
    }

    for (i = 0; i < ARMY_PRIMARY_SAMPLE_COUNT; i++) {
        if (m_samples[i]) {
            m_samples[i]->m_playbackData.volume = ARMY_SAMPLE_VOLUME;
            m_samples[i]->m_playbackData.channelType = ARMY_SAMPLE_CHANNEL;
            m_samples[i]->m_playbackData.loopCount = 0;
        }
    }
}

void army::FreeResources(void) {
    H2SteppedEnumStorage<ArmySampleType, i32> sampleType;

    if (gbNoShowCombat) {
        return;
    }
    if (m_missileIcon) {
        gpResourceManager->Dispose(m_missileIcon);
        m_missileIcon = NULL;
    }
    if (m_creatureIcon) {
        gpResourceManager->Dispose(m_creatureIcon);
        m_creatureIcon = NULL;
    }
    for (sampleType = ARMY_SAMPLE_MOVE; sampleType < ARMY_SAMPLE_COUNT; sampleType++) {
        if (m_samples[H2EnumIndex(sampleType)]) {
            gpResourceManager->Dispose(m_samples[H2EnumIndex(sampleType)]);
            m_samples[H2EnumIndex(sampleType)] = NULL;
        }
    }
}

void army::DrawToBuffer(i32 x, i32 y, i32 effectsOnly) {
    u8* palette;
    i32 idle;
    i32 yoff;
    i32 xoff;
    i32 flags;
    i32 numFrames;
    i32 quantX;
    i32 spellX;
    i32 quantY;
    H2SteppedEnumStorage<ArmySpellInfluence, i32> sp;
    i32 spellY;
    i32 color;
    i32 badSpells;
    i32 qtyOffset;
    IconDrawResult drawn;
    i32 otherHex;
    i32 occupied;
    i32 statusIcon;
    i32 goodEffects;
    char countText[ARMY_QUANTITY_TEXT_SIZE];

    if (gpCombatManager->m_nonVisualCombat) {
        return;
    }
    if (gbNoShowCombat) {
        return;
    }

    flags = 0;
    color = 0;
    if (m_animationSequence == ARMY_ANIMATION_STAND
        || (m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
            && m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST)) {
        idle = 1;
    } else {
        idle = 0;
    }
    y += m_yOffset;
    x += m_xOffset;
    if (m_animationSequence == ARMY_ANIMATION_WALK
        && !(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))) {
        numFrames = m_frameInfo.animationFrameCount[H2EnumIndex(m_animationSequence)];
        yoff = m_animationFrame * COMBAT_HEX_VERTICAL_STEP / numFrames;
        xoff = m_animationFrame * COMBAT_HEX_ROW_STAGGER / numFrames;
        if (m_walkDirection == COMBAT_DIRECTION_NORTHEAST
            || m_walkDirection == COMBAT_DIRECTION_NORTHWEST) {
            y -= yoff;
            giWalkingYMod = -yoff;
        }
        if (m_walkDirection == COMBAT_DIRECTION_SOUTHEAST
            || m_walkDirection == COMBAT_DIRECTION_SOUTHWEST) {
            y += yoff;
            giWalkingYMod = yoff;
        }
        if (m_walkDirection == COMBAT_DIRECTION_NORTHEAST
            || m_walkDirection == COMBAT_DIRECTION_SOUTHEAST) {
            x -= xoff;
        }
        if (m_walkDirection == COMBAT_DIRECTION_NORTHWEST
            || m_walkDirection == COMBAT_DIRECTION_SOUTHWEST) {
            x += xoff;
        }
    }
    if (giSpellEffectShowType == SPELL_EFFECT_DISPLAY_HIGHLIGHT && idle && m_spellCount > 0) {
        color = SPELL_EFFECT_COLOR;
    }
    if (m_hex == gpCombatManager->m_limitCreatureHex && gpCombatManager->m_limitCreature == 1) {
        color = SELECTED_CREATURE_COLOR;
    }

    palette = NULL;
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        palette = gColorTableGray;
    } else if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_RED_PALETTE)))) {
        palette = gColorTableRed;
    } else if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DARK_BROWN_PALETTE)))) {
        palette = gColorTableDarkBrown;
    } else if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_GRAY_PALETTE)))) {
        palette = gColorTableGray;
    } else if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_LIGHT_PALETTE)))) {
        palette = gColorTableLighten;
    }
    if (effectsOnly == 0) {
        m_creatureIcon->CombatClipDrawToBuffer(
            x,
            y,
            m_frameInfo.animationFrames[H2EnumIndex(m_animationSequence)][m_animationFrame],
            &m_spriteLimits,
            m_facing == ARMY_FACING_LEFT ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
            color,
            palette,
            m_palette
        );
    }

    if (idle && gpCombatManager->m_showArmyQuantities && m_showQuantity) {
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_RIGHT) {
                quantX    = x + WIDE_RIGHT_QUANTITY_X_OFFSET;
                otherHex  = m_hex + WIDE_NEIGHBOR_HEX_OFFSET;
            } else {
                quantX    = x - WIDE_LEFT_QUANTITY_X_OFFSET;
                otherHex  = m_hex - WIDE_NEIGHBOR_HEX_OFFSET;
            }
        } else if (m_facing == ARMY_FACING_RIGHT) {
            quantX   = x + RIGHT_QUANTITY_X_OFFSET;
            otherHex = m_hex + 1;
        } else {
            quantX   = x - LEFT_QUANTITY_X_OFFSET;
            otherHex = m_hex - 1;
        }
        occupied  = gpCombatManager->m_hexCells[otherHex].m_occupantSide != COMBAT_SIDE_NONE;
        qtyOffset = m_frameInfo.quantityX[H2EnumIndex(OppositeArmyFacing(m_facing))];
        if (occupied && qtyOffset > 0) {
            qtyOffset = 0;
        }
        if (m_facing == ARMY_FACING_RIGHT) {
            quantX += qtyOffset;
        } else {
            quantX -= qtyOffset;
        }
        if (m_facing == ARMY_FACING_RIGHT) {
            quantY = y - RIGHT_QUANTITY_Y_OFFSET;
        } else {
            quantY = y - LEFT_QUANTITY_Y_OFFSET;
        }
        goodEffects = 0;
        badSpells   = 0;
        for (sp = ARMY_SPELL_INFLUENCE_HASTE; sp < ARMY_SPELL_INFLUENCE_COUNT; sp++) {
            if (m_spellInfluence[H2EnumIndex(sp)]) {
                switch (sp) {
                    case ARMY_SPELL_INFLUENCE_HASTE:
                    case ARMY_SPELL_INFLUENCE_BLESS:
                    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
                    case ARMY_SPELL_INFLUENCE_BLOODLUST:
                    case ARMY_SPELL_INFLUENCE_SHIELD:
                    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
                    case ARMY_SPELL_INFLUENCE_STONESKIN:
                    case ARMY_SPELL_INFLUENCE_STEELSKIN:
                    case ARMY_SPELL_INFLUENCE_FORCE_SHIELD:
                        goodEffects++;
                        break;
                    default:
                        badSpells++;
                }
            }
        }
        if (giSpellEffectShowType == SPELL_EFFECT_DISPLAY_HIGHLIGHT || !idle
            || m_spellCount <= 0) {
            drawn = gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                quantX,
                quantY,
                QUANTITY_STATUS_FRAME,
                &m_creatureLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        } else if (giSpellEffectShowType == SPELL_EFFECT_DISPLAY_SPELL_ICON) {
            drawn = gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                quantX,
                quantY,
                SPELL_STATUS_FRAME,
                &m_creatureLimits,
                ICON_DRAW_NORMAL,
                SPELL_EFFECT_COLOR,
                NULL,
                NULL
            );
        } else {
            statusIcon = EFFECT_STATUS_DEFAULT_FRAME;
            if (goodEffects > 0 && badSpells > 0) {
                statusIcon += 1;
            } else if (badSpells > 0) {
                statusIcon += 2;
            }
            drawn = gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
                quantX,
                quantY,
                statusIcon + EFFECT_STATUS_FRAME_OFFSET,
                &m_creatureLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }
        // A force-shielded stack shows the shield's remaining strength above
        // the normal quantity box.
        if (ironfist::state::Get().combat.stack.forceShieldHP[this] > 0) {
            gpResourceManager->GetIcon("SPELLINF.ICN")
                ->CombatClipDrawToBuffer(
                    quantX + ARMY_FORCE_SHIELD_ICON_X_OFFSET,
                    quantY + ARMY_FORCE_SHIELD_ICON_Y_OFFSET,
                    ARMY_FORCE_SHIELD_ICON_FRAME,
                    &m_creatureLimits,
                    ICON_DRAW_NORMAL,
                    0,
                    gColorTableRed,
                    NULL
                );
            sprintf(countText, "%d", ironfist::state::Get().combat.stack.forceShieldHP[this]);
            smallFont->DrawBoundedString(
                countText,
                quantX,
                quantY + ARMY_FORCE_SHIELD_TEXT_Y_OFFSET + QUANTITY_TEXT_Y_OFFSET,
                QUANTITY_TEXT_WIDTH,
                QUANTITY_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_CENTER
            );
        }
        if (drawn != ICON_DRAW_SKIPPED) {
            sprintf(countText, "%d", m_lastTargetHex != -1 ? m_lastTargetHex : m_quantity);
            smallFont->DrawBoundedString(
                countText,
                quantX,
                quantY + QUANTITY_TEXT_Y_OFFSET,
                QUANTITY_TEXT_WIDTH,
                QUANTITY_TEXT_HEIGHT,
                FONT_DRAW_DEFAULT,
                FONT_ALIGN_CENTER
            );
        }
    }

    if (m_drawSpellEffect && effectsOnly == 0) {
        spellX = x;
        spellY = GetPowBaseY();
        if (m_animationSequence == ARMY_ANIMATION_WINCE
            || m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
            if (m_facing == ARMY_FACING_RIGHT) {
                spellX -= WINCE_SPELL_X_OFFSET;
            } else {
                spellX += WINCE_SPELL_X_OFFSET;
            }
        }
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_RIGHT) {
                spellX += COMBAT_HEX_ROW_STAGGER;
            } else {
                spellX -= COMBAT_HEX_ROW_STAGGER;
            }
        }
        if (gCurLoadedSpellEffect == COMBAT_EFFECT_SHIELD) {
            spellX = m_facing == ARMY_FACING_RIGHT ? RightX() : LeftX();
        }
        if (gCurLoadedSpellEffect == COMBAT_EFFECT_BLIND) {
            spellX = x + m_frameInfo.spellEffectX * ArmyFacingRearHexOffset(m_facing);
            spellY = y + m_frameInfo.spellEffectY;
        }
        gCurLoadedSpellIcon->CombatClipDrawToBuffer(
            spellX,
            spellY + m_spellEffectYOffset,
            gCurSpellEffectFrame,
            &m_spellLimits,
            static_cast<IconDrawOrientation>(H2EnumIndex(ICON_DRAW_FLIPPED) - H2EnumIndex(m_facing)),
            0,
            NULL,
            NULL
        );
    }
}

void army::Wince(void) {
    m_animationSequence = ARMY_ANIMATION_WINCE;
    m_animationFrame = 0;
}

void army::Walk(CombatHexDirection direction, i32 finishStanding, i32 skipDrawing) {
    i32 frame;
    i32 newHex;
    i32 saveHex;
    i32 destHex;
    i32 tempTop;
    i32 tempLeft;
    i32 tempRight;
    i32 otherHex;
    i32 tempBottom;

    destHex = GetAdjacentCellIndex(m_hex, direction);
    gCloseMove = IsCloseMove(destHex);
    if (m_side == COMBAT_DEFENDER_SIDE && gpCombatManager->m_inCastleCombat
        && (destHex == COMBAT_CASTLE_GATE_APPROACH_HEX
            || destHex == H2EnumIndex(COMBAT_CASTLE_HEX_GATE)
            || (destHex == DRAWBRIDGE_WIDE_EXIT_HEX && m_side == COMBAT_DEFENDER_SIDE
                && (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))))
        && gpCombatManager->m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->LowerDoor();
        skipDrawing = 0;
    }

    giWalkingFrom = m_hex;
    giWalkingFrom2 = (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                         ? m_hex + ArmyFacingRearHexOffset(m_facing)
                         : -1;
    giWalkingTo = destHex;
    giWalkingTo2 = (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                       ? destHex + ArmyFacingRearHexOffset(m_facing)
                       : -1;
    giWalkingYMod = 0;
    BuildTempWalkSeq(&m_frameInfo, finishStanding, skipDrawing);
    m_walkDirection = direction;

    if (skipDrawing == 0) {
        giMinExtentY = ARMY_COMBAT_WIDTH;
        giMinExtentX = giMinExtentY;
        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        gbReturnAfterComputeExtent = true;
        DrawToBuffer(
            gpCombatManager->m_hexCells[m_hex].m_x,
            gpCombatManager->m_hexCells[m_hex].m_y,
            0
        );
        gbReturnAfterComputeExtent = false;
        gbSaveBiggestExtent = false;
        gbComputeExtent = false;
    }
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentX > ARMY_COMBAT_MAX_X)
        giMaxExtentX = ARMY_COMBAT_MAX_X;
    if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
        giMaxExtentY = ARMY_COMBAT_MAX_Y;
    tempLeft = giMinExtentX;
    tempTop = giMinExtentY;
    tempRight = giMaxExtentX;
    tempBottom = giMaxExtentY;

    m_facingChanged = 0;
    if (direction < COMBAT_DIRECTION_SOUTHWEST) {
        if (m_facing == ARMY_FACING_LEFT) {
            m_facingChanged = 1;
            m_facing = OppositeArmyFacing(m_facing);
            if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                m_hex--;
            }
        }
    } else if (m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = 1;
        m_facing = OppositeArmyFacing(m_facing);
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            m_hex++;
        }
    }
    if (direction == COMBAT_DIRECTION_NORTHEAST
        || direction == COMBAT_DIRECTION_NORTHWEST) {
        m_drawState = ARMY_DRAW_BEHIND;
    }
    if (direction == COMBAT_DIRECTION_SOUTHEAST
        || direction == COMBAT_DIRECTION_SOUTHWEST) {
        m_drawState = ARMY_DRAW_IN_FRONT;
    }
    saveHex = m_hex;
    m_animationFrame = 0;
    m_animationSequence = ARMY_ANIMATION_WALK;
    if (!gbNoShowCombat) {
        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_MOVE)]);
    }

    if (skipDrawing == 0) {
        gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->m_hexCells[m_hex].m_occupantSide = gpCombatManager->m_currentArmySide;
        if (!gbNoShowCombat) {
            gpWindowManager->m_screen->CopyTo(
                gpCombatManager->m_backgroundBuffer,
                0,
                0,
                0,
                0,
                ARMY_COMBAT_WIDTH,
                ARMY_COMBAT_HEIGHT
            );
        }
        gpCombatManager->m_backgroundDrawn = 0;
    }

    if (!gbNoShowCombat) {
        for (frame = 0; frame < m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)];
             frame++) {
            m_animationFrame = frame;
            if (skipDrawing || frame != 0) {
                gpCombatManager->m_backgroundBuffer->CopyTo(
                    gpWindowManager->m_screen,
                    giMinExtentX,
                    giMinExtentY,
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1
                );
                if (giMinExtentX < 0)
                    giMinExtentX = 0;
                if (giMinExtentY < 0)
                    giMinExtentY = 0;
                if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                    giMaxExtentX = ARMY_COMBAT_MAX_X;
                if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                    giMaxExtentY = ARMY_COMBAT_MAX_Y;
                tempLeft = giMinExtentX;
                tempTop = giMinExtentY;
                tempRight = giMaxExtentX;
                tempBottom = giMaxExtentY;
            }
            giMinExtentY = ARMY_COMBAT_WIDTH;
            giMinExtentX = giMinExtentY;
            giMaxExtentY = 0;
            giMaxExtentX = giMaxExtentY;
            gbComputeExtent = true;
            gbSaveBiggestExtent = true;
            gbReturnAfterComputeExtent = true;
            DrawToBuffer(
                gpCombatManager->m_hexCells[m_hex].m_x,
                gpCombatManager->m_hexCells[m_hex].m_y,
                0
            );
            gbReturnAfterComputeExtent = false;
            gbComputeExtent = false;
            gbSaveBiggestExtent = false;
            if (giMinExtentX < 0)
                giMinExtentX = 0;
            if (giMinExtentY < 0)
                giMinExtentY = 0;
            if (giMaxExtentX > ARMY_COMBAT_MAX_X)
                giMaxExtentX = ARMY_COMBAT_MAX_X;
            if (giMaxExtentY > ARMY_COMBAT_MAX_Y)
                giMaxExtentY = ARMY_COMBAT_MAX_Y;
            gbCurrArmyDrawn = false;
            gbComputeExtent = true;
            gbLimitToExtent = true;
            m_drawEnabled = 0;
            gpCombatManager->DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 0, 1);
            m_drawEnabled = 1;
            gbLimitToExtent = false;
            gbComputeExtent = false;
            gbCurrArmyDrawn = true;
            if (giMinExtentX < tempLeft) {
                tempLeft = giMinExtentX;
            }
            if (giMinExtentY < tempTop) {
                tempTop = giMinExtentY;
            }
            if (giMaxExtentX > tempRight) {
                tempRight = giMaxExtentX;
            }
            if (giMaxExtentY > tempBottom) {
                tempBottom = giMaxExtentY;
            }
            DelayTil(glTimers);
            glTimers[0] = static_cast<i32>(
                platform::Ticks()
                + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                      / m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]
            );
            gpWindowManager->UpdateScreenRegion(
                tempLeft,
                tempTop,
                tempRight - tempLeft + 1,
                tempBottom - tempTop + 1
            );
        }
    }

    newHex = GetAdjacentCellIndex(m_hex, direction);
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = -1;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = COMBAT_SIDE_NONE;
    gpCombatManager->m_hexCells[m_hex].m_occupantFrame = ARMY_FACING_NONE;
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        otherHex = m_hex + ArmyFacingRearHexOffset(m_facing);
        gpCombatManager->m_hexCells[otherHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[otherHex].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->m_hexCells[otherHex].m_occupantFrame = ARMY_FACING_NONE;
    }
    gpCombatManager->m_hexCells[newHex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[newHex].m_occupantIndex = m_index;
    gpCombatManager->m_hexCells[newHex].m_occupantFrame = ARMY_FACING_NONE;
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        otherHex = newHex + ArmyFacingRearHexOffset(m_facing);
        gpCombatManager->m_hexCells[otherHex].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[otherHex].m_occupantIndex = m_index;
        gpCombatManager->m_hexCells[otherHex].m_occupantFrame =
            otherHex >= newHex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
        gpCombatManager->m_hexCells[newHex].m_occupantFrame =
            newHex >= otherHex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
    }
    m_hex = newHex;
    if (m_facingChanged) {
        m_facing = OppositeArmyFacing(m_facing);
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_LEFT) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facingChanged = 0;
    }
    giWalkingFrom = -1;
    giWalkingFrom2 = -1;
    giWalkingTo = -1;
    giWalkingTo2 = -1;
    m_drawState = ARMY_DRAW_NORMAL;
    if (finishStanding == 1) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    gpCombatManager->CheckBurnCreature(this);
}

void army::SpecialAttack(void) {
    i32 xCentre;
    i32 frameIndex;
    i32 oldTipX;
    i32 anchorY;
    char originalColumn;
    H2EnumStorage<IconDrawOrientation, char> bIconFlip;
    i32 moveCount;
    i32 oldTipY;
    i32 castX;
    char combatMsg[ARMY_COMBAT_TEXT_SIZE];
    CombatEffectType powVal;
    i32 castY;
    char hisCol;
    char originalRow;
    i32 yStretch;
    i32 xStretch;
    i32 gainX;
    i32 inFlightX;
    i32 gainY;
    i32 inFlightY;
    i32 yCentre;
    i32 animSlot;
    i32 landX;
    i32 bgPosX;
    char hisRow;
    i32 spacing;
    i32 landY;
    i32 shotDelay;
    i32 clipTop;
    i32 k;
    i32 clipLeft;
    bitmap* pSaveBitmap;
    i32 arrowHalfW;
    i32 bgPosY;
    ArmyFacing wasFacing;
    army* pEnemy;
    float incline;
    i32 pathDist;
    i32 fullXLen;
    i32 arrowHalfH;
    i32 startX;
    i32 maxX;
    i32 killed;
    i32 fullYLen;
    i32 startY;
    i32 maxY;
    i32 damageDone;
    i32 anchorX;
    float fShotAngle;
    i32 baseAtk;

    damageDone = 0;
    killed = 0;
    wasFacing = m_facing;
    m_palette = NULL;
    pEnemy = m_targetIndex + gpCombatManager->m_armies[H2EnumIndex(m_targetSide)];
    hisCol = pEnemy->m_hex % ARMY_HEX_COLUMNS;
    hisRow = pEnemy->m_hex / ARMY_HEX_COLUMNS;
    originalColumn = m_hex % ARMY_HEX_COLUMNS;
    originalRow = m_hex / ARMY_HEX_COLUMNS;
    wasFacing = m_facing;
    if (!(hisCol <= originalColumn)
        || (!(originalRow & 1) && hisCol == originalColumn)) {
        m_facing = ARMY_FACING_RIGHT;
    } else {
        m_facing = ARMY_FACING_LEFT;
    }
    if (m_facing != wasFacing) {
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_RIGHT) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    CheckLuck();
    gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_SHOT)]);
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);

    xCentre = pEnemy->MidX();
    yCentre = pEnemy->MidY();
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        xCentre = gpCombatManager->m_hexCells[pEnemy->m_hex].m_x;
        yCentre =
            gpCombatManager->m_hexCells[pEnemy->m_hex].m_y - PROJECTILE_TARGET_Y_OFFSET;
    }
    if (m_facing == ARMY_FACING_RIGHT) {
        anchorX = gpCombatManager->m_hexCells[m_hex].m_x + m_frameInfo.missileOffsets[1].x;
    } else {
        anchorX = gpCombatManager->m_hexCells[m_hex].m_x - m_frameInfo.missileOffsets[1].x;
    }
    anchorY = gpCombatManager->m_hexCells[m_hex].m_y + m_frameInfo.missileOffsets[1].y;
    fullXLen = xCentre - anchorX;
    bIconFlip = ICON_DRAW_NORMAL;
    if (fullXLen < 0) {
        bIconFlip = ICON_DRAW_FLIPPED;
        fullXLen = -fullXLen;
    }
    fullYLen = yCentre - anchorY;
    if (fullXLen == 0) {
        frameIndex =
            fullYLen > 0 ? m_frameInfo.projectileDirectionCount - 1 : 0;
        fShotAngle = static_cast<float>(
            fullYLen > 0 ? -VERTICAL_ANGLE : VERTICAL_ANGLE
        );
    } else {


        incline = static_cast<float>(-fullYLen)
                / (static_cast<float>(fullXLen));
        fShotAngle = static_cast<float>(
            atan(static_cast<double>(incline)) * PROJECTILE_HALF_TURN_DEGREES_FLOAT
            / ARMY_PROJECTILE_PI
        );
        for (k = 1; k < m_frameInfo.projectileDirectionCount; k++) {
            if ((m_frameInfo.projectileAngles[k]
                 + m_frameInfo.projectileAngles[k - 1])
                    / PROJECTILE_DIRECTION_MIDPOINT_DIVISOR
                < fShotAngle) {
                break;
            }
        }
        if (k < m_frameInfo.projectileDirectionCount) {
            frameIndex = k - 1;
        } else {
            frameIndex = m_frameInfo.projectileDirectionCount - 1;
        }
    }
    if (fShotAngle > ARMY_SHOOT_UP_MIN_ANGLE) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_UP;
        animSlot = H2EnumIndex(PROJECTILE_ATTACK_UP);
    } else if (fShotAngle > ARMY_SHOOT_FORWARD_MIN_ANGLE) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_FORWARD;
        animSlot = H2EnumIndex(PROJECTILE_ATTACK_FORWARD);
    } else {
        m_animationSequence = ARMY_ANIMATION_SHOOT_DOWN;
        animSlot = H2EnumIndex(PROJECTILE_ATTACK_DOWN);
    }
    for (m_animationFrame = 0;
         m_animationFrame < m_frameInfo.animationFrameCount[H2EnumIndex(m_animationSequence)];
         m_animationFrame++) {
        if (m_animationFrame == m_frameInfo.animationFrameCount[H2EnumIndex(m_animationSequence)] - 1) {
            gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
        glTimers[0] = static_cast<i32>(
            platform::Ticks()
            + m_frameInfo.attackDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                  / m_frameInfo.animationFrameCount[H2EnumIndex(m_animationSequence)]
        );
    }
    m_animationFrame = m_frameInfo.animationFrameCount[H2EnumIndex(m_animationSequence)] - 1;

    arrowHalfW = DEFAULT_MISSILE_HALF_WIDTH;
    arrowHalfH = DEFAULT_MISSILE_HALF_HEIGHT;
    spacing = DEFAULT_MISSILE_SPACING;
    shotDelay = DEFAULT_MISSILE_DELAY;
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        spacing = LICH_MISSILE_SPACING;
        shotDelay = LICH_MISSILE_DELAY;
        arrowHalfW = LICH_MISSILE_HALF_WIDTH;
        arrowHalfH = LICH_MISSILE_HALF_HEIGHT;
    }
    maxX = 0;
    clipLeft = ARMY_COMBAT_MAX_X;
    maxY = 0;
    clipTop = ARMY_PROJECTILE_CLIP_HEIGHT - 1;
    if (m_facing == ARMY_FACING_RIGHT) {
        startX = gpCombatManager->m_hexCells[m_hex].m_x
                     + m_frameInfo.missileOffsets[animSlot].x;
    } else {
        startX = gpCombatManager->m_hexCells[m_hex].m_x
                     - m_frameInfo.missileOffsets[animSlot].x;
    }
    startY =
        gpCombatManager->m_hexCells[m_hex].m_y + m_frameInfo.missileOffsets[animSlot].y;
    landX = pEnemy->MidX();
    landY = pEnemy->MidY();
    xStretch = landX - startX;
    yStretch = landY - startY;
    pathDist = static_cast<i32>(
        sqrt(static_cast<double>(xStretch * xStretch + yStretch * yStretch))
    );
    moveCount = (pathDist + (spacing >> 1)) / spacing;

    if (m_monsterType == CREATURE_MAGE || m_monsterType == CREATURE_ARCHMAGE) {
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DelayMilli(
            static_cast<i32l>(
                H2EnumIndex(ARMY_MAGE_BOLT_DELAY)
                * gfCombatSpeedMod[gConfig.combatSpeed]
            )
        );
        gpCombatManager->DoBolt(
            1,
            startX,
            startY,
            landX,
            landY,
            0,
            0,
            MAGE_BOLT_START_WIDTH,
            MAGE_BOLT_END_WIDTH,
            BOLT_COLOR_RED_BEAM,
            0,
            0,
            pathDist / MAGE_BOLT_ANGLE_DISTANCE_DIVISOR + MAGE_BOLT_ANGLE_DISTANCE_BASE,
            1,
            0,
            MAGE_BOLT_FRAME_DELAY,
            0
        );
    } else if (m_monsterType == CREATURE_CYBER_BEHEMOTH) {
        // The Cyber Behemoth lobs its shot on the catapult arc.
        gpCombatManager->ArcShot(m_missileIcon, startX, startY, landX, landY);
    } else {
        if (moveCount > 1) {
            gainX = xStretch / (moveCount - 1);
            gainY = yStretch / (moveCount - 1);
        } else {
            gainX = xStretch;
            gainY = yStretch;
        }
        inFlightX = startX;
        inFlightY = startY;
        pSaveBitmap = new bitmap(
            BITMAP_TYPE_MEMORY,
            static_cast<i16>(arrowHalfW * MISSILE_DIAMETER_MULTIPLIER),
            static_cast<i16>(arrowHalfH * MISSILE_DIAMETER_MULTIPLIER)
        );
        pSaveBitmap->GrabBitmapCareful(
            gpWindowManager->m_screen,
            static_cast<i16>(inFlightX - arrowHalfW),
            static_cast<i16>(inFlightY - arrowHalfH)
        );
        oldTipX = inFlightX;
        oldTipY = inFlightY;
        bgPosX = 0;
        bgPosY = 0;
        for (k = 0; k < moveCount; k++) {
            if (oldTipX - arrowHalfW < clipLeft) {
                clipLeft = oldTipX - arrowHalfW;
            }
            if (clipLeft < 0)
                clipLeft = 0;
            if (oldTipX + arrowHalfW > maxX) {
                maxX = oldTipX + arrowHalfW;
            }
            if (maxX > ARMY_COMBAT_MAX_X)
                maxX = ARMY_COMBAT_MAX_X;
            if (oldTipY - arrowHalfH < clipTop) {
                clipTop = oldTipY - arrowHalfH;
            }
            if (clipTop < 0)
                clipTop = 0;
            if (oldTipY + arrowHalfH > maxY) {
                maxY = oldTipY + arrowHalfH;
            }
            if (maxY > ARMY_COMBAT_MAX_Y)
                maxY = ARMY_COMBAT_MAX_Y;
            if (k != 0) {
                pSaveBitmap->DrawToBufferCareful(
                    static_cast<i16>(bgPosX),
                    static_cast<i16>(bgPosY)
                );
            } else {
                if (clipLeft < giMinExtentX)
                    giMinExtentX = clipLeft;
                if (maxX > giMaxExtentX)
                    giMaxExtentX = maxX;
                if (clipTop < giMinExtentY)
                    giMinExtentY = clipTop;
                if (maxY > giMaxExtentY)
                    giMaxExtentY = maxY;
            }
            bgPosX = inFlightX - arrowHalfW;
            if (bgPosX < 0)
                bgPosX = 0;
            if (bgPosX + pSaveBitmap->m_width > ARMY_COMBAT_WIDTH) {
                bgPosX = ARMY_COMBAT_WIDTH - pSaveBitmap->m_width;
            }
            bgPosY = inFlightY - arrowHalfH;
            if (bgPosY < 0)
                bgPosY = 0;
            if (bgPosY + pSaveBitmap->m_height > ARMY_COMBAT_WIDTH) {
                bgPosY = ARMY_COMBAT_WIDTH - pSaveBitmap->m_height;
            }
            pSaveBitmap->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<i16>(bgPosX),
                static_cast<i16>(bgPosY)
            );
            m_missileIcon->DrawToBuffer(
                inFlightX,
                inFlightY,
                frameIndex,
                bIconFlip
            );
            if (k == 0) {
                gpWindowManager->UpdateScreenRegion(
                    giMinExtentX,
                    giMinExtentY,
                    giMaxExtentX - giMinExtentX + 1,
                    giMaxExtentY - giMinExtentY + 1
                );
            } else {
                DelayTil(glTimers);
                gpWindowManager
                    ->UpdateScreenRegion(clipLeft, clipTop, maxX - clipLeft + 1, maxY - clipTop + 1);
            }
            glTimers[0] = static_cast<i32>(
                platform::Ticks() + shotDelay * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            oldTipX = inFlightX;
            oldTipY = inFlightY;
            inFlightX += gainX;
            inFlightY += gainY;
            clipLeft = inFlightX - arrowHalfW;
            maxX = inFlightX + arrowHalfW;
            clipTop = inFlightY - arrowHalfH;
            maxY = inFlightY + arrowHalfH;
        }
        pSaveBitmap->DrawToBuffer(
            static_cast<i16>(bgPosX),
            static_cast<i16>(bgPosY)
        );
        gpWindowManager->UpdateScreenRegion(
            oldTipX - arrowHalfW,
            oldTipY - arrowHalfH,
            arrowHalfW * MISSILE_DIAMETER_MULTIPLIER,
            arrowHalfH * MISSILE_DIAMETER_MULTIPLIER
        );
        delete pSaveBitmap;
    }
    if (!gpCombatManager->m_heroes[H2EnumIndex(m_side)]
        || !gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_AMMO_CART)) {
        m_monster.shots--;
    }

    baseAtk = m_monster.attack;
    powVal = COMBAT_EFFECT_INVALID;
    castX = -1;
    castY = -1;
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        i32 adjacentHex;
        army* splashTarget;

        gpCombatManager->ClearEffects();
        for (k = 0; k < LICH_SPLASH_DIRECTION_COUNT; k++) {
            if (k < COMBAT_DIRECTION_ADJACENT_COUNT) {
                adjacentHex = pEnemy->GetAdjacentCellIndex(
                    pEnemy->m_hex,
                    static_cast<CombatHexDirection>(k)
                );
            } else {
                adjacentHex = pEnemy->m_hex;
            }
            if (adjacentHex == -1) {
                continue;
            }
            if (gpCombatManager->m_hexCells[adjacentHex].m_occupantSide != COMBAT_SIDE_NONE) {
                splashTarget =
                    &gpCombatManager
                         ->m_armies[H2EnumIndex(gpCombatManager->m_hexCells[adjacentHex].m_occupantSide)]
                                   [gpCombatManager->m_hexCells[adjacentHex].m_occupantIndex];
                if (!gArmyEffected[H2EnumIndex(splashTarget->m_side)][splashTarget->m_index]
                    && (splashTarget != pEnemy || k == LICH_SPLASH_CENTER_DIRECTION)) {
                    gArmyEffected[H2EnumIndex(splashTarget->m_side)][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damageDone, &killed, 1, 0);
                }
            }
        }
        m_spellEffectYOffset = 0;
        powVal = COMBAT_EFFECT_LICH_CLOUD;
        castX = gpCombatManager->m_hexCells[adjacentHex].m_x;
        castY = gpCombatManager->m_hexCells[adjacentHex].m_y - PROJECTILE_TARGET_Y_OFFSET;
        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_EXTRA_ONE)]);
    } else if (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::PlasmaBlast)) {
        // The plasma blast splashes over the eighteen hexes around the target.
        static const i32 plasmaBlastMask[ARMY_PLASMA_BLAST_HEX_COUNT] = {
            -27, -26, -25, -14, -13, -12, -11, -2, -1, 1, 2, 12, 13, 14, 15, 25, 26, 27
        };
        i32 splashHex;
        army* splashTarget;

        gpCombatManager->ClearEffects();
        for (i32 direction = 0; direction < ARMY_PLASMA_BLAST_HEX_COUNT; direction++) {
            splashHex = pEnemy->m_hex + plasmaBlastMask[direction];
            if (splashHex < 0 || splashHex >= COMBAT_HEX_COUNT - 1) {
                continue;
            }
            if (gpCombatManager->m_hexCells[splashHex].m_occupantSide != COMBAT_SIDE_NONE) {
                splashTarget =
                    &gpCombatManager
                         ->m_armies[H2EnumIndex(gpCombatManager->m_hexCells[splashHex].m_occupantSide)]
                                   [gpCombatManager->m_hexCells[splashHex].m_occupantIndex];
                if (!gArmyEffected[H2EnumIndex(splashTarget->m_side)][splashTarget->m_index]
                    && pEnemy != splashTarget) {
                    gArmyEffected[H2EnumIndex(splashTarget->m_side)][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damageDone, &killed, 1, 0);
                }
            }
        }
        if (gpCombatManager->m_hexCells[pEnemy->m_hex].m_occupantSide != COMBAT_SIDE_NONE
            && !gArmyEffected[H2EnumIndex(pEnemy->m_side)][pEnemy->m_index]) {
            gArmyEffected[H2EnumIndex(pEnemy->m_side)][pEnemy->m_index] = 1;
            DamageEnemy(pEnemy, &damageDone, &killed, 1, 0);
        }
        m_spellEffectYOffset = 0;
        powVal = COMBAT_EFFECT_PLASMA_BLAST;
        castX = gpCombatManager->m_hexCells[pEnemy->m_hex].m_x;
        castY = gpCombatManager->m_hexCells[pEnemy->m_hex].m_y - PROJECTILE_TARGET_Y_OFFSET;
    } else {
        DamageEnemy(pEnemy, &damageDone, &killed, 1, 0);
    }
    m_monster.attack = baseAtk;

    if (killed > 0) {
        if (damageDone == -1) {
            strcpy(gText, localization::Tr("combat.mirror_image.destroyed"));
        } else {
            FormatCombatDamage(
                gText,
                m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                               : gArmyNames[H2EnumIndex(m_monsterType)],
                m_quantity > 1,
                damageDone,
                killed,
                killed > 1 ? gArmyNamesPlural[H2EnumIndex(pEnemy->m_monsterType)]
                           : gArmyNames[H2EnumIndex(pEnemy->m_monsterType)]
            );
            utf8::UppercaseFirst(gText);
        }
    } else {
        FormatCombatDamage(
            gText,
            m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                           : gArmyNames[H2EnumIndex(m_monsterType)],
            m_quantity > 1,
            damageDone,
            0,
            NULL
        );
        utf8::UppercaseFirst(gText);
    }
    strcpy(combatMsg, gText);
    switch (m_monsterType) {
        case CREATURE_ARCHMAGE:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ARCHMAGE_DISPEL_CHANCE && pEnemy
                && pEnemy->SpellCastWorks(CREATURE_SPELL_DISPEL)) {
                pEnemy->m_spellEffect = CREATURE_SPELL_DISPEL;
            }
            break;
    }
    PowEffect(powVal, 0, castX, castY);
    gpCombatManager->CombatMessage(combatMsg, 1, 1, 0);
    WaitSample(ARMY_SAMPLE_SHOT);

    if (m_facing != wasFacing) {
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_RIGHT) {
                m_hex++;
            } else {
                m_hex--;
            }
        }
        m_facing = wasFacing;
    }
    if (!bSecondAttack
        && (m_monsterType == CREATURE_ELF || m_monsterType == CREATURE_GRAND_ELF
            || m_monsterType == CREATURE_RANGER)
        && pEnemy->m_quantity > 0) {
        bSecondAttack = 1;
        SpecialAttack();
        bSecondAttack = 0;
    }
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]
        || m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
}

void army::DirDoAttack(CombatHexDirection direction) {
    m_attackDirection = direction;
    DoAttack(0);
}

void army::DoHydraAttack(i32) {
    CombatHexDirection dir;
    i32 hitHex;
    CombatSide occupantSide;
    i16 attackMask;
    army* pTarget;
    i32 armyIndex;
    i32 totKilled;
    i32 damage;
    i32 totDamage;
    i32 killedNow;
    char textBuf[ARMY_COMBAT_TEXT_SIZE];

    totKilled = 0;
    totDamage = totKilled;
    gpCombatManager->ResetHitByCreature();
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]) {
        attackMask = static_cast<i16>(
            GetAttackMask(m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID)
        );
    } else {
        attackMask = static_cast<i16>(
            GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID)
        );
    }
    CheckLuck();
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index]++;
    for (dir = COMBAT_DIRECTION_NORTHEAST;
         H2EnumIndex(dir) < ARMY_COMBAT_DIRECTION_COUNT;
         dir++) {
        if (!(attackMask & (1 << H2EnumIndex(dir)))) {
            hitHex = m_hex;
            if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                && ((m_facing == ARMY_FACING_LEFT
                     && dir > COMBAT_DIRECTION_SOUTHEAST)
                    || (m_facing == ARMY_FACING_RIGHT
                        && (dir < COMBAT_DIRECTION_SOUTHWEST
                            || dir > COMBAT_DIRECTION_NORTHWEST)))) {
                if (m_facing == ARMY_FACING_LEFT) {
                    hitHex = m_hex - 1;
                } else {
                    hitHex = m_hex + 1;
                }
            }
            hitHex = GetAdjacentCellIndex(hitHex, dir);
            if (ValidHex(hitHex)) {
                occupantSide = gpCombatManager->m_hexCells[hitHex].m_occupantSide;
                armyIndex = gpCombatManager->m_hexCells[hitHex].m_occupantIndex;
                if (H2EnumIndex(occupantSide) >= 0 && armyIndex >= 0) {
                    gpCombatManager->m_limitCreatureCount[H2EnumIndex(occupantSide)][armyIndex]++;
                        pTarget = &gpCombatManager->m_armies[H2EnumIndex(occupantSide)][armyIndex];
                        if (!pTarget->m_hitByCreature) {
                            pTarget->m_hitByCreature = 1;
                            DamageEnemy(pTarget, &damage, &killedNow, 0, 0);
                            if (damage < 0) {
                                strcpy(
                                    gText,
                                    localization::Tr(
                                        damage == -2 ? "combat.astral_dodge"
                                                     : "combat.mirror_image.destroyed"
                                    )
                                );
                                gpCombatManager->CombatMessage(gText, 1, 1, 0);
                                ApplyAstralDodgeWince(pTarget);
                            } else {
                                totDamage += damage;
                                totKilled += killedNow;
                            }
                        gpCombatManager->m_limitCreatureCount[H2EnumIndex(occupantSide)][armyIndex]++;
                    }
                }
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    m_animationState = 1;
    m_pendingAnimationSequence = ARMY_ANIMATION_ATTACK_FORWARD;
    gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_ATTACK)]);
    FormatCombatDamage(
        gText,
        m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                       : gArmyNames[H2EnumIndex(m_monsterType)],
        m_quantity > 1,
        totDamage,
        totKilled,
        NULL
    );
    utf8::UppercaseFirst(gText);
    strcpy(textBuf, gText);
    PowEffect(COMBAT_EFFECT_INVALID, 0, -1, -1);
    gpCombatManager->CombatMessage(textBuf, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index] = 1;
}

void army::SetChargingMoveAnimation(ChargingDirection direction) {
    if (m_monsterType != CREATURE_CYBER_PLASMA_LANCER) {
        return;
    }
    i32 inAirFrame;
    switch (direction) {
        case CHARGING_FORWARD:
            inAirFrame = ARMY_CHARGE_FRAME_FORWARD;
            break;
        case CHARGING_UP:
            inAirFrame = ARMY_CHARGE_FRAME_UP;
            break;
        case CHARGING_DOWN:
        default:
            inAirFrame = ARMY_CHARGE_FRAME_DOWN;
            break;
    }
    m_monster.attributes |= MONSTER_ATTRIBUTE_FLYING;
    for (i32 frame = 0; frame < ARMY_WALK_FRAME_COUNT; frame++) {
        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][frame] =
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][frame] =
                static_cast<i8>(inAirFrame);
    }
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0] = static_cast<i8>(inAirFrame);
}

void army::RevertChargingMoveAnimation(void) {
    if (m_monsterType != CREATURE_CYBER_PLASMA_LANCER) {
        return;
    }
    m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] = ARMY_WALK_FRAME_COUNT;
    for (i32 frame = 0; frame < ARMY_WALK_FRAME_COUNT; frame++) {
        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][frame] =
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)][frame] =
                static_cast<i8>(ARMY_CHARGE_WALK_FIRST_FRAME + frame);
    }
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0] = ARMY_CHARGE_STAND_FRAME;
    m_monster.attributes &= ~MONSTER_ATTRIBUTE_FLYING;
}

void army::SetJumpingAnimation(void) {
    if (m_monsterType != CREATURE_CYBER_PLASMA_BERSERKER) {
        return;
    }
    m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP)] =
        m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD)] =
            m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN)] = 1;
    m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)] =
        m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)] =
            m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)] = 2;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP)][0] =
        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD)][0] =
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN)][0] = 34;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)][0] =
        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)][0] =
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)][0] = 35;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)][1] =
        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)][1] =
            m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)][1] = 36;
}

void army::RevertJumpingAnimation(void) {
    if (m_monsterType != CREATURE_CYBER_PLASMA_BERSERKER) {
        return;
    }
    m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN)] =
        m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD)] =
            m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP)] = 4;
    m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)] =
        m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)] =
            m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)] = 2;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP)][0] = 17;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD)][0] = 10;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN)][0] = 24;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)][0] = 21;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)][0] = 14;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)][0] = 28;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_UP_RETURN)][1] = 22;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_FORWARD_RETURN)][1] = 15;
    m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_ATTACK_DOWN_RETURN)][1] = 29;
}

void army::ChargingDamage(const std::vector<i32>& affectedHexes) {
    if (affectedHexes.empty()) {
        return;
    }

    ArmyFacing originalFacing = m_facing;
    i32 totalDamage = 0;
    i32 totalKilled = 0;

    gChargePathDamage = true;
    for (i32 targetHex : affectedHexes) {
        army* target =
            &gpCombatManager->m_armies[H2EnumIndex(gpCombatManager->m_hexCells[targetHex].m_occupantSide)]
                                      [gpCombatManager->m_hexCells[targetHex].m_occupantIndex];
        i32 damage = 0;
        i32 killed = 0;
        DamageEnemy(target, &damage, &killed, 0, 0);
        if (damage > 0) {
            totalDamage += damage;
        }
        totalKilled += killed;
        ApplyAstralDodgeWince(target);
    }
    gChargePathDamage = false;

    if (totalDamage > 0) {
        char* attackerName = m_quantity > 1 ? ironfist::GetCreaturePluralName(H2EnumIndex(m_monsterType))
                                            : ironfist::GetCreatureName(H2EnumIndex(m_monsterType));
        if (totalKilled > 0) {
            sprintf(
                gText,
                "%s %s %d damage.\n%d creatures %s.",
                attackerName,
                m_quantity > 1 ? "do" : "does",
                totalDamage,
                totalKilled,
                totalKilled > 1 ? "perish" : "perishes"
            );
        } else {
            sprintf(
                gText,
                "%s %s %d damage.",
                attackerName,
                m_quantity > 1 ? "do" : "does",
                totalDamage
            );
        }
        gText[0] = toupper(gText[0]);
        gpCombatManager->CombatMessage(gText, 1, 1, 0);
    }

    gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index] = 1;

    if (m_facing != originalFacing && !(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))) {
        m_facing = originalFacing;
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            if (m_facing == ARMY_FACING_RIGHT) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
    }
}

void army::DoAttack(i32 retaliation) {
    ArmyFacing targetOriginalFacing_5;
    CombatHexDirection originalDirection;
    army* breathTarget_6;
    CombatHexDirection secondAttackDirection_6;
    i32 breathDamage;
    ArmyFacing desiredFacing;
    i32 effectStopsRetaliation_4;
    ArmyFacing originalFacing_6;
    CombatSide occupantSide_5;
    char combatText[ARMY_COMBAT_TEXT_SIZE];
    i32 damage;
    i32 targetHex_3;
    i32 killed_13;
    army* target_1;
    i32 breathKilled;
    i32 adjacentHex_1;
    i32 revivedQuantity_3;
    i32 breathHex_1;

    m_drawState = ARMY_DRAW_IN_FRONT;
    damage = 0;
    killed_13 = 0;
    breathDamage = 0;
    breathKilled = 0;
    effectStopsRetaliation_4 = 0;
    target_1 = NULL;
    breathTarget_6 = NULL;
    originalFacing_6 = m_facing;
    ironfist::hooks::MeleeAttackStarted(this, retaliation);
    if (retaliation) {
        gCloseMove = true;
        gpCombatManager->m_currentSide = OppositeCombatSide(gpCombatManager->m_currentSide);
    }
    if (m_monsterType == CREATURE_HYDRA) {
        DoHydraAttack(retaliation);
    } else {
        originalDirection = m_attackDirection;
        targetHex_3 = m_hex;
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
            && ((m_facing == ARMY_FACING_LEFT
                 && m_attackDirection >= COMBAT_DIRECTION_SOUTHWEST)
                || (m_facing == ARMY_FACING_RIGHT
                    && (m_attackDirection <= COMBAT_DIRECTION_SOUTHEAST
                        || m_attackDirection >= COMBAT_DIRECTION_WIDE_WEST)))) {
            if (originalFacing_6 == ARMY_FACING_LEFT) {
                targetHex_3 = m_hex - 1;
            } else {
                targetHex_3 = m_hex + 1;
            }
        }
        targetHex_3 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
        target_1 =
            &gpCombatManager->m_armies[H2EnumIndex(gpCombatManager->m_hexCells[targetHex_3].m_occupantSide)]
                                      [gpCombatManager->m_hexCells[targetHex_3].m_occupantIndex];
        if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_BREATH_ATTACK)))) {
            breathHex_1 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
            if (ValidHex(breathHex_1)
                && H2EnumIndex(gpCombatManager->m_hexCells[breathHex_1].m_occupantSide) >= 0
                && gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex >= 0
                && (gpCombatManager->m_hexCells[breathHex_1].m_occupantSide != target_1->m_side
                    || gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex
                           != target_1->m_index)) {
                breathTarget_6 =
                    &gpCombatManager
                         ->m_armies[H2EnumIndex(gpCombatManager->m_hexCells[breathHex_1].m_occupantSide)]
                                   [gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex];
            }
        }
        gpCombatManager->ResetLimitCreature();
        gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index]++;
        gpCombatManager->m_limitCreatureCount[H2EnumIndex(target_1->m_side)][target_1->m_index]++;
        if (breathTarget_6) {
            gpCombatManager
                ->m_limitCreatureCount[H2EnumIndex(breathTarget_6->m_side)][breathTarget_6->m_index]++;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        targetOriginalFacing_5 = target_1->m_facing;
        if (m_attackDirection <= COMBAT_DIRECTION_SOUTHEAST) {
            desiredFacing = ARMY_FACING_RIGHT;
        } else if (m_attackDirection <= COMBAT_DIRECTION_NORTHWEST) {
            desiredFacing = ARMY_FACING_LEFT;
        } else {
            desiredFacing = m_facing;
        }
        if (m_facing != desiredFacing) {
            m_facing = desiredFacing;
            if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                if (desiredFacing == ARMY_FACING_RIGHT) {
                    m_hex--;
                } else {
                    m_hex++;
                }
            }
            target_1->m_facing = OppositeArmyFacing(m_facing);
            if (target_1->m_facing != targetOriginalFacing_5
                && (H2EnumIndex((target_1->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                if (target_1->m_facing == ARMY_FACING_RIGHT) {
                    target_1->m_hex--;
                } else {
                    target_1->m_hex++;
                }
            }
        }
        CheckLuck();
        m_animationState = 1;
        if (m_attackDirection == COMBAT_DIRECTION_WIDE_WEST
            || m_attackDirection == COMBAT_DIRECTION_NORTHWEST
            || m_attackDirection == COMBAT_DIRECTION_NORTHEAST) {
            m_pendingAnimationSequence = ARMY_ANIMATION_ATTACK_UP;
        } else if (m_attackDirection == COMBAT_DIRECTION_EAST
                   || m_attackDirection == COMBAT_DIRECTION_WEST) {
            m_pendingAnimationSequence = ARMY_ANIMATION_ATTACK_FORWARD;
        } else {
            m_pendingAnimationSequence = ARMY_ANIMATION_ATTACK_DOWN;
        }
        if (breathTarget_6) {
            m_pendingAnimationSequence += ARMY_BREATH_ATTACK_SEQUENCE_OFFSET;
        }
        gpSoundManager->MemorySample(m_samples[H2EnumIndex(ARMY_SAMPLE_ATTACK)]);
        DamageEnemy(target_1, &damage, &killed_13, 0, retaliation);
        if (breathTarget_6) {
            DamageEnemy(breathTarget_6, &breathDamage, &breathKilled, 0, retaliation);
        }
        if (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Charger)) {
            gCharging = false;
        }
        if (damage == -1) {
            strcpy(gText, localization::Tr("combat.mirror_image.destroyed"));
        } else if (damage == -2) {
            strcpy(gText, localization::Tr("combat.astral_dodge"));
        } else if (gbGenieHalf) {
            sprintf(
                gText,
                localization::TrPlural("combat.genie.destroy_half", m_quantity),
                m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                               : gArmyNames[H2EnumIndex(m_monsterType)]
            );
            utf8::UppercaseFirst(gText);
        } else {
            if (killed_13 > 0) {
                FormatCombatDamage(
                    gText,
                    m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                                   : gArmyNames[H2EnumIndex(m_monsterType)],
                    m_quantity > 1,
                    damage,
                    killed_13,
                    killed_13 > 1 ? gArmyNamesPlural[H2EnumIndex(target_1->m_monsterType)]
                                  : gArmyNames[H2EnumIndex(target_1->m_monsterType)]
                );
                utf8::UppercaseFirst(gText);
            } else {
                FormatCombatDamage(
                    gText,
                    m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                                   : gArmyNames[H2EnumIndex(m_monsterType)],
                    m_quantity > 1,
                    damage,
                    0,
                    NULL
                );
                utf8::UppercaseFirst(gText);
            }
        }
        strcpy(combatText, gText);

        switch (m_monsterType) {
            case CREATURE_CYCLOPS:
                if (target_1->m_quantity > 0) {
                    if (!breathTarget_6 || breathTarget_6->m_quantity > 0) {
                        if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                            if (target_1 && target_1->SpellCastWorks(SPELL_PARALYZE)) {
                                target_1->m_spellEffect = SPELL_PARALYZE;
                                effectStopsRetaliation_4 = 1;
                            }
                        } else if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE
                                   && breathTarget_6
                                   && breathTarget_6->SpellCastWorks(SPELL_PARALYZE)) {
                            breathTarget_6->m_spellEffect = SPELL_PARALYZE;
                        }
                    }
                }
                break;
            case CREATURE_UNICORN:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(SPELL_BLIND)) {
                    target_1->m_spellEffect = SPELL_BLIND;
                    effectStopsRetaliation_4 = 1;
                }
                break;
            case CREATURE_MEDUSA:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(CREATURE_SPELL_PETRIFY)) {
                    target_1->m_spellEffect = CREATURE_SPELL_PETRIFY;
                    effectStopsRetaliation_4 = 1;
                }
                break;
            case CREATURE_MUMMY:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                    goto applyMummySpell;
                }
                break;
            case CREATURE_ROYAL_MUMMY:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ROYAL_MUMMY_EFFECT_CHANCE) {
                applyMummySpell:
                    if (target_1 && target_1->SpellCastWorks(SPELL_CURSE)) {
                        target_1->m_spellEffect = SPELL_CURSE;
                    }
                }
                break;
            case CREATURE_ARCHMAGE:
                if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_1
                    && target_1->SpellCastWorks(CREATURE_SPELL_DISPEL)) {
                    target_1->m_spellEffect = CREATURE_SPELL_DISPEL;
                }
                break;
            case CREATURE_GHOST:
                gpCombatManager->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] =
                    killed_13;
                break;
            case CREATURE_VAMPIRE_LORD:
                gpCombatManager->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] =
                    target_1->m_monster.hitPoints * killed_13;
                break;
        }
        // Shadow-marking creatures brand their victim on every hit.
        if (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::ShadowMark)
            && target_1->SpellCastWorks(SPELL_SHADOW_MARK)) {
            target_1->m_spellEffect = SPELL_SHADOW_MARK;
        }
        ApplyAstralDodgeWince(target_1);
        if (ironfist::state::Get().combat.stack.abilityNowAnimating[this][ironfist::CreatureAttribute::Jumper]) {
            SetJumpingAnimation();
            ironfist::state::Get().combat.stack.abilityNowAnimating[this][ironfist::CreatureAttribute::Jumper] = false;
        } else {
            RevertJumpingAnimation();
        }
        PowEffect(COMBAT_EFFECT_INVALID, 0, -1, -1);
        gpCombatManager->CombatMessage(combatText, 1, 1, 0);
        gpCombatManager->m_limitCreatureCount[H2EnumIndex(m_side)][m_index] = 1;

        if (m_monsterType == CREATURE_GHOST) {
            m_quantity +=
                gpCombatManager->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)];
        }
        if (m_monsterType == CREATURE_VAMPIRE_LORD) {
            if (gpCombatManager->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)]
                < m_hitPointsLost) {
                m_hitPointsLost -=
                    gpCombatManager
                        ->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)];
            } else {
                gpCombatManager->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] -=
                    m_hitPointsLost;
                m_hitPointsLost = 0;
                revivedQuantity_3 =
                    gpCombatManager
                        ->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[m_hex].m_occupantSide)]
                    / m_monster.hitPoints;
                if (revivedQuantity_3 < m_initialQuantity - m_quantity) {
                    m_quantity += revivedQuantity_3;
                } else {
                    m_quantity = m_initialQuantity;
                }
            }
        }

        if (target_1 && target_1->m_quantity > 0
            && !target_1->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]
            && !target_1->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]
            && (target_1->m_monsterType == CREATURE_GRIFFIN
                || !(H2EnumIndex((target_1->m_monster.flags.all) & (MONSTER_FLAGS_RETALIATED))))
            && m_monsterType != CREATURE_ROGUE && m_monsterType != CREATURE_SPRITE
            && m_monsterType != CREATURE_VAMPIRE && m_monsterType != CREATURE_VAMPIRE_LORD
            // Teleporting in from afar leaves no chance to retaliate.
            && !(ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Teleporter) && !gCloseMove)
            && !effectStopsRetaliation_4 && !retaliation) {
            DelayMilli(
                static_cast<i32l>(
                    gfCombatSpeedMod[gConfig.combatSpeed]
                    * H2EnumIndex(ARMY_RETALIATION_DELAY)
                )
            );
            target_1->m_attackDirection = OppositeDirection(m_attackDirection);
            if ((H2EnumIndex((target_1->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    target_1->m_facing == ARMY_FACING_RIGHT
                        ? COMBAT_DIRECTION_NORTHEAST
                        : COMBAT_DIRECTION_NORTHWEST
                );
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
                }
                adjacentHex_1 = GetAdjacentCellIndex(
                    target_1->m_hex,
                    target_1->m_facing == ARMY_FACING_RIGHT
                        ? COMBAT_DIRECTION_SOUTHEAST
                        : COMBAT_DIRECTION_SOUTHWEST
                );
                if (adjacentHex_1 == m_hex) {
                    target_1->m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;
                }
            }
            target_1->DoAttack(1);
            target_1->m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
            if (gbRemoteOn && gpCombatManager->m_networkArmyPresent[0]
                && gpCombatManager->m_networkArmyPresent[1]
                && target_1->m_monsterType == CREATURE_GHOST) {
                target_1->m_quantity +=
                    gpCombatManager
                        ->m_killBenefit[H2EnumIndex(gpCombatManager->m_hexCells[target_1->m_hex]
                                               .m_occupantSide)];
            }
        }
        if ((m_monsterType == CREATURE_WOLF || m_monsterType == CREATURE_PALADIN
             || m_monsterType == CREATURE_CRUSADER)
            && target_1 && target_1->m_quantity > 0 && !retaliation
            && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]
            && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]
            && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)] && m_quantity > 0) {
            DelayMilli(
                static_cast<i32l>(
                    gfCombatSpeedMod[gConfig.combatSpeed] * H2EnumIndex(ARMY_SECOND_ATTACK_DELAY)
                )
            );
            secondAttackDirection_6 = m_attackDirection;
            m_attackDirection = originalDirection;
            DoAttack(1);
            m_attackDirection = secondAttackDirection_6;
        }
        if (m_facing != originalFacing_6) {
            if (!(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))) {
                m_facing = originalFacing_6;
                if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                    if (originalFacing_6 == ARMY_FACING_RIGHT) {
                        m_hex--;
                    } else {
                        m_hex++;
                    }
                }
            }
            if (!(H2EnumIndex((target_1->m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))
                && target_1->m_facing != targetOriginalFacing_5) {
                target_1->m_facing = targetOriginalFacing_5;
                if ((H2EnumIndex((target_1->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                    if (target_1->m_facing == ARMY_FACING_RIGHT) {
                        target_1->m_hex--;
                    } else {
                        target_1->m_hex++;
                    }
                }
            }
        }
    }
    if (!retaliation
        && (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]
            || m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    targetHex_3 = -1;
    m_targetSide = COMBAT_SIDE_NONE;
    if (retaliation) {
        gpCombatManager->m_currentSide = OppositeCombatSide(gpCombatManager->m_currentSide);
    } else {
        ironfist::hooks::MeleeAttackCompleted(this, target_1);
    }
}

void army::ResetPath(void) {
    return;
}

i32 army::WalkTo(void) {
    return WalkTo(m_moveTargetHex);
}

i32 army::WalkTo(i32 destination) {
    i32 direction_3;
    i32 steps;
    i32 moatFound;
    i32 moatIndex_1;
    i32 canEnterMoat_1;

    m_targetIndex = -1;
    m_targetSide = m_targetIndex;
    if (gpCombatManager->m_drawbridgeBackgroundVisible
        && (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        moatFound = 0;
        moatIndex_1 = 0;
        for (direction_3 = 0; direction_3 < ARMY_MOAT_CELL_COUNT; direction_3++) {
            if (moatCell[direction_3] == destination) {
                moatFound = 1;
                moatIndex_1 = direction_3;
            }
        }
        if (moatFound) {
            canEnterMoat_1 = 0;
            if (moatIndex_1 == ARMY_MOAT_GATE_INDEX
                && gpCombatManager->m_drawbridgeState != COMBAT_CASTLE_GATE_OPEN) {
                canEnterMoat_1 = 1;
            }
            if ((moatIndex_1 > 0 && moatCell[moatIndex_1 - 1] == m_hex)
                || (moatIndex_1 < ARMY_MOAT_CELL_COUNT - 1 && moatCell[moatIndex_1 + 1] == m_hex)) {
                canEnterMoat_1 = 1;
            }
            for (direction_3 = H2EnumIndex(COMBAT_DIRECTION_NORTHEAST);
                 direction_3 < ARMY_ADJACENT_DIRECTION_COUNT;
                 direction_3++) {
                if (moatCell[moatIndex_1]
                    == GetAdjacentCellIndex(
                        m_hex,
                        static_cast<CombatHexDirection>(direction_3)
                    )) {
                    canEnterMoat_1 = 1;
                }
            }
            if (m_side == COMBAT_ATTACKER_SIDE && moatCell[m_hex / ARMY_HEX_COLUMNS] < m_hex) {
                canEnterMoat_1 = 1;
            }
            if (m_side == COMBAT_DEFENDER_SIDE && m_hex < moatCell[m_hex / ARMY_HEX_COLUMNS]) {
                canEnterMoat_1 = 1;
            }
            if (!canEnterMoat_1) {
                if (m_facing == ARMY_FACING_RIGHT) {
                    destination--;
                } else {
                    destination++;
                }
            }
        }
    }
    if (!FindPath(m_hex, destination, m_monster.speed, 1, ARMY_PATH_ANY_TARGET_HEX)) {
        return ARMY_PATH_BLOCKED;
    }

    steps = 0;
    i32 jumpStartHex = m_hex;
    for (direction_3 = gpSearchArray->m_pathLength - 1; direction_3 >= 0; direction_3--) {
        CombatHexDirection stepDirection = static_cast<CombatHexDirection>(
            gpSearchArray->m_storage.path.directions[direction_3 + 1]
        );
        // The Berserker jumps: onto its victim from up to four hexes out,
        // or over the first obstacle in its way.
        if (m_monsterType == CREATURE_CYBER_PLASMA_BERSERKER
            && ironfist::state::Get().combat.stack.abilityCounter[this][ironfist::CreatureAttribute::Jumper]) {
            i32 stepHex = GetAdjacentCellIndex(m_hex, stepDirection);
            if (gMoveAttack && direction_3 < 4) {
                for (i32 step = direction_3; step >= 0; step--) {
                    stepDirection = static_cast<CombatHexDirection>(
                        gpSearchArray->m_storage.path.directions[step + 1]
                    );
                    stepHex = GetAdjacentCellIndex(m_hex, stepDirection);
                    m_hex = stepHex;
                }
                ArcJump(jumpStartHex, stepHex);
                ironfist::state::Get().combat.stack.abilityNowAnimating[this][ironfist::CreatureAttribute::Jumper] = true;
                CancelSpellType(ArmySpellCancelType(0));
                return 0;
            } else if (gpCombatManager->m_hexCells[stepHex].m_blocked) {
                for (i32 landIndex = direction_3 - 1; landIndex >= 0; landIndex--) {
                    stepDirection = static_cast<CombatHexDirection>(
                        gpSearchArray->m_storage.path.directions[landIndex + 1]
                    );
                    m_hex = stepHex;
                    stepHex = GetAdjacentCellIndex(m_hex, stepDirection);
                    if (!gpCombatManager->m_hexCells[stepHex].m_blocked) {
                        steps += direction_3 - landIndex;
                        direction_3 = landIndex;
                        ArcJump(jumpStartHex, stepHex);
                        break;
                    }
                }
            } else {
                Walk(stepDirection, 0, gpSearchArray->m_pathLength - 1 != direction_3);
            }
        } else {
            Walk(stepDirection, 0, gpSearchArray->m_pathLength - 1 != direction_3);
        }
        steps++;
        if (steps >= m_monster.speed) {
            direction_3 = -1;
        }
        jumpStartHex = m_hex;
    }
    CancelSpellType(ArmySpellCancelType(0));
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 0;
}

i32 army::AttackTo(void) {
    return AttackTo(m_moveTargetHex);
}

i32 army::AttackTo(i32 destination) {
    i32 finishStanding;
    i32 numSteps;
    i32 pathIndex_4;

    // A charger whose walking path is blocked flies its straight line in.
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))
        || (ValidPath(destination, ARMY_PATH_ANY_TARGET_HEX) == 0
            && ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Charger))) {
        if (m_hex != destination) {
            FlyTo(destination);
        }
        DoAttack(0);
        return 0;
    }
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_BREATH_ATTACK))) && m_hex == m_moveTargetHex) {
        DoAttack(0);
        return 0;
    }
    if (FindPath(m_hex, destination, m_monster.speed, 1, ARMY_PATH_ANY_TARGET_HEX)) {
        if (gpSearchArray->m_pathLength == 1) {
            m_attackDirection = static_cast<CombatHexDirection>(
                gpSearchArray->m_storage.path.directions[1]
            );
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        } else {
            pathIndex_4 = 0;
            numSteps = 0;
            i32 jumpStartHex = m_hex;
            for (pathIndex_4 = gpSearchArray->m_pathLength - 1; pathIndex_4 != 0; pathIndex_4--) {
                numSteps++;
                if (pathIndex_4 == 1 || numSteps >= m_monster.speed) {
                    finishStanding = 1;
                } else {
                    finishStanding = 0;
                }
                // The Berserker leaps the last stretch onto its victim.
                if (m_monsterType == CREATURE_CYBER_PLASMA_BERSERKER
                    && ironfist::state::Get().combat.stack.abilityCounter[this][ironfist::CreatureAttribute::Jumper]
                    && gMoveAttack && pathIndex_4 < 5) {
                    i32 stepHex = m_hex;
                    for (i32 step = pathIndex_4; step >= 1; step--) {
                        stepHex = GetAdjacentCellIndex(
                            m_hex,
                            static_cast<CombatHexDirection>(
                                gpSearchArray->m_storage.path.directions[step + 1]
                            )
                        );
                        m_hex = stepHex;
                    }
                    ArcJump(jumpStartHex, stepHex);
                    ironfist::state::Get().combat.stack.abilityNowAnimating[this][ironfist::CreatureAttribute::Jumper] = true;
                    break;
                }
                Walk(
                    static_cast<CombatHexDirection>(
                        gpSearchArray->m_storage.path.directions[pathIndex_4 + 1]
                    ),
                    finishStanding,
                    gpSearchArray->m_pathLength - 1 != pathIndex_4
                );
                if (numSteps >= m_monster.speed && pathIndex_4 != 1) {
                    return ARMY_PATH_BLOCKED;
                }
                jumpStartHex = m_hex;
            }
            CancelSpellType(ArmySpellCancelType(0));
            m_attackDirection = static_cast<CombatHexDirection>(
                gpSearchArray->m_storage.path.directions[1]
            );
            gpCombatManager->TestRaiseDoor();
            DoAttack(0);
        }
        return 0;
    }
    return ARMY_PATH_BLOCKED;
}

void army::CheckLuck(void) {
    SAMPLE2 luckSample;

    m_luckOutcome = 0;
    if (!gpCombatManager->m_heroes[H2EnumIndex(m_side)]) {
        return;
    }
    if (m_luck > 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) <= m_luck) {
        m_luckOutcome = 1;
    }
    if (m_luck < 0 && SRandom(1, ARMY_LUCK_ROLL_MAX) < -m_luck) {
        m_luckOutcome = -1;
    }
    if (m_luckOutcome) {
        if (m_luckOutcome < 0) {
            sprintf(gText, "badluck.82m");
        } else {
            sprintf(gText, "goodluck.82m");
        }
        luckSample = LoadPlaySample(gText);
        if (m_luckOutcome < 0) {
            sprintf(
                gText,
                localization::TrPlural("combat.luck.bad", m_quantity),
                m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                               : gArmyNames[H2EnumIndex(m_monsterType)]
            );
            gpCombatManager->CombatMessage(gText, 1, 1, 0);
            SpellEffect(COMBAT_EFFECT_BAD_LUCK, ARMY_BAD_LUCK_EFFECT_DELAY, 0);
        } else {
            sprintf(
                gText,
                localization::TrPlural("combat.luck.good", m_quantity),
                m_quantity > 1 ? gArmyNamesPlural[H2EnumIndex(m_monsterType)]
                               : gArmyNames[H2EnumIndex(m_monsterType)]
            );
            gpCombatManager->CombatMessage(gText, 1, 1, 0);
            gpCombatManager->DoLuck(m_side, m_index);
        }
        WaitEndSample(&luckSample, -1);
        if (m_luckOutcome > 0) {
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
            gpMouseManager->ShowColorPointer();
        }
    }
}

void army::DamageEnemy(
    army* target,
    i32* damageResult,
    i32* killedResult,
    i32 rangedAttack,
    i32 retaliation
) {
    float damage1;
    i32 attackBonus6;
    i32 defenseBonus9;
    i32 attackDifference7;
    i32 rearHex19;
    i32 index16;
    i32 damageDone2;
    i32 genieDamage26;
    hero* commander1;

    if (!target) {
        return;
    }
    damage1 = 0;
    gbGenieHalf = false;
    for (index16 = 0; index16 < m_quantity; index16++) {
        if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)]) {
            damage1 += m_monster.damageMax;
        } else if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)]) {
            damage1 += m_monster.damageMin;
        } else {
            damage1 += SRandom(m_monster.damageMin, m_monster.damageMax);
        }
    }
    // Dazed or burning creatures fight at half their skill.
    attackBonus6 = m_monster.attack;
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DAZE)]
        || m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BURN)]) {
        attackBonus6 /= 2;
    }
    defenseBonus9 = target->m_monster.defense;
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DAZE)]
        || target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BURN)]) {
        defenseBonus9 /= 2;
    }
    attackDifference7 = attackBonus6 - defenseBonus9;
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && (target->m_monsterType == CREATURE_GREEN_DRAGON
            || target->m_monsterType == CREATURE_RED_DRAGON
            || target->m_monsterType == CREATURE_BLACK_DRAGON
            || target->m_monsterType == CREATURE_BONE_DRAGON)) {
        attackDifference7 += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (gpCombatManager->m_drawbridgeBackgroundVisible) {
        rearHex19 = -1;
        if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            rearHex19 = ArmyFacingRearHexOffset(target->m_facing) + target->m_hex;
        }
        for (index16 = 0; index16 < ARMY_MOAT_CELL_COUNT; index16++) {
            if (moatCell[index16] == target->m_hex || moatCell[index16] == rearHex19) {
                attackDifference7 += ARMY_MOAT_ATTACK_BONUS;
            }
        }
    }
    if (attackDifference7 > ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference7 = ARMY_DAMAGE_STAT_LIMIT;
    }
    if (attackDifference7 < -ARMY_DAMAGE_STAT_LIMIT) {
        attackDifference7 = -ARMY_DAMAGE_STAT_LIMIT;
    }
    damage1 *= gfBattleStat[attackDifference7 + ARMY_DAMAGE_STAT_LIMIT];
    if ((m_monsterType == CREATURE_CRUSADER
         && (H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD))))
        || (m_monsterType == CREATURE_EARTH_ELEMENTAL
            && target->m_monsterType == CREATURE_AIR_ELEMENTAL)
        || (m_monsterType == CREATURE_AIR_ELEMENTAL
            && target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
        || (m_monsterType == CREATURE_WATER_ELEMENTAL
            && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
        || (m_monsterType == CREATURE_FIRE_ELEMENTAL
            && target->m_monsterType == CREATURE_WATER_ELEMENTAL)) {
        damage1 *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (m_luckOutcome > 0) {
        damage1 *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (m_luckOutcome < 0) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    m_luckOutcome = 0;
    if (rangedAttack && gpCombatManager->ShotIsThroughWall(m_side, m_hex, target->m_hex)) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    commander1 = gpCombatManager->m_heroes[H2EnumIndex(m_side)];
    if (commander1 && rangedAttack) {
        damage1 *=
            gfSSArcheryMod[H2EnumIndex(commander1->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)])];
    }
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))) && !rangedAttack
        && m_monsterType != CREATURE_TITAN && m_monsterType != CREATURE_MAGE
        && m_monsterType != CREATURE_ARCHMAGE
        && m_monsterType != CREATURE_CYBER_BEHEMOTH) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    if (rangedAttack && target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHIELD)]) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    if (m_damagePenalty == ARMY_DAMAGE_PENALTY_HALF) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    m_damagePenalty = ARMY_DAMAGE_PENALTY_NONE;
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        damage1 /= DAMAGE_HALF_DIVISOR;
    }
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHADOW_MARK)]) {
        damage1 *= DAMAGE_SHADOW_MARK_MULTIPLIER;
    }
    // A jumper strikes harder mid-jump, once per battle.
    if (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Jumper) && !retaliation
        && ironfist::state::Get().combat.stack.abilityCounter[this][ironfist::CreatureAttribute::Jumper]
        && ironfist::state::Get().combat.stack.abilityNowAnimating[this][ironfist::CreatureAttribute::Jumper]) {
        ironfist::state::Get().combat.stack.abilityCounter[this][ironfist::CreatureAttribute::Jumper] = 0;
        damage1 *= SRandom(125, 150) * 0.01f;
    }
    // A charger's hit softens along its path and lands harder at the end.
    if (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Charger) && gCharging) {
        if (gChargePathDamage) {
            damage1 *= DAMAGE_CHARGE_PATH_MULTIPLIER;
        } else {
            damage1 *= DAMAGE_CHARGE_MULTIPLIER;
        }
    }
    // Teleporting into an enemy from afar hits harder.
    if (!gCloseMove && ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Teleporter)) {
        damage1 *= DAMAGE_TELEPORT_MULTIPLIER;
    }
    damageDone2 = static_cast<i32>(damage1 + DAMAGE_ROUNDING_OFFSET);
    if (m_monsterType == CREATURE_GENIE
        && SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieDamage26 =
            ((target->m_quantity + 1) / GENIE_QUANTITY_DIVISOR) * target->m_monster.hitPoints;
        if (damageDone2 < genieDamage26) {
            gbGenieHalf = true;
            damageDone2 = genieDamage26;
        }
    }
    if (damageDone2 <= 0) {
        damageDone2 = 1;
    }
    if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
        damageDone2 = -1;
    }
    // An astral dodger slips one melee blow per round; -2 marks the dodge.
    if (!rangedAttack && !retaliation
        && ironfist::HasCreatureAttribute(target->m_monsterType, ironfist::CreatureAttribute::AstralDodge)
        && ironfist::state::Get().combat.stack.abilityCounter[target][ironfist::CreatureAttribute::AstralDodge]) {
        ironfist::state::Get().combat.stack.abilityNowAnimating[target][ironfist::CreatureAttribute::AstralDodge] = true;
        ironfist::state::Get().combat.stack.abilityCounter[target][ironfist::CreatureAttribute::AstralDodge] = 0;
        damageDone2 = -2;
    }
    *damageResult = damageDone2;
    if (damageDone2 < 0) {
        *killedResult = target->Damage(0, SPELL_NONE);
        return;
    }
    // A force shield soaks the hit; the overflow passes through and breaks it.
    i32 shieldPoints = ironfist::state::Get().combat.stack.forceShieldHP[target];
    if (shieldPoints > 0) {
        if (shieldPoints - damageDone2 <= 0) {
            target->CancelIndividualSpell(ARMY_SPELL_INFLUENCE_FORCE_SHIELD);
            damageDone2 -= shieldPoints;
        } else {
            ironfist::state::Get().combat.stack.forceShieldHP[target] -= damageDone2;
            damageDone2 = 0;
        }
    }
    *killedResult = target->Damage(damageDone2, SPELL_NONE);
}

i32 army::Damage(i32l damage, SpellType spell) {
    i32 killed;
    ArmyFacing oldFacing;
    i32 quantityFifth;

    damage += m_hitPointsLost;
    if (spell != SPELL_NONE) {
        if (gbRemoteOn) {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage,
                spell,
                gpCombatManager->m_heroes[H2EnumIndex(m_side)],
                gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)]
            );
        } else {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage,
                spell,
                gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)],
                gpCombatManager->m_heroes[H2EnumIndex(m_side)]
            );
        }
    }
    killed = damage / m_monster.hitPoints;
    m_hitPointsLost = damage % m_monster.hitPoints;
    quantityFifth = m_quantity / DAMAGE_DISPLAY_DIVISOR;
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
        killed = m_quantity;
        m_hitPointsLost = 0;
    }
    if (!quantityFifth) {
        quantityFifth = 1;
    }
    m_damagePending = 1;
    if (killed > 0) {
        m_killPending = 1;
        m_lastTargetHex = m_quantity;
    }
    if (killed > m_quantity) {
        killed = m_quantity;
    }
    m_quantity -= killed;
    if (m_quantity <= 0) {
        m_deathPending = 1;
    }
    oldFacing = m_facing;
    m_facing = static_cast<ArmyFacing>(
        H2EnumIndex(gpCombatManager
                ->m_armies[H2EnumIndex(gpCombatManager->m_currentArmySide)]
                          [gpCombatManager->m_currentArmyIndex]
                .m_facing)
        ^ H2EnumIndex(ARMY_FACING_RIGHT)
    );
    m_facing = oldFacing;
    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    return killed;
}

void army::PowEffect(
    CombatEffectType effect,
    i32 resetLimits,
    i32 effectX,
    i32 effectY
) {
    i32 leadFrames;
    i32 adjust;
    i32 spellFrames;
    i32 damageFrameCount;
    i32 maxStartFrames;
    i32 maxFinishFrames;
    i32 animFrame;
    i32 drawEffect;
    army* current;
    i32 animMore;
    i32 armyIndex;
    i32 maximumDamageFrames;
    i32 frameDelay;
    i32 endFrameCount;
    IconEntry* iconRec;
    i32 frameTotal;
    CombatSide sideNum;

    maxStartFrames = 0;
    maxFinishFrames = 0;
    maximumDamageFrames = 0;
    spellFrames = 0;
    frameTotal = 0;
    leadFrames = 0;
    endFrameCount = 0;
    damageFrameCount = 0;
    drawEffect = 0;
    adjust = 1;
    if (m_monsterType == CREATURE_PALADIN || m_monsterType == CREATURE_CRUSADER) {
        adjust = 0;
    }
    if (m_monsterType == CREATURE_DWARF || m_monsterType == CREATURE_BATTLE_DWARF) {
        adjust = POW_EFFECT_DWARF_OVERLAP_ADJUSTMENT;
    }
    if (effectX != NO_POW_EFFECT_COORDINATE) {
        drawEffect = 1;
    } else if (effect != COMBAT_EFFECT_INVALID) {
        for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
                if (gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_drawSpellEffect) {
                    drawEffect = 1;
                }
            }
        }
    }
    if (!gbNoShowCombat && effect != COMBAT_EFFECT_INVALID && drawEffect
        && gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(gCombatFxNames[H2EnumIndex(effect)]);
        gCurLoadedSpellEffect = effect;
    }
    if (drawEffect) {
        spellFrames = giNumPowFrames[H2EnumIndex(gCurLoadedSpellEffect)];
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
            if (static_cast<u8>(current->m_animationState)) {
                leadFrames =
                    current->m_frameInfo.animationFrameCount[H2EnumIndex(m_pendingAnimationSequence)];
                endFrameCount =
                    current->m_frameInfo
                        .animationFrameCount[H2EnumIndex(m_pendingAnimationSequence + 1)]
                    + 1;
            } else {
                if (current->m_deathPending) {
                    damageFrameCount =
                        current->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)];
                } else if (current->m_damagePending) {
                    damageFrameCount =
                        current->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)]
                        + current->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE_RETURN)]
                        + 1;
                }
            }
            maxStartFrames =
                maxStartFrames > leadFrames ? maxStartFrames : leadFrames;
            maxFinishFrames =
                maxFinishFrames > endFrameCount ? maxFinishFrames : endFrameCount;
            maximumDamageFrames =
                maximumDamageFrames > damageFrameCount ? maximumDamageFrames : damageFrameCount;
        }
    }
    frameTotal = maxStartFrames + maxFinishFrames;
    frameTotal =
        frameTotal > maxStartFrames + maximumDamageFrames - adjust
            ? frameTotal
            : maxStartFrames + maximumDamageFrames - adjust;
    frameTotal = frameTotal > maximumDamageFrames ? frameTotal : maximumDamageFrames;
    spellFrames = frameTotal > spellFrames ? frameTotal : spellFrames;
    frameDelay = ARMY_POW_EFFECT_DELAY;
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            if (gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationSequence
                    == ARMY_ANIMATION_SHOOT_UP
                || gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_FORWARD
                || gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_DOWN) {
                gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationCycle = 1;
            } else {
                gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationCycle = 0;
            }
            if ((gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_damagePending
                 || static_cast<u8>(gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationState)
                 || static_cast<u8>(gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].m_animationCycle))
                && !gpCombatManager->m_limitCreatureCount[H2EnumIndex(sideNum)][armyIndex]) {
                gpCombatManager->m_limitCreatureCount[H2EnumIndex(sideNum)][armyIndex]++;
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    if (effectX != NO_POW_EFFECT_COORDINATE) {
        for (armyIndex = 0; armyIndex < gCurLoadedSpellIcon->m_frameCount; armyIndex++) {
            iconRec = reinterpret_cast<IconEntry*>(
                gCurLoadedSpellIcon->m_data + armyIndex * sizeof(IconEntry)
            );
            giMinExtentX = giMinExtentX < effectX + iconRec->x ? giMinExtentX
                                                                : effectX + iconRec->x;
            giMinExtentY = giMinExtentY < effectY + iconRec->y ? giMinExtentY
                                                                : effectY + iconRec->y;
            giMaxExtentX = giMaxExtentX > effectX + iconRec->x + iconRec->w - 1
                               ? giMaxExtentX
                               : effectX + iconRec->x + iconRec->w - 1;
            giMaxExtentY = giMaxExtentY > effectY + iconRec->y + iconRec->h - 1
                               ? giMaxExtentY
                               : effectY + iconRec->y + iconRec->h - 1;
        }
        giMinExtentX = giMinExtentX > 0 ? giMinExtentX : 0;
        giMinExtentY = giMinExtentY > 0 ? giMinExtentY : 0;
        giMaxExtentX = giMaxExtentX < ARMY_COMBAT_MAX_X ? giMaxExtentX : ARMY_COMBAT_MAX_X;
        giMaxExtentY = giMaxExtentY < ARMY_COMBAT_MAX_Y ? giMaxExtentY : ARMY_COMBAT_MAX_Y;
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
            current->m_effectAnimationStart = ARMY_ANIMATION_NONE;
            current->m_effectAnimationEnd = ARMY_ANIMATION_NONE;
            current->m_effectAnimationStarted = 0;
            if (current->m_damagePending || static_cast<u8>(current->m_animationState)) {
                if (static_cast<u8>(current->m_animationState)) {
                    current->m_effectAnimationStart = m_pendingAnimationSequence;
                    current->m_effectAnimationEnd = m_pendingAnimationSequence + 1;
                } else if (current->m_deathPending) {
                    current->m_effectAnimationStart = ARMY_ANIMATION_DEATH;
                } else {
                    current->m_effectAnimationStart = ARMY_ANIMATION_WINCE;
                    current->m_effectAnimationEnd = ARMY_ANIMATION_WINCE_RETURN;
                }
                if (current->m_effectAnimationStart == ARMY_ANIMATION_DEATH) {
                    current->m_effectAnimationLength =
                        current->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)];
                } else {
                    current->m_effectAnimationLength =
                        current->m_frameInfo
                            .animationFrameCount[H2EnumIndex(current->m_effectAnimationStart)]
                        + current->m_frameInfo
                              .animationFrameCount[H2EnumIndex(current->m_effectAnimationStart + 1)];
                }
                if (current->m_effectAnimationStart == current->m_animationSequence) {
                    current->m_effectAnimationLength--;
                }
                if (m_drawState < ARMY_DRAW_EFFECT) {
                    m_drawState = ARMY_DRAW_EFFECT;
                }
            }
        }
    }
    animFrame = 0;
    for (animFrame = 0; animFrame < spellFrames; animFrame++) {
        for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
                current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
                if (static_cast<u8>(current->m_animationCycle)) {
                    if (current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN) {
                        current->m_animationSequence++;
                        current->m_animationFrame = 0;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_STAND) {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[H2EnumIndex(current->m_animationSequence)]) {
                            current->m_animationFrame++;
                        } else {
                            current->m_animationSequence = ARMY_ANIMATION_STAND;
                            current->m_animationFrame = 0;
                        }
                    }
                }
                if (current->m_effectAnimationStart != ARMY_ANIMATION_NONE
                    && !current->m_effectAnimationStarted
                    && (static_cast<u8>(current->m_animationState)
                        || static_cast<i32>(spellFrames - animFrame - 1)
                               <= current->m_effectAnimationLength
                        || (maxStartFrames && animFrame >= maxStartFrames - 1)
                        || (!maxStartFrames
                            && current->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN
                            && (current->m_animationSequence != ARMY_ANIMATION_WINCE
                                || current->m_animationFrame + 1
                                       < current->m_frameInfo.animationFrameCount
                                             [H2EnumIndex(current->m_animationSequence)])))) {
                    if (current->m_animationSequence != current->m_effectAnimationStart
                        && current->m_animationSequence != current->m_effectAnimationEnd) {
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == ARMY_ANIMATION_WINCE) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex]
                                                             .m_samples[H2EnumIndex(ARMY_SAMPLE_WINCE)]);
                        }
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == ARMY_ANIMATION_DEATH) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex]
                                                             .m_samples[H2EnumIndex(ARMY_SAMPLE_KILL)]);
                        }
                        current->m_animationSequence = current->m_effectAnimationStart;
                        current->m_animationFrame = 0;
                    } else {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[H2EnumIndex(current->m_animationSequence)]) {
                            current->m_animationFrame++;
                        } else if (current->m_animationSequence != current->m_effectAnimationEnd
                                   && current->m_effectAnimationEnd != ARMY_ANIMATION_NONE) {
                            current->m_animationSequence = current->m_effectAnimationEnd;
                            current->m_animationFrame = 0;
                        } else {
                            if (current->m_animationSequence != ARMY_ANIMATION_STAND
                                && current->m_animationSequence != ARMY_ANIMATION_DEATH) {
                                current->m_animationSequence = ARMY_ANIMATION_STAND;
                                current->m_animationFrame = 0;
                                current->m_effectAnimationStarted = 1;
                            }
                        }
                    }
                }
            }
        }
        glTimers[0] =
            static_cast<i32>(platform::Ticks() + frameDelay * gfCombatSpeedMod[gConfig.combatSpeed]);
        if (drawEffect && animFrame < giNumPowFrames[H2EnumIndex(gCurLoadedSpellEffect)]) {
            gCurSpellEffectFrame = animFrame;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        if (effectX != NO_POW_EFFECT_COORDINATE
            && animFrame < giNumPowFrames[H2EnumIndex(gCurLoadedSpellEffect)]) {
            gCurLoadedSpellIcon->CombatClipDrawToBuffer(
                effectX,
                effectY + m_spellEffectYOffset,
                gCurSpellEffectFrame,
                &m_spellLimits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
    }
    if (!gbNoShowCombat) {
        WaitSample(ARMY_SAMPLE_ATTACK);
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
            if (current->m_damagePending && current->m_spellEffect != SPELL_NONE
                && current->m_spellEffect != CREATURE_SPELL_PETRIFY) {
                gpCombatManager->CastSpell(
                    current->m_spellEffect,
                    current->m_hex,
                    1,
                    COMBAT_HEX_EMPTY
                );
                current->m_spellEffect = SPELL_NONE;
            }
        }
    }
    animMore = 1;
    while (animMore) {
        animMore = 0;
        for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
                current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
                if (current->m_animationSequence == ARMY_ANIMATION_WINCE
                    || current->m_animationSequence == ARMY_ANIMATION_ATTACK_UP
                    || current->m_animationSequence == ARMY_ANIMATION_ATTACK_FORWARD
                    || current->m_animationSequence == ARMY_ANIMATION_ATTACK_DOWN
                    || current->m_animationSequence == ARMY_ANIMATION_BREATH_UP
                    || current->m_animationSequence == ARMY_ANIMATION_BREATH_FORWARD
                    || current->m_animationSequence == ARMY_ANIMATION_BREATH_DOWN
                    || current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP
                    || current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD
                    || current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN) {
                    current->m_animationSequence++;
                    current->m_animationFrame = 0;
                    animMore = 1;
                } else if (current->m_animationSequence == ARMY_ANIMATION_DEATH
                           || current->m_animationSequence == ARMY_ANIMATION_WINCE_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_ATTACK_UP_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_ATTACK_FORWARD_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_ATTACK_DOWN_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_BREATH_UP_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_BREATH_FORWARD_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_BREATH_DOWN_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD_RETURN
                           || current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN_RETURN) {
                    if (current->m_animationFrame + 1
                        < current->m_frameInfo
                              .animationFrameCount[H2EnumIndex(current->m_animationSequence)]) {
                        current->m_animationFrame++;
                        animMore = 1;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_DEATH) {
                        current->m_animationSequence = ARMY_ANIMATION_STAND;
                        current->m_animationFrame = 0;
                        animMore = 1;
                    }
                }
            }
        }
        if (animMore) {
            glTimers[0] = static_cast<i32>(
                platform::Ticks() + frameDelay * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    memset(gpCombatManager->m_removedArmies, 0, sizeof(gpCombatManager->m_removedArmies));
    gpCombatManager->m_removedArmyPresent = 0;
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
            if (current->m_deathPending) {
                current->ProcessDeath(0);
            }
        }
    }
    if (gpCombatManager->m_removedArmyPresent) {
        gpCombatManager->MakeCreaturesVanish();
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex];
            if (current->m_damagePending && current->m_spellEffect == CREATURE_SPELL_PETRIFY) {
                gpCombatManager->CastSpell(
                    current->m_spellEffect,
                    current->m_hex,
                    1,
                    COMBAT_HEX_EMPTY
                );
                current->m_spellEffect = SPELL_NONE;
            }
            current->m_drawSpellEffect = 0;
            current->m_damagePending = 0;
            current->m_killPending = 0;
            current->m_drawState = ARMY_DRAW_NORMAL;
            current->m_animationState = 0;
            current->m_lastTargetHex = -1;
        }
    }
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    for (sideNum = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[H2EnumIndex(sideNum)]; armyIndex++) {
            gpCombatManager->m_armies[H2EnumIndex(sideNum)][armyIndex].WaitSample(ARMY_SAMPLE_WINCE);
        }
    }
}

u32l army::Strength(void) {
    return gMonsterDatabase[H2EnumIndex(m_monsterType)].fightValue * m_quantity;
}

i32 army::LeaveNoBody(void) {
    return m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
           || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
           || (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)));
}

void army::ProcessDeath(i32 immediate) {
    i32 rearHex;
    army* mirrorImage_4;
    army* mirrorSource;
    hexcell* frontCell_1;
    hexcell* rearCell;

    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))) {
        return;
    }
    if (Random(0, DEATH_RANDOM_MAX) < DEATH_PRIMARY_CHANCE) {
        gpCombatManager->m_heroDeathPending[H2EnumIndex(m_side)] = 1;
    } else if (Random(0, DEATH_RANDOM_MAX) < DEATH_SECONDARY_CHANCE) {
        gpCombatManager->m_heroAlternateDeathPending[H2EnumIndex(OppositeCombatSide(m_side))] = 1;
    }
    m_monster.attributes |= MONSTER_ATTRIBUTE_DEAD;
    m_deathPending = 0;
    frontCell_1 = &gpCombatManager->m_hexCells[m_hex];
    rearHex = 0;
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        rearHex = m_hex + ArmyFacingRearHexOffset(m_facing);
        rearCell = &gpCombatManager->m_hexCells[rearHex];
    } else {
        rearCell = NULL;
    }
    if (LeaveNoBody()) {
        if (immediate
            || (m_monsterType != CREATURE_AIR_ELEMENTAL && m_monsterType != CREATURE_FIRE_ELEMENTAL
                && m_monsterType != CREATURE_WATER_ELEMENTAL
                && m_monsterType != CREATURE_EARTH_ELEMENTAL)) {
            gpCombatManager->m_removedArmies[H2EnumIndex(m_side)][m_index] = 1;
            gpCombatManager->m_removedArmyPresent = 1;
        } else {
            frontCell_1->m_occupantSide = COMBAT_SIDE_NONE;
            frontCell_1->m_occupantIndex = -1;
        }
    }
    if (frontCell_1->m_deadOccupantCount < CORPSE_LIMIT && !LeaveNoBody()
        && (!rearCell || rearCell->m_deadOccupantCount < CORPSE_LIMIT)) {
        frontCell_1->m_deadOccupantSides[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantSide;
        frontCell_1->m_deadOccupantIndices[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantIndex;
        frontCell_1->m_deadOccupantFrames[frontCell_1->m_deadOccupantCount] =
            gpCombatManager->m_hexCells[m_hex].m_occupantFrame;
        frontCell_1->m_deadOccupantCount++;
        if (rearCell) {
            rearCell->m_deadOccupantSides[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantSide;
            rearCell->m_deadOccupantIndices[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantIndex;
            rearCell->m_deadOccupantFrames[rearCell->m_deadOccupantCount] =
                gpCombatManager->m_hexCells[rearHex].m_occupantFrame;
            rearCell->m_deadOccupantCount++;
        }
    }
    if (!LeaveNoBody()) {
        frontCell_1->m_occupantSide = COMBAT_SIDE_NONE;
        frontCell_1->m_occupantIndex = -1;
        if (rearCell) {
            rearCell->m_occupantSide = COMBAT_SIDE_NONE;
            rearCell->m_occupantIndex = -1;
        }
    }
    if (m_mirrorSourceIndex != -1) {
        mirrorSource = &gpCombatManager->m_armies[H2EnumIndex(m_side)][m_mirrorSourceIndex];
        mirrorSource->m_mirrorImageIndex = -1;
    }
    if (m_mirrorImageIndex != -1) {
        mirrorImage_4 = &gpCombatManager->m_armies[H2EnumIndex(m_side)][m_mirrorImageIndex];
        mirrorImage_4->m_quantity = 0;
        mirrorImage_4->ProcessDeath(0);
    }
}

void army::SpellEffect(
    CombatEffectType effect,
    i32 effectFrameDelay,
    i32 animateCreature
) {
    IconEntry* entry;
    i32 smallestY;
    i32 frameDelay;
    i32 unusedWord;
    i32 frame;
    i32 i;
    u32l effectFileId;
    i32 powBaseY;

    effectFileId = MAKEFILEID(gCombatFxNames[H2EnumIndex(effect)]);
    if (m_animationSequence == ARMY_ANIMATION_WINCE
        || m_animationSequence == ARMY_ANIMATION_WINCE_RETURN) {
        animateCreature = 0;
    }
    if (!gbNoShowCombat) {
        if (gCurLoadedSpellEffect != effect) {
            gpResourceManager->Dispose(gCurLoadedSpellIcon);
            gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFileId);
            gCurLoadedSpellEffect = effect;
        }
    }
    frame = 0;
    m_drawSpellEffect = 1;
    m_spellEffectYOffset = 0;
    if (!gbNoShowCombat) {
        smallestY = EFFECT_MINIMUM_Y;
        for (i = 0; i < gCurLoadedSpellIcon->m_frameCount; i++) {
            entry = GetIconEntry(gCurLoadedSpellIcon, i);
            if (entry->y < smallestY) {
                smallestY = entry->y;
            }
        }
        powBaseY = GetPowBaseY();
        powBaseY += smallestY;
        if (powBaseY < 0) {
            m_spellEffectYOffset = -powBaseY;
        }
        if (animateCreature) {
            frameDelay =
                ARMY_SPELL_EFFECT_ANIMATION_DURATION
                / m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)];
            m_animationSequence = ARMY_ANIMATION_WINCE;
            for (; frame
                   < m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)];
                 frame++) {
                m_animationFrame = frame;
                if (frame < giNumPowFrames[H2EnumIndex(effect)]) {
                    gCurSpellEffectFrame = frame;
                } else {
                    gCurSpellEffectFrame = giNumPowFrames[H2EnumIndex(effect)];
                }
                glTimers[1] = static_cast<i32>(
                    platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
        }
        for (; frame < giNumPowFrames[H2EnumIndex(effect)]; frame++) {
            glTimers[1] = static_cast<i32>(
                platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * effectFrameDelay
            );
            gCurSpellEffectFrame = frame;
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
            DelayTil(&glTimers[1]);
        }
    }
    m_drawSpellEffect = 0;
    if (!gbNoShowCombat) {
        if (animateCreature) {
            frameDelay = ARMY_SPELL_EFFECT_ANIMATION_DURATION
                           / m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE_RETURN)];
            m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
            for (frame = 0; frame < m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE_RETURN)];
                 frame++) {
                m_animationFrame = frame;
                glTimers[1] = static_cast<i32>(
                    platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
            m_animationSequence = ARMY_ANIMATION_STAND;
            m_animationFrame = 0;
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
}

void army::CancelSpellType(ArmySpellCancelType cancelType) {
    switch (cancelType) {
        case ARMY_CANCEL_SPELLS_AFTER_MOVE:
            break;
        case ARMY_CANCEL_SPELLS_AFTER_ATTACK:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
            break;
        case ARMY_CANCEL_SPELLS_AFTER_DAMAGE:
            if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)]) {
                CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
                m_damagePenalty = ARMY_DAMAGE_PENALTY_HALF;
                m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
            }
            if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
                m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
                m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
                CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PARALYZE);
                CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PETRIFIED);
            }
            break;
        case ARMY_CANCEL_SPELLS_UNUSED:
            break;
    }
}

void army::CancelIndividualSpell(ArmySpellInfluence influence) {
    if (!m_spellInfluence[H2EnumIndex(influence)]) {
        return;
    }
    m_spellCount--;
    m_spellInfluence[H2EnumIndex(influence)] = 0;
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
        case ARMY_SPELL_INFLUENCE_SLOW:
            m_monster.speed = static_cast<i8>(m_speed);
            m_frameInfo.walkDuration = m_walkDuration;
            m_monster.attributes |=
                gMonsterDatabase[H2EnumIndex(m_monsterType)].attributes & MONSTER_ATTRIBUTE_FLYING;
            break;
        case ARMY_SPELL_INFLUENCE_BLIND:
            break;
        case ARMY_SPELL_INFLUENCE_BLESS:
            break;
        case ARMY_SPELL_INFLUENCE_CURSE:
            break;
        case ARMY_SPELL_INFLUENCE_BERSERK:
            break;
        case ARMY_SPELL_INFLUENCE_PARALYZE:
            break;
        case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
            break;
        case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
            break;
        case ARMY_SPELL_INFLUENCE_BLOODLUST:
            m_monster.attack -= BLOODLUST_ATTACK_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_SHIELD:
            break;
        case ARMY_SPELL_INFLUENCE_PETRIFIED:
            break;
        case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            m_monster.defense -= STONESKIN_DEFENSE_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_STEELSKIN:
            m_monster.defense -= STEELSKIN_DEFENSE_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_SHADOW_MARK:
        case ARMY_SPELL_INFLUENCE_DAZE:
        case ARMY_SPELL_INFLUENCE_BURN:
            break;
        case ARMY_SPELL_INFLUENCE_FORCE_SHIELD:
            ironfist::state::Get().combat.stack.forceShieldHP[this] = 0;
            break;
    }
}

i32 army::SetSpellInfluence(ArmySpellInfluence influence, i32 rounds) {
    H2SteppedEnumStorage<ArmySpellInfluence, i32> i;

    if (m_spellInfluence[H2EnumIndex(influence)]) {
        if (rounds > m_spellInfluence[H2EnumIndex(influence)]) {
            m_spellInfluence[H2EnumIndex(influence)] = static_cast<u8>(rounds);
        }
        return 0;
    }
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SLOW);
            m_monster.speed += HASTE_SPEED_BONUS;
            m_frameInfo.walkDuration =
                static_cast<i32>(m_frameInfo.walkDuration * ARMY_HASTE_WALK_DURATION_SCALE);
            break;
        case ARMY_SPELL_INFLUENCE_SLOW:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HASTE);
            m_monster.speed = static_cast<i8>((m_monster.speed + 1) / SLOW_SPEED_DIVISOR);
            if (H2EnumIndex((m_monster.attributes) & (MONSTER_ATTRIBUTE_FLYING))) {
                ((m_monster.attributes) &= ~(MONSTER_ATTRIBUTE_FLYING));
            }
            m_frameInfo.walkDuration =
                static_cast<i32>(m_frameInfo.walkDuration * ARMY_SLOW_WALK_DURATION_SCALE);
            break;
        case ARMY_SPELL_INFLUENCE_BLIND:
            break;
        case ARMY_SPELL_INFLUENCE_BLESS:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_CURSE);
            break;
        case ARMY_SPELL_INFLUENCE_CURSE:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLESS);
            break;
        case ARMY_SPELL_INFLUENCE_BERSERK:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
            break;
        case ARMY_SPELL_INFLUENCE_PARALYZE:
            break;
        case ARMY_SPELL_INFLUENCE_HYPNOTIZE:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
            break;
        case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
            break;
        case ARMY_SPELL_INFLUENCE_BLOODLUST:
            m_monster.attack += BLOODLUST_ATTACK_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_SHIELD:
            break;
        case ARMY_SPELL_INFLUENCE_PETRIFIED:
            break;
        case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
            for (i = ARMY_SPELL_INFLUENCE_HASTE; i < ARMY_SPELL_INFLUENCE_RETAIL_COUNT; i++) {
                CancelIndividualSpell(i);
            }
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                return 0;
            }
            m_monster.defense += STONESKIN_DEFENSE_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_STEELSKIN:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STONESKIN);
            m_monster.defense += STEELSKIN_DEFENSE_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_SHADOW_MARK:
        case ARMY_SPELL_INFLUENCE_DAZE:
        case ARMY_SPELL_INFLUENCE_BURN:
            break;
        case ARMY_SPELL_INFLUENCE_FORCE_SHIELD:
            ironfist::state::Get().combat.stack.forceShieldHP[this] =
                gMonsterDatabase[H2EnumIndex(m_monsterType)].hitPoints;
            break;
    }
    m_spellCount++;
    m_spellInfluence[H2EnumIndex(influence)] = static_cast<u8>(rounds);
    return 1;
}

void army::DecrementSpellRounds(void) {
    H2SteppedEnumStorage<ArmySpellInfluence, i32> i;

    for (i = ARMY_SPELL_INFLUENCE_HASTE; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
        if (m_spellInfluence[H2EnumIndex(i)]) {
            // A force shield holds until it breaks; burning wounds each round.
            if (i == ARMY_SPELL_INFLUENCE_FORCE_SHIELD)
                continue;
            if (i == ARMY_SPELL_INFLUENCE_BURN)
                gpCombatManager->BurnCreature(this);
            if (m_spellInfluence[H2EnumIndex(i)] == 1) {
                CancelIndividualSpell(i);
            } else {
                m_spellInfluence[H2EnumIndex(i)]--;
            }
        }
    }
    if (m_roundCounter > 0) {
        m_roundCounter--;
    }
}

void army::GoBerserk(void) {
    i32 masks_28[H2EnumIndex(COMBAT_SIDE_COUNT)];
    i32 attackMask_29;
    i32 unusedMask_16;
    i32 targetFound_8;
    i32 savedQuantity_8;
    CombatHexDirection direction_4;
    i32 targetHex_36;
    CombatSide nearestSide_8;
    i32 nearestIndex_1;
    i32 nearestDistance_4;
    CombatSide side_8;
    i32 index_10;
    i32 distance_8;

    targetFound_8 = 0;
    direction_4 = COMBAT_DIRECTION_NORTHEAST;
    unusedMask_16 = 0;
    savedQuantity_8 = m_quantity;
    m_quantity = 0;
    masks_28[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = gpCombatManager->GetAllMask(COMBAT_ATTACKER_SIDE);
    masks_28[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = gpCombatManager->GetAllMask(COMBAT_DEFENDER_SIDE);
    m_quantity = savedQuantity_8;
    attackMask_29 =
        GetAttackMask(m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
    if (attackMask_29 != ARMY_ALL_ATTACK_DIRECTIONS) {
        while (!targetFound_8) {
            direction_4 = static_cast<CombatHexDirection>(
                Random(0, ARMY_COMBAT_DIRECTION_COUNT - 1)
            );
            if (!(attackMask_29 & (1 << H2EnumIndex(direction_4)))) {
                giNextAction = ACTION_MOVE;
                ValidAttack(
                    m_hex,
                    direction_4,
                    ARMY_ATTACK_TARGET_OCCUPIED,
                    ARMY_HEX_INVALID,
                    &targetHex_36
                );
                giNextActionGridIndex = targetHex_36;
                targetFound_8 = 1;
                goto berserkFinish;
            }
        }
    } else {
        nearestIndex_1 = -1;
        nearestSide_8 = COMBAT_SIDE_NONE;
        nearestDistance_4 = ARMY_NEAREST_DISTANCE_LIMIT;
        for (side_8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side_8) < COMBAT_SIDE_COUNT; side_8++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[H2EnumIndex(side_8)]; index_10++) {
                if (side_8 == m_side && index_10 == m_index) {
                    continue;
                }
                army* candidate_16 = &gpCombatManager->m_armies[H2EnumIndex(side_8)][index_10];
                if ((H2EnumIndex((candidate_16->m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))
                    || candidate_16->m_quantity <= 0) {
                    continue;
                }
                distance_8 = gpSearchArray->QuickDistance(
                    gpCombatManager->m_hexCells[m_hex].m_x,
                    gpCombatManager->m_hexCells[m_hex].m_y,
                    gpCombatManager->m_hexCells[candidate_16->m_hex].m_x,
                    gpCombatManager->m_hexCells[candidate_16->m_hex].m_y
                );
                if (distance_8 < nearestDistance_4) {
                    nearestIndex_1 = index_10;
                    nearestSide_8 = side_8;
                    nearestDistance_4 = distance_8;
                }
            }
        }
        if (nearestIndex_1 != -1 && (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER)))
            && m_monster.shots > 0) {
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex =
                gpCombatManager->m_armies[H2EnumIndex(nearestSide_8)][nearestIndex_1].m_hex;
            goto berserkFinish;
        }
        {
            i32 sideZeroTarget_12 = -1;
            i32 sideOneTarget_18 = -1;
            if (gpCombatManager->AttemptAttack(
                    this,
                    COMBAT_ATTACKER_SIDE,
                    masks_28[H2EnumIndex(COMBAT_ATTACKER_SIDE)]
                )) {
                giNextAction = ACTION_MOVE;
                sideZeroTarget_12 = giNextActionGridIndex;
            }
            if (gpCombatManager->AttemptAttack(
                    this,
                    COMBAT_DEFENDER_SIDE,
                    masks_28[H2EnumIndex(COMBAT_DEFENDER_SIDE)]
                )) {
                giNextAction = ACTION_MOVE;
                sideOneTarget_18 = giNextActionGridIndex;
            }
            giNextActionGridIndex = -1;
            if (sideZeroTarget_12 != -1 || sideOneTarget_18 != -1) {
                if (sideZeroTarget_12 != -1 && sideOneTarget_18 != -1) {
                    i32 sideZeroDistance_1 = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_x,
                        gpCombatManager->m_hexCells[sideZeroTarget_12].m_y
                    );
                    i32 sideOneDistance = gpSearchArray->QuickDistance(
                        gpCombatManager->m_hexCells[m_hex].m_x,
                        gpCombatManager->m_hexCells[m_hex].m_y,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_x,
                        gpCombatManager->m_hexCells[sideOneTarget_18].m_y
                    );
                    if (sideZeroDistance_1 < sideOneDistance) {
                        giNextActionGridIndex = sideZeroTarget_12;
                    } else {
                        giNextActionGridIndex = sideOneTarget_18;
                    }
                } else if (sideZeroTarget_12 != -1) {
                    giNextActionGridIndex = sideZeroTarget_12;
                } else if (sideOneTarget_18 != -1) {
                    giNextActionGridIndex = sideOneTarget_18;
                }
            }
            if (giNextActionGridIndex != -1)
                goto berserkFinish;
        }
    }
walkToward:
    if (!(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))) {
        if (gpCombatManager->WalkTowardArmy(this, m_side, masks_28[H2EnumIndex(m_side)]))
            goto berserkFinish;
        if (gpCombatManager->WalkTowardArmy(
                this,
                OppositeCombatSide(m_side),
                masks_28[H2EnumIndex(OppositeCombatSide(m_side))]
            ))
            goto berserkFinish;
    }
    giNextAction = ACTION_WAIT;
berserkFinish:
    if (giNextAction == ACTION_MOVE
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == m_side) {
        gpCombatManager->m_heroDeathPending[H2EnumIndex(m_side)] = 1;
    }
}


void army::MoveTo(i32 destination) {
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))) {
        m_moveTargetHex = destination;
        if (ValidFlight(m_moveTargetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            FlyTo(m_moveTargetHex);
        }
    } else {
        WalkTo(destination);
    }
}

// Whether the target sits exactly on one of the six straight hex lines
// from this stack.
bool army::TargetOnStraightLine(i32 targetHex) {
    i32 deltaY = gpCombatManager->m_hexCells[targetHex].m_y
                 - gpCombatManager->m_hexCells[m_hex].m_y;
    i32 deltaX = gpCombatManager->m_hexCells[targetHex].m_x
                 - gpCombatManager->m_hexCells[m_hex].m_x;
    double angle = fabs((180.0 / M_PI) * atan2(static_cast<double>(deltaY), abs(deltaX)));
    return angle == 0 || angle == ARMY_HEX_DIAGONAL_ANGLE;
}

CombatHexDirection army::GetStraightLineDirection(i32 targetHex) {
    i32 deltaY = gpCombatManager->m_hexCells[targetHex].m_y
                 - gpCombatManager->m_hexCells[m_hex].m_y;
    i32 deltaX = gpCombatManager->m_hexCells[targetHex].m_x
                 - gpCombatManager->m_hexCells[m_hex].m_x;
    double angle =
        (180.0 / M_PI) * atan2(static_cast<double>(deltaY), static_cast<double>(deltaX));
    if (angle >= -62.4 && angle <= -62.3)
        return COMBAT_DIRECTION_NORTHEAST;
    if (angle == 0)
        return COMBAT_DIRECTION_EAST;
    if (angle >= 62.3 && angle <= 62.4)
        return COMBAT_DIRECTION_SOUTHEAST;
    if (angle >= 117.6 && angle <= 117.7)
        return COMBAT_DIRECTION_SOUTHWEST;
    if (angle == 180)
        return COMBAT_DIRECTION_WEST;
    if (angle >= -117.7 && angle <= -117.6)
        return COMBAT_DIRECTION_NORTHWEST;
    return COMBAT_DIRECTION_INVALID;
}

i32 army::GetStraightLineDistanceToHex(i32 targetHex) {
    CombatHexDirection direction = GetStraightLineDirection(targetHex);
    if (direction == COMBAT_DIRECTION_INVALID) {
        return ARMY_STRAIGHT_LINE_UNREACHABLE;
    }
    i32 currentHex = m_hex;
    i32 distance = 0;
    while (currentHex != targetHex) {
        currentHex = GetAdjacentCellIndex(currentHex, direction);
        distance++;
        if (currentHex < 0 || currentHex >= COMBAT_HEX_COUNT) {
            return ARMY_STRAIGHT_LINE_UNREACHABLE;
        }
    }
    return distance;
}

// Whether a straight flight to the hex crosses any blocked cell.
bool army::FlightThroughObstacles(i32 destination) {
    double sourceX = MidX();
    double sourceY = MidY();
    double destX = gpCombatManager->m_hexCells[destination].m_x;
    double destY = gpCombatManager->m_hexCells[destination].m_y;
    double deltaX = destX - sourceX;
    double deltaY = destY - sourceY;
    i32 x = static_cast<i32>(sourceX);
    i32 endX = static_cast<i32>(destX);
    if (sourceX > destX) {
        x = static_cast<i32>(destX);
        endX = static_cast<i32>(sourceX);
    }
    while (x < endX) {
        i32 y = static_cast<i32>(sourceY + deltaY * (x - sourceX) / deltaX);
        i32 cellIndex = gpCombatManager->GetGridIndex(x, y);
        if (gpCombatManager->m_hexCells[cellIndex].m_blocked) {
            return true;
        }
        x++;
    }
    return false;
}

bool army::IsEnemyCreatureHex(i32 hexIndex) {
    return gpCombatManager->m_hexCells[hexIndex].m_occupantIndex != -1
           && gpCombatManager->m_hexCells[hexIndex].m_occupantSide != m_side;
}

void army::MoveAttackNonFlyer(i32 startHex, i32 attackMask) {
    i32 targetAttackMask;
    i32 sourceHex;
    i32 adjacentHex;
    CombatHexDirection direction;
    hexcell* adjacentCell;

    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)]) {
        targetAttackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
    } else {
        targetAttackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID);
    }
    if (targetAttackMask == ARMY_ALL_ATTACK_DIRECTIONS && m_monster.shots > 0) {
        SpecialAttack();
    } else if (attackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
        AttackTo();
    } else {
        for (direction = COMBAT_DIRECTION_NORTHEAST;
             H2EnumIndex(direction) < ARMY_COMBAT_DIRECTION_COUNT;
             direction++) {
            if (H2EnumIndex(direction) < ARMY_ADJACENT_DIRECTION_COUNT
                || (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                sourceHex = m_hex;
                if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                    && m_facing == ARMY_FACING_RIGHT
                    && direction >= COMBAT_DIRECTION_NORTHEAST
                    && direction <= COMBAT_DIRECTION_SOUTHEAST) {
                    sourceHex++;
                }
                if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                    && m_facing == ARMY_FACING_LEFT
                    && direction >= COMBAT_DIRECTION_SOUTHWEST
                    && direction <= COMBAT_DIRECTION_NORTHWEST) {
                    sourceHex--;
                }
                if (direction >= COMBAT_DIRECTION_WIDE_WEST) {
                    if (m_facing == ARMY_FACING_RIGHT) {
                        sourceHex++;
                    } else {
                        sourceHex--;
                    }
                }
                adjacentHex = GetAdjacentCellIndex(sourceHex, direction);
                if (ValidHex(adjacentHex)) {
                    adjacentCell = &gpCombatManager->m_hexCells[adjacentHex];
                    if (adjacentCell->m_occupantSide == m_targetSide
                        && adjacentCell->m_occupantIndex == m_targetIndex) {
                        m_attackDirection = direction;
                    }
                }
            }
        }
        DoAttack(0);
    }

    // Strike-and-return creatures fly home after the blow.
    if (!(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DEAD)))
        && ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::StrikeAndReturn)) {
        MoveTo(startHex);
    }
}

void army::MoveAttack(i32 destination, i32 moveOnly) {
    i32 baseAttackMask;
    i32 returnHex;

    gMoveAttack = false;
    if (ValidHex(destination)
        && (moveOnly == 1
            || (!moveOnly
                && gpCombatManager->m_hexCells[destination].m_occupantSide
                       != COMBAT_SIDE_NONE))) {
        gMoveAttack = true;
    }
    returnHex = m_hex;

again:
    gpCombatManager->m_limitCreature = 0;
    m_targetSide = COMBAT_SIDE_NONE;
    m_targetIndex = -1;
    if (!ValidHex(destination)) {
        return;
    }
    if (gpCombatManager->m_hexCells[destination].m_occupantSide == COMBAT_SIDE_NONE
        || (gpCombatManager->m_hexCells[destination].m_occupantSide
                == gpCombatManager->m_currentArmySide
            && gpCombatManager->m_hexCells[destination].m_occupantIndex
                   == gpCombatManager->m_currentArmyIndex)) {
        goto move;
    }
    if (moveOnly) {
        return;
    }
    m_targetSide = gpCombatManager->m_hexCells[destination].m_occupantSide;
    m_targetIndex = gpCombatManager->m_hexCells[destination].m_occupantIndex;
    m_moveTargetHex = destination;
    baseAttackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
    if (((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))
         || (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Charger)
             && TargetOnStraightLine(destination)))
        && baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
        if (m_hex != m_moveTargetHex
            && !ValidFlight(m_moveTargetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            return;
        }
        FlyTo(m_moveTargetHex);
        goto again;
    }
    MoveAttackNonFlyer(returnHex, baseAttackMask);
    goto finish;
move:
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))
        // A charger on a clear line flies its move-attack in.
        || (ironfist::HasCreatureAttribute(m_monsterType, ironfist::CreatureAttribute::Charger) && gMoveAttack
            && TargetOnStraightLine(giNextActionGridIndex)
            && TargetOnStraightLine(destination))) {
        m_moveTargetHex = destination;
        if (!ValidFlight(m_moveTargetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            return;
        }
        FlyTo(m_moveTargetHex);
    } else {
        WalkTo(destination);
    }
finish:
    gpCombatManager->m_limitCreature = 1;
}

float army::SpellCastWorkChance(SpellType spell) {
    i32 foundSpell_8;
    H2SteppedEnumStorage<ArmySpellInfluence, i32> i_15;
    i32 resurrectPower_5;
    i32 hypnotizeHitPoints_37;

    // A dying stack cannot be shadow-marked.
    if (spell == SPELL_SHADOW_MARK && m_deathPending) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))
        && (spell == SPELL_MIRROR_IMAGE || spell == SPELL_ANTI_MAGIC)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL) {
        foundSpell_8 = 0;
        for (i_15 = ARMY_SPELL_INFLUENCE_HASTE; i_15 < ARMY_SPELL_INFLUENCE_COUNT; i_15++) {
            if (m_spellInfluence[H2EnumIndex(i_15)]) {
                foundSpell_8 = 1;
                break;
            }
        }
        if (!foundSpell_8) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
    }
    if (m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        || ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_DEAD))) && spell != SPELL_RESURRECT
            && spell != SPELL_TRUE_RESURRECT && spell != SPELL_ANIMATE_DEAD)
        || m_deathPending || m_monsterType == CREATURE_GREEN_DRAGON
        || m_monsterType == CREATURE_RED_DRAGON || m_monsterType == CREATURE_BLACK_DRAGON) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (spell == SPELL_MIRROR_IMAGE && m_mirrorImageIndex != -1) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if ((spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT)
        && ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD))) || m_initialQuantity == m_quantity)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (spell == SPELL_ANIMATE_DEAD
        && (!(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD))) || m_initialQuantity == m_quantity)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if ((spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)
        && !(H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD)))) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if ((spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)
        && (H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD)))) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_PHOENIX
        && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST
            || spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING
            || spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING
            || spell == SPELL_ELEMENTAL_STORM)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_CRUSADER && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD))) || m_monsterType == CREATURE_EARTH_ELEMENTAL
         || m_monsterType == CREATURE_AIR_ELEMENTAL || m_monsterType == CREATURE_FIRE_ELEMENTAL
         || m_monsterType == CREATURE_WATER_ELEMENTAL || m_monsterType == CREATURE_GIANT
         || m_monsterType == CREATURE_TITAN)
        && (spell == SPELL_BERSERKER || spell == SPELL_HYPNOTIZE || spell == SPELL_PARALYZE
            || spell == SPELL_BLIND)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD)))
        && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE || spell == SPELL_BLESS
            || spell == SPELL_MASS_BLESS)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL
        && (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING
            || spell == SPELL_METEOR_SHOWER)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_AIR_ELEMENTAL && spell == SPELL_METEOR_SHOWER) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_FIRE_ELEMENTAL
        && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (m_monsterType == CREATURE_WATER_ELEMENTAL
        && (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]) {
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_HOLY_PENDANT)
            && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_PENDANT_FREE_WILL)
            && spell == SPELL_HYPNOTIZE) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_PENDANT_LIFE)
            && (spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_SERENITY_PENDANT)
            && spell == SPELL_BERSERKER) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_SEEING_EYE_PENDANT)
            && spell == SPELL_BLIND) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_KINETIC_PENDANT)
            && spell == SPELL_PARALYZE) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_PENDANT_DEATH)
            && (spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
        if (gpCombatManager->m_heroes[H2EnumIndex(m_side)]->HasArtifact(ARTIFACT_WAND_NEGATION)
            && (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL
                || spell == CREATURE_SPELL_DISPEL)) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
    }
    if (m_quantity == 0
        && (spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT
            || spell == SPELL_ANIMATE_DEAD)) {
        resurrectPower_5 = gpCombatManager->m_spellPower[H2EnumIndex(gpCombatManager->m_currentSide)]
                           * RESURRECT_POWER_PER_SPELL_POWER;
        if (gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)]
            && gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)]->HasArtifact(
                ARTIFACT_ANKH
            )) {
            resurrectPower_5 *= ARTIFACT_POWER_MULTIPLIER;
        }
        if (resurrectPower_5 < m_monster.hitPoints) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
    }
    if (spell == CREATURE_SPELL_DISPEL && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLOODLUST)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHIELD)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STONESKIN)]
        && !m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
    }
    if (spell == SPELL_HYPNOTIZE) {
        hypnotizeHitPoints_37 = gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)]->Stats(
                                    HERO_PRIMARY_SPELL_POWER
                                )
                                * HYPNOTIZE_HIT_POINTS_PER_POWER;
        if (gpCombatManager->m_heroes[H2EnumIndex(gpCombatManager->m_currentSide)]->HasArtifact(
                ARTIFACT_GOLD_WATCH
            )) {
            hypnotizeHitPoints_37 *= ARTIFACT_POWER_MULTIPLIER;
        }
        if (m_monster.hitPoints * m_quantity > hypnotizeHitPoints_37) {
            return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_NONE);
        }
    }
    if ((m_monsterType == CREATURE_DWARF || m_monsterType == CREATURE_BATTLE_DWARF)
        && spell != SPELL_TELEPORT && spell != SPELL_CURE && spell != SPELL_MASS_CURE
        && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT && spell != SPELL_HASTE
        && spell != SPELL_MASS_HASTE && spell != SPELL_BLESS && spell != SPELL_MASS_BLESS
        && spell != SPELL_STONE_SKIN && spell != SPELL_STEEL_SKIN && spell != SPELL_ANTI_MAGIC
        && spell != SPELL_DRAGON_SLAYER && spell != SPELL_BLOOD_LUST && spell != SPELL_MIRROR_IMAGE
        && spell != SPELL_SHIELD && spell != SPELL_MASS_SHIELD
        && spell != SPELL_FORCE_SHIELD && spell != SPELL_MASS_FORCE_SHIELD) {
        return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_DWARF);
    }
    return ironfist::hooks::ModifySpellChance(this, spell, ARMY_SPELL_CHANCE_ALWAYS);
}

i32 army::SpellCastWorks(SpellType spell) {
    i32 chance;

    chance = static_cast<i32>(SpellCastWorkChance(spell) * ARMY_SPELL_CHANCE_PERCENT);
    return SRandom(1, RANDOM_SPELL_ROLL_MAX) <= chance;
}

static void AppendWalkAnimation(struct SMonFrameInfo* frameInfo, ArmyAnimationSequence source) {
    if (frameInfo->animationFrameCount[H2EnumIndex(source)] <= 0) {
        return;
    }
    memcpy(
        &frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)]
                                   [frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
        frameInfo->animationFrames[H2EnumIndex(source)],
        frameInfo->animationFrameCount[H2EnumIndex(source)]
    );
    memcpy(
        &frameInfo->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
        frameInfo->animationXOffsets[H2EnumIndex(source)],
        frameInfo->animationFrameCount[H2EnumIndex(source)]
    );
    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
        frameInfo->animationFrameCount[H2EnumIndex(source)];
}

// The teleporter's walk sequence: on a far move it winds up, blinks, and
// lands, skipping the middle frames entirely; a close move keeps them.
void BuildTeleporterTempWalkSeq(
    struct SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing, bool closeMove
) {
    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] = 0;
    if (!closeMove && skipDrawing) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_BEGIN_STANDING);
    }
    if (!closeMove && !skipDrawing && !finishStanding) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_BEGIN);
    }
    if (closeMove && (skipDrawing || !finishStanding)) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_MIDDLE);
    }
    if (!finishStanding) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_END);
    }
    if (!closeMove && skipDrawing && finishStanding) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_END_STANDING);
    }
    if (!skipDrawing && finishStanding) {
        AppendWalkAnimation(frameInfo, ARMY_ANIMATION_WALK_STAND);
    }
}

void BuildTempWalkSeq(struct SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing) {
    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] = 0;
    if (!skipDrawing && finishStanding) {
        if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)]
            );
            frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_STAND)];
        }
    } else {
        if (!skipDrawing) {
            if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)],
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)],
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)]
                );
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN)];
            }
        } else if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)]
            );
            frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
        }
        if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)]
            );
            frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_MIDDLE)];
        }
        if (finishStanding) {
            if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)][frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)],
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)],
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)]
                );
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END_STANDING)];
            }
        } else if (frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK_END)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[H2EnumIndex(ARMY_ANIMATION_WALK_END)],
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END)]
            );
            frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK_END)];
        }
    }
}

void army::DispelGood(void) {
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HASTE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLESS);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLOODLUST);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SHIELD);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_ANTI_MAGIC);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STONESKIN);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STEELSKIN);
}

void army::Cure(i32 amount) {
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_SLOW);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_CURSE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PARALYZE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
    CancelIndividualSpell(ARMY_SPELL_INFLUENCE_PETRIFIED);
    m_hitPointsLost -= amount * CURE_HIT_POINTS_PER_POWER;
    if (m_hitPointsLost < 0) {
        m_hitPointsLost = 0;
    }
}

i32 army::MidX(void) {
    return gpCombatManager->m_hexCells[m_hex].m_x
           + ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))
                  ? (m_facing == ARMY_FACING_RIGHT ? WIDE_CREATURE_HALF_WIDTH
                                                   : -WIDE_CREATURE_HALF_WIDTH)
                  : 0);
}

i32 army::MidY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - (GetIconEntry(
                  m_creatureIcon,
                  m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
              )
                  ->h
              >> 1);
}

i32 army::TopY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - GetIconEntry(m_creatureIcon, m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0])
                 ->h;
}

i32 army::RightX(void) {
    if (m_facing == ARMY_FACING_RIGHT) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + (GetIconEntry(
                      m_creatureIcon,
                      m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
                  )
                      ->x
                  + GetIconEntry(
                        m_creatureIcon,
                        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
                  )
                        ->w);
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    }
}

i32 army::LeftX(void) {
    if (m_facing == ARMY_FACING_RIGHT) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - (GetIconEntry(
                      m_creatureIcon,
                      m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
                  )
                      ->x
                  + GetIconEntry(
                        m_creatureIcon,
                        m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][0]
                  )
                        ->w);
    }
}

i32 army::OtherArmyAdjacent(CombatSide side, i32 index) {
    army* otherArmy;
    i32 otherSquare;
    i32 otherRearSquare;
    i32 myRearSquare;
    i32 adjacentSquare;
    CombatHexDirection i;

    otherArmy = &gpCombatManager->m_armies[H2EnumIndex(side)][index];
    otherSquare = otherArmy->m_hex;
    if ((H2EnumIndex((otherArmy->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        otherRearSquare = otherSquare + (otherArmy->m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
    } else {
        otherRearSquare = -1;
    }
    for (i = COMBAT_DIRECTION_NORTHEAST; H2EnumIndex(i) < ARMY_ADJACENT_DIRECTION_COUNT; i++) {
        adjacentSquare = GetAdjacentCellIndex(m_hex, i);
        if (adjacentSquare == otherSquare
            || (adjacentSquare != -1 && adjacentSquare == otherRearSquare)) {
            return 1;
        }
    }
    if ((H2EnumIndex((m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
        myRearSquare = m_hex + (m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
        for (i = COMBAT_DIRECTION_NORTHEAST; H2EnumIndex(i) < ARMY_ADJACENT_DIRECTION_COUNT; i++) {
            adjacentSquare = GetAdjacentCellIndex(myRearSquare, i);
            if (adjacentSquare == otherSquare
                || (adjacentSquare != -1 && adjacentSquare == otherRearSquare)) {
                return 1;
            }
        }
    }
    return 0;
}

void ModifyFrameInfo(struct SMonFrameInfo* frameInfo, CreatureType monsterType) {
    i32 speedDifference;

    speedDifference = 0;
    if (monsterType == CREATURE_RANGER || monsterType == CREATURE_VETERAN_PIKEMAN
        || monsterType == CREATURE_MASTER_SWORDSMAN || monsterType == CREATURE_CHAMPION
        || monsterType == CREATURE_CRUSADER || monsterType == CREATURE_ORC_CHIEF
        || monsterType == CREATURE_OGRE_LORD || monsterType == CREATURE_WAR_TROLL
        || monsterType == CREATURE_BATTLE_DWARF || monsterType == CREATURE_GRAND_ELF
        || monsterType == CREATURE_GREATER_DRUID || monsterType == CREATURE_MINOTAUR_KING
        || monsterType == CREATURE_STEEL_GOLEM || monsterType == CREATURE_ARCHMAGE
        || monsterType == CREATURE_MUTANT_ZOMBIE || monsterType == CREATURE_ROYAL_MUMMY
        || monsterType == CREATURE_VAMPIRE_LORD || monsterType == CREATURE_POWER_LICH) {
        speedDifference =
            gMonsterDatabase[H2EnumIndex(monsterType)].speed - gMonsterDatabase[H2EnumIndex(monsterType) - 1].speed;
    }
    if (monsterType == CREATURE_EARTH_ELEMENTAL || monsterType == CREATURE_AIR_ELEMENTAL
        || monsterType == CREATURE_WATER_ELEMENTAL) {
        speedDifference = gMonsterDatabase[H2EnumIndex(monsterType)].speed
                          - gMonsterDatabase[H2EnumIndex(CREATURE_FIRE_ELEMENTAL)].speed;
    }
    if (speedDifference) {
        if (monsterType == CREATURE_RANGER) {
            frameInfo->attackDuration =
                static_cast<i32>(frameInfo->attackDuration * ARMY_RANGER_ATTACK_DURATION_SCALE);
        } else {
            frameInfo->attackDuration = static_cast<i32>(
                frameInfo->attackDuration
                * (ARMY_DURATION_BASE_SCALE - speedDifference * ARMY_ATTACK_DURATION_SPEED_SCALE)
            );
        }
        frameInfo->walkDuration = static_cast<i32>(
            frameInfo->walkDuration
            * (ARMY_DURATION_BASE_SCALE - speedDifference * ARMY_WALK_DURATION_SPEED_SCALE)
        );
    }
}

i32 army::GetPowBaseY(void) {
    i32 y;

    y = MidY();
    if (gCurLoadedSpellEffect == COMBAT_EFFECT_HYPNOTIZE
        || gCurLoadedSpellEffect == COMBAT_EFFECT_PARALYZE
        || gCurLoadedSpellEffect == COMBAT_EFFECT_BLESS
        || gCurLoadedSpellEffect == COMBAT_EFFECT_CURSE
        || gCurLoadedSpellEffect == COMBAT_EFFECT_DRAGON_SLAYER
        || gCurLoadedSpellEffect == COMBAT_EFFECT_BERSERK) {
        y = TopY();
    }
    if (gCurLoadedSpellEffect == COMBAT_EFFECT_STONE_SKIN
        || gCurLoadedSpellEffect == COMBAT_EFFECT_STEEL_SKIN) {
        y = gpCombatManager->m_hexCells[m_hex].m_y + CONTROL_EFFECT_Y_OFFSET;
    }
    return y;
}

i32 bSecondAttack = 0;
b32 gbGenieHalf;
