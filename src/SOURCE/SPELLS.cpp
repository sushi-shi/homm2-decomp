#include <Ints.h>
#include <SOURCE/KB_TYPES.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/Iconf2b.h>
#include <BASE/Misc.h>
#include <BASE/palette.h>
#include <BASE/Blur.h>
#include <BASE/Ripple.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>

#define RIPPLE_MODE_ZERO_AMPLITUDE_BASE 0.3f
#define RIPPLE_MODE_ZERO_AMPLITUDE_STEP 0.05f
#define RIPPLE_OTHER_AMPLITUDE_BASE 0.2f
#define RIPPLE_OTHER_AMPLITUDE_STEP 0.1f
#define SPELL_VANISH_END_DELAY 500.0f
#define CHAIN_LIGHTNING_FRAME_DELAY 100.0f
#define MIRROR_SLIDE_FRAME_DELAY 50.0f
#define BLAST_FRAME_DELAY static_cast<float>(10.0)
#define SPELL_COLD_RAY_DELAY 175.0f
#define SPELL_MAGIC_ARROW_DELAY 100.0f
#define SPELL_AREA_ANIMATION_DELAY 75.0f
#define SPELL_METEOR_ANIMATION_DELAY 112.5
#define SPELL_ARMAGEDDON_PALETTE_DELAY 20.0f
#define SPELL_ARMAGEDDON_SHAKE_DELAY 15.0f
#define SPELL_ARMAGEDDON_SHAKE_SCALE 0.75
#define SPELL_ARTIFACT_DAMAGE_BONUS 1.5
#define SPELL_ARTIFACT_DAMAGE_REDUCTION 0.5
#define BOLT_INITIAL_ANGLE_BIAS 2.5
#define BOLT_INITIAL_ANGLE_DIVISOR 2.0
#define BOLT_RANDOM_ANGLE_BIAS 2.0f
#define BOLT_RANDOM_ANGLE_DIVISOR 1.5
#define BOLT_ANGLE_DISTANCE_FACTOR 1.5
#define BOLT_BRANCH_COOLDOWN_FACTOR 0.75
#define BOLT_CHILD_ANGLE_SCALE 0.66
#define BOLT_CHILD_ANGLE_OFFSET 20.0
#define BOLT_CHILD_MIN_ANGLE_OFFSET -20.0

namespace {

    typedef enum CombatSpellUiConstant {
        COMMAND_CANCEL = 1,
        CONTROL_PREVIOUS_PAGE = 2,
        CONTROL_NEXT_PAGE = 3,
        CONTROL_FIRST_MANA = 6,
        CONTROL_LAST_MANA = 9,
        CONTROL_CLOSE = 0x7800,
        HELP_PREVIOUS_PAGE = 0,
        HELP_NEXT_PAGE = 1,
        HELP_CLOSE = 4,
        HELP_MANA = 6,
        HELP_DEFAULT = 7,
        NO_SELECTION = -1,
        HEX_COLUMN_COUNT = 13,
        HEX_RIGHT_BORDER = 12,
        ELEMENTAL_ARMY_LIMIT = 20
    } CombatSpellUiConstant;

    typedef enum SpellPresentationConstant {
        PLURAL_QUANTITY_MINIMUM = 2,
        MASS_SPELL_FRAME_DELAY = 110,
        TARGET_NAME_CAPACITY = 60,
        MESSAGE_CAPACITY = 400
    } SpellPresentationConstant;

    typedef enum SpellBoltConstant {
        LIGHTNING_BRANCH_DISTANCE = 150,
        LIGHTNING_BRANCH_LENGTH = 100,
        LIGHTNING_START_WIDTH = 9,
        LIGHTNING_END_WIDTH = 2,
        LIGHTNING_MIN_ANGLE = -40,
        LIGHTNING_MAX_ANGLE = 40,
        LIGHTNING_ANGLE_DISTANCE = 30,
        BRANCH_MIN_REMAINING_DISTANCE_MULTIPLIER = 2,
        CHAIN_LIGHTNING_DISTANCE_SENTINEL = 999999,
        CHAIN_LIGHTNING_SHORT_FORCE_ANGLE = 2,
        CHAIN_LIGHTNING_LONG_FORCE_ANGLE = 3
    } SpellBoltConstant;

    typedef enum SpellAreaConstant {
        FIREBLAST_EAST_FIRST_RING = (COMBAT_DIRECTION_EAST) + 1,
        FIREBLAST_WEST_FIRST_RING = (COMBAT_DIRECTION_WEST) + 1,
        ELEMENTAL_WEAKNESS_MULTIPLIER = 2
    } SpellAreaConstant;

    typedef enum VaporizeConstant {
        VAPORIZE_ROW_PAIR_SIZE = 2,
        VAPORIZE_FRAME_DELAY = 30
    } VaporizeConstant;

    typedef enum RippleCreatureConstant {
        RIPPLE_WAVE_PHASE_STEP = 2,
        RIPPLE_DEFAULT_PHASE_STEP = 1,
        RIPPLE_WAVE_FRAME_DELAY = 20,
        RIPPLE_DEFAULT_FRAME_DELAY = 30,
        RIPPLE_WAVE_PHASE_MULTIPLIER = 2
    } RippleCreatureConstant;

    typedef enum MassSpellConstant {
        HOLY_WORD_DAMAGE_PER_POWER = 10,
        HOLY_SHOUT_DAMAGE_PER_POWER = 20,
        HOLY_WORD_COLOR_ADJUSTMENT = -2,
        HOLY_SHOUT_COLOR_ADJUSTMENT = -4,
        DEATH_RIPPLE_STRENGTH = 1,
        DEATH_WAVE_STRENGTH = 2,
        DEATH_RIPPLE_DAMAGE_PER_POWER = 5,
        DEATH_WAVE_DAMAGE_PER_POWER = 10,
        MIRROR_SOURCE_PART_COUNT = 2
    } MassSpellConstant;

    typedef enum ElementalSummonConstant {
        SUMMON_HEX_STORAGE_COUNT = 8,
        SUMMON_HEXES_PER_SIDE = 3,
        SUMMON_RANDOM_OFFSET_MAX = 2,
        ATTACKER_SUMMON_TOP_SLOT = 0,
        ATTACKER_SUMMON_MID_SLOT = 1,
        ATTACKER_SUMMON_LOW_SLOT = 2,
        DEFENDER_SUMMON_TOP_SLOT = 3,
        DEFENDER_SUMMON_MID_SLOT = 4,
        DEFENDER_SUMMON_LOW_SLOT = 5,
        ATTACKER_SUMMON_HEX_TOP = 14,
        ATTACKER_SUMMON_HEX_MID = 27,
        ATTACKER_SUMMON_HEX_LOW = 40,
        DEFENDER_SUMMON_HEX_TOP = 11,
        DEFENDER_SUMMON_HEX_MID = 24,
        DEFENDER_SUMMON_HEX_LOW = 37
    } ElementalSummonConstant;

    typedef enum LuckConstant {
        LUCK_RIGHT_FACING_FLIP_X = 480,
        LUCK_LEFT_FACING_FLIP_X = 200,
        LUCK_DIAGONAL_DIVISOR = 2
    } LuckConstant;

    typedef enum EarthquakeConstant {
        EARTHQUAKE_CHANCE_ROLL_MAX = 100,
        EARTHQUAKE_KEEP_IMPACT_COUNT = 2,
        EARTHQUAKE_MAX_IMPACT_DELAY = 2
    } EarthquakeConstant;

#define RIPPLE_WAVE_CENTER 0.5
#define RIPPLE_WAVE_RANGE 2.0

}

i32 combatManager::HasValidSpellTarget(SpellType spell) {
    i32 hex;

    for (hex = 0; hex < COMBAT_HEX_COUNT; ++hex) {
        if (hex % HEX_COLUMN_COUNT == 0 || hex % HEX_COLUMN_COUNT == HEX_RIGHT_BORDER)
            continue;
        if (ValidSpellTarget(spell, hex))
            return 1;
    }
    return 0;
}

i32 combatManager::ViewSpells(i32) {
    CreatureType elementalType;

    m_selectedSpell =
        gpGame->ViewSpells(m_heroes[(giCurGeneral)], SPELL_TYPE_COMBAT, CombatSpecialHandler, 0);
    if (m_selectedSpell != SPELL_NONE) {
        switch (m_selectedSpell) {
            case SPELL_EARTHQUAKE:
                if (m_combatTowns[(COMBAT_DEFENDER_SIDE)] == NULL) {
                    NormalDialog(
                        "От землетрясения нет никакого прока, если только на карте есть городские стены, которые можно разрушить."
                        ,
                        NORMAL_DIALOG_INFO
                    );
                    break;
                } else
                    goto set_action;

            case SPELL_SUMMON_EARTH_ELEMENTAL:
                elementalType = CREATURE_EARTH_ELEMENTAL;
                goto check_elemental;
            case SPELL_SUMMON_AIR_ELEMENTAL:
                elementalType = CREATURE_AIR_ELEMENTAL;
                goto check_elemental;
            case SPELL_SUMMON_FIRE_ELEMENTAL:
                elementalType = CREATURE_FIRE_ELEMENTAL;
                goto check_elemental;
            case SPELL_SUMMON_WATER_ELEMENTAL:
                elementalType = CREATURE_WATER_ELEMENTAL;
                goto check_elemental;
            check_elemental:
                if (m_summonedCreatureType[(m_currentSide)] != CREATURE_SUMMONED_NONE
                    && m_summonedCreatureType[(m_currentSide)] != elementalType) {
                    NormalDialog(
                        "Вы можете призвать элементалов только одного типа во время одного боя."
                        ,
                        NORMAL_DIALOG_INFO
                    );
                    return 0;
                }
                if (m_armyCount[(m_currentSide)] >= ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        "У вас уже есть %d групп воинов в бою и больше добавить нельзя."
                        ,
                        m_armyCount[(m_currentSide)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO);
                    return 0;
                }
                if (!SpaceForElementalExists()) {
                    sprintf(
                        gText,
                        "Рядом с вашим героем нет достаточного свободного места, чтобы призвать элементалов туда."
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO);
                    return 0;
                }
                goto set_action;

            case SPELL_MASS_CURE:
            case SPELL_MASS_HASTE:
            case SPELL_MASS_SLOW:
            case SPELL_MASS_BLESS:
            case SPELL_MASS_CURSE:
            case SPELL_HOLY_WORD:
            case SPELL_HOLY_SHOUT:
            case SPELL_MASS_DISPEL:
            case SPELL_ARMAGEDDON:
            case SPELL_ELEMENTAL_STORM:
            case SPELL_DEATH_RIPPLE:
            case SPELL_DEATH_WAVE:
            case SPELL_MASS_SHIELD:
                if (!HasValidSpellTarget(m_selectedSpell)) {
                    NormalDialog(
                        "Заклинание ни на кого не подействует!"
                        ,
                        NORMAL_DIALOG_INFO
                    );
                    return 0;
                }
            set_action:
                giNextAction = ACTION_CAST_SPELL;
                giNextActionExtra = (m_selectedSpell);
                break;

            case SPELL_MIRROR_IMAGE:
                if (m_armyCount[(m_currentSide)] >= ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        "У вас уже есть %d групп воинов в бою и больше добавить нельзя."
                        ,
                        m_armyCount[(m_currentSide)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO);
                    return 0;
                }
            default:
                if (!HasValidSpellTarget(m_selectedSpell)) {
                    NormalDialog(
                        "Заклинание ни на кого не подействует!"
                        ,
                        NORMAL_DIALOG_INFO
                    );
                    return 0;
                }
                giNextAction = ACTION_CAST_SPELL;
                giNextActionExtra = (m_selectedSpell);
                gpMouseManager->SetPointer(
                    "spelmous.mse",
                    gsSpellInfo[(m_selectedSpell)].iconIndex,
                    MOUSE_AUTO_CURSOR_TYPE
                );
                gpWindowManager->DoDialog(NULL, HandleCastSpell, 0);
                break;
        }

        gpMouseManager->SetPointer("cmbtmous.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
        if (m_selectedSpell != SPELL_NONE)
            return 1;
    }
    return 0;
}

MessageDispatchResult CombatSpecialHandler(tag_message& message) {
    if (message.type == SPELL_MESSAGE_HOVER) {
        if (message.payload.hover.id == gpWindowManager->m_lastHoverId)
            return MESSAGE_DISPATCH_CONSUME;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;

        switch (message.payload.hover.id) {
            case CONTROL_PREVIOUS_PAGE:
                gpCombatManager->CombatMessage(cSpellHelp[HELP_PREVIOUS_PAGE], 1, 0, 0);
                break;
            case CONTROL_NEXT_PAGE:
                gpCombatManager->CombatMessage(cSpellHelp[HELP_NEXT_PAGE], 1, 0, 0);
                break;
            case CONTROL_CLOSE:
                gpCombatManager->CombatMessage(cSpellHelp[HELP_CLOSE], 1, 0, 0);
                break;
            case CONTROL_FIRST_MANA:
            case CONTROL_FIRST_MANA + 1:
            case CONTROL_LAST_MANA - 1:
            case CONTROL_LAST_MANA:
                gpCombatManager->CombatMessage(cSpellHelp[HELP_MANA], 1, 0, 0);
                break;
            default:
                gpCombatManager->CombatMessage(cSpellHelp[HELP_DEFAULT], 1, 0, 0);
                break;
        }
        return MESSAGE_DISPATCH_CONSUME;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

MessageDispatchResult HandleCastSpell(tag_message& message) {
    i32 hex;

    switch (message.type) {
        case SPELL_MESSAGE_HOVER:
            hex = gpCombatManager->GetGridIndex(message.payload.mouse.x, message.payload.mouse.y);
            if (hex != indexToCastOn) {
                if (!gpCombatManager->ValidSpellTarget(gpCombatManager->m_selectedSpell, hex)) {
                    indexToCastOn = NO_SELECTION;
                    gpMouseManager->SetPointer(0);
                    if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT && bInTeleportGetDest) {
                        gpCombatManager->CombatMessage(
                              "Недопустимое место для телепортации ",
                            1, 0, 0
                        );
                    } else {
                        gpCombatManager->CombatMessage(
                             "Выберите цель для заклинания",
                            1, 0, 0
                        );
                    }
                } else {
                    indexToCastOn = hex;
                    gpMouseManager->SetPointer(
                        gsSpellInfo[(gpCombatManager->m_selectedSpell)].iconIndex
                    );
                    gpCombatManager->SpellMessage(gpCombatManager->m_selectedSpell, hex);
                }
            }
            break;

        case SPELL_MESSAGE_SELECT:
            if (indexToCastOn != NO_SELECTION) {
                if (bInTeleportGetDest) {
                    giNextActionGridIndex2 = indexToCastOn;
                } else {
                    giNextActionGridIndex = indexToCastOn;
                    if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT) {
                        bInTeleportGetDest = true;
                        indexToCastOn = NO_SELECTION;
                        message.type = SPELL_MESSAGE_HOVER;
                        message.payload.mouse.x = message.payload.mouse.screenX;
                        message.payload.mouse.y = message.payload.mouse.screenY;
                        HandleCastSpell(message);
                        gpCombatManager->CombatMessage(
                             "Выберите место назначения телепортации.",
                            1, 0, 0
                        );
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                }
                bInTeleportGetDest = false;
                message.type = SPELL_MESSAGE_DIALOG;
                message.payload.widget.command = SPELL_COMMAND_CLOSE;
                return MESSAGE_DISPATCH_FORWARD;
            }
            break;

        case SPELL_MESSAGE_MOUSE_DOWN:
            if (message.payload.keyboard.keyCode != COMMAND_CANCEL)
                break;

        case SPELL_MESSAGE_CANCEL:
            gpCombatManager->m_selectedSpell = SPELL_NONE;
            giNextAction = ACTION_NONE;
            message.type = SPELL_MESSAGE_DIALOG;
            message.payload.widget.command = SPELL_COMMAND_CLOSE;
            bInTeleportGetDest = false;
            return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i32 combatManager::FindResurrectArmyIndex(
    CombatSide side,
    SpellType spell,
    i32 hex
) {
    army* target;
    i32 corpse;

    if (m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE) {
        if (m_hexCells[hex].m_occupantSide == side) {
            target =
                &m_armies[(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
            if (target->SpellCastWorkChance(spell) > 0.0f)
                return m_hexCells[hex].m_occupantIndex;
        }
        return NO_SELECTION;
    }

    for (corpse = m_hexCells[hex].m_deadOccupantCount - 1; corpse >= 0; --corpse) {
        if (m_hexCells[hex].m_deadOccupantSides[corpse] != COMBAT_SIDE_NONE) {
            if ((m_hexCells[hex].m_deadOccupantFrames[corpse] == ARMY_FACING_LEFT
                 && m_hexCells[hex + 1].m_occupantSide != COMBAT_SIDE_NONE)
                || (m_hexCells[hex].m_deadOccupantFrames[corpse] == ARMY_FACING_RIGHT
                    && m_hexCells[hex - 1].m_occupantSide != COMBAT_SIDE_NONE))
                continue;
            if (m_hexCells[hex].m_deadOccupantSides[corpse] == side) {
                target = &m_armies[(m_hexCells[hex].m_deadOccupantSides[corpse])]
                                  [m_hexCells[hex].m_deadOccupantIndices[corpse]];
                if (target->SpellCastWorkChance(spell) > 0.0f)
                    return m_hexCells[hex].m_deadOccupantIndices[corpse];
            }
        }
    }
    return NO_SELECTION;
}

i32 combatManager::ValidSpellTarget(SpellType spell, i32 hex) {
    army* target = NULL;
    i32 unused [[maybe_unused]];
    i32 destinationHex;
    if (!ValidHex(hex))
        return 0;

    if (spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST && spell != SPELL_METEOR_SHOWER
        && spell != SPELL_COLD_RING && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT
        && spell != SPELL_ANIMATE_DEAD && m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE) {
        target = &m_armies[(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
        if (target->m_spellInfluence[(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)] != 0
            || target->m_monsterType == CREATURE_GREEN_DRAGON)
            return 0;
    }

    if (target != NULL && target->SpellCastWorkChance(spell) <= 0.0f)
        return 0;

    switch (spell) {
        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT:
        case SPELL_DISPEL:
        case SPELL_MASS_DISPEL:
        case SPELL_ARMAGEDDON:
        case SPELL_ELEMENTAL_STORM:
        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE:
            if (m_hexCells[hex].m_occupantSide == COMBAT_SIDE_NONE)
                return 0;
            break;

        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            return FindResurrectArmyIndex(m_currentSide, spell, hex) != NO_SELECTION;

        case SPELL_CURE:
        case SPELL_MASS_CURE:
        case SPELL_HASTE:
        case SPELL_MASS_HASTE:
        case SPELL_BLESS:
        case SPELL_MASS_BLESS:
        case SPELL_STONE_SKIN:
        case SPELL_STEEL_SKIN:
        case SPELL_ANTI_MAGIC:
        case SPELL_DRAGON_SLAYER:
        case SPELL_BLOOD_LUST:
        case SPELL_SHIELD:
        case SPELL_MASS_SHIELD:
            if (m_hexCells[hex].m_occupantSide != m_currentSide)
                return 0;
            break;

        case SPELL_MIRROR_IMAGE:
            if (m_hexCells[hex].m_occupantSide != m_currentSide)
                return 0;
            if (m_armies[(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                        .m_mirrorImageIndex
                    != NO_SELECTION
                || m_armies[(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                           .m_mirrorSourceIndex
                       != NO_SELECTION)
                return 0;
            break;

        case SPELL_LIGHTNING_BOLT:
        case SPELL_CHAIN_LIGHTNING:
        case SPELL_SLOW:
        case SPELL_MASS_SLOW:
        case SPELL_BLIND:
        case SPELL_CURSE:
        case SPELL_MASS_CURSE:
        case SPELL_MAGIC_ARROW:
        case SPELL_BERSERKER:
        case SPELL_PARALYZE:
        case SPELL_HYPNOTIZE:
        case SPELL_COLD_RAY:
        case SPELL_DISRUPTING_RAY:
            if (m_hexCells[hex].m_occupantSide != OppositeCombatSide(m_currentSide))
                return 0;
            break;

        case SPELL_TELEPORT:
            if (bInTeleportGetDest) {
                destinationHex = hex;
                if (destinationHex == giNextActionGridIndex
                    || !m_armies[(gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide)][gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantIndex]
                            .CanFit(destinationHex, 0, NULL))
                    return 0;
            } else {
                if (m_hexCells[hex].m_occupantSide != m_currentSide)
                    return 0;
            }
            break;

        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RING:
            if (hex == COMBAT_HEX_EMPTY || hex % HEX_COLUMN_COUNT == 0
                || hex % HEX_COLUMN_COUNT == HEX_RIGHT_BORDER)
                return 0;
            break;
        default:
            break;
    }
    return 1;
}

void combatManager::SpellMessage(SpellType spell, i32 hex) {
    army* target;

    if (gbNoShowCombat)
        return;

    switch (spell) {
        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RING:
            sprintf(
                gText,
                 "Направить '%s'",
                gSpellNames[(spell)]
            );
            break;
        case SPELL_TELEPORT:
            if (bInTeleportGetDest) {
                sprintf(
                    gText,
                     "Перенестись сюда"
                );
                break;
            }
            goto occupied_target;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            target =
                &m_armies[(m_currentSide)][FindResurrectArmyIndex(m_currentSide, spell, hex)];
            goto format_target;
        default:
        occupied_target:
            target =
                &m_armies[(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
        format_target:
            sprintf(
                gText,
                 "Направить '%s' на %s",
                gSpellNames[(spell)],
                target->m_quantity == 1 ? gArmyNames[(target->m_monsterType)]
                                        : gArmyNamesPlural[(target->m_monsterType)]
            );
            break;
    }

    CombatMessage(gText, 1, 0, 0);
}

void combatManager::CastSpell(
    SpellType spell,
    i32 targetHex,
    i32 castByCreature,
    i32 teleportDestination
) {
    i32l damage;
    i32 spellPower;
    army* target;
    i32 targetIndex;
    SpellType soundSpell;
    i32 armyIndex;
    char sampleName[SPELL_SAMPLE_NAME_CAPACITY];
    SAMPLE2 spellSample;
    i32 targetY;
    i32 targetX;
    i32 side;
    CombatSide targetSide;
    i32 influence;
    i32 adjacentHex;
    i32 oldDefense;
    army* teleportArmy;
    icon* missileIcon;
    float missileAngles[SPELL_MISSILE_ANGLE_COUNT];
    i32 unusedCastA [[maybe_unused]];
    i32 unusedCastB [[maybe_unused]];

    if (castByCreature == 0 && m_eagleEyeSpell[(OppositeCombatSide(m_currentSide))] == SPELL_NONE
        && m_heroes[(OppositeCombatSide(m_currentSide))] != NULL
        && !m_heroes[(OppositeCombatSide(m_currentSide))]->HasSpell(spell)
        && m_heroes[(OppositeCombatSide(m_currentSide))]
                   ->m_secondarySkills[(HERO_SKILL_EAGLE_EYE)]
               != HERO_SKILL_LEVEL_NONE
        && (m_heroes[(OppositeCombatSide(m_currentSide))]
                   ->m_secondarySkills[(HERO_SKILL_EAGLE_EYE)])
                   + 1
               >= (gsSpellInfo[(spell)].level)) {
        if (SRandom(0, SPELL_EAGLE_EYE_ROLL_MAX) <= (m_heroes[(OppositeCombatSide(m_currentSide))]
                    ->m_secondarySkills[(HERO_SKILL_EAGLE_EYE)])) {
            m_eagleEyeSpell[(OppositeCombatSide(m_currentSide))] = spell;
        }
    }

    spellSample = NULL;
    if (m_limitCreature != 0) {
        ResetLimitCreature();
        if (ValidHex(m_limitCreatureHex)
            && m_hexCells[m_limitCreatureHex].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
            m_limitCreatureCount[(m_hexCells[m_limitCreatureHex].m_occupantSide)]
                                [m_hexCells[m_limitCreatureHex].m_occupantIndex]++;
        }
        m_limitCreature = false;
        m_limitCreatureHex = COMBAT_HEX_EMPTY;
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }

    if (castByCreature == 0 && m_heroes[(m_currentSide)] != NULL)
        m_heroes[(m_currentSide)]->UseSpell(spell);

    target = NULL;
    if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
        || spell == SPELL_METEOR_SHOWER || spell == SPELL_SUMMON_EARTH_ELEMENTAL
        || spell == SPELL_SUMMON_AIR_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL
        || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_MASS_BLESS
        || spell == SPELL_MASS_HASTE || spell == SPELL_EARTHQUAKE || spell == SPELL_MASS_CURSE
        || spell == SPELL_MASS_CURE || spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT
        || spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE || spell == SPELL_MASS_SHIELD
        || spell == SPELL_ARMAGEDDON || spell == SPELL_ELEMENTAL_STORM
        || spell == SPELL_MASS_DISPEL) {
        target = NULL;
    } else if (ValidHex(targetHex)
               && m_hexCells[targetHex].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
        target = &m_armies[(m_hexCells[targetHex].m_occupantSide)]
                            [m_hexCells[targetHex].m_occupantIndex];
        targetSide = m_hexCells[targetHex].m_occupantSide;
        targetIndex = m_hexCells[targetHex].m_occupantIndex;
    } else {
        target = NULL;
    }

    if (castByCreature == 0)
        m_heroCastSpell[(m_currentSide)] = true;

    if (castByCreature != 0) {
        spellPower = SPELL_DEFAULT_CREATURE_POWER;
    } else {
        spellPower = m_spellPower[(m_currentSide)];
        if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS)
            && (((gsSpellInfo[(spell)].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION)))) {
            spellPower += SPELL_HOURGLASS_POWER_BONUS;
        }
        if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT)
            && (((gsSpellInfo[(spell)].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION)))) {
            spellPower += SPELL_WIZARD_HAT_POWER_BONUS;
        }
    }

    if (castByCreature == 0) {
        targetX = -1;
        targetY = -1;
        if (target != NULL) {
            targetX = target->MidX();
            targetY = target->MidY();
        } else {
            if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
                || spell == SPELL_METEOR_SHOWER) {
                targetX = m_hexCells[targetHex].m_x;
                targetY = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
            }
        }

        if (targetX != -1) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE) {
                castX = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].x[0]
                        + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                castY = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].y[0]
                        + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
            } else {
                castX = COMBAT_HERO_CAST_RIGHT_X
                        - sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].x[0];
                castY = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].y[0]
                        + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
            }
            if (targetY - castY
                > (targetX - castX) * (m_currentSide == COMBAT_ATTACKER_SIDE ? 1 : -1)) {
                m_heroAnimationState[(m_currentSide)] = COMBAT_HERO_CAST_HIGH;
                if (m_currentSide == COMBAT_ATTACKER_SIDE) {
                    castX = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].x[1]
                            + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                    castY = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].y[1]
                            + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
                } else {
                    castX = COMBAT_HERO_CAST_RIGHT_X
                            - sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].x[1];
                    castY = sCmbtHero[m_heroSpriteIndex[(m_currentSide)]].y[1]
                            + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
                }
            } else {
                m_heroAnimationState[(m_currentSide)] = COMBAT_HERO_CAST_LOW;
            }
        } else {
            m_heroAnimationState[(m_currentSide)] = COMBAT_HERO_CAST_NO_TARGET;
        }
        m_heroAnimationFrame[(m_currentSide)] = 0;
        while (m_heroAnimationFrame[(m_currentSide)]
               < sCmbtHero[m_heroSpriteIndex[(m_currentSide)]]
                     .animationFrameCount[m_heroAnimationState[(m_currentSide)]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[(m_currentSide)]++;
        }
        m_heroAnimationFrame[(m_currentSide)]--;
    }

    soundSpell = spell;
    if (spell == CREATURE_SPELL_PETRIFY)
        soundSpell = SPELL_PARALYZE;
    if (spell == CREATURE_SPELL_DISPEL)
        soundSpell = SPELL_DISPEL;
    if (strlen(gsSpellInfo[(soundSpell)].soundName) > 0)
        sprintf(sampleName, "%s.82M", gsSpellInfo[(soundSpell)].soundName);

    if (castByCreature == 0 && target != NULL && !target->SpellCastWorks(spell)) {
        ShowSpellCastFailure(target, (spell));
        goto cast_done;
    }
    spellSample = LoadPlaySample(sampleName);
    switch (spell) {
        case SPELL_TELEPORT:
            teleportArmy = target;
            targetHex = teleportDestination;
            RippleCreature(
                teleportArmy->m_side,
                teleportArmy->m_index,
                COMBAT_RIPPLE_DEATH_RIPPLE
            );
            CLEAR_HEX_OCCUPANT(m_hexCells[teleportArmy->m_hex]);
            if (m_hexCells[teleportArmy->m_hex].m_occupantFrame == ARMY_FACING_LEFT) {
                CLEAR_HEX_OCCUPANT(m_hexCells[teleportArmy->m_hex + 1]);
            } else if (m_hexCells[teleportArmy->m_hex].m_occupantFrame == ARMY_FACING_RIGHT) {
                CLEAR_HEX_OCCUPANT(m_hexCells[teleportArmy->m_hex - 1]);
            }
            if (gbNoShowCombat == 0)
                WaitEndSample(&spellSample);
            if (gbNoShowCombat == 0) {
                sprintf(gText, "telptin.82m");
                spellSample = LoadPlaySample(gText);
            }
            if ((((teleportArmy->m_monster.flags.all) & (MONSTER_FLAGS_WIDE))) != 0) {
                adjacentHex = targetHex;
                if (teleportArmy->m_facing == ARMY_FACING_RIGHT) {
                    adjacentHex = teleportArmy->GetAdjacentCellIndex(
                        adjacentHex,
                        COMBAT_DIRECTION_EAST
                    );
                    if (adjacentHex == COMBAT_HEX_EMPTY
                        || (m_hexCells[adjacentHex].m_occupantSide != COMBAT_SIDE_NONE
                            && (m_hexCells[adjacentHex].m_occupantSide != targetSide
                                || m_hexCells[adjacentHex].m_occupantIndex != targetIndex))
                        || m_hexCells[adjacentHex].m_blocked != 0) {
                        targetHex--;
                    }
                }
                if (teleportArmy->m_facing == ARMY_FACING_LEFT) {
                    adjacentHex =
                        teleportArmy->GetAdjacentCellIndex(adjacentHex, COMBAT_DIRECTION_WEST);
                    if (adjacentHex == COMBAT_HEX_EMPTY
                        || (m_hexCells[adjacentHex].m_occupantSide != COMBAT_SIDE_NONE
                            && (m_hexCells[adjacentHex].m_occupantSide != targetSide
                                || m_hexCells[adjacentHex].m_occupantIndex != targetIndex))
                        || m_hexCells[adjacentHex].m_blocked != 0) {
                        targetHex++;
                    }
                }
                teleportArmy->m_hex = targetHex;
                switch (teleportArmy->m_facing) {
                    case ARMY_FACING_RIGHT:
                        m_hexCells[teleportArmy->m_hex].m_occupantSide = targetSide;
                        m_hexCells[teleportArmy->m_hex].m_occupantIndex =
                            static_cast<i8>(targetIndex);
                        m_hexCells[teleportArmy->m_hex].m_occupantFrame = ARMY_FACING_LEFT;
                        m_hexCells[teleportArmy->m_hex + 1].m_occupantSide = targetSide;
                        m_hexCells[teleportArmy->m_hex + 1].m_occupantIndex =
                            static_cast<i8>(targetIndex);
                        m_hexCells[teleportArmy->m_hex + 1].m_occupantFrame = ARMY_FACING_RIGHT;
                        break;
                    case ARMY_FACING_LEFT:
                        m_hexCells[teleportArmy->m_hex].m_occupantSide = targetSide;
                        m_hexCells[teleportArmy->m_hex].m_occupantIndex =
                            static_cast<i8>(targetIndex);
                        m_hexCells[teleportArmy->m_hex].m_occupantFrame = ARMY_FACING_RIGHT;
                        m_hexCells[teleportArmy->m_hex - 1].m_occupantSide = targetSide;
                        m_hexCells[teleportArmy->m_hex - 1].m_occupantIndex =
                            static_cast<i8>(targetIndex);
                        m_hexCells[teleportArmy->m_hex - 1].m_occupantFrame = ARMY_FACING_LEFT;
                        break;
                }
                RippleCreature(
                    teleportArmy->m_side,
                    teleportArmy->m_index,
                    COMBAT_RIPPLE_DEATH_WAVE
                );
            } else {
                teleportArmy->m_hex = targetHex;
                m_hexCells[teleportArmy->m_hex].m_occupantSide = targetSide;
                m_hexCells[teleportArmy->m_hex].m_occupantIndex = static_cast<i8>(targetIndex);
                m_hexCells[teleportArmy->m_hex].m_occupantFrame = ARMY_FACING_NONE;
                RippleCreature(
                    teleportArmy->m_side,
                    teleportArmy->m_index,
                    COMBAT_RIPPLE_DEATH_WAVE
                );
            }
            break;
        case SPELL_DISRUPTING_RAY:
            oldDefense = target->m_monster.defense;
            target->m_monster.defense -= SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
            if (target->m_monster.defense < SPELL_MINIMUM_DEFENSE)
                target->m_monster.defense = SPELL_MINIMUM_DEFENSE;
            sprintf(
                gText,
                "Разрушительный луч уменьшает защиту на %d ед.",
                oldDefense - target->m_monster.defense
            );
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            RippleCreature(target->m_side, target->m_index, COMBAT_RIPPLE_WAVE);
            break;
        case SPELL_COLD_RAY:
            DelayMilli(
                static_cast<i32l>(SPELL_COLD_RAY_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
            );
            damage = spellPower * SPELL_COLD_RAY_DAMAGE_PER_POWER;
            if (target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                damage <<= 1;
            if (IS_GOLEM_CREATURE(target->m_monsterType))
                damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(
                &damage,
                SPELL_COLD_RAY,
                m_heroes[(m_currentSide)],
                m_heroes[(OppositeCombatSide(m_currentSide))]
            );
            sprintf(
                gText,
                "Хладный луч наносит %d\n ед. урона отряду %s.",
                static_cast<i32>(damage),
                CREATURE_DISPLAY_NAME(target->m_monsterType, target->m_quantity)
            );
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            target->SpellEffect(gsSpellInfo[(SPELL_COLD_RAY)].combatEffect, 0, 0);
            target->Damage(damage, SPELL_NONE);
            target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            break;
        case SPELL_CHAIN_LIGHTNING:
            ChainLightning(targetHex, spellPower);
            break;
        case SPELL_MAGIC_ARROW:
            if (1) {
                DelayMilli(
                    static_cast<i32l>(SPELL_MAGIC_ARROW_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
                );
                damage = spellPower * SPELL_MAGIC_ARROW_DAMAGE_PER_POWER;
                ModifyDamageForArtifacts(
                    &damage,
                    SPELL_MAGIC_ARROW,
                    m_heroes[(m_currentSide)],
                    m_heroes[(OppositeCombatSide(m_currentSide))]
                );
                sprintf(
                    gText,
                    "Волшебная стрела наносит %d\n ед. урона отряду %s.",
                    static_cast<i32>(damage),
                    CREATURE_DISPLAY_NAME(target->m_monsterType, target->m_quantity)
                );
                CombatMessage(gText, 1, 1, 0);
                missileIcon = gpResourceManager->GetIcon("keep.icn");
                missileAngles[0] = 90.0f;
                missileAngles[1] = 68.5f;
                missileAngles[2] = 45.0f;
                missileAngles[3] = 20.8f;
                missileAngles[4] = 0.0;
                missileAngles[5] = -20.8f;
                missileAngles[6] = -45.0f;
                missileAngles[7] = -68.5f;
                missileAngles[8] = -90.0f;
                ShootMissile(
                    castX,
                    castY,
                    target->MidX(),
                    target->MidY(),
                    missileAngles,
                    missileIcon
                );
                gpResourceManager->Dispose(missileIcon);
                target->Damage(damage, SPELL_NONE);
                target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            }
            break;
        case SPELL_LIGHTNING_BOLT:
            damage = spellPower * SPELL_LIGHTNING_DAMAGE_PER_POWER;
            if (target->m_monsterType == CREATURE_AIR_ELEMENTAL)
                damage <<= 1;
            if (IS_GOLEM_CREATURE(target->m_monsterType))
                damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(
                &damage,
                SPELL_LIGHTNING_BOLT,
                m_heroes[(m_currentSide)],
                m_heroes[(OppositeCombatSide(m_currentSide))]
            );
            sprintf(
                gText,
                "Разряд молнии наносит %d\n ед. урона отряду %s.",
                static_cast<i32>(damage),
                CREATURE_DISPLAY_NAME(target->m_monsterType, target->m_quantity)
            );
            CombatMessage(gText, 1, 1, 0);
            DoBolt(
                1,
                castX,
                castY,
                target->MidX(),
                target->MidY(),
                LIGHTNING_BRANCH_DISTANCE,
                LIGHTNING_BRANCH_LENGTH,
                LIGHTNING_START_WIDTH,
                LIGHTNING_END_WIDTH,
                BOLT_COLOR_LIGHTNING,
                LIGHTNING_MIN_ANGLE,
                LIGHTNING_MAX_ANGLE,
                LIGHTNING_ANGLE_DISTANCE,
                1,
                0,
                0,
                1
            );
            target->SpellEffect(gsSpellInfo[(SPELL_LIGHTNING_BOLT)].combatEffect, 0, 0);
            target->Damage(damage, SPELL_NONE);
            target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            break;
        case SPELL_MASS_CURE:
        case SPELL_MASS_HASTE:
        case SPELL_MASS_SLOW:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_CURSE:
        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT:
        case SPELL_MASS_DISPEL:
        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE:
        case SPELL_MASS_SHIELD:
            CastMassSpell(spell, spellPower);
            break;
        case SPELL_MIRROR_IMAGE:
            MirrorImage(targetHex);
            break;
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            SummonElemental(CREATURE_EARTH_ELEMENTAL, spellPower);
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            SummonElemental(CREATURE_AIR_ELEMENTAL, spellPower);
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            SummonElemental(CREATURE_FIRE_ELEMENTAL, spellPower);
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            SummonElemental(CREATURE_WATER_ELEMENTAL, spellPower);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            Resurrect(spell, targetHex, spellPower);
            break;
        case SPELL_CURE:
            ShowSpellMessage(castByCreature, spell, target);
            target->SpellEffect(gsSpellInfo[(SPELL_CURE)].combatEffect, 0, 0);
            target->Cure(spellPower);
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            break;
        case SPELL_SLOW:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SLOW, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_SLOW)].combatEffect, 0, 0);
            break;
        case SPELL_HASTE:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_HASTE, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_HASTE)].combatEffect, 0, 0);
            break;
        case SPELL_SHIELD:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SHIELD, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_SHIELD)].combatEffect, 0, 0);
            break;
        case SPELL_DRAGON_SLAYER:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_DRAGON_SLAYER)].combatEffect, 0, 0);
            break;
        case SPELL_BLESS:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLESS, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_BLESS)].combatEffect, 0, 0);
            break;
        case SPELL_STONE_SKIN:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_STONESKIN, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_STONE_SKIN)].combatEffect, 0, 0);
            break;
        case SPELL_STEEL_SKIN:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_STEELSKIN, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_STEEL_SKIN)].combatEffect, 0, 0);
            break;
        case SPELL_CURSE:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_CURSE, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_CURSE)].combatEffect, 0, 0);
            break;
        case SPELL_BERSERKER:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BERSERK, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_BERSERKER)].combatEffect, 0, 0);
            break;
        case SPELL_HYPNOTIZE:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_HYPNOTIZE, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_HYPNOTIZE)].combatEffect, 0, 0);
            break;
        case SPELL_PARALYZE:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_PARALYZE, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_PARALYZE)].combatEffect, 0, 0);
            break;
        case CREATURE_SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target);
            target->DispelGood();
            target->SpellEffect(gsSpellInfo[(SPELL_DISPEL)].combatEffect, 0, 1);
            break;
        case SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target);
            target->DispelGood();
            target->SpellEffect(gsSpellInfo[(SPELL_DISPEL)].combatEffect, 0, 0);
            for (influence = ARMY_SPELL_INFLUENCE_HASTE; influence < ARMY_SPELL_INFLUENCE_COUNT;
                 influence++)
                target->CancelIndividualSpell(influence);
            break;
        case SPELL_BLIND:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLIND, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_BLIND)].combatEffect, 0, 0);
            break;
        case SPELL_BLOOD_LUST:
            ShowSpellMessage(castByCreature, spell, target);
            BloodLustEffect(target, MONSTER_ABILITY_FLAG_BLOOD_LUST);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLOODLUST, SPELL_BLOOD_LUST_DURATION);
            break;
        case SPELL_ANTI_MAGIC:
            ShowSpellMessage(castByCreature, spell, target);
            target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_ANTI_MAGIC, spellPower);
            target->SpellEffect(gsSpellInfo[(SPELL_ANTI_MAGIC)].combatEffect, 0, 0);
            break;
        case CREATURE_SPELL_PETRIFY:
            ShowSpellMessage(castByCreature, spell, target);
            TurnToStone(target);
            break;
        case SPELL_COLD_RING:
            Fireball(targetHex, SPELL_COLD_RING);
            break;
        case SPELL_FIREBALL:
            Fireball(targetHex, SPELL_FIREBALL);
            break;
        case SPELL_FIREBLAST:
            Fireball(targetHex, SPELL_FIREBLAST);
            break;
        case SPELL_METEOR_SHOWER:
            MeteorShower(targetHex);
            break;
        case SPELL_ELEMENTAL_STORM:
            ElementalStorm();
            break;
        case SPELL_ARMAGEDDON:
            Armageddon();
            break;
        case SPELL_EARTHQUAKE:
            Earthquake();
            break;
        default:
            DefaultSpell(targetHex);
            break;
    }

cast_done:

    for (side = 0; side < COMBAT_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; armyIndex++) {
            army& combatArmy = m_armies[(side)][armyIndex];
            combatArmy.m_killPending = false;
            combatArmy.m_deathPending = false;
            combatArmy.m_damagePending = false;
            combatArmy.m_drawState = ARMY_DRAW_NORMAL;
            combatArmy.m_animationState = false;
            combatArmy.m_lastTargetHex = COMBAT_HEX_EMPTY;
        }
    }

    if (castByCreature == 0) {
        m_heroAnimationState[(m_currentSide)]++;
        m_heroAnimationFrame[(m_currentSide)] = 0;
        while (m_heroAnimationFrame[(m_currentSide)]
               < sCmbtHero[m_heroSpriteIndex[(m_currentSide)]]
                     .animationFrameCount[m_heroAnimationState[(m_currentSide)]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[(m_currentSide)]++;
        }
        m_heroAnimationState[(m_currentSide)] = 0;
        m_heroAnimationFrame[(m_currentSide)] = 0;
        DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }
    WaitEndSample(&spellSample);
    CheckChangeSelector();
}

void combatManager::DefaultSpell(i32 targetHex) {
    if (!ValidHex(targetHex) || m_hexCells[targetHex].m_occupantSide < COMBAT_ATTACKER_SIDE)
        return;
    army* target =
        &m_armies[(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];
    target->SpellEffect(gsSpellInfo[(m_selectedSpell)].combatEffect, 0, 1);
}

void combatManager::Fireball(i32 targetHex, SpellType spell) {
    if (!ValidHex(targetHex))
        return;

    i32l baseDamage;
    i32l damage;
    i32 frameCount;
    i32 affectedCount;
    army* target;
    i32 targetX;
    icon* spellIcon;
    i32 targetY;
    i32 frame;
    i16 affectedHexes[SPELL_FIREBALL_AFFECTED_HEX_COUNT];
    b32 anyAffected;

    targetX = m_hexCells[targetHex].m_x;
    targetY = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
    if (!gbNoShowCombat) {
        frameCount = SPELL_FIREBALL_FRAME_COUNT;
        if (spell == SPELL_FIREBALL)
            spellIcon = gpResourceManager->GetIcon("fireball.icn");
        else if (spell == SPELL_FIREBLAST)
            spellIcon = gpResourceManager->GetIcon("firebal2.icn");
        else {
            spellIcon = gpResourceManager->GetIcon("coldring.icn");
            frameCount = SPELL_COLD_RING_FRAME_COUNT;
        }

        for (frame = 0; frame < frameCount; ++frame) {
            glTimers[0] = COMBAT_DEADLINE(SPELL_AREA_ANIMATION_DELAY);
            IconToBitmap(
                spellIcon,
                gpWindowManager->m_screen,
                targetX,
                targetY,
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
            if (spell == SPELL_COLD_RING) {
                FlipIconToBitmap(
                    spellIcon,
                    gpWindowManager->m_screen,
                    targetX,
                    targetY,
                    frame,
                    ICON_DRAW_CLIP,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT,
                    0
                );
            }
            UpdateCombatArea();
            DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            DelayTil(&glTimers[0]);
        }
        gpResourceManager->Dispose(spellIcon);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target =
        m_armies[(m_currentSide)] + m_currentArmyIndex;
    for (frame = 0; frame < SPELL_FIREBALL_AFFECTED_HEX_COUNT; ++frame)
        affectedHexes[frame] = COMBAT_HEX_EMPTY;
    if (spell != SPELL_COLD_RING)
        affectedHexes[0] = static_cast<i16>(targetHex);

    for (frame = (COMBAT_DIRECTION_NORTHEAST); frame < SPELL_ADJACENT_DIRECTION_COUNT;
         ++frame) {
        affectedHexes[frame + 1] = static_cast<i16>(
            GetAdjacentCellIndexNoArmy(targetHex, static_cast<CombatHexDirection>(frame))
        );
        if (spell == SPELL_FIREBLAST) {
            affectedHexes[frame + SPELL_FIREBLAST_SECOND_RING_FIRST] =
                static_cast<i16>(target->GetAdjacentCellIndex(
                    affectedHexes[frame + 1],
                    static_cast<CombatHexDirection>(frame)
                ));
        }
    }
    if (spell == SPELL_FIREBLAST) {
        affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] =
            static_cast<i16>(targetHex - SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] < 0)
            affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] = COMBAT_HEX_EMPTY;
        affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] =
            static_cast<i16>(targetHex + SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] >= COMBAT_HEX_COUNT)
            affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] = COMBAT_HEX_EMPTY;
        affectedHexes[SPELL_FIREBLAST_CORNER_FIRST] = static_cast<i16>(GetAdjacentCellIndexNoArmy(
            affectedHexes[FIREBLAST_EAST_FIRST_RING],
            COMBAT_DIRECTION_NORTHEAST
        ));
        affectedHexes[SPELL_FIREBLAST_CORNER_SECOND] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(
                affectedHexes[FIREBLAST_EAST_FIRST_RING],
                COMBAT_DIRECTION_SOUTHEAST
            ));
        affectedHexes[SPELL_FIREBLAST_CORNER_THIRD] = static_cast<i16>(GetAdjacentCellIndexNoArmy(
            affectedHexes[FIREBLAST_WEST_FIRST_RING],
            COMBAT_DIRECTION_NORTHWEST
        ));
        affectedHexes[SPELL_FIREBLAST_CORNER_FOURTH] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(
                affectedHexes[FIREBLAST_WEST_FIRST_RING],
                COMBAT_DIRECTION_SOUTHWEST
            ));
    }

    baseDamage = m_spellPower[(m_currentSide)] * SPELL_FIREBALL_DAMAGE_PER_POWER;
    ClearEffects();
    anyAffected = false;
    affectedCount = SPELL_FIREBALL_AFFECTED_HEX_COUNT;
    for (frame = 0; frame < affectedCount; ++frame) {
        if (affectedHexes[frame] != COMBAT_HEX_EMPTY
            && m_hexCells[affectedHexes[frame]].m_occupantSide != COMBAT_SIDE_NONE) {
            target = &m_armies[(m_hexCells[affectedHexes[frame]].m_occupantSide)]
                                [m_hexCells[affectedHexes[frame]].m_occupantIndex];
            if (target->SpellCastWorks(spell)
                && !gArmyEffected[(m_hexCells[affectedHexes[frame]].m_occupantSide)]
                                 [m_hexCells[affectedHexes[frame]].m_occupantIndex]) {
                gArmyEffected[(m_hexCells[affectedHexes[frame]].m_occupantSide)]
                             [m_hexCells[affectedHexes[frame]].m_occupantIndex] = 1;
                if (target->m_damagePending == 0) {
                    damage = baseDamage;
                    if (spell == SPELL_COLD_RING
                        && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                        damage <<= 1;
                    if ((spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                        && target->m_monsterType == CREATURE_WATER_ELEMENTAL)
                        damage *= ELEMENTAL_WEAKNESS_MULTIPLIER;
                    if (IS_GOLEM_CREATURE(target->m_monsterType)) {
                        damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                    }
                    target->Damage(damage, spell);
                    anyAffected = true;
                }
            }
        }
    }
    if (anyAffected) {
        ModifyDamageForArtifacts(
            &baseDamage,
            spell,
            m_heroes[(m_currentSide)],
            m_heroes[(OppositeCombatSide(m_currentSide))]
        );
        if (spell == SPELL_COLD_RING)
            sprintf(gText, "Ледяное кольцо наносит %d ед. урона.", static_cast<i32>(baseDamage));
        else
            sprintf(gText, "Огненный шар наносит %d ед. урона.", static_cast<i32>(baseDamage));
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    }
}

void combatManager::MeteorShower(i32 targetHex) {
    if (!ValidHex(targetHex))
        return;

    i32 direction;
    i32 hexes[SPELL_METEOR_AFFECTED_HEX_COUNT];
    i32 baseDamage;
    i32 frame;
    i32l damage;
    army* target;
    b32 anyAffected;

    target = &m_armies[(m_currentSide)][0] + m_currentArmyIndex;
    hexes[0] = targetHex;
    for (direction = (COMBAT_DIRECTION_NORTHEAST); direction < SPELL_ADJACENT_DIRECTION_COUNT;
         ++direction) {
        hexes[direction + 1] =
            GetAdjacentCellIndexNoArmy(targetHex, static_cast<CombatHexDirection>(direction));
    }

    if (!gbNoShowCombat) {
        icon* meteorIcon = gpResourceManager->GetIcon("meteor.icn");
        for (direction = 0; direction < SPELL_METEOR_PASS_COUNT; ++direction) {
            for (frame = 0; frame < SPELL_METEOR_FRAME_COUNT; ++frame) {
                glTimers[0] = static_cast<i32>(
                    KBTickCount()
                    + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_METEOR_ANIMATION_DELAY
                );
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                meteorIcon->CombatClipDrawToBuffer(
                    m_hexCells[targetHex].m_x,
                    m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET,
                    frame,
                    NULL,
                    ICON_DRAW_NORMAL
                );
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(meteorIcon);
    }

    baseDamage = m_spellPower[(m_currentSide)] * SPELL_METEOR_DAMAGE_PER_POWER;
    ClearEffects();
    anyAffected = false;
    for (direction = 0; direction < SPELL_METEOR_AFFECTED_HEX_COUNT; ++direction) {
        if (hexes[direction] != COMBAT_HEX_EMPTY
            && m_hexCells[hexes[direction]].m_occupantSide != COMBAT_SIDE_NONE) {
            target = &m_armies[(m_hexCells[hexes[direction]].m_occupantSide)]
                              [m_hexCells[hexes[direction]].m_occupantIndex];
            if (target->SpellCastWorks(SPELL_METEOR_SHOWER)
                && !gArmyEffected[(m_hexCells[hexes[direction]].m_occupantSide)]
                                 [m_hexCells[hexes[direction]].m_occupantIndex]) {
                gArmyEffected[(m_hexCells[hexes[direction]].m_occupantSide)]
                             [m_hexCells[hexes[direction]].m_occupantIndex] = 1;
                if (target->m_damagePending == 0) {
                    damage = baseDamage;
                    if (target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
                        damage <<= 1;
                    target->Damage(damage, SPELL_METEOR_SHOWER);
                    anyAffected = true;
                }
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, "Звездопад наносит %d ед. урона.", baseDamage);
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    }
}

void combatManager::ElementalStorm(void) {
    i32 baseDam;
    i32 column;
    army* stack;
    i32 member;
    i32 frame;
    i32 tileRow;
    i32 iter;
    i32 whichSide;
    b32 hit;
    i32l damage;
    icon* stormIcon;
    SLimitData limits;

    if (!gbNoShowCombat) {
        stormIcon = gpResourceManager->GetIcon("storm.icn");
        for (iter = 0; iter < SPELL_STORM_PASS_COUNT; ++iter) {
            for (frame = 0; frame < SPELL_STORM_FRAME_COUNT; ++frame) {
                glTimers[0] = COMBAT_DEADLINE(SPELL_AREA_ANIMATION_DELAY);
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                for (tileRow = 0; tileRow < SPELL_STORM_ROW_COUNT; ++tileRow) {
                    for (column = 0; column < SPELL_STORM_COLUMN_COUNT; ++column) {
                        stormIcon->CombatClipDrawToBuffer(
                            column * SPELL_STORM_TILE_SIZE,
                            tileRow * SPELL_STORM_TILE_SIZE,
                            (frame + column * SPELL_STORM_FRAME_COLUMN_STEP + tileRow)
                                % SPELL_STORM_FRAME_COUNT,
                            &limits,
                            ICON_DRAW_NORMAL
                        );
                    }
                }
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(stormIcon);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    hit = false;
    baseDam = m_spellPower[(m_currentSide)] * SPELL_ELEMENTAL_STORM_DAMAGE_PER_POWER;
    for (whichSide = 0; whichSide < COMBAT_SIDE_COUNT; ++whichSide) {
        for (member = 0; member < m_armyCount[whichSide]; ++member) {
            stack = m_armies[whichSide] + member;
            if (stack->SpellCastWorks(SPELL_ELEMENTAL_STORM)) {
                damage = baseDam;
                if (stack->m_monsterType == CREATURE_AIR_ELEMENTAL)
                    damage <<= 1;
                if (IS_GOLEM_CREATURE(stack->m_monsterType)) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[whichSide] && m_heroes[whichSide]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                stack->Damage(damage, SPELL_ELEMENTAL_STORM);
                hit = true;
            }
        }
    }
    if (hit) {
        sprintf(
            gText,
            "Буря стихий наносит %d ед. урона.",
            baseDam
        );
        CombatMessage(gText, 1, 1, 0);
        stack->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    }
}

void combatManager::Armageddon(void) {
    i32 baseDamage;
    b32 anyAffected;
    army* target;
    i32 side;
    i32 armyIndex;
    i32l damage;
    palette* originalPalette;
    palette* effectPalette;
    i8* effectDataRestore;
    i8* originalData;
    i32 restorePass;
    i32 component;

    baseDamage = m_spellPower[(m_currentSide)] * SPELL_ARMAGEDDON_DAMAGE_PER_POWER;
    anyAffected = false;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            target = &m_armies[side][armyIndex];
            if (target->SpellCastWorks(SPELL_ARMAGEDDON)) {
                damage = baseDamage;
                if (IS_GOLEM_CREATURE(target->m_monsterType)) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side] && m_heroes[side]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                target->Damage(damage, SPELL_ARMAGEDDON);
                anyAffected = true;
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, "Армагеддон наносит %d ед. урона.", baseDamage);
        CombatMessage(gText, 1, 1, 0);
    }

    originalPalette = NULL;
    effectPalette = NULL;
    if (!gbNoShowCombat) {
        i8* effectData;
        i32 pass;
        i32 color;

        gpWindowManager->m_updateFlags = 0;
        originalPalette = gpResourceManager->GetPalette("kb.pal");
        effectPalette = new palette;
        if (!effectPalette)
            MemError();
        memcpy(effectPalette->Data(), originalPalette->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);

        glTimers[0] = COMBAT_DEADLINE(SPELL_AREA_ANIMATION_DELAY);
        effectData = effectPalette->Data();
        for (pass = 0; pass < SPELL_ARMAGEDDON_BRIGHTEN_PASS_COUNT; ++pass) {
            for (color = 0; color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color) {
                if (effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_RED)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_RED)];
                if (effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_GREEN)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_GREEN)];
                if (effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_BLUE)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_BLUE)];
            }
            DelayTil(&glTimers[0]);
            SetPalette(effectData, 1);
            glTimers[0] = COMBAT_DEADLINE(SPELL_ARMAGEDDON_PALETTE_DELAY);
        }

        i32 shakeOffsets[SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT][(COORDINATE_AXIS_COUNT)] = {
            {2, 2},
            {4, 1},
            {3, -2},
            {0, -6},
            {2, -2},
            {-1, 3},
            {-5, 4},
            {-8, 6},
            {-4, 2},
            {-1, 1},
            {-3, -3},
            {-7, -5},
            {-5, -7},
            {-2, -3},
            {0, 0}
        };
        float scale;
        i32 frame;
        i32 offsetX;
        i32 offsetY;
        i32 timer;
        i32 copyWidth;
        i32 copyHeight;
        u8* source;
        u8* destination;
        i32 copyRow;
        i32 shakePass;

        gpMouseManager->HideColorPointer();
        memcpy(
            m_backgroundBuffer->m_pixels,
            gpWindowManager->m_screen->m_pixels,
            COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
        );

        for (shakePass = 0; shakePass < SPELL_ARMAGEDDON_SHAKE_PASS_COUNT; ++shakePass) {
            scale = static_cast<float>(
                (SPELL_ARMAGEDDON_SHAKE_PASS_COUNT - shakePass) * SPELL_ARMAGEDDON_SHAKE_SCALE
            );
            for (frame = 0; frame < SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT; ++frame) {
                if (shakePass == 0) {
                    for (color = 0; color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color) {
                        if (effectData
                                [color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_GREEN)]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData
                                [color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_GREEN)] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                        if (effectData
                                [color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_BLUE)]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData
                                [color * (PALETTE_CHANNEL_COUNT) + (PALETTE_CHANNEL_BLUE)] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                    }
                    SetPalette(effectData, 1);
                }

                offsetX =
                    static_cast<i32>(shakeOffsets[frame][(COORDINATE_AXIS_X)] * scale);
                offsetY = static_cast<i32>(shakeOffsets[frame][(COORDINATE_AXIS_Y)] * scale);
                timer = COMBAT_DEADLINE(SPELL_ARMAGEDDON_SHAKE_DELAY);
                PollSound();
                copyWidth =
                    COMBAT_SCREEN_WIDTH
                    - abs(static_cast<i32>(shakeOffsets[frame][(COORDINATE_AXIS_X)] * scale));
                copyHeight =
                    COMBAT_AREA_HEIGHT
                    - abs(static_cast<i32>(shakeOffsets[frame][(COORDINATE_AXIS_Y)] * scale))
                    - 1;
                source = m_backgroundBuffer->m_pixels + (offsetX >= 0 ? offsetX : 0)
                          + (offsetY >= 0 ? offsetY : 0) * COMBAT_SCREEN_WIDTH;
                destination = gpWindowManager->m_screen->m_pixels
                               + (offsetX > 0 ? 0 : -offsetX)
                               + (offsetY > 0 ? 0 : -offsetY) * COMBAT_SCREEN_WIDTH;
                for (copyRow = 0; copyRow < copyHeight; ++copyRow) {
                    memcpy(destination, source, copyWidth);
                    source += COMBAT_SCREEN_WIDTH;
                    destination += COMBAT_SCREEN_WIDTH;
                }
                BlitBitmapToScreen(
                    gpWindowManager->m_screen,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT,
                    0,
                    0
                );
                DelayTil(&timer);
            }
        }
    }

    m_backgroundDrawn = false;
    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);

    effectDataRestore = effectPalette->Data();
    originalData = originalPalette->Data();
    for (restorePass = 0; restorePass < SPELL_ARMAGEDDON_RESTORE_PASS_COUNT; ++restorePass) {
        for (component = 0; component < SPELL_ARMAGEDDON_PALETTE_SIZE; ++component) {
            if (static_cast<u8>(effectDataRestore[component])
                == static_cast<u8>(originalData[component]))
                continue;

            if (static_cast<u8>(effectDataRestore[component])
                > static_cast<u8>(originalData[component])) {
                if (static_cast<u8>(effectDataRestore[component])
                        - SPELL_ARMAGEDDON_CHANNEL_STEP
                    > static_cast<u8>(originalData[component]))
                    effectDataRestore[component] -= SPELL_ARMAGEDDON_CHANNEL_STEP;
                else
                    effectDataRestore[component] = originalData[component];
            } else if (static_cast<u8>(effectDataRestore[component])
                           + SPELL_ARMAGEDDON_CHANNEL_STEP
                       < static_cast<u8>(originalData[component])) {
                effectDataRestore[component] += SPELL_ARMAGEDDON_CHANNEL_STEP;
            } else {
                effectDataRestore[component] = originalData[component];
            }
        }
        SetPalette(effectDataRestore, 1);
        DelayMilli(
            static_cast<i32l>(
                SPELL_ARMAGEDDON_PALETTE_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            )
        );
    }
    SetPalette(originalPalette->Data(), 1);
    gpWindowManager->m_updateFlags = 1;
    gpResourceManager->Dispose(originalPalette);
    delete effectPalette;
    gpMouseManager->ShowColorPointer();
}

void combatManager::TurnToStone(army* target) {
    ResetLimitCreature();
    m_limitCreatureCount[(target->m_side)][target->m_index]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_PETRIFIED, SPELL_PETRIFY_DURATION);
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1,
        SPELL_PETRIFY_FIZZLE_STEPS,
        NULL,
        NULL
    );
}

void combatManager::BloodLustEffect(army* target, MonsterFlags effect) {
    ResetLimitCreature();
    m_limitCreatureCount[(target->m_side)][target->m_index]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->m_monster.flags.abilityFlags |= effect;
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1,
        SPELL_BLOOD_LUST_FIZZLE_STEPS,
        NULL,
        NULL
    );
    gpWindowManager->SaveFizzleSource(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    ((target->m_monster.flags.abilityFlags) &= ~(effect));
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1,
        SPELL_BLOOD_LUST_FIZZLE_STEPS,
        NULL,
        NULL
    );
}

void combatManager::Ripple(i32 strength) {
    i32 unusedValue1 [[maybe_unused]];
    i32 unusedValue2 [[maybe_unused]];

    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    DoRipple(m_backgroundBuffer, gpWindowManager->m_screen, COMBAT_AREA_HEIGHT, strength);
    m_backgroundDrawn = false;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::Blur(i32 redAdjust, i32 greenAdjust, i32 blueAdjust) {
    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    DoBlur(
        m_backgroundBuffer,
        gpWindowManager->m_screen,
        COMBAT_AREA_HEIGHT,
        redAdjust,
        greenAdjust,
        blueAdjust
    );
    m_backgroundDrawn = false;
}

void combatManager::ResetBoltAngle(SBolt* bolt) {
    i32 angleX;
    float averageAngle;
    i32 unusedBoltAngleWord [[maybe_unused]];
    i32 currentWidth;
    i32 angleY;
    i32 distance;
    i32 widthDelta;
    i32 deltaX;
    i32 deltaY;
    float randomAngle;

    if (bolt->nearTarget != 0)
        return;

    deltaX = abs(bolt->endX - bolt->pixelX);
    deltaY = abs(bolt->endY - bolt->pixelY);
    distance = INTEGER_VECTOR_LENGTH(deltaX, deltaY);
    if (distance > bolt->totalDistance)
        bolt->distanceRatio = 0;
    else
        bolt->distanceRatio = static_cast<float>(bolt->totalDistance - distance)
                              / (static_cast<float>(bolt->totalDistance));

    if (bolt->startWidth != bolt->endWidth) {
        widthDelta = bolt->endWidth - bolt->startWidth;
        if (widthDelta > 0)
            ++widthDelta;
        else
            --widthDelta;
        currentWidth = bolt->startWidth + static_cast<i32>(widthDelta * bolt->distanceRatio);
        if (currentWidth < 1)
            currentWidth = 1;
        bolt->width = currentWidth;
    }
    bolt->widthFirst = -(bolt->width >> 1);
    bolt->widthLast = bolt->widthFirst + bolt->width - 1;

    angleX = bolt->endX - bolt->pixelX;
    angleY = bolt->endY - bolt->pixelY;
    bolt->baseAngle =
        static_cast<float>(atan2(static_cast<double>(angleX), static_cast<double>(angleY)));
    averageAngle =
        static_cast<float>((bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR);
    averageAngle = averageAngle
                     * ((BOLT_INITIAL_ANGLE_BIAS - bolt->distanceRatio)
                        / BOLT_INITIAL_ANGLE_DIVISOR);
    bolt->angle = averageAngle + bolt->baseAngle;

    if (bolt->minAngle == 0 && bolt->maxAngle == 0)
        return;
    if (distance > bolt->angleDistance * BOLT_ANGLE_DISTANCE_FACTOR || bolt->forceAngle != 0) {
        if (bolt->minAngle == bolt->maxAngle)
            randomAngle = static_cast<float>(bolt->minAngle) / (BOLT_ANGLE_PERCENT_SCALE);
        else
            randomAngle = static_cast<float>(Random(bolt->minAngle, bolt->maxAngle))
                          / (BOLT_ANGLE_PERCENT_SCALE);
        randomAngle = randomAngle
                      * ((BOLT_RANDOM_ANGLE_BIAS - bolt->distanceRatio)
                         / BOLT_RANDOM_ANGLE_DIVISOR);
        bolt->baseAngle = randomAngle + bolt->baseAngle;
    }
}

void combatManager::DrawBolt(SBolt* bolt, i32 stepCount) {
    i32 widthRollResult [[maybe_unused]];
    i32 distance;
    i32 unusedBoltWord [[maybe_unused]];
    i32 unusedDrawWord [[maybe_unused]];
    i32 beamOffset;
    i32 drawX;
    i32 drawStep;
    i32 drawY;
    u8 color;
    i32 widthFirst;
    i32 previousX;
    i32 previousY;
    i32 lastBeamOffset;
    i32 edgeShade;

    previousX = static_cast<i32>(bolt->currentX);
    previousY = static_cast<i32>(bolt->currentY);
    widthFirst = bolt->widthFirst;
    lastBeamOffset = bolt->widthLast;
    widthRollResult = Random(BOLT_RANDOM_WIDTH_LOW, BOLT_RANDOM_WIDTH_HIGH);
    for (drawStep = 0; drawStep < stepCount; ++drawStep) {
        bolt->currentX = bolt->currentX + sin(static_cast<double>(bolt->baseAngle));
        bolt->currentY = bolt->currentY + cos(static_cast<double>(bolt->baseAngle));
        bolt->pixelX = static_cast<i32>(bolt->currentX);
        bolt->pixelY = static_cast<i32>(bolt->currentY);
        if (bolt->pixelX < 0) {
            bolt->pixelX = 0;
            bolt->currentX = 0;
        }
        if (COMBAT_SCREEN_WIDTH - 1 < bolt->pixelX) {
            bolt->pixelX = COMBAT_SCREEN_WIDTH - 1;
            bolt->currentX = static_cast<float>(COMBAT_SCREEN_WIDTH - 1);
        }
        if (bolt->pixelY < 0) {
            bolt->pixelY = 0;
            bolt->currentY = 0;
        }
        if (COMBAT_AREA_HEIGHT - 1 < bolt->pixelY) {
            bolt->pixelY = COMBAT_AREA_HEIGHT - 1;
            bolt->currentY = static_cast<float>(COMBAT_AREA_HEIGHT - 1);
        }

        drawX = bolt->pixelX;
        drawY = bolt->pixelY;
        if (bolt->pixelX == previousX) {
            if (bolt->pixelY == previousY)
                continue;
        }
        {
            previousX = bolt->pixelX;
            previousY = bolt->pixelY;
            for (beamOffset = widthFirst; beamOffset <= lastBeamOffset; ++beamOffset) {
                if (bolt->drawVertically != 0)
                    drawY = bolt->pixelY + beamOffset;
                else
                    drawX = bolt->pixelX + beamOffset;
                if (drawX < 0 || drawX >= COMBAT_SCREEN_WIDTH || drawY < 0
                    || drawY >= COMBAT_AREA_HEIGHT)
                    continue;
                {
                    if (beamOffset < 0)
                        edgeShade = -(widthFirst - beamOffset);
                    else
                        edgeShade = lastBeamOffset - beamOffset;
                    switch (bolt->colorMode) {
                        case BOLT_COLOR_RED_TABLE:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] =
                                gColorTableRed[static_cast<i8>(
                                    (gpWindowManager->m_screen->m_pixels
                                     + drawY * COMBAT_SCREEN_WIDTH)[drawX]
                                )];
                            break;
                        case BOLT_COLOR_RED_BEAM:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] = uRedBeam[edgeShade];
                            break;
                        case BOLT_COLOR_RAINBOW_FORWARD:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] =
                                uRainbow[beamOffset - widthFirst];
                            break;
                        case BOLT_COLOR_RAINBOW_REVERSE:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] =
                                uRainbow[BOLT_RAINBOW_LAST_INDEX - (beamOffset - widthFirst)];
                            break;
                        case BOLT_COLOR_LIGHTNING: {
                            if (edgeShade == 0)
                                color = BOLT_LIGHTNING_SHADE_0;
                            else if (edgeShade == 1)
                                color = BOLT_LIGHTNING_SHADE_1;
                            else if (edgeShade == 2)
                                color = BOLT_LIGHTNING_SHADE_2;
                            else if (edgeShade == 3)
                                color = BOLT_LIGHTNING_SHADE_3;
                            else if (edgeShade == 4)
                                color = BOLT_LIGHTNING_SHADE_4;
                            else
                                color = BOLT_LIGHTNING_SHADE_5;
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] = color;
                            break;
                        }
                        default:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX] =
                                static_cast<u8>(bolt->colorMode);
                            break;
                    }
                }
            }

            distance = MANHATTAN_LENGTH(bolt->endX - bolt->pixelX, bolt->endY - bolt->pixelY);
            if (bolt->nearTarget != 0) {
                if (distance > bolt->nearestDistance + 1
                    || distance <= BOLT_FINISHED_DISTANCE_MAX) {
                    bolt->finished = true;
                    return;
                } else if (distance < bolt->nearestDistance) {
                    bolt->nearestDistance = distance;
                }
            } else {
                if (distance < BOLT_NEAR_TARGET_DISTANCE) {
                    bolt->nearTarget = true;
                    bolt->nearestDistance = distance;
                }
            }
        }
    }
}

void combatManager::AddBolt(
    SBolt* bolt,
    i32 startX,
    i32 startY,
    i32 endX,
    i32 endY,
    i32 branchDistance,
    i32 startWidth,
    i32 endWidth,
    BoltColorMode colorMode,
    i32 minAngle,
    i32 maxAngle,
    i32 angleDistance,
    i32 forceAngle
) {
    if (startX < 0)
        startX = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < startX)
        startX = COMBAT_SCREEN_WIDTH - 1;
    if (startY < 0)
        startY = 0;
    if (COMBAT_AREA_HEIGHT - 1 < startY)
        startY = COMBAT_AREA_HEIGHT - 1;
    if (endX < 0)
        endX = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < endX)
        endX = COMBAT_SCREEN_WIDTH - 1;
    if (endY < 0)
        endY = 0;
    if (COMBAT_AREA_HEIGHT - 1 < endY)
        endY = COMBAT_AREA_HEIGHT - 1;

    bolt->startX = startX;
    bolt->startY = startY;
    bolt->endX = endX;
    bolt->endY = endY;
    bolt->branchDistance = branchDistance;
    bolt->width = startWidth;
    bolt->startWidth = startWidth;
    bolt->endWidth = endWidth;
    bolt->colorMode = colorMode;
    bolt->minAngle = minAngle;
    bolt->maxAngle = maxAngle;
    bolt->angleDistance = angleDistance;
    bolt->currentX = static_cast<float>(startX);
    bolt->currentY = static_cast<float>(startY);
    bolt->pixelX = startX;
    bolt->pixelY = startY;
    bolt->finished = false;
    bolt->nearTarget = false;
    bolt->lastBranchX = startX;
    bolt->lastBranchY = startY;
    bolt->distanceRatio = 0;
    bolt->forceAngle = forceAngle;

    if (colorMode == BOLT_COLOR_RAINBOW_FORWARD || colorMode == BOLT_COLOR_RAINBOW_REVERSE) {
        if (startX <= 0 || startX >= COMBAT_SCREEN_WIDTH - 1)
            bolt->drawVertically = true;
        else
            bolt->drawVertically = false;
    } else
        bolt->drawVertically = abs(endX - startX) > abs(endY - startY);

    i32 deltaX = abs(endX - startX);
    i32 deltaY = abs(endY - startY);
    bolt->totalDistance = INTEGER_VECTOR_LENGTH(deltaX, deltaY);
    ResetBoltAngle(bolt);
}

void combatManager::DoBolt(
    i32 managePointer,
    i32 startX,
    i32 startY,
    i32 endX,
    i32 endY,
    i32 branchDistance,
    i32 branchLength,
    i32 startWidth,
    i32 endWidth,
    BoltColorMode colorMode,
    i32 minAngle,
    i32 maxAngle,
    i32 angleDistance,
    i32 unusedParameter [[maybe_unused]],
    i32 forceAngle,
    i32 frameDelay,
    i32 brightenPalette
) {
    i32 drawDistance;
    b32 allFinished;
    i32 drawPassCount;
    i32 branchChance;
    i32 deadline;
    palette* originalPalette;
    palette* effectPalette;
    i8* effectData;
    i32 index;
    SBolt* bolts;
    i32 extentPadding;
    i32 boltCount;
    i32 drawPass;
    i32 minY;
    i32 minX;
    i32 maxY;
    i32 maxX;
    i32 oldBoltCount;
    i32 remainingDistance;
    i32 branchSeparation;
    float branchAngle;
    float currentAngle;
    i32 childDistance;
    i32 childX;
    i32 childY;
    i32 childWidth;

    if (managePointer != 0)
        gpMouseManager->HideColorPointer();

    drawDistance = angleDistance;
    allFinished = false;
    drawPassCount = (angleDistance - 1) / drawDistance + 1;
    branchChance = branchDistance * BOLT_ANGLE_PERCENT_SCALE / angleDistance;
    deadline = KBTickCount();
    gpWindowManager->m_updateFlags = 0;

    originalPalette = NULL;
    effectPalette = NULL;
    if (brightenPalette != 0) {
        originalPalette = gpResourceManager->GetPalette("kb.pal");
        effectPalette = new palette;
        if (!effectPalette)
            MemError();
        memcpy(effectPalette->Data(), originalPalette->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);
        effectData = effectPalette->Data();
        for (index = 0; index < SPELL_ARMAGEDDON_PALETTE_SIZE; ++index) {
            effectData[index] += BOLT_PALETTE_BRIGHTEN_STEP;
            if (effectData[index] > BOLT_PALETTE_COMPONENT_MAX)
                effectData[index] = BOLT_PALETTE_COMPONENT_MAX;
        }
        SetPalette(effectData, 1);
    }

    if (startX > endX) {
        minAngle = -minAngle;
        maxAngle = -maxAngle;
    }
    if (minAngle > maxAngle) {
        index = maxAngle;
        maxAngle = minAngle;
        minAngle = index;
    }

    bolts = new SBolt[BOLT_MAX_COUNT];
    extentPadding = (startWidth > endWidth ? startWidth : endWidth) >> 1;
    AddBolt(
        bolts,
        startX,
        startY,
        endX,
        endY,
        branchDistance,
        startWidth,
        endWidth,
        colorMode,
        minAngle,
        maxAngle,
        angleDistance,
        forceAngle
    );

    boltCount = 1;
    while (allFinished == 0) {
        for (drawPass = 0; drawPass < drawPassCount; ++drawPass) {
            allFinished = true;
            minY = BOLT_EXTENT_SENTINEL;
            minX = minY;
            maxY = -1;
            maxX = maxY;
            for (index = 0; index < boltCount; ++index) {
                if (bolts[index].finished == 0) {
                    if (bolts[index].pixelX > maxX)
                        maxX = bolts[index].pixelX;
                    if (bolts[index].pixelX < minX)
                        minX = bolts[index].pixelX;
                    if (bolts[index].pixelY > maxY)
                        maxY = bolts[index].pixelY;
                    if (bolts[index].pixelY < minY)
                        minY = bolts[index].pixelY;
                    DrawBolt(&bolts[index], drawDistance);
                    if (bolts[index].pixelX > maxX)
                        maxX = bolts[index].pixelX;
                    if (bolts[index].pixelX < minX)
                        minX = bolts[index].pixelX;
                    if (bolts[index].pixelY > maxY)
                        maxY = bolts[index].pixelY;
                    if (bolts[index].pixelY < minY)
                        minY = bolts[index].pixelY;
                }
            }

            maxX += extentPadding;
            minX -= extentPadding;
            maxY += extentPadding;
            minY -= extentPadding;
            if (minX < 0)
                minX = 0;
            if (minY < 0)
                minY = 0;
            if (COMBAT_SCREEN_WIDTH - 1 < maxX)
                maxX = COMBAT_SCREEN_WIDTH - 1;
            if (COMBAT_AREA_HEIGHT - 1 < maxY)
                maxY = COMBAT_AREA_HEIGHT - 1;

            DelayTil(&deadline);
            deadline = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
            );
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                minX,
                minY,
                maxX - minX + 1,
                maxY - minY + 1,
                minX,
                minY
            );
            PollSound();

            for (index = 0; index < boltCount; ++index) {
                if (bolts[index].finished == 0)
                    allFinished = false;
            }
            if (allFinished != 0)
                goto boltsDone;

            if (branchDistance != 0) {
                oldBoltCount = boltCount;
                for (index = 0; index < oldBoltCount; ++index) {
                    if (bolts[index].finished == 0) {
                        remainingDistance = MANHATTAN_LENGTH(
                            bolts[index].endX - bolts[index].pixelX,
                            bolts[index].endY - bolts[index].pixelY
                        );
                        if (boltCount < BOLT_MAX_COUNT
                            && remainingDistance
                                   > angleDistance * BRANCH_MIN_REMAINING_DISTANCE_MULTIPLIER
                            && Random(0, branchChance) < BOLT_BRANCH_PERCENT_LIMIT) {
                            if (bolts[index].lastBranchX != 0) {
                                branchSeparation = MANHATTAN_LENGTH(
                                    bolts[index].lastBranchX - bolts[index].pixelX,
                                    bolts[index].lastBranchY - bolts[index].pixelY
                                );
                                if (branchSeparation < branchDistance * BOLT_BRANCH_COOLDOWN_FACTOR)
                                    continue;
                            }
                            bolts[index].lastBranchX = bolts[index].pixelX;
                            bolts[index].lastBranchY = bolts[index].pixelY;
                            branchAngle =
                                static_cast<float>(
                                    Random(BOLT_BRANCH_RANDOM_LOW, BOLT_BRANCH_RANDOM_HIGH)
                                )
                                / (BOLT_ANGLE_PERCENT_SCALE);
                            if (Random(0, 1) != 0)
                                branchAngle = -branchAngle;
                            currentAngle = bolts[index].angle;
                            currentAngle = currentAngle + branchAngle;
                            childDistance = Random(branchLength >> 1, branchLength);
                            if (childDistance > (remainingDistance >> 1))
                                childDistance = remainingDistance >> 1;
                            childX = static_cast<i32>(
                                bolts[index].pixelX
                                + childDistance * sin(static_cast<double>(currentAngle))
                            );
                            childY = static_cast<i32>(
                                bolts[index].pixelY
                                + childDistance * cos(static_cast<double>(currentAngle))
                            );
                            if (bolts[index].endWidth < bolts[index].startWidth)
                                childWidth = bolts[index].width - 1;
                            else
                                childWidth = bolts[index].width;
                            AddBolt(
                                &bolts[boltCount],
                                bolts[index].pixelX,
                                bolts[index].pixelY,
                                childX,
                                childY,
                                branchDistance,
                                childWidth,
                                1,
                                colorMode,
                                static_cast<i32>(
                                    minAngle * BOLT_CHILD_ANGLE_SCALE + BOLT_CHILD_MIN_ANGLE_OFFSET
                                ),
                                static_cast<i32>(
                                    maxAngle * BOLT_CHILD_ANGLE_SCALE + BOLT_CHILD_ANGLE_OFFSET
                                ),
                                angleDistance,
                                bolts[index].forceAngle
                            );
                            ++boltCount;
                        }
                    }
                }
            }
        }

        for (index = 0; index < boltCount; ++index) {
            if (bolts[index].finished == 0)
                ResetBoltAngle(&bolts[index]);
        }
    }

boltsDone:
    delete[] bolts;
    if (managePointer != 0) {
        DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
        gpMouseManager->ShowColorPointer();
    }
    if (brightenPalette != 0) {
        SetPalette(originalPalette->Data(), 1);
        gpResourceManager->Dispose(originalPalette);
        delete effectPalette;
    }
    gpWindowManager->m_updateFlags = 1;
}

i32 combatManager::GetNextChainLightningTarget(army* source, i32 requireWorks) {
    i32 xDelta;
    i32 closestCell;
    i32 fromY;
    i32 sourceX;
    i32 armyIndex;
    i32 y;
    i32 length;
    army* candidate;
    CombatSide sideIndex;
    i32 closest;
    closest = CHAIN_LIGHTNING_DISTANCE_SENTINEL;
    closestCell = COMBAT_HEX_EMPTY;
    sourceX = source->MidX();
    fromY = source->MidY();
    for (sideIndex = COMBAT_ATTACKER_SIDE; (sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (armyIndex = 0; armyIndex < m_armyCount[(sideIndex)]; ++armyIndex) {
            candidate = &m_armies[(sideIndex)][armyIndex];
            if (gArmyEffected[(sideIndex)][armyIndex] == 0) {
                if ((requireWorks != 0 && candidate->SpellCastWorks(SPELL_CHAIN_LIGHTNING))
                    || (requireWorks == 0
                        && candidate->SpellCastWorkChance(SPELL_CHAIN_LIGHTNING) != 0.0f)) {
                    xDelta = abs(candidate->MidX() - sourceX);
                    y = abs(candidate->MidY() - fromY);
                    length = INTEGER_VECTOR_LENGTH(xDelta, y);
                    if (length < closest) {
                        closest = length;
                        closestCell = candidate->m_hex;
                    }
                }
            }
        }
    }
    return closestCell;
}

void combatManager::ChainLightning(i32 targetHex, i32 spellPower) {
    i32 damage;
    army* target;
    i32 deltaY;
    i32 unusedValue1 [[maybe_unused]];
    i32 deltaX;
    i32 targetY;
    i32 startY;
    i32 targetX;
    i32 startX;
    i32 distance;
    b32 firstBolt;
    i32 nextTarget;
    i32 strike;
    i32 forceAngle;
    i32 unusedValue2 [[maybe_unused]];
    i32 targetDamage;
    i32 deadline;
    i32 unusedValue3 [[maybe_unused]];
    i32 branchDistance;

    firstBolt = true;
    damage = spellPower * CHAIN_LIGHTNING_INITIAL_DAMAGE_PER_POWER;
    deadline = KBTickCount();
    startX = castX;
    startY = castY;
    ClearEffects();
    gpMouseManager->HideColorPointer();
    for (strike = 0; strike < CHAIN_LIGHTNING_MAX_TARGETS; ++strike) {
        target = &m_armies[(m_hexCells[targetHex].m_occupantSide)]
                            [m_hexCells[targetHex].m_occupantIndex];
        if (strike <= CHAIN_LIGHTNING_MAX_TARGETS - 2
            && m_hexCells[targetHex].m_occupantSide == m_currentSide)
            gpCombatManager->m_heroDeathPending[(m_currentSide)] = 1;

        targetDamage = damage;
        if (target->m_monsterType == CREATURE_AIR_ELEMENTAL)
            targetDamage *= CHAIN_LIGHTNING_AIR_ELEMENTAL_MULTIPLIER;
        if (IS_GOLEM_CREATURE(target->m_monsterType))
            targetDamage = static_cast<i32>(targetDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
        target->Damage(targetDamage, SPELL_NONE);
        damage >>= 1;
        gArmyEffected[(target->m_side)][target->m_index] = true;

        targetX = target->MidX();
        targetY = target->MidY();
        deltaX = abs(targetX - startX);
        deltaY = abs(targetY - startY);
        distance = INTEGER_VECTOR_LENGTH(deltaX, deltaY);
        branchDistance = distance / CHAIN_LIGHTNING_DISTANCE_DIVISOR;
        if (branchDistance > CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE;
        if (branchDistance < CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE;
        if (branchDistance > CHAIN_LIGHTNING_SHORT_BRANCH_MAX)
            forceAngle = CHAIN_LIGHTNING_LONG_FORCE_ANGLE;
        else
            forceAngle = CHAIN_LIGHTNING_SHORT_FORCE_ANGLE;
        DoBolt(
            0,
            startX,
            startY,
            targetX,
            targetY,
            0,
            CHAIN_LIGHTNING_BOLT_LENGTH,
            CHAIN_LIGHTNING_START_WIDTH,
            CHAIN_LIGHTNING_END_WIDTH,
            BOLT_COLOR_LIGHTNING,
            firstBolt ? CHAIN_LIGHTNING_FIRST_MIN_ANGLE : CHAIN_LIGHTNING_MIN_ANGLE,
            firstBolt ? CHAIN_LIGHTNING_FIRST_MAX_ANGLE : CHAIN_LIGHTNING_MAX_ANGLE,
            branchDistance,
            forceAngle,
            0,
            0,
            strike == 0
        );
        firstBolt = false;
        startX = targetX;
        startY = targetY;
        DelayMilli(
            static_cast<i32l>(CHAIN_LIGHTNING_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
        );
        nextTarget = GetNextChainLightningTarget(target, 1);
        if (nextTarget == COMBAT_HEX_EMPTY)
            break;
        targetHex = nextTarget;
        DrawFrame(1, 0, 0, 0, 0, 1, 1);
        DelayTil(&deadline);
        deadline = COMBAT_DEADLINE(CHAIN_LIGHTNING_FRAME_DELAY);
    }
    ShowMassSpell(gArmyEffected, gsSpellInfo[(SPELL_CHAIN_LIGHTNING)].combatEffect, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpMouseManager->ShowColorPointer();
}

void combatManager::VaporizeCreature(CombatSide side, i32 armyIndex) {
    i32 firstY;
    i32 lastY;
    i32 stripeRow;
    i32 rowCount;
    army* target;
    i32 topOffset;
    i32 unusedVaporizeWord [[maybe_unused]];
    i32 bottomOffset;
    i32 height [[maybe_unused]];
    i32 phase;

    target = &m_armies[(side)][armyIndex];
    ResetLimitCreature();
    ++m_limitCreatureCount[(side)][armyIndex];
    gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gyModify = static_cast<i8*>(H2_ALLOC(SPELL_MODIFIER_ROW_COUNT));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    height = giMaxExtentY - giMinExtentY + 1;
    target->m_palette = gyModify;
    target->m_showQuantity = false;

    firstY = (giMinExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    lastY = (giMaxExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    rowCount = (lastY - firstY) / VAPORIZE_STRIPE_WIDTH + 1;
    for (phase = 0; phase < VAPORIZE_PHASE_COUNT; ++phase) {
        switch (phase) {
            case 0:
                topOffset = 0;
                bottomOffset = 1;
                break;
            case 1:
                topOffset = 1;
                bottomOffset = 3;
                break;
            default:
                topOffset = 3;
                bottomOffset = 2;
                break;
        }
        if (phase == VAPORIZE_PHASE_COUNT - 1)
            rowCount = (rowCount - 1) / VAPORIZE_ROW_PAIR_SIZE + 1;
        for (stripeRow = 0; stripeRow < rowCount; ++stripeRow) {
            *(stripeRow * VAPORIZE_STRIPE_WIDTH + gyModify + topOffset + firstY) = VAPORIZE_MASKED;
            *(gyModify - stripeRow * VAPORIZE_STRIPE_WIDTH - bottomOffset + lastY) = VAPORIZE_MASKED;
            gbLimitToExtent = true;
            gpCombatManager->DrawFrame(1, 0, 1, 0, VAPORIZE_FRAME_DELAY, 1, 1);
        }
    }
    DelayMilli(static_cast<i32l>(SPELL_VANISH_END_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]));
    target->m_palette = NULL;
    target->m_showQuantity = true;
    H2_FREE(gyModify);
    gyModify = NULL;
    gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::RippleCreature(
    CombatSide side,
    i32 armyIndex,
    CombatRippleMode mode
) {
    float amplitudeStep;
    float amplitude;
    i32 frameDelay;
    i32 height [[maybe_unused]];
    i32 phase;
    float amplitudeBase;
    i32 rowIndex;
    i32 start;
    i32 maskEnd;
    i32 extentHeight;
    i32 waveIndex;
    army* target;
    float* wave;
    i32 phaseStep;
    i32 skipDistance;
    i32 amplitudeIndex;

    target = &m_armies[(side)][armyIndex];
    switch (mode) {
        case COMBAT_RIPPLE_WAVE:
            phaseStep = RIPPLE_WAVE_PHASE_STEP;
            frameDelay = RIPPLE_WAVE_FRAME_DELAY;
            amplitudeBase = RIPPLE_MODE_ZERO_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_MODE_ZERO_AMPLITUDE_STEP;
            break;
        case COMBAT_RIPPLE_DEATH_RIPPLE:
            phaseStep = RIPPLE_DEFAULT_PHASE_STEP;
            frameDelay = RIPPLE_DEFAULT_FRAME_DELAY;
            amplitudeBase = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
        default:
            phaseStep = RIPPLE_DEFAULT_PHASE_STEP;
            frameDelay = RIPPLE_DEFAULT_FRAME_DELAY;
            amplitudeBase = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
    }

    ResetLimitCreature();
    ++m_limitCreatureCount[(side)][armyIndex];
    if (mode == COMBAT_RIPPLE_DEATH_WAVE)
        gpCombatManager->DrawFrame(0, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    else
        gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    height = giMaxExtentY - giMinExtentY + 1;
    gyModify = static_cast<i8*>(H2_ALLOC(SPELL_MODIFIER_ROW_COUNT));
    wave = static_cast<float*>(H2_ALLOC(sizeof(float) * SPELL_MODIFIER_ROW_COUNT));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    for (rowIndex = 0; rowIndex < SPELL_MODIFIER_ROW_COUNT; ++rowIndex) {
        wave[rowIndex] = static_cast<float>(
            (sin(static_cast<double>(
                 static_cast<float>(rowIndex % RIPPLE_WAVE_PERIOD)
                 / static_cast<float>(RIPPLE_WAVE_DIVISOR)
             ))
             - RIPPLE_WAVE_CENTER)
            * RIPPLE_WAVE_RANGE
        );
    }
    target->m_palette = gyModify;
    target->m_showQuantity = false;
    giMinExtentX -= RIPPLE_MARGIN;
    giMaxExtentX += RIPPLE_MARGIN;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_SCREEN_WIDTH - 1)
        giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
    extentHeight = giMaxExtentY - giMinExtentY + 1;

    for (phase = RIPPLE_PHASE_START; phase < RIPPLE_PHASE_END; phase += phaseStep) {
        skipDistance =
            abs(RIPPLE_PHASE_CENTER - phase % RIPPLE_PHASE_PERIOD) - RIPPLE_SKIP_CENTER_OFFSET;
        amplitudeIndex = (phase - RIPPLE_PHASE_START) / RIPPLE_AMPLITUDE_INDEX_DIVISOR + 1;
        if (mode == COMBAT_RIPPLE_DEATH_WAVE)
            amplitudeIndex = RIPPLE_MODE_TWO_AMPLITUDE_START - amplitudeIndex;
        else if (mode == COMBAT_RIPPLE_WAVE) {
            if (amplitudeIndex == 0)
                amplitudeIndex = RIPPLE_MODE_ZERO_CENTER_AMPLITUDE;
            else
                amplitudeIndex = RIPPLE_MODE_ZERO_AMPLITUDE_START - amplitudeIndex;
        }
        if (phase > RIPPLE_PHASE_CENTER && phase < RIPPLE_PHASE_END - RIPPLE_SKIP_CENTER_OFFSET
            && (skipDistance == RIPPLE_SKIP_DISTANCE_0 || skipDistance == RIPPLE_SKIP_DISTANCE_1
                || skipDistance == RIPPLE_SKIP_DISTANCE_2
                || skipDistance == RIPPLE_SKIP_DISTANCE_3
                || skipDistance == RIPPLE_SKIP_DISTANCE_4))
            continue;
        amplitude = skipDistance * (amplitudeIndex * amplitudeStep + amplitudeBase);
        memset(gyModify + giMinExtentY, 0, extentHeight);
        for (rowIndex = giMinExtentY; rowIndex < giMaxExtentY; ++rowIndex) {
            if (mode == COMBAT_RIPPLE_DEATH_WAVE)
                waveIndex = -giMaxExtentY + rowIndex
                            + (phase - RIPPLE_PHASE_START) * RIPPLE_WAVE_PHASE_MULTIPLIER;
            else
                waveIndex = giMinExtentY - rowIndex
                            + (phase - RIPPLE_PHASE_START) * RIPPLE_WAVE_PHASE_MULTIPLIER;
            waveIndex += RIPPLE_WAVE_INDEX_OFFSET;
            if (waveIndex >= 0 && waveIndex < SPELL_MODIFIER_ROW_COUNT)
                gyModify[rowIndex] = static_cast<i8>((wave[waveIndex]) * amplitude);
        }
        if (mode == COMBAT_RIPPLE_DEATH_RIPPLE && phase >= RIPPLE_DEATH_RIPPLE_FADE_START) {
            start = giMinExtentY - 1;
            maskEnd = giMinExtentY
                   + (RIPPLE_DEATH_RIPPLE_FADE_BASE - (RIPPLE_PHASE_END - phase)) * extentHeight
                         / RIPPLE_FADE_DIVISOR
                   + 1;
            memset(gyModify + start, VAPORIZE_MASKED, maskEnd - start + 1);
        }
        if (mode == COMBAT_RIPPLE_DEATH_WAVE && phase < RIPPLE_DEATH_WAVE_FADE_END) {
            start = giMinExtentY - 1;
            maskEnd = giMaxExtentY - 1
                   - (phase - RIPPLE_DEATH_WAVE_FADE_BASE) * extentHeight / RIPPLE_FADE_DIVISOR;
            memset(gyModify + start, VAPORIZE_MASKED, maskEnd - start + 1);
        }
        gbLimitToExtent = true;
        gpCombatManager->DrawFrame(1, 0, 1, 0, frameDelay, 1, 1);
    }
    DelayMilli(static_cast<i32l>(SPELL_VANISH_END_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]));
    target->m_palette = NULL;
    target->m_showQuantity = true;
    H2_FREE(gyModify);
    H2_FREE(wave);
    gyModify = NULL;
    if (mode != COMBAT_RIPPLE_DEATH_RIPPLE)
        gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::ShowMassSpell(
    i8 (*const affected)[COMBAT_ARMY_SLOT_COUNT],
    CombatEffectType effect,
    i32 animateCreatures
) {
    i32 unusedMassSpellWord [[maybe_unused]];
    CombatSide side;
    army* target;
    i32 returnFrames;
    b32 creatureDied;
    i32 frame;
    i32 effectFrames;
    i32 armyIndex;
    u32l effectFile;

    effectFile = MAKEFILEID(gCombatFxNames[(effect)]);
    effectFrames = giNumPowFrames[(effect)] - 1;
    returnFrames = 0;
    if (gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFile);
        gCurLoadedSpellEffect = effect;
    }

    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
            target = &m_armies[(side)][armyIndex];
            if (affected[(side)][armyIndex] != 0)
                target->m_drawSpellEffect = true;
            if (animateCreatures != 0 && affected[(side)][armyIndex] != 0
                && target->m_animationSequence != ARMY_ANIMATION_WINCE
                && target->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN) {
                if (target->m_quantity == 0) {
                    if (target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_DEATH)]
                        > effectFrames)
                        effectFrames =
                            target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_DEATH)];
                    gpSoundManager->MemorySample(target->m_samples[(ARMY_SAMPLE_KILL)]);
                } else {
                    if (target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WINCE)]
                        > effectFrames)
                        effectFrames =
                            target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WINCE)];
                    if (target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_WINCE_RETURN)]
                        > returnFrames)
                        returnFrames = target->m_frameInfo
                                            .animationFrameCount[(ARMY_ANIMATION_WINCE_RETURN)];
                    gpSoundManager->MemorySample(target->m_samples[(ARMY_SAMPLE_WINCE)]);
                }
            }
        }
    }

    for (frame = 0; frame < effectFrames; ++frame) {
        for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                target = &m_armies[(side)][armyIndex];
                target->m_spellEffectYOffset = 0;
                if (animateCreatures != 0 && affected[(side)][armyIndex] != 0) {
                    if (target->m_quantity == 0) {
                        if (target->m_animationSequence != ARMY_ANIMATION_DEATH) {
                            target->m_animationSequence = ARMY_ANIMATION_DEATH;
                            target->m_animationFrame = 0;
                        } else if (target->m_animationFrame + 1
                                   < target->m_frameInfo
                                         .animationFrameCount[(target->m_animationSequence)]) {
                            ++target->m_animationFrame;
                        }
                    } else if (target->m_animationSequence != ARMY_ANIMATION_WINCE) {
                        target->m_animationSequence = ARMY_ANIMATION_WINCE;
                        target->m_animationFrame = 0;
                    } else if (target->m_animationFrame + 1
                               < target->m_frameInfo
                                     .animationFrameCount[(target->m_animationSequence)]) {
                        ++target->m_animationFrame;
                    }
                }
                if (frame + 1 < giNumPowFrames[(effect)])
                    gCurSpellEffectFrame = frame;
            }
        }
        DrawFrame(1, 0, 0, 0, MASS_SPELL_FRAME_DELAY, 1, 1);
    }

    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex)
            m_armies[(side)][armyIndex].m_drawSpellEffect = false;
    }
    for (frame = 0; frame < returnFrames + 1; ++frame) {
        for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                target = &m_armies[(side)][armyIndex];
                if (animateCreatures != 0 && affected[(side)][armyIndex] != 0
                    && target->m_animationSequence != ARMY_ANIMATION_STAND) {
                    if (target->m_animationSequence == ARMY_ANIMATION_WINCE) {
                        target->m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
                        target->m_animationFrame = 0;
                    } else if (target->m_animationFrame + 1
                               < target->m_frameInfo
                                     .animationFrameCount[(target->m_animationSequence)]) {
                        ++target->m_animationFrame;
                    } else if (target->m_animationSequence != ARMY_ANIMATION_DEATH) {
                        target->m_animationSequence = ARMY_ANIMATION_STAND;
                        target->m_animationFrame = 0;
                    }
                }
            }
        }
        DrawFrame(1, 0, 0, 0, MASS_SPELL_FRAME_DELAY, 1, 1);
    }

    creatureDied = false;
    memset(m_removedArmies, 0, sizeof(m_removedArmies));
    m_removedArmyPresent = 0;
    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
            target = &m_armies[(side)][armyIndex];
            if (affected[(side)][armyIndex] != 0 && target->m_quantity == 0) {
                target->ProcessDeath(0);
                creatureDied = true;
            }
        }
    }
    if (creatureDied)
        DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    if (m_removedArmyPresent != 0)
        MakeCreaturesVanish();
}

void combatManager::CastMassSpell(SpellType spell, i32 spellPower) {
    CombatSide side;
    army* target;
    b32 animateCreatures;
    i32 damage;
    b32 anyAffected;
    i32 influence;
    CombatEffectType effect;
    i32 armyIndex;
    i8 affected[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];

    target = NULL;
    effect = gsSpellInfo[(spell)].combatEffect;
    animateCreatures = false;
    gpWindowManager->m_updateFlags = 0;
    ShowSpellMessage(0, spell, NULL);
    memset(affected, 0, sizeof(affected));

    switch (spell) {
        case SPELL_MASS_SLOW:
        case SPELL_MASS_CURSE:
            side = COMBAT_DEFENDER_SIDE - (m_currentSide);
            for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                if (m_armies[(side)][armyIndex].SpellCastWorks(spell))
                    affected[(side)][armyIndex] = 1;
            }
            break;

        case SPELL_MASS_CURE:
        case SPELL_MASS_HASTE:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_SHIELD:
            side = m_currentSide;
            for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                if (m_armies[(side)][armyIndex].SpellCastWorks(spell))
                    affected[(side)][armyIndex] = 1;
            }
            break;

        case SPELL_MASS_DISPEL:
            for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
                for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                    if (m_armies[(side)][armyIndex].SpellCastWorks(spell))
                        affected[(side)][armyIndex] = 1;
                }
            }
            break;

        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT: {
            animateCreatures = true;
            damage = (spell == SPELL_HOLY_WORD ? HOLY_WORD_DAMAGE_PER_POWER
                                                 : HOLY_SHOUT_DAMAGE_PER_POWER)
                       * spellPower;
            for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
                for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                    if ((((m_armies[(side)][armyIndex].m_monster.attributes) & (MONSTER_ATTRIBUTE_UNDEAD)))
                        && m_armies[(side)][armyIndex].SpellCastWorks(spell))
                        affected[(side)][armyIndex] = 1;
                }
            }
            if (spell == SPELL_HOLY_WORD)
                Blur(0, HOLY_WORD_COLOR_ADJUSTMENT, HOLY_WORD_COLOR_ADJUSTMENT);
            else
                Blur(0, HOLY_SHOUT_COLOR_ADJUSTMENT, HOLY_SHOUT_COLOR_ADJUSTMENT);
            for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
                for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                    if (affected[(side)][armyIndex] != 0)
                        m_armies[(side)][armyIndex].Damage(damage, SPELL_NONE);
                }
            }
            sprintf(
                gText,
                "Заклинание '%s' наносит %d ед. урона\nвсем неупокоенным существам.",
                gSpellNames[(spell)],
                damage
            );
            CombatMessage(gText, 1, 1, 0);
            break;
        }

        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE: {
            animateCreatures = true;
            for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
                for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                    if (!(((m_armies[(side)][armyIndex].m_monster.attributes) & (MONSTER_ATTRIBUTE_UNDEAD)))
                        && m_armies[(side)][armyIndex].SpellCastWorks(spell))
                        affected[(side)][armyIndex] = 1;
                }
            }
            Ripple(spell == SPELL_DEATH_RIPPLE ? DEATH_RIPPLE_STRENGTH : DEATH_WAVE_STRENGTH);
            damage = (spell == SPELL_DEATH_RIPPLE ? DEATH_RIPPLE_DAMAGE_PER_POWER
                                                    : DEATH_WAVE_DAMAGE_PER_POWER)
                       * spellPower;
            for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
                for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
                    if (affected[(side)][armyIndex] != 0)
                        m_armies[(side)][armyIndex].Damage(damage, SPELL_NONE);
                }
            }
            sprintf(
                gText,
                "Заклинание Смерти наносит %d ед. урона\nвсем живым существам.",
                damage
            );
            CombatMessage(gText, 1, 1, 0);
            break;
        }
    }

    if (gbNoShowCombat)
        goto applySpellInfluence;

    anyAffected = false;
    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
            if (affected[(side)][armyIndex] != 0)
                anyAffected = true;
        }
    }
    if (anyAffected)
        ShowMassSpell(affected, effect, animateCreatures);

applySpellInfluence:
    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[(side)]; ++armyIndex) {
            if (affected[(side)][armyIndex] == 0)
                continue;

            target = &m_armies[(side)][armyIndex];
            switch (spell) {
                case SPELL_MASS_CURSE:
                    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_CURSE, spellPower);
                    break;
                case SPELL_MASS_SLOW:
                    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SLOW, spellPower);
                    break;
                case SPELL_MASS_HASTE:
                    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_HASTE, spellPower);
                    break;
                case SPELL_MASS_BLESS:
                    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLESS, spellPower);
                    break;
                case SPELL_MASS_SHIELD:
                    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SHIELD, spellPower);
                    break;
                case SPELL_MASS_CURE:
                    target->Cure(spellPower);
                    break;
                case SPELL_MASS_DISPEL: {
                    for (influence = ARMY_SPELL_INFLUENCE_HASTE;
                         influence < ARMY_SPELL_INFLUENCE_COUNT;
                         ++influence)
                        target->CancelIndividualSpell(influence);
                    break;
                }
                case SPELL_DEATH_RIPPLE:
                case SPELL_DEATH_WAVE:
                    break;
            }
        }
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->m_updateFlags = 1;
}

void combatManager::MirrorImage(i32 targetHex) {
    CombatHexDirection searchDirection;
    i32 xOffset;
    i32 duration;
    i32 sourcePart;
    army* source;
    i32 searchHex;
    army* image;
    i32 step;
    i32 yOffset;
    i32 distance;
    i32 candidateHex;
    i32 frame;
    CombatHexDirection direction;
    i32 deadline;

    source =
        &m_armies[(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];

    distance = 1;
    while (distance < MIRROR_SEARCH_DISTANCE_LIMIT) {
        for (sourcePart = 0; sourcePart < MIRROR_SOURCE_PART_COUNT; ++sourcePart) {
            if (sourcePart == 0) {
                searchHex = source->m_hex;
            } else {
                if ((((source->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                    searchHex = source->m_facing == ARMY_FACING_RIGHT ? source->m_hex + 1
                                : source->m_hex - 1;
                } else {
                    continue;
                }
            }

            for (direction = COMBAT_DIRECTION_NORTHEAST;
                 (direction) < SPELL_ADJACENT_DIRECTION_COUNT;
                 ++direction) {
                if (source->m_facing == ARMY_FACING_RIGHT)
                    searchDirection = direction;
                else
                    searchDirection = COMBAT_DIRECTION_NORTHWEST - (direction);

                if (source->m_facing == ARMY_FACING_RIGHT
                    && searchDirection == COMBAT_DIRECTION_EAST && sourcePart == 0
                    && distance == 1)
                    continue;
                if (source->m_facing == ARMY_FACING_RIGHT
                    && searchDirection == COMBAT_DIRECTION_WEST && sourcePart == 0
                    && distance == 1)
                    continue;
                if (source->m_facing == ARMY_FACING_RIGHT
                    && searchDirection == COMBAT_DIRECTION_WEST && sourcePart == 1
                    && distance <= 2)
                    continue;
                if (source->m_facing == ARMY_FACING_LEFT
                    && searchDirection == COMBAT_DIRECTION_WEST && sourcePart == 0
                    && distance == 1)
                    continue;
                if (source->m_facing == ARMY_FACING_LEFT
                    && searchDirection == COMBAT_DIRECTION_EAST && sourcePart == 0
                    && distance == 1)
                    continue;
                if (source->m_facing == ARMY_FACING_LEFT
                    && searchDirection == COMBAT_DIRECTION_EAST && sourcePart == 1
                    && distance <= 2)
                    continue;

                candidateHex = searchHex;
                for (step = 0; step < distance; ++step) {
                    candidateHex = GetAdjacentCellIndexNoArmy(candidateHex, searchDirection);
                    if (!IS_INTERIOR_COMBAT_HEX(candidateHex))
                        continue;
                    if (source->CanFit(candidateHex, 0, NULL))
                        goto mirror_found;
                }
            }
        }
        ++distance;
    }
    sprintf(gText, "Сорвалось заклинание сотворения фантомов!");
    NormalDialog(gText, NORMAL_DIALOG_INFO);
    return;

mirror_found:
    AddArmy(
        m_currentSide,
        source->m_monsterType,
        source->m_quantity,
        candidateHex,
        MONSTER_FLAGS_MIRROR_IMAGE,
        0
    );
    image = &m_armies[(m_hexCells[candidateHex].m_occupantSide)]
                      [m_hexCells[candidateHex].m_occupantIndex];
    image->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_SUMMONED;
    duration = m_spellPower[(m_currentSide)];
    if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        duration += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
        duration += SPELL_WIZARD_HAT_POWER_BONUS;
    image->m_roundCounter = duration;
    source->m_mirrorImageIndex = image->m_index;
    image->m_mirrorSourceIndex = source->m_index;

    xOffset = m_hexCells[source->m_hex].m_x - m_hexCells[image->m_hex].m_x;
    yOffset = m_hexCells[source->m_hex].m_y - m_hexCells[image->m_hex].m_y;
    ResetLimitCreature();
    m_limitCreatureCount[(m_hexCells[candidateHex].m_occupantSide)]
                        [m_hexCells[candidateHex].m_occupantIndex]++;
    m_limitCreatureCount[(m_hexCells[targetHex].m_occupantSide)]
                        [m_hexCells[targetHex].m_occupantIndex]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    deadline = COMBAT_DEADLINE(MIRROR_SLIDE_FRAME_DELAY);
    for (frame = 0; frame < MIRROR_SLIDE_FRAME_COUNT; ++frame) {
        image->m_xOffset =
            xOffset * (MIRROR_SLIDE_FRAME_COUNT - frame) / MIRROR_SLIDE_FRAME_COUNT;
        image->m_yOffset = yOffset * (MIRROR_SLIDE_FRAME_COUNT - frame) / MIRROR_SLIDE_FRAME_COUNT;
        gbLimitToExtent = true;
        gpCombatManager->DrawFrame(0, 0, 0, 0, 0, 1, 0);
        UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
        gbLimitToExtent = false;
        DelayTil(&deadline);
        deadline = COMBAT_DEADLINE(MIRROR_SLIDE_FRAME_DELAY);
    }
    image->m_xOffset = 0;
    image->m_yOffset = 0;
    UpdateGrid(0, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::SummonElemental(CreatureType monsterType, i32 spellPower) {
    u8 summonHexes[SUMMON_HEX_STORAGE_COUNT];
    u32 summonHex;
    i32 offset;
    army* elementals;
    i32 randomOffset;

    summonHexes[ATTACKER_SUMMON_TOP_SLOT] = ATTACKER_SUMMON_HEX_TOP;
    summonHexes[ATTACKER_SUMMON_MID_SLOT] = ATTACKER_SUMMON_HEX_MID;
    summonHexes[ATTACKER_SUMMON_LOW_SLOT] = ATTACKER_SUMMON_HEX_LOW;
    summonHexes[DEFENDER_SUMMON_TOP_SLOT] = DEFENDER_SUMMON_HEX_TOP;
    summonHexes[DEFENDER_SUMMON_MID_SLOT] = DEFENDER_SUMMON_HEX_MID;
    summonHexes[DEFENDER_SUMMON_LOW_SLOT] = DEFENDER_SUMMON_HEX_LOW;
    randomOffset = SRandom(0, SUMMON_RANDOM_OFFSET_MAX);
    summonHex = static_cast<u32>(COMBAT_HEX_EMPTY);
    if (m_heroes[(m_currentSide)] != NULL
        && m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;
    for (offset = 0; offset < SUMMON_HEXES_PER_SIDE; ++offset) {
        if (m_hexCells[*(summonHexes + (m_currentSide) * SUMMON_HEXES_PER_SIDE
                         + (randomOffset + offset) % SUMMON_HEXES_PER_SIDE)]
                .m_occupantSide
            == COMBAT_SIDE_NONE)
            summonHex =
                *(summonHexes + (m_currentSide) * SUMMON_HEXES_PER_SIDE
                  + (randomOffset + offset) % SUMMON_HEXES_PER_SIDE);
    }
    m_summonedCreatureType[(m_currentSide)] = monsterType;
    AddArmy(
        m_currentSide,
        monsterType,
        spellPower * ELEMENTAL_SUMMON_QUANTITY_PER_POWER,
        summonHex,
        MONSTER_FLAGS_NONE,
        1
    );
    elementals = &m_armies[(m_hexCells[summonHex].m_occupantSide)]
                          [m_hexCells[summonHex].m_occupantIndex];
    elementals->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_SUMMONED;
    spellPower = m_spellPower[(m_currentSide)];
    if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        spellPower += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
        spellPower += SPELL_WIZARD_HAT_POWER_BONUS;
}

void combatManager::DoLuck(CombatSide side, i32 armyIndex) {
    army* target;
    i32 targetX;
    i32 startY;
    i32 startX;
    i32 targetY;

    target = &m_armies[(side)][armyIndex];
    targetX = target->MidX();
    targetY = m_hexCells[target->m_hex].m_y;
    targetY -=
        GetIconEntry(
            target->m_creatureIcon,
            target->m_frameInfo.animationFrames[(ARMY_ANIMATION_STAND)][LUCK_TARGET_FRAME_INDEX]
        )
            ->h
        + LUCK_ICON_Y_PADDING;
    if (targetY < LUCK_MIN_TARGET_Y)
        targetY = LUCK_MIN_TARGET_Y;
    startY = LUCK_EDGE_START_Y;
    if (target->m_facing == ARMY_FACING_RIGHT) {
        if (targetX < LUCK_RIGHT_FACING_FLIP_X)
            startX = targetX + targetY / LUCK_DIAGONAL_DIVISOR + LUCK_EDGE_INSET;
        else
            startX = targetX - LUCK_EDGE_INSET - targetY / LUCK_DIAGONAL_DIVISOR;
    } else if (targetX > LUCK_LEFT_FACING_FLIP_X) {
        startX = targetX - LUCK_EDGE_INSET - targetY / LUCK_DIAGONAL_DIVISOR;
    } else {
        startX = targetX + targetY / LUCK_DIAGONAL_DIVISOR + LUCK_EDGE_INSET;
    }
    if (targetY < LUCK_SHORT_BOLT_HEIGHT) {
        startX = side == COMBAT_ATTACKER_SIDE ? 0 : COMBAT_SCREEN_WIDTH - 1;
        startY = targetY + LUCK_MIN_TARGET_Y;
    }
    if (startX < 0)
        startX = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < startX)
        startX = COMBAT_SCREEN_WIDTH - 1;
    DoBolt(
        0,
        startX,
        startY,
        targetX,
        targetY,
        0,
        0,
        LUCK_BOLT_WIDTH,
        LUCK_BOLT_WIDTH,
        startX > targetX ? BOLT_COLOR_RAINBOW_FORWARD : BOLT_COLOR_RAINBOW_REVERSE,
        LUCK_BOLT_ANGLE,
        LUCK_BOLT_ANGLE,
        LUCK_BOLT_DISTANCE,
        LUCK_BOLT_FORCE_ANGLE,
        1,
        LUCK_BOLT_FRAME_DELAY,
        0
    );
}

void combatManager::DoBlast(i32 targetHex, SpellType spell) {
    SLimitData limits;
    i32 frameSpacing;
    i32 distance;
    icon* blastIcon;
    army* target;
    i32 startY;
    i32 deltaX;
    i32 deadline;
    i32 segmentCount;
    i32 segment;
    float currentY;
    float currentX;
    i32 targetY;
    i32 frame;
    i32 targetX;
    float stepY;
    i32 startX;
    float stepX;
    i32 deltaY;

    if (spell == SPELL_COLD_RAY) {
        blastIcon = gpResourceManager->GetIcon("coldray.icn");
        frameSpacing = BLAST_COLD_RAY_FRAME_SPACING;
    } else {
        blastIcon = gpResourceManager->GetIcon("disrray.icn");
        frameSpacing = BLAST_DISRUPTING_RAY_FRAME_SPACING;
    }
    target =
        &m_armies[(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];
    targetX = target->MidX();
    targetY = target->MidY();
    startX = castX;
    startY = castY;
    deltaX = targetX - startX;
    deltaY = targetY - startY;
    distance = INTEGER_VECTOR_LENGTH(deltaX, deltaY);
    segmentCount = distance / frameSpacing;
    currentX = static_cast<float>(startX);
    currentY = static_cast<float>(startY);
    stepX = static_cast<float>(deltaX) / segmentCount;
    stepY = static_cast<float>(deltaY) / segmentCount;
    deadline = 0;
    for (segment = 0; segment < segmentCount; ++segment) {
        ResetLimitCreature();
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        currentX = currentX + stepX;
        currentY = currentY + stepY;
        frame = (segment * BLAST_FRAME_COUNT - 1) / segmentCount;
        blastIcon->CombatClipDrawToBuffer(
            static_cast<i32>(currentX),
            static_cast<i32>(currentY),
            frame,
            &limits,
            stepX < 0.0f ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL
        );
        if (giMinExtentX < 0)
            giMinExtentX = 0;
        if (giMinExtentY < 0)
            giMinExtentY = 0;
        if (COMBAT_SCREEN_WIDTH - 1 < giMaxExtentX)
            giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
        if (COMBAT_AREA_HEIGHT - 1 < giMaxExtentY)
            giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
        DelayTil(&deadline);
        deadline = COMBAT_DEADLINE(BLAST_FRAME_DELAY);
        UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
    }
    gbComputeExtent = false;
    gbSaveBiggestExtent = false;
    DrawFrame(1, 0, 0, 0, 0, 1, 0);
    gpResourceManager->Dispose(blastIcon);
}

void combatManager::Resurrect(SpellType spell, i32 targetHex, i32 spellPower) {
    army* target;
    i32 otherHex;
    i32 unusedValue1 [[maybe_unused]];
    i32 deadHex;
    i32 unusedValue2 [[maybe_unused]];
    i32 index;
    i32 effectY;
    i32 effectX;
    i32 deadIndex;
    i32 unusedValue3 [[maybe_unused]];
    i32 oldQuantity;
    b32 keepSearching;
    i32 armyIndex;
    icon* resurrectIcon;
    b32 processedOtherHex;

    if (m_heroes[(m_currentSide)] != NULL
        && m_heroes[(m_currentSide)]->HasArtifact(ARTIFACT_ANKH))
        spellPower <<= 1;
    armyIndex = FindResurrectArmyIndex(m_currentSide, spell, targetHex);
    target = &m_armies[(m_currentSide)][armyIndex];
    processedOtherHex = false;
    oldQuantity = target->m_quantity;
    target->m_quantity +=
        spellPower * RESURRECT_HIT_POINTS_PER_POWER / target->m_monster.hitPoints;
    if (target->m_quantity > target->m_initialQuantity)
        target->m_quantity = target->m_initialQuantity;
    if (spell == SPELL_RESURRECT)
        target->m_temporaryResurrectionQuantity += target->m_quantity - oldQuantity;

    if (oldQuantity <= 0) {
        otherHex = COMBAT_HEX_EMPTY;
        deadIndex = COMBAT_HEX_EMPTY;
        keepSearching = true;
        deadHex = targetHex;
        while (keepSearching) {
            for (index = 0; index < m_hexCells[deadHex].m_deadOccupantCount; ++index) {
                if (m_hexCells[deadHex].m_deadOccupantSides[index] == m_currentSide
                    && m_hexCells[deadHex].m_deadOccupantIndices[index] == armyIndex) {
                    deadIndex = index;
                    if (!processedOtherHex) {
                        if (m_hexCells[deadHex].m_deadOccupantFrames[index]
                            == ARMY_FACING_RIGHT)
                            otherHex = deadHex - 1;
                        else if (m_hexCells[deadHex].m_deadOccupantFrames[index]
                                 == ARMY_FACING_LEFT)
                            otherHex = deadHex + 1;
                    }
                }
                if (deadIndex != COMBAT_HEX_EMPTY) {
                    m_hexCells[deadHex].m_occupantSide =
                        m_hexCells[deadHex].m_deadOccupantSides[index];
                    m_hexCells[deadHex].m_occupantIndex =
                        m_hexCells[deadHex].m_deadOccupantIndices[index];
                    m_hexCells[deadHex].m_occupantFrame =
                        m_hexCells[deadHex].m_deadOccupantFrames[index];
                    if (index + 1 == m_hexCells[deadHex].m_deadOccupantCount) {
                        m_hexCells[deadHex].m_deadOccupantSides[index] = COMBAT_SIDE_NONE;
                        m_hexCells[deadHex].m_deadOccupantIndices[index] = COMBAT_HEX_EMPTY;
                    } else {
                        m_hexCells[deadHex].m_deadOccupantSides[index] =
                            m_hexCells[deadHex].m_deadOccupantSides[index + 1];
                        m_hexCells[deadHex].m_deadOccupantIndices[index] =
                            m_hexCells[deadHex].m_deadOccupantIndices[index + 1];
                    }
                }
            }
            --m_hexCells[deadHex].m_deadOccupantCount;
            if (processedOtherHex) {
                keepSearching = false;
            } else if (otherHex == COMBAT_HEX_EMPTY) {
                keepSearching = false;
            } else {
                deadHex = otherHex;
                processedOtherHex = true;
                deadIndex = COMBAT_HEX_EMPTY;
            }
        }
    }

    effectX = target->MidX();
    effectY = target->MidY();
    if (target->m_quantity - oldQuantity > 1)
        sprintf(
            gText,
            "%d %s воскресают из мертвых!",
            target->m_quantity - oldQuantity,
            gArmyNamesPlural[(target->m_monsterType)]
        );
    else
        sprintf(
            gText,
            "%d %s воскресает из мертвых!",
            target->m_quantity - oldQuantity,
            gArmyNames[(target->m_monsterType)]
        );
    CombatMessage(gText, 1, 1, 0);

    if (!gbNoShowCombat) {
        resurrectIcon = gpResourceManager->GetIcon("yinyang.icn");
        for (index = 0; index < RESURRECT_ANIMATION_FRAME_COUNT; ++index) {
            glTimers[0] = static_cast<i32>(
                KBTickCount()
                + (SPELL_FIZZLE_FRAME_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            IconToBitmap(
                resurrectIcon,
                gpWindowManager->m_screen,
                effectX,
                effectY,
                index,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
            UpdateCombatArea();
            target->m_facing = ArmyFacingForSide(target->m_side);
            if (target->m_animationSequence == ARMY_ANIMATION_DEATH) {
                if (index >= RESURRECT_DEATH_REVERSE_FRAME) {
                    target->m_animationSequence = ARMY_ANIMATION_STAND;
                    target->m_animationFrame = 0;
                } else {
                    target->m_animationFrame =
                        RESURRECT_DEATH_REVERSE_FRAME - 1 - index
                                < target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_DEATH)] - 1
                            ? RESURRECT_DEATH_REVERSE_FRAME - 1 - index
                            : target->m_frameInfo.animationFrameCount[(ARMY_ANIMATION_DEATH)]
                                  - 1;
                }
            }
            DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
            DelayTil(glTimers);
        }
        gpResourceManager->Dispose(resurrectIcon);
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->m_monster.flags.abilityFlags &= MONSTER_FLAGS_RESURRECTED_MASK;
}

i32 combatManager::SpaceForElementalExists(void) {
    if ((m_currentSide == COMBAT_ATTACKER_SIDE
         && m_hexCells[ATTACKER_SUMMON_HEX_TOP].m_occupantSide != COMBAT_SIDE_NONE
         && m_hexCells[ATTACKER_SUMMON_HEX_MID].m_occupantSide != COMBAT_SIDE_NONE
         && m_hexCells[ATTACKER_SUMMON_HEX_LOW].m_occupantSide != COMBAT_SIDE_NONE)
        || (m_currentSide == COMBAT_DEFENDER_SIDE
            && m_hexCells[DEFENDER_SUMMON_HEX_TOP].m_occupantSide != COMBAT_SIDE_NONE
            && m_hexCells[DEFENDER_SUMMON_HEX_MID].m_occupantSide != COMBAT_SIDE_NONE
            && m_hexCells[DEFENDER_SUMMON_HEX_LOW].m_occupantSide != COMBAT_SIDE_NONE))
        return 0;
    else
        return 1;
}

void combatManager::ShowSpellCastFailure(army* target, i32) {
    SAMPLE2 fizzleSample = LoadPlaySample("rsbryfzl.82m");
    sprintf(
        gText,
        "%s%s сопротивляется этому заклинанию!",
        target->m_quantity == 1
            ? ""
            : "Отряд ",
        target->m_quantity == 1 ? gArmyNames[(target->m_monsterType)]
                                : gArmyNamesPlural[(target->m_monsterType)]
    );
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    WaitEndSample(&fizzleSample);
}

void combatManager::ModifyDamageForArtifacts(
    i32l* damage,
    SpellType spell,
    hero* attacker,
    hero* defender
) {
    if (attacker != NULL) {
        if (attacker->HasArtifact(ARTIFACT_EVERCOLD_ICICLE)
            && (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
        if (attacker->HasArtifact(ARTIFACT_EVERHOT_LAVA_ROCK)
            && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
        if (attacker->HasArtifact(ARTIFACT_LIGHTNING_ROD)
            && (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
    }
    if (defender != NULL) {
        if (defender->HasArtifact(ARTIFACT_ICE_CLOAK)
            && (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(ARTIFACT_FIRE_CLOAK)
            && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(ARTIFACT_LIGHTNING_HELM)
            && (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING))
            *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(ARTIFACT_HEART_FIRE)) {
            if (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)
                *damage <<= 1;
            else if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        }
        if (defender->HasArtifact(ARTIFACT_HEART_ICE)) {
            if (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)
                *damage = static_cast<i32l>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
            else if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                *damage <<= 1;
        }
    }
}

void combatManager::Earthquake(void) {
    i32 pass;
    i32 frame;
    i32 deadline;
    i32 width;
    i32 height;
    u8* source;
    u8* destination;
    i32 copyRow;
    CombatCastleWallState newWallStates[EARTHQUAKE_STRUCTURE_COUNT];
    CombatCastleWallState newTowerStates[EARTHQUAKE_STRUCTURE_COUNT];
    i32 impactPositions[EARTHQUAKE_MAX_IMPACTS][(COORDINATE_AXIS_COUNT)];
    i32 impactCount;
    i32 index;
    CombatDrawbridgeState newKeepState;
    i32 impactDelay[EARTHQUAKE_MAX_IMPACTS];
    icon* cloudIcon;
    i32 impact;

    i32 shakeOffsets[EARTHQUAKE_SHAKE_FRAME_COUNT][(COORDINATE_AXIS_COUNT)] = {
        {2, 2},
        {4, 1},
        {3, -2},
        {0, -6},
        {2, -2},
        {-1, 3},
        {-5, 4},
        {-8, 6},
        {-4, 2},
        {-1, 1},
        {-3, -3},
        {-7, -5},
        {-5, -7},
        {-2, -3},
        {0, 0}
    };
    i32 unusedQuakeA [[maybe_unused]];
    i32 unusedQuakeB [[maybe_unused]];

    gpMouseManager->HideColorPointer();
    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    for (pass = 0; pass < EARTHQUAKE_SHAKE_PASS_COUNT; ++pass) {
        for (frame = 0; frame < EARTHQUAKE_SHAKE_FRAME_COUNT; ++frame) {
            deadline = COMBAT_DEADLINE(SPELL_ARMAGEDDON_SHAKE_DELAY);
            PollSound();
            width = COMBAT_SCREEN_WIDTH - abs(shakeOffsets[frame][(COORDINATE_AXIS_X)]);
            height = COMBAT_AREA_HEIGHT - abs(shakeOffsets[frame][(COORDINATE_AXIS_Y)]) - 1;
            source = m_backgroundBuffer->m_pixels
                         + (shakeOffsets[frame][(COORDINATE_AXIS_X)] < 0
                                ? 0
                                : shakeOffsets[frame][(COORDINATE_AXIS_X)])
                         + (shakeOffsets[frame][(COORDINATE_AXIS_Y)] < 0
                                ? 0
                                : shakeOffsets[frame][(COORDINATE_AXIS_Y)])
                               * COMBAT_SCREEN_WIDTH;
            destination = gpWindowManager->m_screen->m_pixels
                          + (shakeOffsets[frame][(COORDINATE_AXIS_X)] > 0
                                 ? 0
                                 : -shakeOffsets[frame][(COORDINATE_AXIS_X)])
                          + (shakeOffsets[frame][(COORDINATE_AXIS_Y)] > 0
                                 ? 0
                                 : -shakeOffsets[frame][(COORDINATE_AXIS_Y)])
                                * COMBAT_SCREEN_WIDTH;
            for (copyRow = 0; copyRow < height; ++copyRow) {
                memcpy(destination, source, width);
                source += COMBAT_SCREEN_WIDTH;
                destination += COMBAT_SCREEN_WIDTH;
            }
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0,
                0
            );
            DelayTil(&deadline);
        }
    }
    m_backgroundDrawn = false;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    impactCount = 0;
    for (index = 0; index < EARTHQUAKE_STRUCTURE_COUNT; ++index) {
        newWallStates[index] = m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)];
        if (m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                != COMBAT_WALL_STATE_DESTROYED
            && m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                   != COMBAT_WALL_STATE_SECTION_DESTROYED
            && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_WALL_HIT_CHANCE) {
            ++newWallStates[index];
            if (m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_TOWER_STANDING
                && m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED
                && m_wallStates[index + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DAMAGE_LAST
                && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_WALL_SECOND_HIT_CHANCE)
                ++newWallStates[index];
            impactPositions[impactCount][(COORDINATE_AXIS_X)] =
                wallPos[index][(COORDINATE_AXIS_X)];
            impactPositions[impactCount][(COORDINATE_AXIS_Y)] =
                wallPos[index][(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
            if (newWallStates[index] == COMBAT_WALL_STATE_DESTROYED
                || newWallStates[index] == COMBAT_WALL_STATE_SECTION_DESTROYED)
                m_hexCells[iWallToHexCell[index]].m_blocked = 0;
        }

        newTowerStates[index] = m_wallStates[index];
        if (m_wallStates[index] != COMBAT_WALL_STATE_DESTROYED
            && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_TOWER_HIT_CHANCE) {
            newTowerStates[index] = COMBAT_WALL_STATE_DESTROYED;
            impactPositions[impactCount][(COORDINATE_AXIS_X)] =
                towerPos[index][(COORDINATE_AXIS_X)];
            impactPositions[impactCount][(COORDINATE_AXIS_Y)] =
                towerPos[index][(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
        }
    }

    newKeepState = m_drawbridgeState;
    if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN
        && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_KEEP_HIT_CHANCE) {
        newKeepState = COMBAT_CASTLE_GATE_HIDDEN;
        impactPositions[impactCount][(COORDINATE_AXIS_X)] = towerPos[0][(COORDINATE_AXIS_X)];
        impactPositions[impactCount][(COORDINATE_AXIS_Y)] =
            towerPos[0][(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        ++impactCount;
        impactPositions[impactCount][(COORDINATE_AXIS_X)] = towerPos[1][(COORDINATE_AXIS_X)];
        impactPositions[impactCount][(COORDINATE_AXIS_Y)] =
            towerPos[1][(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        ++impactCount;
    }

    for (index = 0; index < EARTHQUAKE_MAX_IMPACTS; ++index)
        impactDelay[index] = Random(0, EARTHQUAKE_MAX_IMPACT_DELAY);
    giMinExtentX = EARTHQUAKE_EXTENT_MIN_X;
    giMinExtentY = 0;
    giMaxExtentX = EARTHQUAKE_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
    if (impactCount != 0) {
        cloudIcon = gpResourceManager->GetIcon("lichclod.icn");
        for (index = 0; index < EARTHQUAKE_CLOUD_FRAME_COUNT; ++index) {
            glTimers[1] = static_cast<i32>(
                KBTickCount()
                + (SPELL_FIZZLE_FRAME_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            DrawFrame(0, 0, 1, 0, 0, 1, 0);
            for (impact = 0; impact < impactCount; ++impact) {
                if (index >= impactDelay[impact]
                    && index - impactDelay[impact] < EARTHQUAKE_CLOUD_VISIBLE_FRAME_COUNT) {
                    IconToBitmap(
                        cloudIcon,
                        gpWindowManager->m_screen,
                        impactPositions[impact][(COORDINATE_AXIS_X)],
                        impactPositions[impact][(COORDINATE_AXIS_Y)],
                        index - impactDelay[impact],
                        ICON_DRAW_CLIP,
                        0,
                        0,
                        COMBAT_SCREEN_WIDTH,
                        COMBAT_AREA_HEIGHT,
                        0
                    );
                }
            }
            UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
            DelayTil(&glTimers[1]);
            if (index == EARTHQUAKE_APPLY_DAMAGE_FRAME) {
                for (impact = 0; impact < EARTHQUAKE_STRUCTURE_COUNT; ++impact) {
                    m_wallStates[impact + (COMBAT_WALL_SLOT_SECTION_FIRST)] =
                        newWallStates[impact];
                    m_wallStates[impact] = newTowerStates[impact];
                }
                m_drawbridgeState = newKeepState;
            }
        }
        gpResourceManager->Dispose(cloudIcon);
    }
    gpMouseManager->ShowColorPointer();
}

void combatManager::ShowSpellMessage(
    i32 castByCreature,
    SpellType spell,
    army* target
) {
    char targetName[TARGET_NAME_CAPACITY];
    char message[MESSAGE_CAPACITY];
    i32 unhandledSpell [[maybe_unused]];
    if (target != NULL)
        sprintf(targetName, gArmyNamesPlural[(target->m_monsterType)]);
    if (castByCreature != 0) {
        if (spell == SPELL_PARALYZE)
            sprintf(message, "%s парализованы циклопами!", targetName);
        else if (spell == SPELL_BLIND)
            sprintf(message, "%s ослеплены атакой единорогов!", targetName);
        else if (spell == CREATURE_SPELL_PETRIFY)
            sprintf(message, "%s обращены взором медузы в камень!", targetName);
        else if (spell == SPELL_CURSE)
            sprintf(message, "%s прокляты мумиями!", targetName);
        else if (spell == CREATURE_SPELL_DISPEL)
            sprintf(message, "%s лишились наложенных на них чар\nиз-за архимагов!", targetName);
        else {
            unhandledSpell = 0;
            ++unhandledSpell;
        }
    } else {
        if (target != NULL) {
            if (m_heroes[(m_currentSide)]->m_isCaptain != 0)
                sprintf(
                    message,
                    "Капитан направляет \nзаклинание '%s' на отряд %s.",
                    gSpellNames[(spell)],
                    targetName
                );
            else
                sprintf(
                    message,
                    "%s направляет '%s' на %s",
                    m_heroes[(m_currentSide)]->m_name,
                    gSpellNames[(spell)],
                    targetName
                );
        } else {
            if (m_heroes[(m_currentSide)]->m_isCaptain != 0)
                sprintf(message, "Капитан направляет заклинание '%s'", gSpellNames[(spell)]);
            else
                sprintf(
                    message,
                    "%s направляет заклинание '%s'",
                    m_heroes[(m_currentSide)]->m_name,
                    gSpellNames[(spell)]
                );
        }
    }
    CombatMessage(message, 1, 1, 0);
}

i32 castX = 0;
i32 castY = 0;
b32 bInTeleportGetDest = false;
i32 indexToCastOn = -1;
u8 uRedBeam[] = {0x77, 0xb5, 0xbc, 0xc0, 0xc3};
u8 uRainbow[] =
    {0xbc, 0xba, 0xce, 0xca, 0xc8, 0x6f, 0x71, 0x73, 0xde, 0x5a, 0x9f, 0x47, 0x4a, 0x8d, 0x8f};
i8* gyModify = NULL;
