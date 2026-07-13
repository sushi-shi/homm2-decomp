#ifndef HOMM2_ARMY_H
#define HOMM2_ARMY_H
// Declarations of the free functions DEFINED in ARMY.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

typedef enum ArmyAnimationSequence {
    ARMY_ANIMATION_WALK = 6,
    ARMY_ANIMATION_STAND = 7,
    ARMY_ANIMATION_STANDING_FIRST = 8,
    ARMY_ANIMATION_STANDING_END = 13,
    ARMY_ANIMATION_WINCE = 14,
    ARMY_ANIMATION_DEATH = 15,
    ARMY_ANIMATION_SHOOT_UP = 28,
    ARMY_ANIMATION_SHOOT_FORWARD = 30,
    ARMY_ANIMATION_SHOOT_DOWN = 32
} ArmyAnimationSequence;

typedef enum ArmySampleType {
    ARMY_SAMPLE_MOVE,
    ARMY_SAMPLE_ATTACK,
    ARMY_SAMPLE_WINCE,
    ARMY_SAMPLE_SHOT,
    ARMY_SAMPLE_KILL,
    ARMY_SAMPLE_EXTRA_ONE,
    ARMY_SAMPLE_EXTRA_TWO,
    ARMY_SAMPLE_COUNT
} ArmySampleType;

typedef enum ArmyCombatConstant {
    ARMY_SAMPLE_VOLUME = 64,
    ARMY_SAMPLE_CHANNEL = 3,
    ARMY_PRIMARY_SAMPLE_COUNT = ARMY_SAMPLE_KILL + 1,
    ARMY_QUANTITY_TEXT_SIZE = 12,
    ARMY_SPELL_INFLUENCE_COUNT = 15,
    ARMY_HEX_COLUMNS = 13,
    ARMY_COMBAT_WIDTH = 640,
    ARMY_COMBAT_HEIGHT = 443,
    ARMY_COMBAT_MAX_X = ARMY_COMBAT_WIDTH - 1,
    ARMY_COMBAT_MAX_Y = ARMY_COMBAT_HEIGHT - 1,
    ARMY_COMBAT_FRAME_DELAY = 75,
    ARMY_LICH_EXPLOSION_EFFECT = 20,
    ARMY_ENDLESS_AMMUNITION_ARTIFACT = 68,
    ARMY_ARCHMAGE_DISPEL_CHANCE = 20,
    ARMY_PERCENT_MAX = 100
} ArmyCombatConstant;

typedef enum ArmyCreatureType {
    ARMY_CREATURE_ARCHER = 1,
    ARMY_CREATURE_RANGER = 2,
    ARMY_CREATURE_ORC = 12,
    ARMY_CREATURE_ORC_CHIEF = 13,
    ARMY_CREATURE_TROLL = 17,
    ARMY_CREATURE_WAR_TROLL = 18,
    ARMY_CREATURE_ELF = 23,
    ARMY_CREATURE_GRAND_ELF = 24,
    ARMY_CREATURE_DRUID = 25,
    ARMY_CREATURE_GREATER_DRUID = 26,
    ARMY_CREATURE_HALFLING = 38,
    ARMY_CREATURE_MAGE = 43,
    ARMY_CREATURE_ARCHMAGE = 44,
    ARMY_CREATURE_GIANT = 45,
    ARMY_CREATURE_TITAN = 46,
    ARMY_CREATURE_VAMPIRE = 52,
    ARMY_CREATURE_VAMPIRE_LORD = 53,
    ARMY_CREATURE_LICH = 54,
    ARMY_CREATURE_POWER_LICH = 55,
    ARMY_CREATURE_EARTH_ELEMENTAL = 62,
    ARMY_CREATURE_AIR_ELEMENTAL = 63,
    ARMY_CREATURE_FIRE_ELEMENTAL = 64,
    ARMY_CREATURE_WATER_ELEMENTAL = 65
} ArmyCreatureType;

typedef enum ArmyFrameConstant {
    ARMY_MISSILE_OFFSET_COUNT = 3,
    ARMY_PROJECTILE_ANGLE_COUNT = 12,
    ARMY_ANIMATION_SEQUENCE_COUNT = 34,
    ARMY_ANIMATION_FRAME_COUNT = 16
} ArmyFrameConstant;

#pragma pack(push, 1)
struct SMonFrameInfo {
    struct MissileOffset {
        short x;
        short y;
    };
    char unknown00;
    short spellEffectX;              // +0x01
    short spellEffectY;              // +0x03
    char unknown05[0x60];
    signed char walkXOffsets[0x3d];   // +0x65
    int walkDuration;                 // +0xa2
    char unknownA6[4];
    int attackDuration;               // +0xaa
    struct MissileOffset missileOffsets[ARMY_MISSILE_OFFSET_COUNT]; // +0xae
    unsigned char projectileDirectionCount; // +0xba
    float projectileAngles[ARMY_PROJECTILE_ANGLE_COUNT]; // +0xbb
    int quantityX[2];                 // +0xeb
    signed char animationFrameCount[ARMY_ANIMATION_SEQUENCE_COUNT]; // +0xf3
    signed char animationFrames[ARMY_ANIMATION_SEQUENCE_COUNT]
                               [ARMY_ANIMATION_FRAME_COUNT]; // +0x115
};
SIZE(SMonFrameInfo, 0x335);
#pragma pack(pop)
extern SMonFrameInfo sViewArmyMonFrameInfo;

void BuildTempWalkSeq(struct SMonFrameInfo *, int, int);
void ModifyFrameInfo(struct SMonFrameInfo *, int);

#endif // HOMM2_ARMY_H
