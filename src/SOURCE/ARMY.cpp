#include <va.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
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
#include <SOURCE/kbwin.h>
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

namespace {

H2_ENUM_BEGIN(ArmySpellStatConstant)
    HASTE_SPEED_BONUS       = 2,
    BLOODLUST_ATTACK_BONUS  = 3,
    STONESKIN_DEFENSE_BONUS = 3,
    STEELSKIN_DEFENSE_BONUS = 5
H2_ENUM_END(ArmySpellStatConstant)

H2_ENUM_BEGIN(ArmyDeathConstant)
    CORPSE_LIMIT           = 14,
    DEATH_RANDOM_MAX       = 100,
    DEATH_PRIMARY_CHANCE   = 60,
    DEATH_SECONDARY_CHANCE = 80
H2_ENUM_END(ArmyDeathConstant)

H2_ENUM_BEGIN(SpellEffectConstant)
    NO_POW_EFFECT_COORDINATE = -1,
    EFFECT_MINIMUM_Y         = 999
H2_ENUM_END(SpellEffectConstant)

H2_ENUM_BEGIN(ArmySpellChanceConstant)
    RANDOM_SPELL_ROLL_MAX           = 99,
    RESURRECT_POWER_PER_SPELL_POWER = 50,
    HYPNOTIZE_HIT_POINTS_PER_POWER  = 25,
    ARTIFACT_POWER_MULTIPLIER       = 2,
    CURE_HIT_POINTS_PER_POWER       = 5,
    WIDE_CREATURE_HALF_WIDTH        = 22,
    CONTROL_EFFECT_Y_OFFSET         = 5
H2_ENUM_END(ArmySpellChanceConstant)

H2_ENUM_BEGIN(InitializationConstant)
    INITIAL_UNKNOWN_D4 = 6
H2_ENUM_END(InitializationConstant)

H2_ENUM_BEGIN(DrawingConstant)
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
H2_ENUM_END(DrawingConstant)

H2_ENUM_CLASS_BEGIN(ProjectileAttackDirection)
    PROJECTILE_ATTACK_UP      = 0,
    PROJECTILE_ATTACK_FORWARD = 1,
    PROJECTILE_ATTACK_DOWN    = 2
H2_ENUM_CLASS_END(ProjectileAttackDirection)

H2_ENUM_BEGIN(ProjectileConstant)
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
    LICH_SPLASH_DIRECTION_COUNT      = IDX(COMBAT_DIRECTION_WIDE_WEST) + 1,
    LICH_SPLASH_CENTER_DIRECTION     = IDX(COMBAT_DIRECTION_WIDE_WEST)
H2_ENUM_END(ProjectileConstant)

H2_ENUM_BEGIN(DamageConstant)
    GENIE_QUANTITY_DIVISOR = 2,
    DAMAGE_DISPLAY_DIVISOR = 5,
    SLOW_SPEED_DIVISOR     = 2
H2_ENUM_END(DamageConstant)

H2_ENUM_CLASS_BEGIN(BerserkMaskIndex)
    BERSERK_MASK_SIDE_ZERO    = 0,
    BERSERK_MASK_SIDE_ONE     = 1,
    BERSERK_MASK_ATTACK       = 2,
    BERSERK_MASK_UNUSED       = 3,
    BERSERK_MASK_TARGET_FOUND = 4,
    BERSERK_MASK_COUNT        = 5
H2_ENUM_CLASS_END(BerserkMaskIndex)

H2_ENUM_BEGIN(Cp1251Letter)
    CP1251_CAPITAL_YO = 0xa8,
    CP1251_SMALL_YO   = 0xb8,
    CP1251_CAPITAL_A  = 0xc0,
    CP1251_CAPITAL_YA = 0xdf,
    CP1251_SMALL_A    = 0xe0,
    CP1251_SMALL_YA   = 0xff,
    TARGET_NAME_SIZE  = 100
H2_ENUM_END(Cp1251Letter)

#define PROJECTILE_HALF_TURN_DEGREES_FLOAT 180.0
#define PROJECTILE_DIRECTION_MIDPOINT_DIVISOR 2.0f
#define DAMAGE_DOUBLE_MULTIPLIER 2.0f
#define DAMAGE_HALF_DIVISOR 2.0f
#define DAMAGE_ROUNDING_OFFSET 0.5

// The localised build case-shifts the leading letter of a combat message
// through the CP1251 alphabet, not through a bare +/- 32.
inline char ToLowerCp1251(u8 letter) {
    if (letter >= 'A' && letter <= 'Z') {
        return static_cast<char>(letter + ARMY_ASCII_CASE_OFFSET);
    }
    if (letter >= CP1251_CAPITAL_A && letter <= CP1251_CAPITAL_YA) {
        return static_cast<char>(letter + ARMY_ASCII_CASE_OFFSET);
    }
    if (letter == CP1251_CAPITAL_YO) {
        return static_cast<char>(CP1251_SMALL_YO);
    }
    return static_cast<char>(letter);
}

inline char ToUpperCp1251(u8 letter) {
    if (letter >= 'a' && letter <= 'z') {
        return static_cast<char>(letter - ARMY_ASCII_CASE_OFFSET);
    }
    if (letter >= CP1251_SMALL_A && letter <= CP1251_SMALL_YA) {
        return static_cast<char>(letter - ARMY_ASCII_CASE_OFFSET);
    }
    if (letter == CP1251_SMALL_YO) {
        return static_cast<char>(CP1251_CAPITAL_YO);
    }
    return static_cast<char>(letter);
}

}

// Scratch copy of the defending creature's name, case-folded for the middle of
// a combat sentence. File-static rather than unnamed-namespace: VC6 decorates
// an unnamed-namespace object with the absolute source path plus a per-
// compilation cookie, so the claim could never name one stable symbol.
DATA(0x00524038) static char gTargetName[TARGET_NAME_SIZE];

VA(0x00418650, 0xc2)
army::army(void) {
    H2_ENUM_STORAGE_STEPPED(ArmySampleType, i32) sampleType;

    m_creatureIcon = NULL;
    m_hex = 0;
    for (sampleType = ARMY_SAMPLE_MOVE; sampleType < ARMY_SAMPLE_COUNT; sampleType++) {
        m_samples[IDX(sampleType)] = NULL;
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

VA(0x00418712, 0xd)
void army::WaitSample(ArmySampleType sampleIndex) {
    return;
}

VA(0x0041871f, 0xd2)
void army::InitClean(void) {
    H2_ENUM_STORAGE_STEPPED(ArmySampleType, i32) sampleType;

    for (sampleType = ARMY_SAMPLE_MOVE; sampleType < ARMY_SAMPLE_COUNT; sampleType++) {
        m_samples[IDX(sampleType)] = NULL;
    }
    m_roundCounter = -1;
    m_spellCount = 0;
    memset(m_spellInfluence, 0, sizeof(m_spellInfluence));
    m_lastAnimationTime = KBTickCount();
    m_drawEnabled = 1;
    m_creatureIcon = NULL;
    m_drawSpellEffect = 0;
    m_spellEffect = SPELL_NONE;
    m_mirrorSourceIndex = -1;
    m_mirrorImageIndex = -1;
    m_armyGroupSlot = -1;
    m_lastTargetHex = -1;
}

VA(0x004187f1, 0x38c)
void army::Init(
    CreatureType monsterType, i32 quantity, H2_ENUM_PARAM(CombatSide, i32) side, i32 index, i32 hex, i32 unknown
) {
    hero* commander;
    i32 rearHex;

    InitClean();
    m_monsterType = monsterType;
    m_drawState = ARMY_DRAW_NORMAL;
    m_monster = gMonsterDatabase[IDX(monsterType)];
    m_unknownD4 = INITIAL_UNKNOWN_D4;
    commander = gpCombatManager->m_heroes[IDX(side)];
    if (commander) {
        m_monster.attack += commander->Stats(HERO_PRIMARY_ATTACK);
        m_monster.defense += commander->Stats(HERO_PRIMARY_DEFENSE);
    }
#if H2_STRICT_ENUMS
    m_facing = side == COMBAT_ATTACKER_SIDE ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
#else
    m_facing = static_cast<ArmyFacing>(static_cast<i32>(side) ^ ARMY_FACING_RIGHT);
#endif
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
    m_morale = gpCombatManager->m_armyGroups[IDX(m_side)]->GetMorale(
        gpCombatManager->m_heroes[IDX(m_side)],
        gpCombatManager->m_combatTowns[IDX(m_side)],
        gpCombatManager->m_armyGroups[IDX(OppositeCombatSide(m_side))]
    );
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
        || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
        || HAS(m_monster.flags.all, MONSTER_FLAGS_NO_MORALE)) {
        m_morale = 0;
    }
    m_luck = gpGame->GetLuck(
        gpCombatManager->m_heroes[IDX(m_side)],
        this,
        gpCombatManager->m_combatTowns[IDX(m_side)]
    );
    m_hex = hex;
    gpCombatManager->m_hexCells[m_hex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[m_hex].m_occupantIndex = m_index;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        rearHex = m_hex + (m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
        gpCombatManager->m_hexCells[rearHex].m_occupantSide = m_side;
        gpCombatManager->m_hexCells[rearHex].m_occupantIndex = m_index;
        gpCombatManager->m_hexCells[rearHex].m_occupantFrame =
            rearHex >= m_hex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
        gpCombatManager->m_hexCells[m_hex].m_occupantFrame =
            rearHex < m_hex ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
    }
    m_armyGroupSlot = unknown;
}

VA(0x00418b7d, 0x44d)
void army::LoadResources(void) {
    i32 i;

    if (gbNoShowCombat) {
        return;
    }

    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[IDX(m_monsterType)], 1)
    );
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(&m_frameInfo), sizeof(m_frameInfo));
    ModifyFrameInfo(&m_frameInfo, m_monsterType);
    m_walkDuration = m_frameInfo.walkDuration;

    sprintf(gText, "%smove.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_MOVE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%sattk.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_ATTACK)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%swnce.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_WINCE)] = gpResourceManager->GetSample(gText);
    sprintf(gText, "%skill.82M", m_monster.spriteName);
    m_samples[IDX(ARMY_SAMPLE_KILL)] = gpResourceManager->GetSample(gText);

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)) {
        sprintf(gText, "%sshot.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_SHOT)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_VAMPIRE || m_monsterType == CREATURE_VAMPIRE_LORD) {
        sprintf(gText, "%sext1.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
        sprintf(gText, "%sext2.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_TWO)] = gpResourceManager->GetSample(gText);
    }
    if (m_monsterType == CREATURE_LICH || m_monsterType == CREATURE_POWER_LICH) {
        sprintf(gText, "%sexpl.82M", m_monster.spriteName);
        m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)] = gpResourceManager->GetSample(gText);
    }

    m_creatureIcon = gpResourceManager->GetIcon(cMonFilename[IDX(m_monsterType)]);
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)) {
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
        m_samples[IDX(ARMY_SAMPLE_SHOT)] = NULL;
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

VA(0x00418fca, 0xcb)
void army::FreeResources(void) {
    H2_ENUM_STORAGE_STEPPED(ArmySampleType, i32) sampleType;

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
        if (m_samples[IDX(sampleType)]) {
            gpResourceManager->Dispose(m_samples[IDX(sampleType)]);
            m_samples[IDX(sampleType)] = NULL;
        }
    }
}

VA(0x00419095, 0x6fb)
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
    H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) sp;
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
        && !HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        numFrames = m_frameInfo.animationFrameCount[IDX(m_animationSequence)];
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
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        palette = gColorTableGray;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_RED_PALETTE)) {
        palette = gColorTableRed;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_DARK_BROWN_PALETTE)) {
        palette = gColorTableDarkBrown;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_GRAY_PALETTE)) {
        palette = gColorTableGray;
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE)) {
        palette = gColorTableLighten;
    }
    if (effectsOnly == 0) {
        m_creatureIcon->CombatClipDrawToBuffer(
            x,
            y,
            m_frameInfo.animationFrames[IDX(m_animationSequence)][m_animationFrame],
            &m_spriteLimits,
            m_facing == ARMY_FACING_LEFT ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
            color,
            palette,
            m_palette
        );
    }

    if (idle && gpCombatManager->m_showArmyQuantities && m_showQuantity) {
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
        qtyOffset = m_frameInfo.quantityX[IDX(OppositeArmyFacing(m_facing))];
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
            if (m_spellInfluence[IDX(sp)]) {
                switch (sp) {
                    case ARMY_SPELL_INFLUENCE_HASTE:
                    case ARMY_SPELL_INFLUENCE_BLESS:
                    case ARMY_SPELL_INFLUENCE_DRAGON_SLAYER:
                    case ARMY_SPELL_INFLUENCE_BLOODLUST:
                    case ARMY_SPELL_INFLUENCE_SHIELD:
                    case ARMY_SPELL_INFLUENCE_ANTI_MAGIC:
                    case ARMY_SPELL_INFLUENCE_STONESKIN:
                    case ARMY_SPELL_INFLUENCE_STEELSKIN:
                        goodEffects++;
                        break;
                    default:
                        badSpells++;
                }
            }
        }
        if (giSpellEffectShowType == SPELL_EFFECT_DISPLAY_HIGHLIGHT || !idle
            || m_spellCount <= 0) {
            drawn = gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
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
            drawn = gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
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
            drawn = gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_STATUS)]->CombatClipDrawToBuffer(
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
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
            static_cast<IconDrawOrientation>(IDX(ICON_DRAW_FLIPPED) - IDX(m_facing)),
            0,
            NULL,
            NULL
        );
    }
}

VA(0x00419790, 0x22)
void army::Wince(void) {
    m_animationSequence = ARMY_ANIMATION_WINCE;
    m_animationFrame = 0;
}

VA(0x004197b2, 0xa27)
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
    if (m_side == COMBAT_DEFENDER_SIDE && gpCombatManager->m_inCastleCombat
        && (destHex == COMBAT_CASTLE_GATE_APPROACH_HEX
            || destHex == IDX(COMBAT_CASTLE_HEX_GATE)
            || (destHex == DRAWBRIDGE_WIDE_EXIT_HEX && m_side == COMBAT_DEFENDER_SIDE
                && HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)))
        && gpCombatManager->m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
        m_animationSequence = ARMY_ANIMATION_STAND;
        m_animationFrame = 0;
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        gpCombatManager->LowerDoor();
        skipDrawing = 0;
    }

    giWalkingFrom = m_hex;
    giWalkingFrom2 = HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
                         ? m_hex + ArmyFacingRearHexOffset(m_facing)
                         : -1;
    giWalkingTo = destHex;
    giWalkingTo2 = HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
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
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                m_hex--;
            }
        }
    } else if (m_facing == ARMY_FACING_RIGHT) {
        m_facingChanged = 1;
        m_facing = OppositeArmyFacing(m_facing);
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_MOVE)]);
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
        for (frame = 0; frame < m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
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
                KBTickCount()
                + m_frameInfo.walkDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                      / m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
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
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        otherHex = m_hex + ArmyFacingRearHexOffset(m_facing);
        gpCombatManager->m_hexCells[otherHex].m_occupantIndex = -1;
        gpCombatManager->m_hexCells[otherHex].m_occupantSide = COMBAT_SIDE_NONE;
        gpCombatManager->m_hexCells[otherHex].m_occupantFrame = ARMY_FACING_NONE;
    }
    gpCombatManager->m_hexCells[newHex].m_occupantSide = m_side;
    gpCombatManager->m_hexCells[newHex].m_occupantIndex = m_index;
    gpCombatManager->m_hexCells[newHex].m_occupantFrame = ARMY_FACING_NONE;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
}

VA(0x0041a1d9, 0x165f)
void army::SpecialAttack(void) {
    i32 xCentre;
    i32 frameIndex;
    i32 oldTipX;
    i32 anchorY;
    char originalColumn;
    H2_ENUM_STORAGE(IconDrawOrientation, char) bIconFlip;
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
    pEnemy = m_targetIndex + gpCombatManager->m_armies[IDX(m_targetSide)];
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
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (m_facing == ARMY_FACING_RIGHT) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }

    CheckLuck();
    gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_SHOT)]);
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[IDX(m_side)][m_index]++;
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
        /* The parenthesised divisor cast keeps both operands on the x87 stack;
           without it VC6 folds the divisor into a single `fidiv`. */
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
        animSlot = IDX(PROJECTILE_ATTACK_UP);
    } else if (fShotAngle > ARMY_SHOOT_FORWARD_MIN_ANGLE) {
        m_animationSequence = ARMY_ANIMATION_SHOOT_FORWARD;
        animSlot = IDX(PROJECTILE_ATTACK_FORWARD);
    } else {
        m_animationSequence = ARMY_ANIMATION_SHOOT_DOWN;
        animSlot = IDX(PROJECTILE_ATTACK_DOWN);
    }
    for (m_animationFrame = 0;
         m_animationFrame < m_frameInfo.animationFrameCount[IDX(m_animationSequence)];
         m_animationFrame++) {
        if (m_animationFrame == m_frameInfo.animationFrameCount[IDX(m_animationSequence)] - 1) {
            gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        } else {
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + m_frameInfo.attackDuration * gfCombatSpeedMod[gConfig.combatSpeed]
                  / m_frameInfo.animationFrameCount[IDX(m_animationSequence)]
        );
    }
    m_animationFrame = m_frameInfo.animationFrameCount[IDX(m_animationSequence)] - 1;

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
                IDX(ARMY_MAGE_BOLT_DELAY)
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
                KBTickCount() + shotDelay * gfCombatSpeedMod[gConfig.combatSpeed]
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
    if (!gpCombatManager->m_heroes[IDX(m_side)]
        || !gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_AMMO_CART)) {
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
                         ->m_armies[IDX(gpCombatManager->m_hexCells[adjacentHex].m_occupantSide)]
                                   [gpCombatManager->m_hexCells[adjacentHex].m_occupantIndex];
                if (!gArmyEffected[IDX(splashTarget->m_side)][splashTarget->m_index]
                    && (splashTarget != pEnemy || k == LICH_SPLASH_CENTER_DIRECTION)) {
                    gArmyEffected[IDX(splashTarget->m_side)][splashTarget->m_index] = 1;
                    DamageEnemy(splashTarget, &damageDone, &killed, 1, 0);
                }
            }
        }
        m_spellEffectYOffset = 0;
        powVal = COMBAT_EFFECT_LICH_CLOUD;
        castX = gpCombatManager->m_hexCells[adjacentHex].m_x;
        castY = gpCombatManager->m_hexCells[adjacentHex].m_y - PROJECTILE_TARGET_Y_OFFSET;
        gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_EXTRA_ONE)]);
    } else {
        DamageEnemy(pEnemy, &damageDone, &killed, 1, 0);
    }
    m_monster.attack = baseAtk;

    if (killed > 0) {
        if (damageDone == -1) {
            sprintf(gText, "\xd4\xe0\xed\xf2\xee\xec \xe2\xee\xe8\xed\xe0 \xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed!!");
        } else {
            strcpy(gTargetName, gArmyNames[IDX(pEnemy->m_monsterType)]);
            gTargetName[0] = ToLowerCp1251(gTargetName[0]);
            sprintf(
                gText,
                "%s %s %s %d %s.\n%d %s %s.",
                "\xc0\xf2\xe0\xea\xe0",
                gArmyNamesPlural[IDX(m_monsterType)],
                "\xed\xe0\xed\xee\xf1\xe8\xf2",
                damageDone,
                "\xe5\xe4. \xf3\xf0\xee\xed\xe0",
                killed,
                killed <= 1 ? gTargetName : gArmyNamesPlural[IDX(pEnemy->m_monsterType)],
                killed <= 1 ? "\xf3\xec\xe8\xf0\xe0\xe5\xf2" : "\xf3\xe1\xe8\xf2\xee"
            );
            gText[0] = ToUpperCp1251(gText[0]);
        }
    } else {
        sprintf(
            gText,
            "%s %s %s %d %s.",
            "\xc0\xf2\xe0\xea\xe0",
            gArmyNamesPlural[IDX(m_monsterType)],
            "\xed\xe0\xed\xee\xf1\xe8\xf2",
            damageDone,
            "\xe5\xe4. \xf3\xf0\xee\xed\xe0"
        );
        gText[0] = ToUpperCp1251(gText[0]);
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
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
        || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
}

VA(0x0041b838, 0x20)
void army::DirDoAttack(CombatHexDirection direction) {
    m_attackDirection = direction;
    DoAttack(0);
}

VA(0x0041b858, 0x583)
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
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
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
    gpCombatManager->m_limitCreatureCount[IDX(m_side)][m_index]++;
    for (dir = COMBAT_DIRECTION_NORTHEAST;
         IDX(dir) < ARMY_COMBAT_DIRECTION_COUNT;
         dir++) {
        if (!(attackMask & BIT(dir))) {
            hitHex = m_hex;
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
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
                if (occupantSide >= 0 && armyIndex >= 0) {
                    gpCombatManager->m_limitCreatureCount[IDX(occupantSide)][armyIndex]++;
                    pTarget = &gpCombatManager->m_armies[IDX(occupantSide)][armyIndex];
                    if (!pTarget->m_hitByCreature) {
                        pTarget->m_hitByCreature = 1;
                        DamageEnemy(pTarget, &damage, &killedNow, 0, 0);
                        totDamage += damage;
                        totKilled += killedNow;
                        gpCombatManager->m_limitCreatureCount[IDX(occupantSide)][armyIndex]++;
                    }
                }
            }
        }
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    m_animationState = 1;
    m_pendingAnimationSequence = ARMY_ANIMATION_ATTACK_FORWARD;
    gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_ATTACK)]);
    if (totKilled > 0) {
        sprintf(
            gText,
            "%s %s %s %d %s.\n%d %s %s.",
            "\xc0\xf2\xe0\xea\xe0",
            gArmyNamesPlural[IDX(m_monsterType)],
            "\xed\xe0\xed\xee\xf1\xe8\xf2",
            totDamage,
            "\xe5\xe4. \xf3\xf0\xee\xed\xe0",
            totKilled,
            totKilled <= 1 ? "\xe2\xee\xe8\xed" : "\xe2\xee\xe8\xed\xee\xe2",
            totKilled <= 1 ? "\xf3\xec\xe8\xf0\xe0\xe5\xf2" : "\xf3\xe1\xe8\xf2\xee"
        );
    } else {
        sprintf(
            gText,
            "%s %s %s %d %s.",
            "\xc0\xf2\xe0\xea\xe0",
            gArmyNamesPlural[IDX(m_monsterType)],
            "\xed\xe0\xed\xee\xf1\xe8\xf2",
            totDamage,
            "\xe5\xe4. \xf3\xf0\xee\xed\xe0"
        );
    }
    gText[0] = ToUpperCp1251(gText[0]);
    strcpy(textBuf, gText);
    PowEffect(COMBAT_EFFECT_INVALID, 0, -1, -1);
    gpCombatManager->CombatMessage(textBuf, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[IDX(m_side)][m_index] = 1;
}

VA(0x0041bddb, 0x1445)
void army::DoAttack(i32 retaliation) {
    ArmyFacing targetOriginalFacing_6;
    CombatHexDirection originalDirection_13;
    army* breathTarget_6;
    CombatHexDirection secondAttackDirection_18;
    i32 breathDamage_12;
    ArmyFacing desiredFacing_2;
    i32 effectStopsRetaliation_4;
    ArmyFacing originalFacing_7;
    CombatSide occupantSide_5;
    char combatText_10[ARMY_COMBAT_TEXT_SIZE];
    i32 damage_4;
    i32 targetHex_3;
    i32 killed_1;
    army* target_18;
    i32 breathKilled;
    i32 adjacentHex_1;
    i32 revivedQuantity_3;
    i32 breathHex_1;

    m_drawState = ARMY_DRAW_IN_FRONT;
    damage_4 = 0;
    killed_1 = 0;
    breathDamage_12 = 0;
    breathKilled = 0;
    effectStopsRetaliation_4 = 0;
    target_18 = NULL;
    breathTarget_6 = NULL;
    originalFacing_7 = m_facing;
    if (retaliation) {
        gpCombatManager->m_currentSide = OppositeCombatSide(gpCombatManager->m_currentSide);
    }
    if (m_monsterType == CREATURE_HYDRA) {
        DoHydraAttack(retaliation);
        goto attackDone;
    }
    originalDirection_13 = m_attackDirection;
    targetHex_3 = m_hex;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
        && ((m_facing == ARMY_FACING_LEFT
             && m_attackDirection >= COMBAT_DIRECTION_SOUTHWEST)
            || (m_facing == ARMY_FACING_RIGHT
                && (m_attackDirection <= COMBAT_DIRECTION_SOUTHEAST
                    || m_attackDirection >= COMBAT_DIRECTION_WIDE_WEST)))) {
        if (originalFacing_7 == ARMY_FACING_LEFT) {
            targetHex_3 = m_hex - 1;
        } else {
            targetHex_3 = m_hex + 1;
        }
    }
    targetHex_3 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
    target_18 =
        &gpCombatManager->m_armies[IDX(gpCombatManager->m_hexCells[targetHex_3].m_occupantSide)]
                                  [gpCombatManager->m_hexCells[targetHex_3].m_occupantIndex];
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_BREATH_ATTACK)) {
        breathHex_1 = GetAdjacentCellIndex(targetHex_3, m_attackDirection);
        if (ValidHex(breathHex_1)
            && gpCombatManager->m_hexCells[breathHex_1].m_occupantSide >= 0
            && gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex >= 0
            && (gpCombatManager->m_hexCells[breathHex_1].m_occupantSide != target_18->m_side
                || gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex
                       != target_18->m_index)) {
            breathTarget_6 =
                &gpCombatManager
                     ->m_armies[IDX(gpCombatManager->m_hexCells[breathHex_1].m_occupantSide)]
                               [gpCombatManager->m_hexCells[breathHex_1].m_occupantIndex];
        }
    }
    gpCombatManager->ResetLimitCreature();
    gpCombatManager->m_limitCreatureCount[IDX(m_side)][m_index]++;
    gpCombatManager->m_limitCreatureCount[IDX(target_18->m_side)][target_18->m_index]++;
    if (breathTarget_6) {
        gpCombatManager
            ->m_limitCreatureCount[IDX(breathTarget_6->m_side)][breathTarget_6->m_index]++;
    }
    gpCombatManager->DrawFrame(0, 1, 0, 1, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    targetOriginalFacing_6 = target_18->m_facing;
    if (m_attackDirection <= COMBAT_DIRECTION_SOUTHEAST) {
        desiredFacing_2 = ARMY_FACING_RIGHT;
    } else if (m_attackDirection <= COMBAT_DIRECTION_NORTHWEST) {
        desiredFacing_2 = ARMY_FACING_LEFT;
    } else {
        desiredFacing_2 = m_facing;
    }
    if (m_facing != desiredFacing_2) {
        m_facing = desiredFacing_2;
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (desiredFacing_2 == ARMY_FACING_RIGHT) {
                m_hex--;
            } else {
                m_hex++;
            }
        }
        target_18->m_facing = OppositeArmyFacing(m_facing);
        if (targetOriginalFacing_6 != target_18->m_facing
            && HAS(target_18->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            if (target_18->m_facing == ARMY_FACING_RIGHT) {
                target_18->m_hex--;
            } else {
                target_18->m_hex++;
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
    gpSoundManager->MemorySample(m_samples[IDX(ARMY_SAMPLE_ATTACK)]);
    DamageEnemy(target_18, &damage_4, &killed_1, 0, 0);
    if (breathTarget_6) {
        DamageEnemy(breathTarget_6, &breathDamage_12, &breathKilled, 0, 0);
    }
    if (damage_4 == -1) {
        sprintf(gText, "\xd4\xe0\xed\xf2\xee\xec \xe2\xee\xe8\xed\xe0 \xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed!!");
    } else if (gbGenieHalf) {
        sprintf(
            gText,
            "%s %s \xef\xee\xeb\xee\xe2\xe8\xed\xf3 \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xf5 \xe2\xee\xe9\xf1\xea!",
            m_quantity <= 1 ? gArmyNames[IDX(m_monsterType)]
                            : gArmyNamesPlural[IDX(m_monsterType)],
            m_quantity <= 1 ? "\xf3\xed\xe8\xf7\xf2\xee\xe6\xe0\xfe\xf2"
                            : "\xf3\xed\xe8\xf7\xf2\xee\xe6\xe0\xe5\xf2"
        );
        gText[0] = ToUpperCp1251(gText[0]);
    } else {
        if (killed_1 > 0) {
            strcpy(gTargetName, gArmyNames[IDX(target_18->m_monsterType)]);
            gTargetName[0] = ToLowerCp1251(gTargetName[0]);
            sprintf(
                gText,
                "%s %s %s %d %s.\n%d %s %s.",
                "\xc0\xf2\xe0\xea\xe0",
                gArmyNamesPlural[IDX(m_monsterType)],
                "\xed\xe0\xed\xee\xf1\xe8\xf2",
                damage_4,
                "\xe5\xe4. \xf3\xf0\xee\xed\xe0",
                killed_1,
                killed_1 <= 1 ? gTargetName
                               : gArmyNamesPlural[IDX(target_18->m_monsterType)],
                killed_1 <= 1 ? "\xf3\xec\xe8\xf0\xe0\xe5\xf2" : "\xf3\xe1\xe8\xf2\xee"
            );
            gText[0] = ToUpperCp1251(gText[0]);
        } else {
            sprintf(
                gText,
                "%s %s %s %d %s.",
                "\xc0\xf2\xe0\xea\xe0",
                gArmyNamesPlural[IDX(m_monsterType)],
                "\xed\xe0\xed\xee\xf1\xe8\xf2",
                damage_4,
                "\xe5\xe4. \xf3\xf0\xee\xed\xe0"
            );
            gText[0] = ToUpperCp1251(gText[0]);
        }
    }
    strcpy(combatText_10, gText);

    switch (m_monsterType) {
        case CREATURE_CYCLOPS:
            if (target_18->m_quantity <= 0
                || (breathTarget_6 && breathTarget_6->m_quantity <= 0)) {
                break;
            }
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE) {
                if (target_18 && target_18->SpellCastWorks(SPELL_PARALYZE)) {
                    target_18->m_spellEffect = SPELL_PARALYZE;
                    effectStopsRetaliation_4 = 1;
                }
            } else if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE
                       && breathTarget_6
                       && breathTarget_6->SpellCastWorks(SPELL_PARALYZE)) {
                breathTarget_6->m_spellEffect = SPELL_PARALYZE;
            }
            break;
        case CREATURE_UNICORN:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_18
                && target_18->SpellCastWorks(SPELL_BLIND)) {
                target_18->m_spellEffect = SPELL_BLIND;
                effectStopsRetaliation_4 = 1;
            }
            break;
        case CREATURE_MEDUSA:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_18
                && target_18->SpellCastWorks(CREATURE_SPELL_PETRIFY)) {
                target_18->m_spellEffect = CREATURE_SPELL_PETRIFY;
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
                if (target_18 && target_18->SpellCastWorks(SPELL_CURSE)) {
                    target_18->m_spellEffect = SPELL_CURSE;
                }
            }
            break;
        case CREATURE_ARCHMAGE:
            if (SRandom(1, ARMY_PERCENT_MAX) < ARMY_ATTACK_EFFECT_CHANCE && target_18
                && target_18->SpellCastWorks(CREATURE_SPELL_DISPEL)) {
                target_18->m_spellEffect = CREATURE_SPELL_DISPEL;
            }
            break;
        case CREATURE_GHOST:
            gpCombatManager->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] =
                killed_1;
            break;
        case CREATURE_VAMPIRE_LORD:
            gpCombatManager->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] =
                killed_1 * target_18->m_monster.hitPoints;
            break;
    }
    PowEffect(COMBAT_EFFECT_INVALID, 0, -1, -1);
    gpCombatManager->CombatMessage(combatText_10, 1, 1, 0);
    gpCombatManager->m_limitCreatureCount[IDX(m_side)][m_index] = 1;

    if (m_monsterType == CREATURE_GHOST) {
        m_quantity +=
            gpCombatManager->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)];
    }
    if (m_monsterType == CREATURE_VAMPIRE_LORD) {
        if (m_hitPointsLost
            > gpCombatManager->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)]) {
            m_hitPointsLost -=
                gpCombatManager
                    ->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)];
        } else {
            gpCombatManager->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)] -=
                m_hitPointsLost;
            m_hitPointsLost = 0;
            revivedQuantity_3 =
                gpCombatManager
                    ->m_killBenefit[IDX(gpCombatManager->m_hexCells[m_hex].m_occupantSide)]
                / m_monster.hitPoints;
            if (m_initialQuantity - m_quantity > revivedQuantity_3) {
                m_quantity += revivedQuantity_3;
            } else {
                m_quantity = m_initialQuantity;
            }
        }
    }

    if (target_18 && target_18->m_quantity > 0) {
        if (target_18->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
            || target_18->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
            || (target_18->m_monsterType != CREATURE_GRIFFIN
                && HAS(target_18->m_monster.flags.all, MONSTER_FLAGS_RETALIATED))
            || m_monsterType == CREATURE_ROGUE || m_monsterType == CREATURE_SPRITE
            || m_monsterType == CREATURE_VAMPIRE || m_monsterType == CREATURE_VAMPIRE_LORD
            || effectStopsRetaliation_4 || retaliation) {
            goto secondAttack;
        }
        DelayMilli(
            static_cast<i32l>(
                IDX(ARMY_RETALIATION_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            )
        );
        target_18->m_attackDirection = OppositeDirection(m_attackDirection);
        if (HAS(target_18->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            adjacentHex_1 = GetAdjacentCellIndex(
                target_18->m_hex,
                target_18->m_facing == ARMY_FACING_RIGHT
                    ? COMBAT_DIRECTION_NORTHEAST
                    : COMBAT_DIRECTION_NORTHWEST
            );
            if (adjacentHex_1 == m_hex) {
                target_18->m_attackDirection = COMBAT_DIRECTION_WIDE_WEST;
            }
            adjacentHex_1 = GetAdjacentCellIndex(
                target_18->m_hex,
                target_18->m_facing == ARMY_FACING_RIGHT
                    ? COMBAT_DIRECTION_SOUTHEAST
                    : COMBAT_DIRECTION_SOUTHWEST
            );
            if (adjacentHex_1 == m_hex) {
                target_18->m_attackDirection = COMBAT_DIRECTION_WIDE_EAST;
            }
        }
        target_18->DoAttack(1);
        target_18->m_monster.attributes |= MONSTER_ATTRIBUTE_RETALIATED;
        if (gbRemoteOn && gpCombatManager->m_networkArmyPresent[0]
            && gpCombatManager->m_networkArmyPresent[1]
            && target_18->m_monsterType == CREATURE_GHOST) {
            target_18->m_quantity +=
                gpCombatManager
                    ->m_killBenefit[IDX(gpCombatManager->m_hexCells[target_18->m_hex]
                                           .m_occupantSide)];
        }
    }
secondAttack:
    if ((m_monsterType == CREATURE_WOLF || m_monsterType == CREATURE_PALADIN
         || m_monsterType == CREATURE_CRUSADER)
        && target_18 && target_18->m_quantity > 0 && !retaliation
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] && m_quantity > 0) {
        DelayMilli(
            static_cast<i32l>(
                IDX(ARMY_SECOND_ATTACK_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            )
        );
        secondAttackDirection_18 = m_attackDirection;
        m_attackDirection = originalDirection_13;
        DoAttack(1);
        m_attackDirection = secondAttackDirection_18;
    }
    if (m_facing != originalFacing_7) {
        if (!HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD)) {
            m_facing = originalFacing_7;
            if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                if (originalFacing_7 == ARMY_FACING_RIGHT) {
                    m_hex--;
                } else {
                    m_hex++;
                }
            }
        }
        if (!HAS(target_18->m_monster.flags.all, MONSTER_FLAGS_DEAD)
            && targetOriginalFacing_6 != target_18->m_facing) {
            target_18->m_facing = targetOriginalFacing_6;
            if (HAS(target_18->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                if (target_18->m_facing == ARMY_FACING_RIGHT) {
                    target_18->m_hex--;
                } else {
                    target_18->m_hex++;
                }
            }
        }
    }
attackDone:
    if (!retaliation
        && (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
            || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])) {
        CancelSpellType(ArmySpellCancelType(1));
        gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    }
    targetHex_3 = ARMY_HEX_INVALID;
    m_targetSide = static_cast<CombatSide>(targetHex_3);
    if (retaliation) {
        gpCombatManager->m_currentSide = OppositeCombatSide(gpCombatManager->m_currentSide);
    }
}

VA(0x0041d220, 0xb)
void army::ResetPath(void) {
    return;
}

VA(0x0041d22b, 0x1a)
i32 army::WalkTo(void) {
    return WalkTo(m_moveTargetHex);
}

VA(0x0041d245, 0x2aa)
i32 army::WalkTo(i32 destination) {
    i32 direction_3;
    i32 steps;
    i32 moatFound;
    i32 moatIndex_1;
    i32 canEnterMoat_1;

    m_targetIndex = -1;
    m_targetSide = COMBAT_SIDE_NONE;
    if (gpCombatManager->m_drawbridgeBackgroundVisible
        && HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
            if ((moatIndex_1 > 0 && m_hex == moatCell[moatIndex_1 - 1])
                || (moatIndex_1 < ARMY_MOAT_CELL_COUNT - 1
                    && m_hex == moatCell[moatIndex_1 + 1])) {
                canEnterMoat_1 = 1;
            }
            for (direction_3 = IDX(COMBAT_DIRECTION_NORTHEAST);
                 direction_3 < ARMY_ADJACENT_DIRECTION_COUNT;
                 direction_3++) {
                if (GetAdjacentCellIndex(m_hex, static_cast<CombatHexDirection>(direction_3))
                    == moatCell[moatIndex_1]) {
                    canEnterMoat_1 = 1;
                }
            }
            if (m_side == COMBAT_ATTACKER_SIDE && m_hex > moatCell[m_hex / ARMY_HEX_COLUMNS]) {
                canEnterMoat_1 = 1;
            }
            if (m_side == COMBAT_DEFENDER_SIDE && m_hex < moatCell[m_hex / ARMY_HEX_COLUMNS]) {
                canEnterMoat_1 = 1;
            }
            if (!canEnterMoat_1) {
                destination += m_facing == ARMY_FACING_RIGHT ? -1 : 1;
            }
        }
    }
    if (!FindPath(m_hex, destination, m_monster.speed, 1, ARMY_PATH_ANY_TARGET_HEX)) {
        return ARMY_PATH_BLOCKED;
    }

    steps = 0;
    for (direction_3 = gpSearchArray->m_pathLength - 1; direction_3 >= 0; direction_3--) {
        Walk(
            static_cast<CombatHexDirection>(
                gpSearchArray->m_storage.path.directions[direction_3 + 1]
            ),
            0,
            direction_3 != gpSearchArray->m_pathLength - 1
        );
        steps++;
        if (steps >= m_monster.speed) {
            direction_3 = -1;
        }
    }
    CancelSpellType(ArmySpellCancelType(0));
    m_animationSequence = ARMY_ANIMATION_STAND;
    m_animationFrame = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    gpCombatManager->TestRaiseDoor();
    return 0;
}

VA(0x0041d4ef, 0x1a)
i32 army::AttackTo(void) {
    return AttackTo(m_moveTargetHex);
}

VA(0x0041d509, 0x1c8)
i32 army::AttackTo(i32 destination) {
    i32 stepCount;
    i32 pathIndex;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        if (m_hex != destination) {
            FlyTo(destination);
        }
        DoAttack(0);
        return 0;
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_BREATH_ATTACK) && m_moveTargetHex == m_hex) {
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
            pathIndex = 0;
            stepCount = 0;
            for (pathIndex = gpSearchArray->m_pathLength - 1; pathIndex != 0; pathIndex--) {
                stepCount++;
                Walk(
                    static_cast<CombatHexDirection>(
                        gpSearchArray->m_storage.path.directions[pathIndex + 1]
                    ),
                    pathIndex != 1 && stepCount < m_monster.speed ? 0 : 1,
                    pathIndex != gpSearchArray->m_pathLength - 1
                );
                if (stepCount >= m_monster.speed && pathIndex != 1) {
                    return ARMY_PATH_BLOCKED;
                }
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

VA(0x0041d6d1, 0x21d)
void army::CheckLuck(void) {
    SAMPLE2 luckSample;

    m_luckOutcome = 0;
    if (!gpCombatManager->m_heroes[IDX(m_side)]) {
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
                "\xcf\xeb\xee\xf5\xe0\xff \xf3\xe4\xe0\xf7\xe0 \xe1\xfb\xeb\xe0 "
                "\xed\xe8\xf1\xef\xee\xf1\xeb\xe0\xed\xe0 \xed\xe0 %s!",
                m_quantity <= 1 ? gArmyNames[IDX(m_monsterType)]
                                : gArmyNamesPlural[IDX(m_monsterType)]
            );
            gpCombatManager->CombatMessage(gText, 1, 1, 0);
            SpellEffect(COMBAT_EFFECT_BAD_LUCK, ARMY_BAD_LUCK_EFFECT_DELAY, 0);
        } else {
            sprintf(
                gText,
                "\xd3\xe4\xe0\xf7\xe0 \xed\xe0 \xf1\xf2\xee\xf0\xee\xed\xe5 "
                "\xee\xf2\xf0\xff\xe4\xe0 %s!",
                m_quantity <= 1 ? gArmyNames[IDX(m_monsterType)]
                                : gArmyNamesPlural[IDX(m_monsterType)]
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

VA(0x0041d8ee, 0x444)
void army::DamageEnemy(
    army* target,
    i32* damageResult,
    i32* killedResult,
    i32 rangedAttack,
    i32 defenseModifier
) {
    float baseDamage;
    i32 attackAdd;
    i32 defBonus;
    i32 diff;
    i32 genieHalfDamage;
    i32 rearHex;
    i32 count;
    i32 damageTotal;
    hero* commander;

    if (!target) {
        return;
    }
    baseDamage = 0;
    gbGenieHalf = false;
    for (count = 0; count < m_quantity; count++) {
        if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]) {
            baseDamage += m_monster.damageMax;
        } else if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_CURSE)]) {
            baseDamage += m_monster.damageMin;
        } else {
            baseDamage += SRandom(m_monster.damageMin, m_monster.damageMax);
        }
    }
    attackAdd = 0;
    defBonus = 0;
    diff = m_monster.attack + attackAdd
                        - (target->m_monster.defense + defBonus + defenseModifier);
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && (target->m_monsterType == CREATURE_GREEN_DRAGON
            || target->m_monsterType == CREATURE_RED_DRAGON
            || target->m_monsterType == CREATURE_BLACK_DRAGON
            || target->m_monsterType == CREATURE_BONE_DRAGON)) {
        diff += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (gpCombatManager->m_drawbridgeBackgroundVisible) {
        rearHex = -1;
        if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
            rearHex = target->m_hex + ArmyFacingRearHexOffset(target->m_facing);
        }
        for (count = 0; count < ARMY_MOAT_CELL_COUNT; count++) {
            if (target->m_hex == moatCell[count] || rearHex == moatCell[count]) {
                diff += ARMY_MOAT_ATTACK_BONUS;
            }
        }
    }
    if (diff > ARMY_DAMAGE_STAT_LIMIT) {
        diff = ARMY_DAMAGE_STAT_LIMIT;
    }
    if (diff < -ARMY_DAMAGE_STAT_LIMIT) {
        diff = -ARMY_DAMAGE_STAT_LIMIT;
    }
    baseDamage *= gfBattleStat[diff + ARMY_DAMAGE_STAT_LIMIT];
    if ((m_monsterType == CREATURE_CRUSADER
         && HAS(target->m_monster.flags.all, MONSTER_FLAGS_UNDEAD))
        || (m_monsterType == CREATURE_EARTH_ELEMENTAL
            && target->m_monsterType == CREATURE_AIR_ELEMENTAL)
        || (m_monsterType == CREATURE_AIR_ELEMENTAL
            && target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
        || (m_monsterType == CREATURE_WATER_ELEMENTAL
            && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
        || (m_monsterType == CREATURE_FIRE_ELEMENTAL
            && target->m_monsterType == CREATURE_WATER_ELEMENTAL)) {
        baseDamage *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (m_luckOutcome > 0) {
        baseDamage *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (m_luckOutcome < 0) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    m_luckOutcome = 0;
    if (rangedAttack && gpCombatManager->ShotIsThroughWall(m_side, m_hex, target->m_hex)) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    commander = gpCombatManager->m_heroes[IDX(m_side)];
    if (commander && rangedAttack) {
        baseDamage *=
            gfSSArcheryMod[IDX(commander->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])];
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER) && !rangedAttack
        && m_monsterType != CREATURE_TITAN && m_monsterType != CREATURE_MAGE
        && m_monsterType != CREATURE_ARCHMAGE) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    if (rangedAttack && target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)]) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    if (m_damagePenalty == ARMY_DAMAGE_PENALTY_HALF) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    m_damagePenalty = ARMY_DAMAGE_PENALTY_NONE;
    if (target->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        baseDamage /= DAMAGE_HALF_DIVISOR;
    }
    damageTotal = static_cast<i32>(baseDamage + DAMAGE_ROUNDING_OFFSET);
    if (m_monsterType == CREATURE_GENIE
        && SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieHalfDamage =
            ((target->m_quantity + 1) / GENIE_QUANTITY_DIVISOR) * target->m_monster.hitPoints;
        if (genieHalfDamage > damageTotal) {
            gbGenieHalf = true;
            damageTotal = genieHalfDamage;
        }
    }
    if (damageTotal <= 0) {
        damageTotal = 1;
    }
    if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
        damageTotal = -1;
    }
    *damageResult = damageTotal;
    *killedResult = target->Damage(damageTotal, SPELL_NONE);
}

VA(0x0041dd32, 0x21c)
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
                gpCombatManager->m_heroes[IDX(m_side)],
                gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)]
            );
        } else {
            gpCombatManager->ModifyDamageForArtifacts(
                &damage,
                spell,
                gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)],
                gpCombatManager->m_heroes[IDX(m_side)]
            );
        }
    }
    killed = damage / m_monster.hitPoints;
    m_hitPointsLost = damage % m_monster.hitPoints;
    quantityFifth = m_quantity / DAMAGE_DISPLAY_DIVISOR;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
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
        IDX(gpCombatManager
                ->m_armies[IDX(gpCombatManager->m_currentArmySide)]
                          [gpCombatManager->m_currentArmyIndex]
                .m_facing)
        ^ IDX(ARMY_FACING_RIGHT)
    );
    m_facing = oldFacing;
    CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    return killed;
}

VA(0x0041df4e, 0x11d7)
void army::PowEffect(
    H2_ENUM_PARAM(CombatEffectType, i32) effect,
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
        for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
                if (gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_drawSpellEffect) {
                    drawEffect = 1;
                }
            }
        }
    }
    if (!gbNoShowCombat && effect != COMBAT_EFFECT_INVALID && drawEffect
        && gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(gCombatFxNames[IDX(effect)]);
        gCurLoadedSpellEffect = effect;
    }
    if (drawEffect) {
        spellFrames = giNumPowFrames[IDX(gCurLoadedSpellEffect)];
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
            if (static_cast<u8>(current->m_animationState)) {
                leadFrames =
                    current->m_frameInfo.animationFrameCount[IDX(m_pendingAnimationSequence)];
                endFrameCount =
                    current->m_frameInfo
                        .animationFrameCount[IDX(m_pendingAnimationSequence + 1)]
                    + 1;
            } else {
                if (current->m_deathPending) {
                    damageFrameCount =
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)];
                } else if (current->m_damagePending) {
                    damageFrameCount =
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)]
                        + current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)]
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
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            if (gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationSequence
                    == ARMY_ANIMATION_SHOOT_UP
                || gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_FORWARD
                || gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationSequence
                       == ARMY_ANIMATION_SHOOT_DOWN) {
                gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationCycle = 1;
            } else {
                gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationCycle = 0;
            }
            if ((gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_damagePending
                 || static_cast<u8>(gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationState)
                 || static_cast<u8>(gpCombatManager->m_armies[IDX(sideNum)][armyIndex].m_animationCycle))
                && !gpCombatManager->m_limitCreatureCount[IDX(sideNum)][armyIndex]) {
                gpCombatManager->m_limitCreatureCount[IDX(sideNum)][armyIndex]++;
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
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
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
                        current->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)];
                } else {
                    current->m_effectAnimationLength =
                        current->m_frameInfo
                            .animationFrameCount[IDX(current->m_effectAnimationStart)]
                        + current->m_frameInfo
                              .animationFrameCount[IDX(current->m_effectAnimationStart + 1)];
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
        for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
                current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
                if (static_cast<u8>(current->m_animationCycle)) {
                    if (current->m_animationSequence == ARMY_ANIMATION_SHOOT_UP
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_FORWARD
                        || current->m_animationSequence == ARMY_ANIMATION_SHOOT_DOWN) {
                        current->m_animationSequence++;
                        current->m_animationFrame = 0;
                    } else if (current->m_animationSequence != ARMY_ANIMATION_STAND) {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[IDX(current->m_animationSequence)]) {
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
                                             [IDX(current->m_animationSequence)])))) {
                    if (current->m_animationSequence != current->m_effectAnimationStart
                        && current->m_animationSequence != current->m_effectAnimationEnd) {
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == ARMY_ANIMATION_WINCE) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[IDX(sideNum)][armyIndex]
                                                             .m_samples[IDX(ARMY_SAMPLE_WINCE)]);
                        }
                        if (!gbNoShowCombat
                            && current->m_effectAnimationStart == ARMY_ANIMATION_DEATH) {
                            gpSoundManager->MemorySample(gpCombatManager->m_armies[IDX(sideNum)][armyIndex]
                                                             .m_samples[IDX(ARMY_SAMPLE_KILL)]);
                        }
                        current->m_animationSequence = current->m_effectAnimationStart;
                        current->m_animationFrame = 0;
                    } else {
                        if (current->m_animationFrame + 1
                            < current->m_frameInfo
                                  .animationFrameCount[IDX(current->m_animationSequence)]) {
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
            static_cast<i32>(KBTickCount() + frameDelay * gfCombatSpeedMod[gConfig.combatSpeed]);
        if (drawEffect && animFrame < giNumPowFrames[IDX(gCurLoadedSpellEffect)]) {
            gCurSpellEffectFrame = animFrame;
        }
        gpCombatManager->DrawFrame(0, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        if (effectX != NO_POW_EFFECT_COORDINATE
            && animFrame < giNumPowFrames[IDX(gCurLoadedSpellEffect)]) {
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
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
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
        for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
            for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
                current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
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
                              .animationFrameCount[IDX(current->m_animationSequence)]) {
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
                KBTickCount() + frameDelay * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            gpCombatManager->DrawFrame(1, 1, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
        }
    }
    if (resetLimits) {
        gpCombatManager->ResetLimitCreature();
    }
    memset(gpCombatManager->m_removedArmies, 0, sizeof(gpCombatManager->m_removedArmies));
    gpCombatManager->m_removedArmyPresent = 0;
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
            if (current->m_deathPending) {
                current->ProcessDeath(0);
            }
        }
    }
    if (gpCombatManager->m_removedArmyPresent) {
        gpCombatManager->MakeCreaturesVanish();
    }
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            current = &gpCombatManager->m_armies[IDX(sideNum)][armyIndex];
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
    for (sideNum = COMBAT_ATTACKER_SIDE; IDX(sideNum) < COMBAT_SIDE_COUNT; sideNum++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[IDX(sideNum)]; armyIndex++) {
            gpCombatManager->m_armies[IDX(sideNum)][armyIndex].WaitSample(ARMY_SAMPLE_WINCE);
        }
    }
}

VA(0x0041f125, 0x24)
u32l army::Strength(void) {
    return gMonsterDatabase[IDX(m_monsterType)].fightValue * m_quantity;
}

VA(0x0041f149, 0x57)
i32 army::LeaveNoBody(void) {
    return m_monsterType == CREATURE_EARTH_ELEMENTAL || m_monsterType == CREATURE_AIR_ELEMENTAL
           || m_monsterType == CREATURE_FIRE_ELEMENTAL || m_monsterType == CREATURE_WATER_ELEMENTAL
           || HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE);
}

VA(0x0041f1a0, 0x35f)
void army::ProcessDeath(i32 immediate) {
    i32 rearHex;
    army* mirrorImage_4;
    army* mirrorSource;
    hexcell* frontCell_1;
    hexcell* rearCell;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD)) {
        return;
    }
    if (Random(0, DEATH_RANDOM_MAX) < DEATH_PRIMARY_CHANCE) {
        gpCombatManager->m_heroDeathPending[IDX(m_side)] = 1;
    } else if (Random(0, DEATH_RANDOM_MAX) < DEATH_SECONDARY_CHANCE) {
        gpCombatManager->m_heroAlternateDeathPending[IDX(OppositeCombatSide(m_side))] = 1;
    }
    m_monster.attributes |= MONSTER_ATTRIBUTE_DEAD;
    m_deathPending = 0;
    frontCell_1 = &gpCombatManager->m_hexCells[m_hex];
    rearHex = 0;
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
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
            gpCombatManager->m_removedArmies[IDX(m_side)][m_index] = 1;
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
        mirrorSource = &gpCombatManager->m_armies[IDX(m_side)][m_mirrorSourceIndex];
        mirrorSource->m_mirrorImageIndex = -1;
    }
    if (m_mirrorImageIndex != -1) {
        mirrorImage_4 = &gpCombatManager->m_armies[IDX(m_side)][m_mirrorImageIndex];
        mirrorImage_4->m_quantity = 0;
        mirrorImage_4->ProcessDeath(0);
    }
}

VA(0x0041f4ff, 0x35f)
void army::SpellEffect(
    H2_ENUM_PARAM(CombatEffectType, i32) effect,
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

    effectFileId = MAKEFILEID(gCombatFxNames[IDX(effect)]);
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
                / m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)];
            m_animationSequence = ARMY_ANIMATION_WINCE;
            for (; frame
                   < m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)];
                 frame++) {
                m_animationFrame = frame;
                if (frame < giNumPowFrames[IDX(effect)]) {
                    gCurSpellEffectFrame = frame;
                } else {
                    gCurSpellEffectFrame = giNumPowFrames[IDX(effect)];
                }
                glTimers[1] = static_cast<i32>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
                );
                gpCombatManager->DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(&glTimers[1]);
            }
        }
        for (; frame < giNumPowFrames[IDX(effect)]; frame++) {
            glTimers[1] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * effectFrameDelay
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
                           / m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)];
            m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
            for (frame = 0; frame < m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)];
                 frame++) {
                m_animationFrame = frame;
                glTimers[1] = static_cast<i32>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
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

VA(0x0041f85e, 0xe5)
void army::CancelSpellType(ArmySpellCancelType cancelType) {
    switch (cancelType) {
        case ARMY_CANCEL_SPELLS_AFTER_MOVE:
            break;
        case ARMY_CANCEL_SPELLS_AFTER_ATTACK:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BERSERK);
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_HYPNOTIZE);
            break;
        case ARMY_CANCEL_SPELLS_AFTER_DAMAGE:
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]) {
                CancelIndividualSpell(ARMY_SPELL_INFLUENCE_BLIND);
                m_damagePenalty = ARMY_DAMAGE_PENALTY_HALF;
                m_monster.attributes |= MONSTER_ATTRIBUTE_WOKE_FROM_DAMAGE;
            }
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                || m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
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

VA(0x0041f943, 0x11c)
void army::CancelIndividualSpell(ArmySpellInfluence influence) {
    if (!m_spellInfluence[IDX(influence)]) {
        return;
    }
    m_spellCount--;
    m_spellInfluence[IDX(influence)] = 0;
    switch (influence) {
        case ARMY_SPELL_INFLUENCE_HASTE:
        case ARMY_SPELL_INFLUENCE_SLOW:
            m_monster.speed = static_cast<i8>(m_speed);
            m_frameInfo.walkDuration = m_walkDuration;
            m_monster.attributes |=
                gMonsterDatabase[IDX(m_monsterType)].attributes & MONSTER_ATTRIBUTE_FLYING;
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
    }
}

VA(0x0041fa5f, 0x239)
i32 army::SetSpellInfluence(ArmySpellInfluence influence, i32 rounds) {
    H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) i;

    if (m_spellInfluence[IDX(influence)]) {
        if (rounds > m_spellInfluence[IDX(influence)]) {
            m_spellInfluence[IDX(influence)] = static_cast<u8>(rounds);
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
            if HAS (m_monster.attributes, MONSTER_ATTRIBUTE_FLYING) {
                H2_ENUM_CLEAR_FLAG(
                    m_monster.attributes, MONSTER_ATTRIBUTE_FLYING
                );
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
            for (i = ARMY_SPELL_INFLUENCE_HASTE; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
                CancelIndividualSpell(i);
            }
            break;
        case ARMY_SPELL_INFLUENCE_STONESKIN:
            if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
                return 0;
            }
            m_monster.defense += STONESKIN_DEFENSE_BONUS;
            break;
        case ARMY_SPELL_INFLUENCE_STEELSKIN:
            CancelIndividualSpell(ARMY_SPELL_INFLUENCE_STONESKIN);
            m_monster.defense += STEELSKIN_DEFENSE_BONUS;
            break;
    }
    m_spellCount++;
    m_spellInfluence[IDX(influence)] = static_cast<u8>(rounds);
    return 1;
}

VA(0x0041fc98, 0x8d)
void army::DecrementSpellRounds(void) {
    H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) i;

    for (i = ARMY_SPELL_INFLUENCE_HASTE; i < ARMY_SPELL_INFLUENCE_COUNT; i++) {
        if (m_spellInfluence[IDX(i)]) {
            if (m_spellInfluence[IDX(i)] == 1) {
                CancelIndividualSpell(i);
            } else {
                m_spellInfluence[IDX(i)]--;
            }
        }
    }
    if (m_roundCounter > 0) {
        m_roundCounter--;
    }
}

VA(0x0041fd25, 0x513)
void army::GoBerserk(void) {
    i32 masks_28[IDX(COMBAT_SIDE_COUNT)];
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
    masks_28[IDX(COMBAT_ATTACKER_SIDE)] = gpCombatManager->GetAllMask(COMBAT_ATTACKER_SIDE);
    masks_28[IDX(COMBAT_DEFENDER_SIDE)] = gpCombatManager->GetAllMask(COMBAT_DEFENDER_SIDE);
    m_quantity = savedQuantity_8;
    attackMask_29 =
        GetAttackMask(m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
    if (attackMask_29 != ARMY_ALL_ATTACK_DIRECTIONS) {
        while (!targetFound_8) {
            direction_4 = static_cast<CombatHexDirection>(
                Random(0, ARMY_COMBAT_DIRECTION_COUNT - 1)
            );
            if (!(attackMask_29 & BIT(direction_4))) {
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
        for (side_8 = COMBAT_ATTACKER_SIDE; IDX(side_8) < COMBAT_SIDE_COUNT; side_8++) {
            for (index_10 = 0; index_10 < gpCombatManager->m_armyCount[IDX(side_8)]; index_10++) {
                if (side_8 == m_side && index_10 == m_index) {
                    continue;
                }
                army* candidate_16 = &gpCombatManager->m_armies[IDX(side_8)][index_10];
                if (HAS(candidate_16->m_monster.flags.all, MONSTER_FLAGS_DEAD)
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
        if (nearestIndex_1 != -1 && HAS(m_monster.flags.all, MONSTER_FLAGS_SHOOTER)
            && m_monster.shots > 0) {
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex =
                gpCombatManager->m_armies[IDX(nearestSide_8)][nearestIndex_1].m_hex;
            goto berserkFinish;
        }
        {
            i32 sideZeroTarget_12 = -1;
            i32 sideOneTarget_18 = -1;
            if (gpCombatManager->AttemptAttack(
                    this,
                    COMBAT_ATTACKER_SIDE,
                    masks_28[IDX(COMBAT_ATTACKER_SIDE)]
                )) {
                giNextAction = ACTION_MOVE;
                sideZeroTarget_12 = giNextActionGridIndex;
            }
            if (gpCombatManager->AttemptAttack(
                    this,
                    COMBAT_DEFENDER_SIDE,
                    masks_28[IDX(COMBAT_DEFENDER_SIDE)]
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
    if (!HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        if (gpCombatManager->WalkTowardArmy(this, m_side, masks_28[IDX(m_side)]))
            goto berserkFinish;
        if (gpCombatManager->WalkTowardArmy(
                this,
                OppositeCombatSide(m_side),
                masks_28[IDX(OppositeCombatSide(m_side))]
            ))
            goto berserkFinish;
    }
    giNextAction = ACTION_WAIT;
berserkFinish:
    if (giNextAction == ACTION_MOVE
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == m_side) {
        gpCombatManager->m_heroDeathPending[IDX(m_side)] = 1;
    }
}

VA(0x00420238, 0x35d)
void army::MoveAttack(i32 destination, i32 moveOnly) {
    i32 baseAttackMask;
    i32 targetAttackMask_1;
    i32 sourceHex_8;
    i32 adjacentHex_16;
    CombatHexDirection direction_3;
    hexcell* adjacentCell;

again:
    gpCombatManager->m_limitCreature = 0;
    m_targetSide = COMBAT_SIDE_NONE;
    m_targetIndex = -1;
    if (!ValidHex(destination)) {
        return;
    }
    if (gpCombatManager->m_hexCells[destination].m_occupantSide != COMBAT_SIDE_NONE
        && (gpCombatManager->m_hexCells[destination].m_occupantSide
                != gpCombatManager->m_currentArmySide
            || gpCombatManager->m_hexCells[destination].m_occupantIndex
                   != gpCombatManager->m_currentArmyIndex)) {
        if (moveOnly) {
            return;
        }
        m_targetSide = gpCombatManager->m_hexCells[destination].m_occupantSide;
        m_targetIndex = gpCombatManager->m_hexCells[destination].m_occupantIndex;
        m_moveTargetHex = destination;
        baseAttackMask = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID);
        if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)
            && baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
            if (m_hex != m_moveTargetHex
                && !ValidFlight(m_moveTargetHex, ARMY_PATH_ANY_TARGET_HEX)) {
                return;
            }
            FlyTo(m_moveTargetHex);
            goto again;
        }

        if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]) {
            targetAttackMask_1 = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_OCCUPIED, ARMY_HEX_INVALID);
        } else {
            targetAttackMask_1 = GetAttackMask(m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID);
        }
        if (targetAttackMask_1 == ARMY_ALL_ATTACK_DIRECTIONS && m_monster.shots > 0) {
            SpecialAttack();
        } else if (baseAttackMask == ARMY_ALL_ATTACK_DIRECTIONS) {
            AttackTo();
        } else {
            for (direction_3 = COMBAT_DIRECTION_NORTHEAST;
                 IDX(direction_3) < ARMY_COMBAT_DIRECTION_COUNT;
                 direction_3++) {
                if (IDX(direction_3) < ARMY_ADJACENT_DIRECTION_COUNT
                    || HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                    sourceHex_8 = m_hex;
                    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
                        && m_facing == ARMY_FACING_RIGHT
                        && direction_3 >= COMBAT_DIRECTION_NORTHEAST
                        && direction_3 <= COMBAT_DIRECTION_SOUTHEAST) {
                        sourceHex_8++;
                    }
                    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
                        && m_facing == ARMY_FACING_LEFT
                        && direction_3 >= COMBAT_DIRECTION_SOUTHWEST
                        && direction_3 <= COMBAT_DIRECTION_NORTHWEST) {
                        sourceHex_8--;
                    }
                    if (direction_3 >= COMBAT_DIRECTION_WIDE_WEST) {
                        sourceHex_8 += ArmyFacingRearHexOffset(m_facing);
                    }
                    adjacentHex_16 = GetAdjacentCellIndex(sourceHex_8, direction_3);
                    if (ValidHex(adjacentHex_16)) {
                        adjacentCell = &gpCombatManager->m_hexCells[adjacentHex_16];
                        if (adjacentCell->m_occupantSide == m_targetSide
                            && adjacentCell->m_occupantIndex == m_targetIndex) {
                            m_attackDirection = direction_3;
                        }
                    }
                }
            }
            DoAttack(0);
        }
    } else if (HAS(m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
        m_moveTargetHex = destination;
        if (!ValidFlight(m_moveTargetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            return;
        }
        FlyTo(m_moveTargetHex);
    } else {
        WalkTo(destination);
    }
    gpCombatManager->m_limitCreature = 1;
}

VA(0x00420595, 0x75c)
float army::SpellCastWorkChance(SpellType spell) {
    i32 foundSpell_8;
    H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) i_15;
    i32 resurrectPower_5;
    i32 hypnotizeHitPoints_37;

    if (HAS(m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)
        && (spell == SPELL_MIRROR_IMAGE || spell == SPELL_ANTI_MAGIC)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL) {
        foundSpell_8 = 0;
        for (i_15 = ARMY_SPELL_INFLUENCE_HASTE; i_15 < ARMY_SPELL_INFLUENCE_COUNT; i_15++) {
            if (m_spellInfluence[IDX(i_15)]) {
                foundSpell_8 = 1;
                break;
            }
        }
        if (!foundSpell_8) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        || (HAS(m_monster.flags.all, MONSTER_FLAGS_DEAD) && spell != SPELL_RESURRECT
            && spell != SPELL_TRUE_RESURRECT && spell != SPELL_ANIMATE_DEAD)
        || m_deathPending || m_monsterType == CREATURE_GREEN_DRAGON
        || m_monsterType == CREATURE_RED_DRAGON || m_monsterType == CREATURE_BLACK_DRAGON) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_MIRROR_IMAGE && m_mirrorImageIndex != -1) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT)
        && (HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_quantity == m_initialQuantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_ANIMATE_DEAD
        && (!HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_quantity == m_initialQuantity)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)
        && !HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)
        && HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_PHOENIX
        && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST
            || spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING
            || spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING
            || spell == SPELL_ELEMENTAL_STORM)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_CRUSADER && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if ((HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD) || m_monsterType == CREATURE_EARTH_ELEMENTAL
         || m_monsterType == CREATURE_AIR_ELEMENTAL || m_monsterType == CREATURE_FIRE_ELEMENTAL
         || m_monsterType == CREATURE_WATER_ELEMENTAL || m_monsterType == CREATURE_GIANT
         || m_monsterType == CREATURE_TITAN)
        && (spell == SPELL_BERSERKER || spell == SPELL_HYPNOTIZE || spell == SPELL_PARALYZE
            || spell == SPELL_BLIND)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_UNDEAD)
        && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE || spell == SPELL_BLESS
            || spell == SPELL_MASS_BLESS)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_EARTH_ELEMENTAL
        && (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING
            || spell == SPELL_METEOR_SHOWER)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_AIR_ELEMENTAL && spell == SPELL_METEOR_SHOWER) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_FIRE_ELEMENTAL
        && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (m_monsterType == CREATURE_WATER_ELEMENTAL
        && (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (gpCombatManager->m_heroes[IDX(m_side)]) {
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_HOLY_PENDANT)
            && (spell == SPELL_CURSE || spell == SPELL_MASS_CURSE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_PENDANT_FREE_WILL)
            && spell == SPELL_HYPNOTIZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_PENDANT_LIFE)
            && (spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_SERENITY_PENDANT)
            && spell == SPELL_BERSERKER) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_SEEING_EYE_PENDANT)
            && spell == SPELL_BLIND) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_KINETIC_PENDANT)
            && spell == SPELL_PARALYZE) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_PENDANT_DEATH)
            && (spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
        if (gpCombatManager->m_heroes[IDX(m_side)]->HasArtifact(ARTIFACT_WAND_NEGATION)
            && (spell == SPELL_DISPEL || spell == SPELL_MASS_DISPEL
                || spell == CREATURE_SPELL_DISPEL)) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (m_quantity == 0
        && (spell == SPELL_RESURRECT || spell == SPELL_TRUE_RESURRECT
            || spell == SPELL_ANIMATE_DEAD)) {
        resurrectPower_5 = gpCombatManager->m_spellPower[IDX(gpCombatManager->m_currentSide)]
                           * RESURRECT_POWER_PER_SPELL_POWER;
        if (gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)]
            && gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)]->HasArtifact(
                ARTIFACT_ANKH
            )) {
            resurrectPower_5 *= ARTIFACT_POWER_MULTIPLIER;
        }
        if (resurrectPower_5 < m_monster.hitPoints) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if (spell == CREATURE_SPELL_DISPEL && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HASTE)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLESS)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLOODLUST)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_SHIELD)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STONESKIN)]
        && !m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_STEELSKIN)]) {
        return ARMY_SPELL_CHANCE_NONE;
    }
    if (spell == SPELL_HYPNOTIZE) {
        hypnotizeHitPoints_37 = gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)]->Stats(
                                    HERO_PRIMARY_SPELL_POWER
                                )
                                * HYPNOTIZE_HIT_POINTS_PER_POWER;
        if (gpCombatManager->m_heroes[IDX(gpCombatManager->m_currentSide)]->HasArtifact(
                ARTIFACT_GOLD_WATCH
            )) {
            hypnotizeHitPoints_37 *= ARTIFACT_POWER_MULTIPLIER;
        }
        if (m_quantity * m_monster.hitPoints > hypnotizeHitPoints_37) {
            return ARMY_SPELL_CHANCE_NONE;
        }
    }
    if ((m_monsterType == CREATURE_DWARF || m_monsterType == CREATURE_BATTLE_DWARF)
        && spell != SPELL_TELEPORT && spell != SPELL_CURE && spell != SPELL_MASS_CURE
        && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT && spell != SPELL_HASTE
        && spell != SPELL_MASS_HASTE && spell != SPELL_BLESS && spell != SPELL_MASS_BLESS
        && spell != SPELL_STONE_SKIN && spell != SPELL_STEEL_SKIN && spell != SPELL_ANTI_MAGIC
        && spell != SPELL_DRAGON_SLAYER && spell != SPELL_BLOOD_LUST && spell != SPELL_MIRROR_IMAGE
        && spell != SPELL_SHIELD && spell != SPELL_MASS_SHIELD) {
        return ARMY_SPELL_CHANCE_DWARF;
    }
    return ARMY_SPELL_CHANCE_ALWAYS;
}

VA(0x00420cf1, 0x42)
i32 army::SpellCastWorks(SpellType spell) {
    i32 chance;

    chance = static_cast<i32>(SpellCastWorkChance(spell) * ARMY_SPELL_CHANCE_PERCENT);
    return SRandom(1, RANDOM_SPELL_ROLL_MAX) <= chance;
}

VA(0x00420d33, 0x37b)
void BuildTempWalkSeq(struct SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing) {
    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] = 0;
    if (!skipDrawing && finishStanding) {
        if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_STAND)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_STAND)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_STAND)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_STAND)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_STAND)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_STAND)];
        }
    } else {
        if (!skipDrawing) {
            if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[IDX(
                        ARMY_ANIMATION_WALK
                    )][frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_BEGIN)],
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_BEGIN)],
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN)]
                );
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN)];
            }
        } else if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)];
        }
        if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_MIDDLE)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_MIDDLE)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_MIDDLE)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_MIDDLE)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_MIDDLE)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_MIDDLE)];
        }
        if (finishStanding) {
            if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END_STANDING)] > 0) {
                memcpy(
                    &frameInfo->animationFrames[IDX(
                        ARMY_ANIMATION_WALK
                    )][frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_END_STANDING)],
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END_STANDING)]
                );
                memcpy(
                    &frameInfo
                         ->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                    frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_END_STANDING)],
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END_STANDING)]
                );
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                    frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END_STANDING)];
            }
        } else if (frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END)] > 0) {
            memcpy(
                &frameInfo
                     ->animationFrames[IDX(ARMY_ANIMATION_WALK)]
                                      [frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationFrames[IDX(ARMY_ANIMATION_WALK_END)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END)]
            );
            memcpy(
                &frameInfo->walkXOffsets[frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)]],
                frameInfo->animationXOffsets[IDX(ARMY_ANIMATION_WALK_END)],
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END)]
            );
            frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK)] +=
                frameInfo->animationFrameCount[IDX(ARMY_ANIMATION_WALK_END)];
        }
    }
}

VA(0x004210ae, 0x5b)
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

VA(0x00421109, 0x86)
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

VA(0x0042118f, 0x57)
i32 army::MidX(void) {
    return gpCombatManager->m_hexCells[m_hex].m_x
           + (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)
                  ? (m_facing == ARMY_FACING_RIGHT ? WIDE_CREATURE_HALF_WIDTH
                                                   : -WIDE_CREATURE_HALF_WIDTH)
                  : 0);
}

VA(0x004211e6, 0x46)
i32 army::MidY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - (GetIconEntry(
                  m_creatureIcon,
                  m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
              )
                  ->h
              >> 1);
}

VA(0x0042122c, 0x44)
i32 army::TopY(void) {
    return gpCombatManager->m_hexCells[m_hex].m_y
           - GetIconEntry(m_creatureIcon, m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0])
                 ->h;
}

VA(0x00421270, 0xa6)
i32 army::RightX(void) {
    if (m_facing == ARMY_FACING_RIGHT) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + (GetIconEntry(
                      m_creatureIcon,
                      m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                      ->x
                  + GetIconEntry(
                        m_creatureIcon,
                        m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                        ->w);
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    }
}

VA(0x00421316, 0xa6)
i32 army::LeftX(void) {
    if (m_facing == ARMY_FACING_RIGHT) {
        return gpCombatManager->m_hexCells[m_hex].m_x
               + GetIconEntry(
                     m_creatureIcon,
                     m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
               )
                     ->x;
    } else {
        return gpCombatManager->m_hexCells[m_hex].m_x
               - (GetIconEntry(
                      m_creatureIcon,
                      m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                      ->x
                  + GetIconEntry(
                        m_creatureIcon,
                        m_frameInfo.animationFrames[IDX(ARMY_ANIMATION_STAND)][0]
                  )
                        ->w);
    }
}

VA(0x004213bc, 0x136)
i32 army::OtherArmyAdjacent(H2_ENUM_PARAM(CombatSide, i32) side, i32 index) {
    army* otherArmy;
    i32 otherSquare;
    i32 otherRearSquare;
    i32 myRearSquare;
    i32 adjacentSquare;
    CombatHexDirection i;

    otherArmy = &gpCombatManager->m_armies[IDX(side)][index];
    otherSquare = otherArmy->m_hex;
    if (HAS(otherArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        otherRearSquare = otherSquare + (otherArmy->m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
    } else {
        otherRearSquare = -1;
    }
    for (i = COMBAT_DIRECTION_NORTHEAST; IDX(i) < ARMY_ADJACENT_DIRECTION_COUNT; i++) {
        adjacentSquare = GetAdjacentCellIndex(m_hex, i);
        if (adjacentSquare == otherSquare
            || (adjacentSquare != -1 && adjacentSquare == otherRearSquare)) {
            return 1;
        }
    }
    if (HAS(m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
        myRearSquare = m_hex + (m_side == COMBAT_ATTACKER_SIDE ? 1 : -1);
        for (i = COMBAT_DIRECTION_NORTHEAST; IDX(i) < ARMY_ADJACENT_DIRECTION_COUNT; i++) {
            adjacentSquare = GetAdjacentCellIndex(myRearSquare, i);
            if (adjacentSquare == otherSquare
                || (adjacentSquare != -1 && adjacentSquare == otherRearSquare)) {
                return 1;
            }
        }
    }
    return 0;
}

VA(0x004214f2, 0x14b)
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
            gMonsterDatabase[IDX(monsterType)].speed - gMonsterDatabase[IDX(monsterType) - 1].speed;
    }
    if (monsterType == CREATURE_EARTH_ELEMENTAL || monsterType == CREATURE_AIR_ELEMENTAL
        || monsterType == CREATURE_WATER_ELEMENTAL) {
        speedDifference = gMonsterDatabase[IDX(monsterType)].speed
                          - gMonsterDatabase[IDX(CREATURE_FIRE_ELEMENTAL)].speed;
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

VA(0x0042163d, 0x8b)
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

DATA(0x005240a0) i32 bSecondAttack = 0;
DATA(0x0052409c) b32 gbGenieHalf;
