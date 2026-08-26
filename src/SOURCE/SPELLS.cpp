#include <Ints.h>
#include <BASE/Utf8.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <IRONFIST/creatures.h>
#include <IRONFIST/state.h>
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
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/Localization.h>


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
        FIREBLAST_EAST_FIRST_RING = H2EnumIndex(COMBAT_DIRECTION_EAST) + 1,
        FIREBLAST_WEST_FIRST_RING = H2EnumIndex(COMBAT_DIRECTION_WEST) + 1,
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
        gpGame->ViewSpells(m_heroes[H2EnumIndex(giCurGeneral)], SPELL_TYPE_COMBAT, CombatSpecialHandler, 0);
    if (m_selectedSpell != SPELL_NONE) {
        switch (m_selectedSpell) {
            case SPELL_EARTHQUAKE:
                if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == NULL) {
                    NormalDialog(
                        localization::Tr("spell.earthquake.requires_walls"),
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
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
                if (m_summonedCreatureType[H2EnumIndex(m_currentSide)] != CREATURE_SUMMONED_NONE
                    && m_summonedCreatureType[H2EnumIndex(m_currentSide)] != elementalType) {
                    NormalDialog(
                        localization::Tr("spell.summon_elemental.one_type_only"),
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                    return 0;
                }
                if (m_armyCount[H2EnumIndex(m_currentSide)] >= ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        localization::Tr("spell.summon_elemental.army_full"),
                        m_armyCount[H2EnumIndex(m_currentSide)]
                    );
                    NormalDialog(
                        gText,
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                    return 0;
                }
                if (!SpaceForElementalExists()) {
                    sprintf(
                        gText,
                        "%s",
                        localization::Tr("spell.summon_elemental.no_adjacent_space")
                    );
                    NormalDialog(
                        gText,
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
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
            case SPELL_MASS_FORCE_SHIELD:
                if (!HasValidSpellTarget(m_selectedSpell)) {
                    NormalDialog(
                        localization::Tr("spell.no_valid_target")
                        ,
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                    return 0;
                }
            set_action:
                giNextAction = ACTION_CAST_SPELL;
                giNextActionExtra = H2EnumIndex(m_selectedSpell);
                break;

            case SPELL_MIRROR_IMAGE:
                if (m_armyCount[H2EnumIndex(m_currentSide)] >= ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        localization::Tr("spell.summon_elemental.army_full"),
                        m_armyCount[H2EnumIndex(m_currentSide)]
                    );
                    NormalDialog(
                        gText,
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                    return 0;
                }
            default:
                if (!HasValidSpellTarget(m_selectedSpell)) {
                    NormalDialog(
                        localization::Tr("spell.no_valid_target")
                        ,
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_VALUE,
                        0
                    );
                    return 0;
                }
                giNextAction = ACTION_CAST_SPELL;
                giNextActionExtra = H2EnumIndex(m_selectedSpell);
                gpMouseManager->SetPointer(
                    "spelmous.mse",
                    gsSpellInfo[H2EnumIndex(m_selectedSpell)].iconIndex,
                    MOUSE_AUTO_CURSOR_TYPE
                );
                gpWindowManager->DoDialog(NULL, HandleCastSpell, 0);
                break;
        }

    restore_pointer:
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
        case SPELL_MESSAGE_HOVER: {
            hex = gpCombatManager->GetGridIndex(message.payload.mouse.x, message.payload.mouse.y);
            // Shade the whole grid by target validity, then highlight the
            // spell's footprint under the cursor.
            u8 savedShades[COMBAT_HEX_COUNT];
            for (i32 hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
                savedShades[hexIndex] = H2EnumIndex(gpCombatManager->m_gridState[hexIndex]);
            }
            for (i32 hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
                if (gpCombatManager->ValidSpellTarget(gpCombatManager->m_selectedSpell, hexIndex)) {
                    gpCombatManager->m_gridState[hexIndex] = GRID_SHADE_EMPTY_BLOCKED;
                    if (gpCombatManager->m_hexCells[hexIndex].m_occupantSide
                        != COMBAT_SIDE_NONE) {
                        gpCombatManager->m_gridState[hexIndex] = GRID_SHADE_REACHABLE;
                    }
                } else if (hexIndex % HEX_COLUMN_COUNT
                           && hexIndex % HEX_COLUMN_COUNT != HEX_RIGHT_BORDER) {
                    gpCombatManager->m_gridState[hexIndex] = GRID_SHADE_NONE;
                }
            }

            if (gpCombatManager->ValidSpellTarget(gpCombatManager->m_selectedSpell, hex)) {
                indexToCastOn = hex;
                if (gpCombatManager->m_selectedSpell == SPELL_PLASMA_CONE) {
                    // The cone streams in the direction the cursor points.
                    CursorDirection direction = gpCombatManager->GetCursorDirection(
                        message.payload.mouse.screenX, message.payload.mouse.screenY, hex
                    );
                    i32 cursorFrame;
                    switch (direction) {
                        case CURSOR_DIRECTION_LEFT_DOWN:
                            cursorFrame = 10;
                            break;
                        case CURSOR_DIRECTION_LEFT:
                            cursorFrame = 11;
                            break;
                        case CURSOR_DIRECTION_LEFT_UP:
                            cursorFrame = 12;
                            break;
                        case CURSOR_DIRECTION_RIGHT_UP:
                            cursorFrame = 7;
                            break;
                        case CURSOR_DIRECTION_RIGHT:
                            cursorFrame = 8;
                            break;
                        case CURSOR_DIRECTION_RIGHT_DOWN:
                        default:
                            cursorFrame = 9;
                            break;
                    }
                    gpMouseManager->SetPointer(
                        const_cast<char*>("cmbtmous.mse"), cursorFrame, MOUSE_AUTO_CURSOR_TYPE
                    );
                    gSpellDirection = H2EnumIndex(direction);
                } else {
                    gpMouseManager->SetPointer(
                        gsSpellInfo[H2EnumIndex(gpCombatManager->m_selectedSpell)].iconIndex
                    );
                }
                std::vector<i32> spellMask = gpCombatManager->GetSpellMask(
                    gpCombatManager->m_selectedSpell, indexToCastOn, gSpellDirection
                );
                for (i32 maskHex : spellMask) {
                    gpCombatManager->m_gridState[maskHex] = GRID_SHADE_REACHABLE;
                }
                gpCombatManager->SpellMessage(gpCombatManager->m_selectedSpell, hex);
            } else {
                indexToCastOn = NO_SELECTION;
                gpMouseManager->SetPointer(0);
                if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT && bInTeleportGetDest) {
                    gpCombatManager->CombatMessage(
                        localization::Tr("combat.spell.invalid_teleport_destination"), 1, 0, 0
                    );
                } else {
                    gpCombatManager->CombatMessage(
                        localization::Tr("combat.spell.select_target"), 1, 0, 0
                    );
                }
            }

            gbLimitToExtent = false;
            gpCombatManager->UpdateGrid(0, 0);
            gpCombatManager->DrawFrame(1, 0, 0, 0, 0, 1, 1);
            for (i32 hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
                gpCombatManager->m_gridState[hexIndex] =
                    static_cast<CombatGridShade>(savedShades[hexIndex]);
            }
            break;
        }

        case SPELL_MESSAGE_SELECT:
            if (indexToCastOn != NO_SELECTION) {
                if (bInTeleportGetDest) {
                    giNextActionGridIndex2 = indexToCastOn;
                } else {
                    giNextActionGridIndex = indexToCastOn;
                    if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT) {
                        bInTeleportGetDest = 1;
                        indexToCastOn = NO_SELECTION;
                        message.type = SPELL_MESSAGE_HOVER;
                        message.payload.mouse.x = message.payload.mouse.screenX;
                        message.payload.mouse.y = message.payload.mouse.screenY;
                        HandleCastSpell(message);
                        gpCombatManager->CombatMessage(
                            localization::Tr("combat.spell.select_teleport_destination"),
                            1, 0, 0
                        );
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                }
                bInTeleportGetDest = 0;
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
            bInTeleportGetDest = 0;
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
                &m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
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
                target = &m_armies[H2EnumIndex(m_hexCells[hex].m_deadOccupantSides[corpse])]
                                  [m_hexCells[hex].m_deadOccupantIndices[corpse]];
                if (target->SpellCastWorkChance(spell) > 0.0f)
                    return m_hexCells[hex].m_deadOccupantIndices[corpse];
            }
        }
    }
    return NO_SELECTION;
}

i32 combatManager::ValidSpellTarget(SpellType spell, i32 hex) {
    army* target_j = NULL;

    i32 destHex;
    if (!ValidHex(hex))
        return 0;

    if (spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST && spell != SPELL_METEOR_SHOWER
        && spell != SPELL_COLD_RING && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT
        && spell != SPELL_ANIMATE_DEAD && m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE) {
        target_j = &m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
        if (target_j->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_ANTI_MAGIC)] != 0
            || target_j->m_monsterType == CREATURE_GREEN_DRAGON)
            return 0;
    }

    if (target_j != NULL && target_j->SpellCastWorkChance(spell) <= 0.0f)
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

        case SPELL_FORCE_SHIELD:
        case SPELL_MASS_FORCE_SHIELD:
            // Only shieldable while the shield is below the creature's health.
            if (m_hexCells[hex].m_occupantSide != m_currentSide)
                return 0;
            if (target_j
                && ironfist::state::Get().combat.stack.forceShieldHP[target_j]
                       >= gMonsterDatabase[H2EnumIndex(target_j->m_monsterType)].hitPoints)
                return 0;
            break;

        case SPELL_MIRROR_IMAGE:
            if (m_hexCells[hex].m_occupantSide != m_currentSide)
                return 0;
            if (m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
                        .m_mirrorImageIndex
                    != NO_SELECTION
                || m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex]
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
        case SPELL_SHADOW_MARK:
        case SPELL_MARKSMAN_PIERCE:
            if (m_hexCells[hex].m_occupantSide != OppositeCombatSide(m_currentSide))
                return 0;
            break;

        case SPELL_TELEPORT:
            if (bInTeleportGetDest) {
                destHex = hex;
                if (destHex == giNextActionGridIndex
                    || !m_armies[H2EnumIndex(gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide)][gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantIndex]
                            .CanFit(destHex, 0, NULL))
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
        case SPELL_PLASMA_CONE:
        case SPELL_FIRE_BOMB:
        case SPELL_IMPLOSION_GRENADE:
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
        case SPELL_PLASMA_CONE:
        case SPELL_FIRE_BOMB:
        case SPELL_IMPLOSION_GRENADE:
            sprintf(
                gText,
                localization::Tr("combat.spell.cast"),
                gSpellNames[H2EnumIndex(spell)]
            );
            break;
        case SPELL_TELEPORT:
            if (bInTeleportGetDest) {
                strcpy(gText, localization::Tr("combat.spell.teleport_here"));
                break;
            }
            goto occupied_target;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            target =
                &m_armies[H2EnumIndex(m_currentSide)][FindResurrectArmyIndex(m_currentSide, spell, hex)];
            goto format_target;
        default:
        occupied_target:
            target =
                &m_armies[H2EnumIndex(m_hexCells[hex].m_occupantSide)][m_hexCells[hex].m_occupantIndex];
        format_target:
            sprintf(
                gText,
                localization::Tr("combat.spell.cast_on_target"),
                gSpellNames[H2EnumIndex(spell)],
                target->m_quantity == 1 ? gArmyNames[H2EnumIndex(target->m_monsterType)]
                                        : gArmyNamesPlural[H2EnumIndex(target->m_monsterType)]
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
    i32l damage1;
    i32 spellPower6;
    army* target3;
    i32 targetIndex5;
    SpellType soundSpell18;
    i32 armyIndex;
    char sampleName10[SPELL_SAMPLE_NAME_CAPACITY];
    SAMPLE2 spellSample6;
    i32 targetY9;
    i32 targetX0;
    i32 side3;
    CombatSide targetSide8;
    H2SteppedEnumStorage<ArmySpellInfluence, i32> influence1;
    i32 adjacentHex8;
    i32 oldDefense;
    army* teleportArmy6;
    icon* missileIcon6;
    float missileAngles[SPELL_MISSILE_ANGLE_COUNT];

    if (castByCreature == 0 && H2EnumIndex(spell) < H2EnumIndex(SPELL_COUNT)
        && m_eagleEyeSpell[H2EnumIndex(OppositeCombatSide(m_currentSide))] == SPELL_NONE
        && m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))] != NULL
        && !m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->HasSpell(spell)
        && m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
                   ->m_secondarySkills[H2EnumIndex(HERO_SKILL_EAGLE_EYE)]
               != HERO_SKILL_LEVEL_NONE
        && H2EnumIndex(m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
                   ->m_secondarySkills[H2EnumIndex(HERO_SKILL_EAGLE_EYE)])
                   + 1
               >= H2EnumIndex(gsSpellInfo[H2EnumIndex(spell)].level)) {
        if (SRandom(0, SPELL_EAGLE_EYE_ROLL_MAX) <= H2EnumIndex(m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
                    ->m_secondarySkills[H2EnumIndex(HERO_SKILL_EAGLE_EYE)])) {
            m_eagleEyeSpell[H2EnumIndex(OppositeCombatSide(m_currentSide))] = spell;
        }
    }

    spellSample6 = NULL;
    if (m_limitCreature != 0) {
        ResetLimitCreature();
        if (ValidHex(m_limitCreatureHex)
            && m_hexCells[m_limitCreatureHex].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
            m_limitCreatureCount[H2EnumIndex(m_hexCells[m_limitCreatureHex].m_occupantSide)]
                                [m_hexCells[m_limitCreatureHex].m_occupantIndex]++;
        }
        m_limitCreature = 0;
        m_limitCreatureHex = COMBAT_HEX_EMPTY;
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }

    if (castByCreature == 0 && m_heroes[H2EnumIndex(m_currentSide)] != NULL)
        m_heroes[H2EnumIndex(m_currentSide)]->UseSpell(spell);

    target3 = NULL;
    if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
        || spell == SPELL_METEOR_SHOWER || spell == SPELL_SUMMON_EARTH_ELEMENTAL
        || spell == SPELL_SUMMON_AIR_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL
        || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_MASS_BLESS
        || spell == SPELL_MASS_HASTE || spell == SPELL_EARTHQUAKE || spell == SPELL_MASS_CURSE
        || spell == SPELL_MASS_CURE || spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT
        || spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE || spell == SPELL_MASS_SHIELD
        || spell == SPELL_ARMAGEDDON || spell == SPELL_ELEMENTAL_STORM
        || spell == SPELL_MASS_DISPEL || spell == SPELL_MASS_FORCE_SHIELD) {
        target3 = NULL;
    } else if (ValidHex(targetHex)
               && m_hexCells[targetHex].m_occupantSide >= COMBAT_SIDE_VALID_BEGIN) {
        target3 = &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)]
                            [m_hexCells[targetHex].m_occupantIndex];
        targetSide8 = m_hexCells[targetHex].m_occupantSide;
        targetIndex5 = m_hexCells[targetHex].m_occupantIndex;
    } else {
        target3 = NULL;
    }

    if (castByCreature == 0)
        m_heroCastSpell[H2EnumIndex(m_currentSide)] = 1;

    if (castByCreature != 0) {
        spellPower6 = SPELL_DEFAULT_CREATURE_POWER;
    } else {
        spellPower6 = m_spellPower[H2EnumIndex(m_currentSide)];
        if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS)
            && (H2EnumIndex((gsSpellInfo[H2EnumIndex(spell)].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION)))) {
            spellPower6 += SPELL_HOURGLASS_POWER_BONUS;
        }
        if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT)
            && (H2EnumIndex((gsSpellInfo[H2EnumIndex(spell)].attributes) & (SPELL_INFO_ATTRIBUTE_DURATION)))) {
            spellPower6 += SPELL_WIZARD_HAT_POWER_BONUS;
        }
    }

    if (castByCreature == 0) {
        targetX0 = -1;
        targetY9 = -1;
        if (target3 != NULL) {
            targetX0 = target3->MidX();
            targetY9 = target3->MidY();
        } else {
            if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
                || spell == SPELL_METEOR_SHOWER) {
                targetX0 = m_hexCells[targetHex].m_x;
                targetY9 = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
            }
        }

        if (targetX0 != -1) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE) {
                castX = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].x[0]
                        + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                castY = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].y[0]
                        + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
            } else {
                castX = COMBAT_HERO_CAST_RIGHT_X
                        - sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].x[0];
                castY = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].y[0]
                        + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
            }
            if (targetY9 - castY
                > (targetX0 - castX) * (m_currentSide == COMBAT_ATTACKER_SIDE ? 1 : -1)) {
                m_heroAnimationState[H2EnumIndex(m_currentSide)] = COMBAT_HERO_CAST_HIGH;
                if (m_currentSide == COMBAT_ATTACKER_SIDE) {
                    castX = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].x[1]
                            + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                    castY = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].y[1]
                            + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
                } else {
                    castX = COMBAT_HERO_CAST_RIGHT_X
                            - sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].x[1];
                    castY = sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]].y[1]
                            + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
                }
            } else {
                m_heroAnimationState[H2EnumIndex(m_currentSide)] = COMBAT_HERO_CAST_LOW;
            }
        } else {
            m_heroAnimationState[H2EnumIndex(m_currentSide)] = COMBAT_HERO_CAST_NO_TARGET;
        }
        m_heroAnimationFrame[H2EnumIndex(m_currentSide)] = 0;
        while (m_heroAnimationFrame[H2EnumIndex(m_currentSide)]
               < sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]]
                     .animationFrameCount[m_heroAnimationState[H2EnumIndex(m_currentSide)]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[H2EnumIndex(m_currentSide)]++;
        }
        m_heroAnimationFrame[H2EnumIndex(m_currentSide)]--;
    }

    soundSpell18 = spell;
    if (spell == CREATURE_SPELL_PETRIFY)
        soundSpell18 = SPELL_PARALYZE;
    if (spell == CREATURE_SPELL_DISPEL)
        soundSpell18 = SPELL_DISPEL;
    if (strlen(gsSpellInfo[H2EnumIndex(soundSpell18)].soundName) > 0)
        sprintf(sampleName10, "%s.82M", gsSpellInfo[H2EnumIndex(soundSpell18)].soundName);

    if (castByCreature == 0 && target3 != NULL && !target3->SpellCastWorks(spell)) {
        ShowSpellCastFailure(target3, H2EnumIndex(spell));
        goto cast_done;
    }
    spellSample6 = LoadPlaySample(sampleName10);
    switch (spell) {
        case SPELL_TELEPORT:
            teleportArmy6 = target3;
            targetHex = teleportDestination;
            RippleCreature(
                teleportArmy6->m_side,
                teleportArmy6->m_index,
                COMBAT_RIPPLE_DEATH_RIPPLE
            );
            m_hexCells[teleportArmy6->m_hex].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[teleportArmy6->m_hex].m_occupantIndex = COMBAT_HEX_EMPTY;
            if (m_hexCells[teleportArmy6->m_hex].m_occupantFrame == ARMY_FACING_LEFT) {
                m_hexCells[teleportArmy6->m_hex + 1].m_occupantSide = COMBAT_SIDE_NONE;
                m_hexCells[teleportArmy6->m_hex + 1].m_occupantIndex = COMBAT_HEX_EMPTY;
            } else if (m_hexCells[teleportArmy6->m_hex].m_occupantFrame == ARMY_FACING_RIGHT) {
                m_hexCells[teleportArmy6->m_hex - 1].m_occupantSide = COMBAT_SIDE_NONE;
                m_hexCells[teleportArmy6->m_hex - 1].m_occupantIndex = COMBAT_HEX_EMPTY;
            }
            if (gbNoShowCombat == 0)
                WaitEndSample(&spellSample6, -1);
            if (gbNoShowCombat == 0) {
                sprintf(gText, "telptin.82m");
                spellSample6 = LoadPlaySample(gText);
            }
            if ((H2EnumIndex((teleportArmy6->m_monster.flags.all) & (MONSTER_FLAGS_WIDE))) != 0) {
                adjacentHex8 = targetHex;
                if (teleportArmy6->m_facing == ARMY_FACING_RIGHT) {
                    adjacentHex8 = teleportArmy6->GetAdjacentCellIndex(
                        adjacentHex8,
                        COMBAT_DIRECTION_EAST
                    );
                    if (adjacentHex8 == COMBAT_HEX_EMPTY
                        || (m_hexCells[adjacentHex8].m_occupantSide != COMBAT_SIDE_NONE
                            && (m_hexCells[adjacentHex8].m_occupantSide != targetSide8
                                || m_hexCells[adjacentHex8].m_occupantIndex != targetIndex5))
                        || m_hexCells[adjacentHex8].m_blocked != 0) {
                        targetHex--;
                    }
                }
                if (teleportArmy6->m_facing == ARMY_FACING_LEFT) {
                    adjacentHex8 =
                        teleportArmy6->GetAdjacentCellIndex(adjacentHex8, COMBAT_DIRECTION_WEST);
                    if (adjacentHex8 == COMBAT_HEX_EMPTY
                        || (m_hexCells[adjacentHex8].m_occupantSide != COMBAT_SIDE_NONE
                            && (m_hexCells[adjacentHex8].m_occupantSide != targetSide8
                                || m_hexCells[adjacentHex8].m_occupantIndex != targetIndex5))
                        || m_hexCells[adjacentHex8].m_blocked != 0) {
                        targetHex++;
                    }
                }
                teleportArmy6->m_hex = targetHex;
                switch (teleportArmy6->m_facing) {
                    case ARMY_FACING_RIGHT:
                        m_hexCells[teleportArmy6->m_hex].m_occupantSide = targetSide8;
                        m_hexCells[teleportArmy6->m_hex].m_occupantIndex =
                            static_cast<i8>(targetIndex5);
                        m_hexCells[teleportArmy6->m_hex].m_occupantFrame = ARMY_FACING_LEFT;
                        m_hexCells[teleportArmy6->m_hex + 1].m_occupantSide = targetSide8;
                        m_hexCells[teleportArmy6->m_hex + 1].m_occupantIndex =
                            static_cast<i8>(targetIndex5);
                        m_hexCells[teleportArmy6->m_hex + 1].m_occupantFrame = ARMY_FACING_RIGHT;
                        break;
                    case ARMY_FACING_LEFT:
                        m_hexCells[teleportArmy6->m_hex].m_occupantSide = targetSide8;
                        m_hexCells[teleportArmy6->m_hex].m_occupantIndex =
                            static_cast<i8>(targetIndex5);
                        m_hexCells[teleportArmy6->m_hex].m_occupantFrame = ARMY_FACING_RIGHT;
                        m_hexCells[teleportArmy6->m_hex - 1].m_occupantSide = targetSide8;
                        m_hexCells[teleportArmy6->m_hex - 1].m_occupantIndex =
                            static_cast<i8>(targetIndex5);
                        m_hexCells[teleportArmy6->m_hex - 1].m_occupantFrame = ARMY_FACING_LEFT;
                        break;
                }
                RippleCreature(
                    teleportArmy6->m_side,
                    teleportArmy6->m_index,
                    COMBAT_RIPPLE_DEATH_WAVE
                );
            } else {
                teleportArmy6->m_hex = targetHex;
                m_hexCells[teleportArmy6->m_hex].m_occupantSide = targetSide8;
                m_hexCells[teleportArmy6->m_hex].m_occupantIndex = static_cast<i8>(targetIndex5);
                m_hexCells[teleportArmy6->m_hex].m_occupantFrame = ARMY_FACING_NONE;
                RippleCreature(
                    teleportArmy6->m_side,
                    teleportArmy6->m_index,
                    COMBAT_RIPPLE_DEATH_WAVE
                );
            }
            break;
        case SPELL_DISRUPTING_RAY:
            oldDefense = target3->m_monster.defense;
            target3->m_monster.defense -= SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
            if (target3->m_monster.defense < SPELL_MINIMUM_DEFENSE)
                target3->m_monster.defense = SPELL_MINIMUM_DEFENSE;
            sprintf(
                gText,
                localization::Tr("combat.spell.disrupting_ray"),
                oldDefense - target3->m_monster.defense
            );
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            RippleCreature(target3->m_side, target3->m_index, COMBAT_RIPPLE_WAVE);
            break;
        case SPELL_COLD_RAY:
            DelayMilli(
                static_cast<i32l>(SPELL_COLD_RAY_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
            );
            damage1 = spellPower6 * SPELL_COLD_RAY_DAMAGE_PER_POWER;
            if (target3->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                damage1 <<= 1;
            if (target3->m_monsterType == CREATURE_IRON_GOLEM
                || target3->m_monsterType == CREATURE_STEEL_GOLEM)
                damage1 = static_cast<i32l>(damage1 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(
                &damage1,
                SPELL_COLD_RAY,
                m_heroes[H2EnumIndex(m_currentSide)],
                m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
            );
            sprintf(
                gText,
                localization::Tr("combat.spell.cold_ray_damage"),
                damage1,
                target3->m_quantity <= PLURAL_QUANTITY_MINIMUM - 1
                    ? gArmyNames[H2EnumIndex(target3->m_monsterType)]
                    : gArmyNamesPlural[H2EnumIndex(target3->m_monsterType)]
            );
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_COLD_RAY)].combatEffect, 0, 0);
            target3->Damage(damage1, SPELL_NONE);
            target3->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            break;
        case SPELL_CHAIN_LIGHTNING:
            ChainLightning(targetHex, spellPower6);
            break;
        case SPELL_MAGIC_ARROW:
            if (1) {
                DelayMilli(
                    static_cast<i32l>(SPELL_MAGIC_ARROW_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
                );
                damage1 = spellPower6 * SPELL_MAGIC_ARROW_DAMAGE_PER_POWER;
                ModifyDamageForArtifacts(
                    &damage1,
                    SPELL_MAGIC_ARROW,
                    m_heroes[H2EnumIndex(m_currentSide)],
                    m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
                );
                sprintf(
                    gText,
                    localization::Tr("combat.spell.magic_arrow_damage"),
                    damage1,
                    target3->m_quantity <= PLURAL_QUANTITY_MINIMUM - 1
                        ? gArmyNames[H2EnumIndex(target3->m_monsterType)]
                        : gArmyNamesPlural[H2EnumIndex(target3->m_monsterType)]
                );
                CombatMessage(gText, 1, 1, 0);
                missileIcon6 = gpResourceManager->GetIcon("keep.icn");
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
                    target3->MidX(),
                    target3->MidY(),
                    missileAngles,
                    missileIcon6
                );
                gpResourceManager->Dispose(missileIcon6);
                target3->Damage(damage1, SPELL_NONE);
                target3->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            }
            break;
        case SPELL_LIGHTNING_BOLT:
            damage1 = spellPower6 * SPELL_LIGHTNING_DAMAGE_PER_POWER;
            if (target3->m_monsterType == CREATURE_AIR_ELEMENTAL)
                damage1 <<= 1;
            if (target3->m_monsterType == CREATURE_IRON_GOLEM
                || target3->m_monsterType == CREATURE_STEEL_GOLEM)
                damage1 = static_cast<i32l>(damage1 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(
                &damage1,
                SPELL_LIGHTNING_BOLT,
                m_heroes[H2EnumIndex(m_currentSide)],
                m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
            );
            sprintf(
                gText,
                localization::Tr("combat.spell.lightning_bolt_damage"),
                damage1,
                target3->m_quantity <= PLURAL_QUANTITY_MINIMUM - 1
                    ? gArmyNames[H2EnumIndex(target3->m_monsterType)]
                    : gArmyNamesPlural[H2EnumIndex(target3->m_monsterType)]
            );
            CombatMessage(gText, 1, 1, 0);
            DoBolt(
                1,
                castX,
                castY,
                target3->MidX(),
                target3->MidY(),
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
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_LIGHTNING_BOLT)].combatEffect, 0, 0);
            target3->Damage(damage1, SPELL_NONE);
            target3->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
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
        case SPELL_MASS_FORCE_SHIELD:
            CastMassSpell(spell, spellPower6);
            break;
        case SPELL_MIRROR_IMAGE:
            MirrorImage(targetHex);
            break;
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            SummonElemental(CREATURE_EARTH_ELEMENTAL, spellPower6);
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            SummonElemental(CREATURE_AIR_ELEMENTAL, spellPower6);
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            SummonElemental(CREATURE_FIRE_ELEMENTAL, spellPower6);
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            SummonElemental(CREATURE_WATER_ELEMENTAL, spellPower6);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            Resurrect(spell, targetHex, spellPower6);
            break;
        case SPELL_CURE:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_CURE)].combatEffect, 0, 0);
            target3->Cure(spellPower6);
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            break;
        case SPELL_SLOW:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SLOW, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_SLOW)].combatEffect, 0, 0);
            break;
        case SPELL_HASTE:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_HASTE, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_HASTE)].combatEffect, 0, 0);
            break;
        case SPELL_SHIELD:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SHIELD, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_SHIELD)].combatEffect, 0, 0);
            break;
        case SPELL_DRAGON_SLAYER:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_DRAGON_SLAYER)].combatEffect, 0, 0);
            break;
        case SPELL_BLESS:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLESS, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_BLESS)].combatEffect, 0, 0);
            break;
        case SPELL_STONE_SKIN:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_STONESKIN, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_STONE_SKIN)].combatEffect, 0, 0);
            break;
        case SPELL_STEEL_SKIN:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_STEELSKIN, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_STEEL_SKIN)].combatEffect, 0, 0);
            break;
        case SPELL_CURSE:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_CURSE, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_CURSE)].combatEffect, 0, 0);
            break;
        case SPELL_BERSERKER:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BERSERK, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_BERSERKER)].combatEffect, 0, 0);
            break;
        case SPELL_HYPNOTIZE:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_HYPNOTIZE, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_HYPNOTIZE)].combatEffect, 0, 0);
            break;
        case SPELL_PARALYZE:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_PARALYZE, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_PARALYZE)].combatEffect, 0, 0);
            break;
        case CREATURE_SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->DispelGood();
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_DISPEL)].combatEffect, 0, 1);
            break;
        case SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->DispelGood();
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_DISPEL)].combatEffect, 0, 0);
            for (influence1 = ARMY_SPELL_INFLUENCE_HASTE; influence1 < ARMY_SPELL_INFLUENCE_COUNT;
                 influence1++)
                target3->CancelIndividualSpell(influence1);
            break;
        case SPELL_BLIND:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLIND, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_BLIND)].combatEffect, 0, 0);
            break;
        case SPELL_BLOOD_LUST:
            ShowSpellMessage(castByCreature, spell, target3);
            BloodLustEffect(target3, MONSTER_ABILITY_FLAG_BLOOD_LUST);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLOODLUST, SPELL_BLOOD_LUST_DURATION);
            break;
        case SPELL_ANTI_MAGIC:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_ANTI_MAGIC, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_ANTI_MAGIC)].combatEffect, 0, 0);
            break;
        case CREATURE_SPELL_PETRIFY:
            ShowSpellMessage(castByCreature, spell, target3);
            TurnToStone(target3);
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
        case SPELL_SHADOW_MARK:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_SHADOW_MARK, 1);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_SHADOW_MARK)].combatEffect, 0, 0);
            break;
        case SPELL_MARKSMAN_PIERCE: {
            DelayMilli(
                static_cast<i32l>(
                    gfCombatSpeedMod[gConfig.combatSpeed]
                    * static_cast<float>(SPELL_PIERCE_DELAY)
                )
            );
            i32l pierceDamage = SPELL_PIERCE_DAMAGE;
            // The keep's walls blunt the round.
            if (m_inCastleCombat && m_currentSide == COMBAT_ATTACKER_SIDE
                && InCastle(target3->m_hex)) {
                pierceDamage = static_cast<i32l>(pierceDamage * 0.75);
            }
            sprintf(
                gText,
                localization::Tr("combat.spell.marksman_pierce_damage"),
                static_cast<i32>(pierceDamage),
                target3->m_quantity > 1 ? ironfist::GetCreaturePluralName(H2EnumIndex(target3->m_monsterType))
                                        : ironfist::GetCreatureName(H2EnumIndex(target3->m_monsterType))
            );
            CombatMessage(gText, 1, 1, 0);
            float pierceAngles[SPELL_MISSILE_ANGLE_COUNT] = {
                90.000000f,
                45.000038f,
                26.565073f,
                18.262905f,
                0.000000f,
                -18.262905f,
                -26.565073f,
                -45.000038f,
                -90.000000f
            };
            icon* arrowIcon = gpResourceManager->GetIcon("keep.icn");
            ShootMissile(
                castX, castY, target3->MidX(), target3->MidY(), pierceAngles, arrowIcon
            );
            gpResourceManager->Dispose(arrowIcon);
            target3->Damage(pierceDamage, SPELL_NONE);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_DAZE, 1);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_MARKSMAN_PIERCE)].combatEffect, 0, 0);
            target3->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
            break;
        }
        case SPELL_PLASMA_CONE:
            PlasmaCone(targetHex);
            break;
        case SPELL_FORCE_SHIELD:
            ShowSpellMessage(castByCreature, spell, target3);
            target3->SetSpellInfluence(ARMY_SPELL_INFLUENCE_FORCE_SHIELD, spellPower6);
            target3->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_FORCE_SHIELD)].combatEffect, 0, 0);
            break;
        case SPELL_FIRE_BOMB:
            FireBomb(targetHex);
            break;
        case SPELL_IMPLOSION_GRENADE:
            ImplosionGrenade(targetHex);
            break;
        default:
            DefaultSpell(targetHex);
            break;
    }

cast_done:

    for (side3 = 0; side3 < COMBAT_SIDE_COUNT; side3++) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side3)]; armyIndex++) {
            army& combatArmy = m_armies[H2EnumIndex(side3)][armyIndex];
            combatArmy.m_killPending = 0;
            combatArmy.m_deathPending = 0;
            combatArmy.m_damagePending = 0;
            combatArmy.m_drawState = ARMY_DRAW_NORMAL;
            combatArmy.m_animationState = 0;
            combatArmy.m_lastTargetHex = COMBAT_HEX_EMPTY;
        }
    }

    if (castByCreature == 0) {
        m_heroAnimationState[H2EnumIndex(m_currentSide)]++;
        m_heroAnimationFrame[H2EnumIndex(m_currentSide)] = 0;
        while (m_heroAnimationFrame[H2EnumIndex(m_currentSide)]
               < sCmbtHero[m_heroSpriteIndex[H2EnumIndex(m_currentSide)]]
                     .animationFrameCount[m_heroAnimationState[H2EnumIndex(m_currentSide)]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[H2EnumIndex(m_currentSide)]++;
        }
        m_heroAnimationState[H2EnumIndex(m_currentSide)] = 0;
        m_heroAnimationFrame[H2EnumIndex(m_currentSide)] = 0;
        DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }
    WaitEndSample(&spellSample6, -1);
    CheckChangeSelector();
}

void combatManager::DefaultSpell(i32 targetHex) {
    if (!ValidHex(targetHex) || m_hexCells[targetHex].m_occupantSide < COMBAT_ATTACKER_SIDE)
        return;
    army* target =
        &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];
    target->SpellEffect(gsSpellInfo[H2EnumIndex(m_selectedSpell)].combatEffect, 0, 1);
}

// Ironfist's area-spell engine: hex masks per spell shape, one impact
// animation, and shared damage application. The Plasma Cone streams in the
// direction the caster picked.
i32 gSpellDirection;

static i32 GetHexNeighboursLine(
    i32 startingFrom, i32 neighborDirection, i32 length, std::vector<i32>& hexes
) {
    hexes.clear();
    i32 currentHex = startingFrom;
    hexes.push_back(startingFrom);
    for (i32 step = 0; step < length - 1; step++) {
        i32 neighbor = GetAdjacentCellIndexNoArmy(
            currentHex, static_cast<CombatHexDirection>(neighborDirection)
        );
        if (neighbor == -1) {
            break;
        }
        hexes.push_back(neighbor);
        currentHex = neighbor;
    }
    return hexes.empty();
}

static std::vector<i32> GetPlasmaConeSpellMask(i32 fromHex, i32 direction) {
    static const i32 spellNeighbourDirections[ARMY_ADJACENT_DIRECTION_COUNT][3] = {
        {5, 0, 1}, // right up
        {0, 1, 2}, // right
        {1, 2, 3}, // right down
        {2, 3, 4}, // left down
        {3, 4, 5}, // left
        {4, 5, 0}  // left up
    };
    struct HexLineData {
        i32 fromHex;
        i32 length;
    };
    std::vector<i32> mask;
    std::vector<HexLineData> hexLines;
    CombatHexDirection leftDirection =
        static_cast<CombatHexDirection>(spellNeighbourDirections[direction][0]);
    CombatHexDirection centerDirection =
        static_cast<CombatHexDirection>(spellNeighbourDirections[direction][1]);
    CombatHexDirection rightDirection =
        static_cast<CombatHexDirection>(spellNeighbourDirections[direction][2]);

    hexLines.push_back({fromHex, 8});

    i32 sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(fromHex, centerDirection), leftDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 6});
    }
    sideHex = GetAdjacentCellIndexNoArmy(sideHex, centerDirection);
    sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(sideHex, centerDirection), leftDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 4});
    }
    sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(sideHex, centerDirection), leftDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 2});
    }

    sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(fromHex, centerDirection), rightDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 6});
    }
    sideHex = GetAdjacentCellIndexNoArmy(sideHex, centerDirection);
    sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(sideHex, centerDirection), rightDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 4});
    }
    sideHex = GetAdjacentCellIndexNoArmy(
        GetAdjacentCellIndexNoArmy(sideHex, centerDirection), rightDirection
    );
    if (sideHex != -1) {
        hexLines.push_back({sideHex, 2});
    }

    for (auto& line : hexLines) {
        std::vector<i32> lineHexes;
        if (!GetHexNeighboursLine(line.fromHex, H2EnumIndex(centerDirection), line.length, lineHexes)) {
            mask.insert(mask.end(), lineHexes.begin(), lineHexes.end());
        }
    }
    return mask;
}

static double GetDistanceBetweenPoints(i32 fromX, i32 fromY, i32 toX, i32 toY) {
    return sqrt(pow(fromX - toX, 2) + pow(fromY - toY, 2));
}

static bool IsOutOfBoundsHex(i32 hexIndex) {
    return !ValidHex(hexIndex)
           || !(hexIndex % ARMY_HEX_COLUMNS
                && hexIndex % ARMY_HEX_COLUMNS != ARMY_HEX_COLUMNS - 1);
}

std::vector<i32> combatManager::GetSpellMask(SpellType spell, i32 fromHex, i32 direction) {
    std::vector<i32> mask;
    switch (spell) {
        case SPELL_PLASMA_CONE:
            mask = GetPlasmaConeSpellMask(fromHex, direction);
            break;
        case SPELL_FIREBALL:
        case SPELL_FIRE_BOMB:
        case SPELL_METEOR_SHOWER:
            mask = GetSpellMask(SPELL_COLD_RING, fromHex, direction);
            mask.push_back(fromHex);
            break;
        case SPELL_COLD_RING:
            for (i32 dir = 0; dir < ARMY_ADJACENT_DIRECTION_COUNT; dir++) {
                i32 neighbor =
                    GetAdjacentCellIndexNoArmy(fromHex, static_cast<CombatHexDirection>(dir));
                if (ValidSpellTarget(spell, neighbor)) {
                    mask.push_back(neighbor);
                }
            }
            break;
        case SPELL_FIREBLAST:
            mask = GetSpellMask(SPELL_FIREBALL, fromHex, direction);
            for (i32 dir = 0; dir < ARMY_ADJACENT_DIRECTION_COUNT; dir++) {
                i32 straightHex = GetAdjacentCellIndexNoArmy(
                    GetAdjacentCellIndexNoArmy(fromHex, static_cast<CombatHexDirection>(dir)),
                    static_cast<CombatHexDirection>(dir)
                );
                if (ValidSpellTarget(spell, straightHex)) {
                    mask.push_back(straightHex);
                }
            }
            for (i32 dir = 0; dir < ARMY_ADJACENT_DIRECTION_COUNT; dir++) {
                i32 closestNeighbor =
                    GetAdjacentCellIndexNoArmy(fromHex, static_cast<CombatHexDirection>(dir));
                if (ValidSpellTarget(spell, closestNeighbor)) {
                    i32 rightHex = GetAdjacentCellIndexNoArmy(
                        closestNeighbor,
                        static_cast<CombatHexDirection>((dir + 1) % ARMY_ADJACENT_DIRECTION_COUNT)
                    );
                    if (ValidSpellTarget(spell, rightHex)) {
                        mask.push_back(rightHex);
                    }
                    i32 leftHex = GetAdjacentCellIndexNoArmy(
                        closestNeighbor,
                        static_cast<CombatHexDirection>(
                            (dir + ARMY_ADJACENT_DIRECTION_COUNT - 1)
                            % ARMY_ADJACENT_DIRECTION_COUNT
                        )
                    );
                    if (ValidSpellTarget(spell, leftHex)) {
                        mask.push_back(leftHex);
                    }
                }
            }
            break;
        case SPELL_IMPLOSION_GRENADE: {
            double maxDistance =
                COMBAT_HEX_SIZE * SPELL_IMPLOSION_HEX_RADIUS - COMBAT_HEX_SIZE / 2;
            for (i32 hexIndex = 0; hexIndex < COMBAT_HEX_COUNT; hexIndex++) {
                if (!ValidSpellTarget(spell, hexIndex)) {
                    continue;
                }
                double distance = GetDistanceBetweenPoints(
                    m_hexCells[fromHex].m_x,
                    m_hexCells[fromHex].m_gridBottom,
                    m_hexCells[hexIndex].m_x,
                    m_hexCells[hexIndex].m_gridBottom
                );
                if (distance < maxDistance) {
                    mask.push_back(hexIndex);
                }
            }
        }
        default:
            mask.push_back(fromHex);
    }
    std::set<i32> uniqueHexes;
    for (i32 hexIndex : mask) {
        if (hexIndex != -1) {
            uniqueHexes.insert(hexIndex);
        }
    }
    mask.assign(uniqueHexes.begin(), uniqueHexes.end());
    return mask;
}

void combatManager::AreaSpellMessage(SpellType spell, i32l damage) {
    const char* messageId;
    switch (spell) {
        case SPELL_FIREBALL:
            messageId = "combat.spell.fireball_damage";
            break;
        case SPELL_FIREBLAST:
            messageId = "combat.spell.fireblast_damage";
            break;
        case SPELL_COLD_RING:
            messageId = "combat.spell.cold_ring_damage";
            break;
        case SPELL_METEOR_SHOWER:
            messageId = "combat.spell.meteor_shower_damage";
            break;
        case SPELL_PLASMA_CONE:
            messageId = "combat.spell.plasma_cone_damage";
            break;
        case SPELL_FIRE_BOMB:
            messageId = "combat.spell.fire_bomb_damage";
            break;
        case SPELL_IMPLOSION_GRENADE:
            messageId = "combat.spell.implosion_grenade_damage";
            break;
        default:
            sprintf(
                gText,
                localization::Tr("combat.spell.area_damage"),
                H2EnumIndex(spell),
                static_cast<i32>(damage)
            );
            CombatMessage(gText, 1, 1, 0);
            return;
    }
    sprintf(gText, localization::Tr(messageId), static_cast<i32>(damage));
    CombatMessage(gText, 1, 1, 0);
}

void combatManager::AreaSpellDrawImpact(
    i32 hexIndex, icon* spellIcon, double speedMultiplier, i32 drawTimes, i32 flip
) {
    if (!gbNoShowCombat) {
        i32 x = m_hexCells[hexIndex].m_x;
        i32 y = m_hexCells[hexIndex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
        for (i32 pass = 0; pass < drawTimes; pass++) {
            for (i32 frame = 0; frame < spellIcon->m_frameCount; frame++) {
                glTimers[0] = static_cast<i32>(
                    platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * speedMultiplier
                );
                IconToBitmap(
                    spellIcon,
                    gpWindowManager->m_screen,
                    x,
                    y,
                    frame,
                    ICON_DRAW_CLIP,
                    0,
                    0,
                    COMBAT_SCREEN_WIDTH,
                    COMBAT_AREA_HEIGHT,
                    0
                );
                if (flip) {
                    FlipIconToBitmap(
                        spellIcon,
                        gpWindowManager->m_screen,
                        x,
                        y,
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
                DrawFrame(0, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
                DelayTil(glTimers);
            }
        }
        gpResourceManager->Dispose(spellIcon);
    }
    DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
}

void combatManager::AreaSpellDoDamage(i32l spellDamage, SpellType spell, army* target) {
    ModifyDamageForArtifacts(
        &spellDamage,
        spell,
        m_heroes[H2EnumIndex(m_currentSide)],
        m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
    );
    AreaSpellMessage(spell, spellDamage);
    target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
}

bool combatManager::AreaSpellAffectHexes(
    i32 hexIndex, army* target, SpellType spell, i32l spellDamage, std::vector<i32>& affectedHexes
) {
    affectedHexes = GetSpellMask(spell, hexIndex, gSpellDirection);
    bool anyoneDamaged = false;
    for (i32 hex : affectedHexes) {
        hexcell* cell = &m_hexCells[hex];
        if (spell == SPELL_FIRE_BOMB) {
            // Fire walls linger where the bomb burst.
            bool wallExists = false;
            for (auto& wall : ironfist::state::Get().combat.spell.fireBombWalls) {
                if (wall.hexIdx == hex) {
                    wallExists = true;
                    wall.turnsLeft = COMBAT_BURN_ROUNDS;
                    wall.currentFrame = 0;
                    break;
                }
            }
            if (!wallExists) {
                ironfist::state::Get().combat.spell.fireBombWalls.push_back(
                    {hex, COMBAT_BURN_ROUNDS, 0}
                );
            }
        }
        if (cell->m_occupantSide == COMBAT_SIDE_NONE) {
            continue;
        }
        target = &m_armies[H2EnumIndex(cell->m_occupantSide)][cell->m_occupantIndex];
        if (!target->SpellCastWorks(spell)) {
            continue;
        }
        if (gArmyEffected[H2EnumIndex(target->m_side)][target->m_index]) {
            continue;
        }
        gArmyEffected[H2EnumIndex(target->m_side)][target->m_index] = 1;
        if (!target->m_damagePending) {
            i32l damage = spellDamage;
            if (target->m_monsterType == CREATURE_FIRE_ELEMENTAL && spell == SPELL_COLD_RING) {
                damage *= ELEMENTAL_WEAKNESS_MULTIPLIER;
            }
            if (target->m_monsterType == CREATURE_WATER_ELEMENTAL
                && (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)) {
                damage *= ELEMENTAL_WEAKNESS_MULTIPLIER;
            }
            if (target->m_monsterType == CREATURE_IRON_GOLEM
                || target->m_monsterType == CREATURE_STEEL_GOLEM) {
                damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            }
            if (target->m_monsterType == CREATURE_EARTH_ELEMENTAL
                && spell == SPELL_METEOR_SHOWER) {
                damage *= ELEMENTAL_WEAKNESS_MULTIPLIER;
            }
            if (spell == SPELL_FIRE_BOMB) {
                CheckBurnCreature(target);
            } else {
                target->Damage(damage, spell);
            }
            anyoneDamaged = true;
        }
    }
    return anyoneDamaged;
}

bool combatManager::AreaSpellAffectHexes(
    i32 hexIndex, army* target, SpellType spell, i32l spellDamage
) {
    std::vector<i32> affectedHexes;
    return AreaSpellAffectHexes(hexIndex, target, spell, spellDamage, affectedHexes);
}

void combatManager::PlasmaCone(i32 hexIndex) {
    if (!ValidHex(hexIndex)) {
        return;
    }
    AreaSpellDrawImpact(hexIndex, gpResourceManager->GetIcon("fireball.icn"), 75.0, 1, 0);
    ClearEffects();

    i32l spellDamage = SPELL_AREA_DAMAGE_PER_POWER * m_spellPower[H2EnumIndex(m_currentSide)];
    army* target = &m_armies[H2EnumIndex(m_currentSide)][m_currentArmyIndex];
    if (AreaSpellAffectHexes(hexIndex, target, SPELL_PLASMA_CONE, spellDamage)) {
        AreaSpellDoDamage(spellDamage, SPELL_PLASMA_CONE, target);
    }
}

void combatManager::FireBomb(i32 hexIndex) {
    if (!ValidHex(hexIndex)) {
        return;
    }
    AreaSpellDrawImpact(hexIndex, gpResourceManager->GetIcon("fireball.icn"), 75.0, 1, 0);
    ClearEffects();

    i32l spellDamage = SPELL_AREA_DAMAGE_PER_POWER * m_spellPower[H2EnumIndex(m_currentSide)];
    army* target = &m_armies[H2EnumIndex(m_currentSide)][m_currentArmyIndex];
    if (AreaSpellAffectHexes(hexIndex, target, SPELL_FIRE_BOMB, spellDamage)) {
        AreaSpellDoDamage(spellDamage, SPELL_FIRE_BOMB, target);
    }
}

void combatManager::ImplosionGrenade(i32 hexIndex) {
    if (!ValidHex(hexIndex)) {
        return;
    }
    AreaSpellDrawImpact(hexIndex, gpResourceManager->GetIcon("fireball.icn"), 75.0, 1, 0);
    ClearEffects();

    i32l spellDamage = SPELL_AREA_DAMAGE_PER_POWER * m_spellPower[H2EnumIndex(m_currentSide)];
    army* target = &m_armies[H2EnumIndex(m_currentSide)][m_currentArmyIndex];
    std::vector<i32> affectedHexes;
    bool anyoneDamaged =
        AreaSpellAffectHexes(hexIndex, target, SPELL_IMPLOSION_GRENADE, spellDamage, affectedHexes);

    affectedHexes.erase(
        std::remove(affectedHexes.begin(), affectedHexes.end(), hexIndex), affectedHexes.end()
    );

    // Each affected stack gets pulled one hex toward the blast's center.
    std::map<i32, i32> pullDestinations;
    i32 centerRow = hexIndex / ARMY_HEX_COLUMNS;
    i32 centerColumn = hexIndex % ARMY_HEX_COLUMNS;
    for (i32 affectedHex : affectedHexes) {
        i32 rowDiff = centerRow - affectedHex / ARMY_HEX_COLUMNS;
        i32 columnDiff = centerColumn - affectedHex % ARMY_HEX_COLUMNS;
        i32 pullDirection;
        if (rowDiff < 0) {
            if (columnDiff < 0) {
                pullDirection = abs(columnDiff) > abs(rowDiff) ? 4 : 5;
            } else {
                pullDirection = abs(columnDiff) > abs(rowDiff) ? 1 : 0;
            }
        } else {
            if (columnDiff < 0) {
                pullDirection = abs(columnDiff) > abs(rowDiff) ? 4 : 3;
            } else {
                pullDirection = abs(columnDiff) > abs(rowDiff) ? 1 : 2;
            }
        }
        pullDestinations[affectedHex] = GetAdjacentCellIndexNoArmy(
            affectedHex, static_cast<CombatHexDirection>(pullDirection)
        );
    }

    // Pull ring by ring from the inside out, keeping two-hexer occupancy
    // straight, then animate the drags together.
    hexcell* centerCell = &m_hexCells[hexIndex];
    std::map<army*, i32> creatureMovesFrom;
    for (i32 ring = 1; ring <= SPELL_IMPLOSION_HEX_RADIUS; ring++) {
        double maxDistance = COMBAT_HEX_SIZE * ring - COMBAT_HEX_SIZE / 2;
        auto it = affectedHexes.begin();
        while (it != affectedHexes.end()) {
            i32 affectedHex = *it;
            hexcell* movableCell = &m_hexCells[affectedHex];
            if (movableCell->m_occupantSide == COMBAT_SIDE_NONE) {
                it = affectedHexes.erase(it);
                continue;
            }
            double distance = GetDistanceBetweenPoints(
                centerCell->m_x,
                centerCell->m_gridBottom,
                movableCell->m_x,
                movableCell->m_gridBottom
            );
            if (!(distance < maxDistance)) {
                ++it;
                continue;
            }
            it = affectedHexes.erase(it);

            i32 destinationHex = pullDestinations[affectedHex];
            if (destinationHex == -1) {
                continue;
            }
            hexcell* destinationCell = &m_hexCells[destinationHex];
            army* creatureToMove =
                &m_armies[H2EnumIndex(movableCell->m_occupantSide)][movableCell->m_occupantIndex];
            if (destinationCell->m_blocked) {
                continue;
            }
            if (!creatureToMove->CanFit(destinationHex, 0, 0)) {
                continue;
            }
            if (destinationCell->m_occupantSide != COMBAT_SIDE_NONE
                && !(movableCell->m_occupantSide == destinationCell->m_occupantSide
                     && movableCell->m_occupantIndex == destinationCell->m_occupantIndex)) {
                continue;
            }

            if ((H2EnumIndex((creatureToMove->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                i32 destinationSecondHex;
                i32 affectedSecondHex;
                if (movableCell->m_occupantFrame == ARMY_FACING_RIGHT) {
                    destinationSecondHex = destinationHex - 1;
                    affectedSecondHex = affectedHex - 1;
                } else {
                    destinationSecondHex = destinationHex + 1;
                    affectedSecondHex = affectedHex + 1;
                }
                hexcell* destinationSecondCell = &m_hexCells[destinationSecondHex];
                hexcell* movableSecondCell = &m_hexCells[affectedSecondHex];

                if (destinationSecondCell->m_blocked) {
                    continue;
                }
                if (destinationSecondCell->m_occupantSide != COMBAT_SIDE_NONE
                    && !(movableSecondCell->m_occupantSide
                             == destinationSecondCell->m_occupantSide
                         && movableSecondCell->m_occupantIndex
                                == destinationSecondCell->m_occupantIndex)) {
                    continue;
                }

                if (movableCell->m_occupantFrame == ARMY_FACING_RIGHT) {
                    if (creatureToMove->m_facing == ARMY_FACING_RIGHT) {
                        if (IsOutOfBoundsHex(destinationHex)) {
                            continue;
                        }
                        creatureMovesFrom[creatureToMove] = creatureToMove->m_hex;
                        creatureToMove->m_hex = destinationSecondHex;
                    } else {
                        if (IsOutOfBoundsHex(destinationSecondHex)) {
                            continue;
                        }
                        creatureMovesFrom[creatureToMove] = creatureToMove->m_hex;
                        creatureToMove->m_hex = destinationHex;
                    }
                } else {
                    if (creatureToMove->m_facing == ARMY_FACING_RIGHT) {
                        if (IsOutOfBoundsHex(destinationHex)) {
                            continue;
                        }
                        creatureMovesFrom[creatureToMove] = creatureToMove->m_hex;
                        creatureToMove->m_hex = destinationHex;
                    } else {
                        if (IsOutOfBoundsHex(destinationSecondHex)) {
                            continue;
                        }
                        creatureMovesFrom[creatureToMove] = creatureToMove->m_hex;
                        creatureToMove->m_hex = destinationSecondHex;
                    }
                }

                i32 occupantIndex = movableCell->m_occupantIndex;
                movableCell->m_occupantIndex = movableSecondCell->m_occupantIndex = -1;
                destinationCell->m_occupantIndex = destinationSecondCell->m_occupantIndex =
                    static_cast<i8>(occupantIndex);

                CombatSide occupantSide = movableCell->m_occupantSide;
                movableCell->m_occupantSide = movableSecondCell->m_occupantSide =
                    COMBAT_SIDE_NONE;
                destinationCell->m_occupantSide = destinationSecondCell->m_occupantSide =
                    occupantSide;

                ArmyFacing frontFrame = movableCell->m_occupantFrame;
                ArmyFacing rearFrame = movableSecondCell->m_occupantFrame;
                movableCell->m_occupantFrame = movableSecondCell->m_occupantFrame =
                    ARMY_FACING_NONE;
                destinationCell->m_occupantFrame = frontFrame;
                destinationSecondCell->m_occupantFrame = rearFrame;

                auto secondFound =
                    std::find(affectedHexes.begin(), affectedHexes.end(), affectedSecondHex);
                if (secondFound != affectedHexes.end()) {
                    it = affectedHexes.erase(secondFound);
                }
            } else {
                creatureMovesFrom[creatureToMove] = creatureToMove->m_hex;
                creatureToMove->m_hex = destinationHex;
                destinationCell->m_occupantIndex = movableCell->m_occupantIndex;
                destinationCell->m_occupantSide = movableCell->m_occupantSide;
                movableCell->m_occupantIndex = -1;
                movableCell->m_occupantSide = COMBAT_SIDE_NONE;
            }
        }
    }

    for (i32 frame = 0; frame < SPELL_IMPLOSION_DRAG_FRAMES; frame++) {
        for (auto& animated : creatureMovesFrom) {
            army* creature = animated.first;
            i32 initialHex = animated.second;
            creature->m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
            creature->m_animationFrame = 0;

            i32 startX = m_hexCells[initialHex].m_x;
            i32 startY = m_hexCells[initialHex].m_y;
            float stepX = static_cast<float>(m_hexCells[creature->m_hex].m_x - startX)
                          / static_cast<float>(SPELL_IMPLOSION_DRAG_FRAMES);
            float stepY = static_cast<float>(m_hexCells[creature->m_hex].m_y - startY)
                          / static_cast<float>(SPELL_IMPLOSION_DRAG_FRAMES);
            creature->DrawToBuffer(
                static_cast<i32>(startX + stepX * frame),
                static_cast<i32>(startY + stepY * frame),
                0
            );
        }
        gpWindowManager->UpdateScreenRegion(0, 0, COMBAT_SCREEN_WIDTH, COMBAT_SCREEN_HEIGHT);
        DelayTil(glTimers);
        glTimers[0] = static_cast<i32>(
            platform::Ticks() + 100 * gfCombatSpeedMod[gConfig.combatSpeed] * 1.3
        );
    }

    if (anyoneDamaged) {
        AreaSpellDoDamage(spellDamage, SPELL_IMPLOSION_GRENADE, target);
    }
}

void combatManager::Fireball(i32 targetHex, SpellType spell) {
    if (!ValidHex(targetHex))
        return;

    i32l baseDamage;
    i32l damage;
    i32 frameCount7;
    i32 affectedCount4;
    army* target;
    i32 targetX7;
    icon* spellIcon4;
    i32 targetY9;
    i32 frame;
    i16 affectedHexes[SPELL_FIREBALL_AFFECTED_HEX_COUNT];
    i32 anyAffected;

    targetX7 = m_hexCells[targetHex].m_x;
    targetY9 = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
    if (!gbNoShowCombat) {
        frameCount7 = SPELL_FIREBALL_FRAME_COUNT;
        if (spell == SPELL_FIREBALL)
            spellIcon4 = gpResourceManager->GetIcon("fireball.icn");
        else if (spell == SPELL_FIREBLAST)
            spellIcon4 = gpResourceManager->GetIcon("firebal2.icn");
        else {
            spellIcon4 = gpResourceManager->GetIcon("coldring.icn");
            frameCount7 = SPELL_COLD_RING_FRAME_COUNT;
        }

        for (frame = 0; frame < frameCount7; ++frame) {
            glTimers[0] = static_cast<i32>(
                platform::Ticks() + SPELL_AREA_ANIMATION_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            IconToBitmap(
                spellIcon4,
                gpWindowManager->m_screen,
                targetX7,
                targetY9,
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
                    spellIcon4,
                    gpWindowManager->m_screen,
                    targetX7,
                    targetY9,
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
        gpResourceManager->Dispose(spellIcon4);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target =
        m_armies[H2EnumIndex(m_currentSide)] + m_currentArmyIndex;
    for (frame = 0; frame < SPELL_FIREBALL_AFFECTED_HEX_COUNT; ++frame)
        affectedHexes[frame] = COMBAT_HEX_EMPTY;
    if (spell != SPELL_COLD_RING)
        affectedHexes[0] = static_cast<i16>(targetHex);

    for (frame = H2EnumIndex(COMBAT_DIRECTION_NORTHEAST); frame < SPELL_ADJACENT_DIRECTION_COUNT;
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

    baseDamage = m_spellPower[H2EnumIndex(m_currentSide)] * SPELL_FIREBALL_DAMAGE_PER_POWER;
    ClearEffects();
    anyAffected = 0;
    affectedCount4 = SPELL_FIREBALL_AFFECTED_HEX_COUNT;
    for (frame = 0; frame < affectedCount4; ++frame) {
        if (affectedHexes[frame] != COMBAT_HEX_EMPTY
            && m_hexCells[affectedHexes[frame]].m_occupantSide != COMBAT_SIDE_NONE) {
            target = &m_armies[H2EnumIndex(m_hexCells[affectedHexes[frame]].m_occupantSide)]
                                [m_hexCells[affectedHexes[frame]].m_occupantIndex];
            if (target->SpellCastWorks(spell)
                && !*(
                    gArmyEffected[0]
                    + H2EnumIndex(m_hexCells[affectedHexes[frame]].m_occupantSide)
                          * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[affectedHexes[frame]].m_occupantIndex
                )) {
                *(gArmyEffected[0]
                  + H2EnumIndex(m_hexCells[affectedHexes[frame]].m_occupantSide)
                        * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[affectedHexes[frame]].m_occupantIndex) = 1;
                if (target->m_damagePending == 0) {
                    damage = baseDamage;
                    if (spell == SPELL_COLD_RING
                        && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                        damage <<= 1;
                    if ((spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                        && target->m_monsterType == CREATURE_WATER_ELEMENTAL)
                        damage *= ELEMENTAL_WEAKNESS_MULTIPLIER;
                    if (target->m_monsterType == CREATURE_IRON_GOLEM
                        || target->m_monsterType == CREATURE_STEEL_GOLEM) {
                        damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                    }
                    target->Damage(damage, spell);
                    anyAffected = 1;
                }
            }
        }
    }
    if (anyAffected) {
        ModifyDamageForArtifacts(
            &baseDamage,
            spell,
            m_heroes[H2EnumIndex(m_currentSide)],
            m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]
        );
        if (spell == SPELL_COLD_RING)
            sprintf(gText, localization::Tr("spell.damage.cold_ring"), baseDamage);
        else
            sprintf(gText, localization::Tr("spell.damage.fireball"), baseDamage);
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
    i32 anyAffected;

    target = &m_armies[H2EnumIndex(m_currentSide)][0] + m_currentArmyIndex;
    hexes[0] = targetHex;
    for (direction = H2EnumIndex(COMBAT_DIRECTION_NORTHEAST); direction < SPELL_ADJACENT_DIRECTION_COUNT;
         ++direction) {
        hexes[direction + 1] =
            GetAdjacentCellIndexNoArmy(targetHex, static_cast<CombatHexDirection>(direction));
    }

    if (!gbNoShowCombat) {
        icon* meteorIcon = gpResourceManager->GetIcon("meteor.icn");
        for (direction = 0; direction < SPELL_METEOR_PASS_COUNT; ++direction) {
            for (frame = 0; frame < SPELL_METEOR_FRAME_COUNT; ++frame) {
                glTimers[0] = static_cast<i32>(
                    platform::Ticks()
                    + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_METEOR_ANIMATION_DELAY
                );
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                meteorIcon->CombatClipDrawToBuffer(
                    m_hexCells[targetHex].m_x,
                    m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET,
                    frame,
                    NULL,
                    ICON_DRAW_NORMAL,
                    0,
                    NULL,
                    NULL
                );
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(meteorIcon);
    }

    baseDamage = m_spellPower[H2EnumIndex(m_currentSide)] * SPELL_METEOR_DAMAGE_PER_POWER;
    ClearEffects();
    anyAffected = 0;
    for (direction = 0; direction < SPELL_METEOR_AFFECTED_HEX_COUNT; ++direction) {
        if (hexes[direction] != COMBAT_HEX_EMPTY
            && m_hexCells[hexes[direction]].m_occupantSide != COMBAT_SIDE_NONE) {
            target = &m_armies[H2EnumIndex(m_hexCells[hexes[direction]].m_occupantSide)]
                              [m_hexCells[hexes[direction]].m_occupantIndex];
            if (target->SpellCastWorks(SPELL_METEOR_SHOWER)
                && !*(
                    gArmyEffected[0]
                    + H2EnumIndex(m_hexCells[hexes[direction]].m_occupantSide) * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[hexes[direction]].m_occupantIndex
                )) {
                *(gArmyEffected[0]
                  + H2EnumIndex(m_hexCells[hexes[direction]].m_occupantSide) * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[hexes[direction]].m_occupantIndex) = 1;
                if (target->m_damagePending == 0) {
                    damage = baseDamage;
                    if (target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
                        damage <<= 1;
                    target->Damage(damage, SPELL_METEOR_SHOWER);
                    anyAffected = 1;
                }
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, localization::Tr("spell.damage.meteor_shower"), baseDamage);
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    }
}

void combatManager::ElementalStorm(void) {
    i32 baseDam;
    i32 c;
    army* stack;
    i32 member;
    i32 frame_i;
    i32 row_b;
    i32 iter;
    i32 whichSide;
    i32 hit;
    i32l dmg2;
    icon* stormIcon_i;
    SLimitData limits_n;

    if (!gbNoShowCombat) {
        stormIcon_i = gpResourceManager->GetIcon("storm.icn");
        for (iter = 0; iter < SPELL_STORM_PASS_COUNT; ++iter) {
            for (frame_i = 0; frame_i < SPELL_STORM_FRAME_COUNT; ++frame_i) {
                glTimers[0] = static_cast<i32>(
                    platform::Ticks()
                    + SPELL_AREA_ANIMATION_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
                );
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                for (row_b = 0; row_b < SPELL_STORM_ROW_COUNT; ++row_b) {
                    for (c = 0; c < SPELL_STORM_COLUMN_COUNT; ++c) {
                        stormIcon_i->CombatClipDrawToBuffer(
                            c * SPELL_STORM_TILE_SIZE,
                            row_b * SPELL_STORM_TILE_SIZE,
                            (frame_i + c * SPELL_STORM_FRAME_COLUMN_STEP + row_b)
                                % SPELL_STORM_FRAME_COUNT,
                            &limits_n,
                            ICON_DRAW_NORMAL,
                            0,
                            NULL,
                            NULL
                        );
                    }
                }
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(stormIcon_i);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    hit = 0;
    baseDam = m_spellPower[H2EnumIndex(m_currentSide)] * SPELL_ELEMENTAL_STORM_DAMAGE_PER_POWER;
    for (whichSide = 0; whichSide < COMBAT_SIDE_COUNT; ++whichSide) {
        for (member = 0; member < m_armyCount[whichSide]; ++member) {
            stack = m_armies[whichSide] + member;
            if (stack->SpellCastWorks(SPELL_ELEMENTAL_STORM)) {
                dmg2 = baseDam;
                if (stack->m_monsterType == CREATURE_AIR_ELEMENTAL)
                    dmg2 <<= 1;
                if (stack->m_monsterType == CREATURE_IRON_GOLEM
                    || stack->m_monsterType == CREATURE_STEEL_GOLEM) {
                    dmg2 = static_cast<i32l>(dmg2 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[whichSide] && m_heroes[whichSide]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    dmg2 = static_cast<i32l>(dmg2 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                stack->Damage(dmg2, SPELL_ELEMENTAL_STORM);
                hit = 1;
            }
        }
    }
    if (hit) {
        sprintf(
            gText,
            localization::Tr("combat.spell.elemental_storm_damage"),
            baseDam
        );
        CombatMessage(gText, 1, 1, 0);
        stack->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    }
}

void combatManager::Armageddon(void) {
    i32 baseDamage;
    i32 anyAffected;
    army* target1;
    i32 side6;
    i32 armyIndex8;
    i32l damage;
    palette* originalPalette9;
    palette* effectPalette;
    i8* effectDataRestore9;
    i8* originalData3;
    i32 restorePass9;
    i32 component;

    baseDamage = m_spellPower[H2EnumIndex(m_currentSide)] * SPELL_ARMAGEDDON_DAMAGE_PER_POWER;
    anyAffected = 0;
    for (side6 = 0; side6 < COMBAT_SIDE_COUNT; ++side6) {
        for (armyIndex8 = 0; armyIndex8 < m_armyCount[side6]; ++armyIndex8) {
            target1 = &m_armies[side6][armyIndex8];
            if (target1->SpellCastWorks(SPELL_ARMAGEDDON)) {
                damage = baseDamage;
                if (target1->m_monsterType == CREATURE_IRON_GOLEM
                    || target1->m_monsterType == CREATURE_STEEL_GOLEM) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side6] && m_heroes[side6]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    damage = static_cast<i32l>(damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                target1->Damage(damage, SPELL_ARMAGEDDON);
                anyAffected = 1;
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, localization::Tr("spell.damage.armageddon"), baseDamage);
        CombatMessage(gText, 1, 1, 0);
    }

    originalPalette9 = NULL;
    effectPalette = NULL;
    if (!gbNoShowCombat) {
        i8* effectData28;
        i32 pass1;
        i32 color9;

        gpWindowManager->m_updateFlags = 0;
        originalPalette9 = gpResourceManager->GetPalette("kb.pal");
        effectPalette = new palette;
        if (!effectPalette)
            MemError();
        memcpy(effectPalette->Data(), originalPalette9->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);

        glTimers[0] = static_cast<i32>(
            platform::Ticks() + SPELL_AREA_ANIMATION_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        effectData28 = effectPalette->Data();
        for (pass1 = 0; pass1 < SPELL_ARMAGEDDON_BRIGHTEN_PASS_COUNT; ++pass1) {
            for (color9 = 0; color9 < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color9) {
                if (effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_RED)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_RED)];
                if (effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)];
                if (effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData28[color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)];
            }
            DelayTil(&glTimers[0]);
            SetPalette(effectData28, 1);
            glTimers[0] = static_cast<i32>(
                platform::Ticks()
                + SPELL_ARMAGEDDON_PALETTE_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            );
        }

        i32 shakeOffsets15[SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT][H2EnumIndex(COORDINATE_AXIS_COUNT)] = {
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
        float scale1;
        i32 frame8;
        i32 offsetX18;
        i32 offsetY9;
        i32 timer0;
        i32 copyWidth;
        i32 copyHeight1;
        u8* source0;
        u8* destination3;
        i32 row5;
        i32 shakePass5;

        gpMouseManager->HideColorPointer();
        memcpy(
            m_backgroundBuffer->m_pixels,
            gpWindowManager->m_screen->m_pixels,
            COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
        );

        for (shakePass5 = 0; shakePass5 < SPELL_ARMAGEDDON_SHAKE_PASS_COUNT; ++shakePass5) {
            scale1 = static_cast<float>(
                (SPELL_ARMAGEDDON_SHAKE_PASS_COUNT - shakePass5) * SPELL_ARMAGEDDON_SHAKE_SCALE
            );
            for (frame8 = 0; frame8 < SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT; ++frame8) {
                if (shakePass5 == 0) {
                    for (color9 = 0; color9 < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color9) {
                        if (effectData28
                                [color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData28
                                [color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_GREEN)] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                        if (effectData28
                                [color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData28
                                [color9 * H2EnumIndex(PALETTE_CHANNEL_COUNT) + H2EnumIndex(PALETTE_CHANNEL_BLUE)] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                    }
                    SetPalette(effectData28, 1);
                }

                offsetX18 =
                    static_cast<i32>(shakeOffsets15[frame8][H2EnumIndex(COORDINATE_AXIS_X)] * scale1);
                offsetY9 = static_cast<i32>(shakeOffsets15[frame8][H2EnumIndex(COORDINATE_AXIS_Y)] * scale1);
                timer0 = static_cast<i32>(
                    platform::Ticks()
                    + SPELL_ARMAGEDDON_SHAKE_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
                );
                PollSound();
                copyWidth =
                    COMBAT_SCREEN_WIDTH
                    - abs(static_cast<i32>(shakeOffsets15[frame8][H2EnumIndex(COORDINATE_AXIS_X)] * scale1));
                copyHeight1 =
                    COMBAT_AREA_HEIGHT
                    - abs(static_cast<i32>(shakeOffsets15[frame8][H2EnumIndex(COORDINATE_AXIS_Y)] * scale1))
                    - 1;
                source0 = m_backgroundBuffer->m_pixels + (offsetX18 >= 0 ? offsetX18 : 0)
                          + (offsetY9 >= 0 ? offsetY9 : 0) * COMBAT_SCREEN_WIDTH;
                destination3 = gpWindowManager->m_screen->m_pixels
                               + (offsetX18 > 0 ? 0 : -offsetX18)
                               + (offsetY9 > 0 ? 0 : -offsetY9) * COMBAT_SCREEN_WIDTH;
                for (row5 = 0; row5 < copyHeight1; ++row5) {
                    memcpy(destination3, source0, copyWidth);
                    source0 += COMBAT_SCREEN_WIDTH;
                    destination3 += COMBAT_SCREEN_WIDTH;
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
                DelayTil(&timer0);
            }
        }
    }

    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target1->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);

    effectDataRestore9 = effectPalette->Data();
    originalData3 = originalPalette9->Data();
    for (restorePass9 = 0; restorePass9 < SPELL_ARMAGEDDON_RESTORE_PASS_COUNT; ++restorePass9) {
        for (component = 0; component < SPELL_ARMAGEDDON_PALETTE_SIZE; ++component) {
            if (static_cast<u8>(effectDataRestore9[component])
                == static_cast<u8>(originalData3[component]))
                continue;

            if (static_cast<u8>(effectDataRestore9[component])
                > static_cast<u8>(originalData3[component])) {
                if (static_cast<u8>(effectDataRestore9[component])
                        - SPELL_ARMAGEDDON_CHANNEL_STEP
                    > static_cast<u8>(originalData3[component]))
                    effectDataRestore9[component] -= SPELL_ARMAGEDDON_CHANNEL_STEP;
                else
                    effectDataRestore9[component] = originalData3[component];
            } else if (static_cast<u8>(effectDataRestore9[component])
                           + SPELL_ARMAGEDDON_CHANNEL_STEP
                       < static_cast<u8>(originalData3[component])) {
                effectDataRestore9[component] += SPELL_ARMAGEDDON_CHANNEL_STEP;
            } else {
                effectDataRestore9[component] = originalData3[component];
            }
        }
        SetPalette(effectDataRestore9, 1);
        DelayMilli(
            static_cast<i32l>(
                SPELL_ARMAGEDDON_PALETTE_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            )
        );
    }
    SetPalette(originalPalette9->Data(), 1);
    gpWindowManager->m_updateFlags = 1;
    gpResourceManager->Dispose(originalPalette9);
    delete effectPalette;
    gpMouseManager->ShowColorPointer();
}

void combatManager::TurnToStone(army* target) {
    ResetLimitCreature();
    m_limitCreatureCount[H2EnumIndex(target->m_side)][target->m_index]++;
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
    m_limitCreatureCount[H2EnumIndex(target->m_side)][target->m_index]++;
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

    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    DoRipple(m_backgroundBuffer, gpWindowManager->m_screen, COMBAT_AREA_HEIGHT, strength);
    m_backgroundDrawn = 0;
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
    m_backgroundDrawn = 0;
}

void combatManager::ResetBoltAngle(SBolt* bolt) {
    i32 angleX7;
    float averageAngle0;

    i32 width6;
    i32 angleY9;
    i32 distance2;
    i32 widthDelta0;
    i32 deltaX5;
    i32 deltaY3;
    float randomAngle;

    if (bolt->nearTarget != 0)
        return;

    deltaX5 = abs(bolt->endX - bolt->pixelX);
    deltaY3 = abs(bolt->endY - bolt->pixelY);
    distance2 = static_cast<i32>(sqrt(static_cast<double>(deltaX5 * deltaX5 + deltaY3 * deltaY3)));
    if (distance2 > bolt->totalDistance)
        bolt->distanceRatio = 0;
    else
        bolt->distanceRatio = static_cast<float>(bolt->totalDistance - distance2)
                              / (static_cast<float>(bolt->totalDistance));

    if (bolt->startWidth != bolt->endWidth) {
        widthDelta0 = bolt->endWidth - bolt->startWidth;
        if (widthDelta0 > 0)
            ++widthDelta0;
        else
            --widthDelta0;
        width6 = bolt->startWidth + static_cast<i32>(widthDelta0 * bolt->distanceRatio);
        if (width6 < 1)
            width6 = 1;
        bolt->width = width6;
    }
    bolt->widthFirst = -(bolt->width >> 1);
    bolt->widthLast = bolt->widthFirst + bolt->width - 1;

    angleX7 = bolt->endX - bolt->pixelX;
    angleY9 = bolt->endY - bolt->pixelY;
    bolt->baseAngle =
        static_cast<float>(atan2(static_cast<double>(angleX7), static_cast<double>(angleY9)));
    averageAngle0 =
        static_cast<float>((bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR);
    averageAngle0 = averageAngle0
                     * ((BOLT_INITIAL_ANGLE_BIAS - bolt->distanceRatio)
                        / BOLT_INITIAL_ANGLE_DIVISOR);
    bolt->angle = averageAngle0 + bolt->baseAngle;

    if (bolt->minAngle == 0 && bolt->maxAngle == 0)
        return;
    if (distance2 > bolt->angleDistance * BOLT_ANGLE_DISTANCE_FACTOR || bolt->forceAngle != 0) {
        if (bolt->minAngle == bolt->maxAngle)
            randomAngle = static_cast<float>(bolt->minAngle) / H2EnumIndex(BOLT_ANGLE_PERCENT_SCALE);
        else
            randomAngle = static_cast<float>(Random(bolt->minAngle, bolt->maxAngle))
                          / H2EnumIndex(BOLT_ANGLE_PERCENT_SCALE);
        randomAngle = randomAngle
                      * ((BOLT_RANDOM_ANGLE_BIAS - bolt->distanceRatio)
                         / BOLT_RANDOM_ANGLE_DIVISOR);
        bolt->baseAngle = randomAngle + bolt->baseAngle;
    }
}

void combatManager::DrawBolt(SBolt* bolt, i32 stepCount) {

    i32 distance15;

    i32 beamOffset0;
    i32 drawX6;
    i32 drawStep;
    i32 drawY;
    u8 color4;
    i32 widthFirst;
    i32 previousX6;
    i32 previousY;
    i32 widthLast5;
    i32 edgeShade29;

    previousX6 = static_cast<i32>(bolt->currentX);
    previousY = static_cast<i32>(bolt->currentY);
    widthFirst = bolt->widthFirst;
    widthLast5 = bolt->widthLast;
    Random(BOLT_RANDOM_WIDTH_LOW, BOLT_RANDOM_WIDTH_HIGH);
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

        drawX6 = bolt->pixelX;
        drawY = bolt->pixelY;
        if (bolt->pixelX == previousX6) {
            if (bolt->pixelY == previousY)
                continue;
        }
        {
            previousX6 = bolt->pixelX;
            previousY = bolt->pixelY;
            for (beamOffset0 = widthFirst; beamOffset0 <= widthLast5; ++beamOffset0) {
                if (bolt->drawVertically != 0)
                    drawY = bolt->pixelY + beamOffset0;
                else
                    drawX6 = bolt->pixelX + beamOffset0;
                if (drawX6 < 0 || drawX6 >= COMBAT_SCREEN_WIDTH || drawY < 0
                    || drawY >= COMBAT_AREA_HEIGHT)
                    continue;
                {
                    if (beamOffset0 < 0)
                        edgeShade29 = -(widthFirst - beamOffset0);
                    else
                        edgeShade29 = widthLast5 - beamOffset0;
                    switch (bolt->colorMode) {
                        case BOLT_COLOR_RED_TABLE:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] =
                                gColorTableRed[static_cast<i8>(
                                    (gpWindowManager->m_screen->m_pixels
                                     + drawY * COMBAT_SCREEN_WIDTH)[drawX6]
                                )];
                            break;
                        case BOLT_COLOR_RED_BEAM:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] = uRedBeam[edgeShade29];
                            break;
                        case BOLT_COLOR_RAINBOW_FORWARD:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] =
                                uRainbow[beamOffset0 - widthFirst];
                            break;
                        case BOLT_COLOR_RAINBOW_REVERSE:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] =
                                uRainbow[BOLT_RAINBOW_LAST_INDEX - (beamOffset0 - widthFirst)];
                            break;
                        case BOLT_COLOR_LIGHTNING: {
                            if (edgeShade29 == 0)
                                color4 = BOLT_LIGHTNING_SHADE_0;
                            else if (edgeShade29 == 1)
                                color4 = BOLT_LIGHTNING_SHADE_1;
                            else if (edgeShade29 == 2)
                                color4 = BOLT_LIGHTNING_SHADE_2;
                            else if (edgeShade29 == 3)
                                color4 = BOLT_LIGHTNING_SHADE_3;
                            else if (edgeShade29 == 4)
                                color4 = BOLT_LIGHTNING_SHADE_4;
                            else
                                color4 = BOLT_LIGHTNING_SHADE_5;
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] = color4;
                            break;
                        }
                        default:
                            (gpWindowManager->m_screen->m_pixels
                             + drawY * COMBAT_SCREEN_WIDTH)[drawX6] =
                                static_cast<u8>(bolt->colorMode);
                            break;
                    }
                }
            }

            distance15 = abs(bolt->endX - bolt->pixelX) + abs(bolt->endY - bolt->pixelY);
            if (bolt->nearTarget != 0) {
                if (distance15 > bolt->nearestDistance + 1
                    || distance15 <= BOLT_FINISHED_DISTANCE_MAX) {
                    bolt->finished = 1;
                    return;
                } else if (distance15 < bolt->nearestDistance) {
                    bolt->nearestDistance = distance15;
                }
            } else {
                if (distance15 < BOLT_NEAR_TARGET_DISTANCE) {
                    bolt->nearTarget = 1;
                    bolt->nearestDistance = distance15;
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
    bolt->finished = 0;
    bolt->nearTarget = 0;
    bolt->lastBranchX = startX;
    bolt->lastBranchY = startY;
    bolt->distanceRatio = 0;
    bolt->forceAngle = forceAngle;

    if (colorMode == BOLT_COLOR_RAINBOW_FORWARD || colorMode == BOLT_COLOR_RAINBOW_REVERSE) {
        if (startX <= 0 || startX >= COMBAT_SCREEN_WIDTH - 1)
            bolt->drawVertically = 1;
        else
            bolt->drawVertically = 0;
    } else
        bolt->drawVertically = abs(endX - startX) > abs(endY - startY);

    i32 deltaX = abs(endX - startX);
    i32 deltaY = abs(endY - startY);
    bolt->totalDistance =
        static_cast<i32>(sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
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
    i32,
    i32 forceAngle,
    i32 frameDelay,
    i32 brightenPalette
) {
    i32 drawDistance3;
    i32 allFinished9;
    i32 drawPassCount9;
    i32 branchChance5;
    i32 deadline7;
    palette* originalPalette;
    palette* effectPalette5;
    i8* effectData3;
    i32 index8;
    SBolt* bolts10;
    i32 extentPadding9;
    i32 boltCount6;
    i32 drawPass0;
    i32 minY8;
    i32 minX9;
    i32 maxY19;
    i32 maxX6;
    i32 oldBoltCount18;
    i32 remainingDistance36;
    i32 branchSeparation4;
    float branchAngle15;
    float currentAngle16;
    i32 childDistance2;
    i32 childX;
    i32 childY4;
    i32 childWidth28;

    if (managePointer != 0)
        gpMouseManager->HideColorPointer();

    drawDistance3 = angleDistance;
    allFinished9 = 0;
    drawPassCount9 = (angleDistance - 1) / drawDistance3 + 1;
    branchChance5 = branchDistance * BOLT_ANGLE_PERCENT_SCALE / angleDistance;
    deadline7 = platform::Ticks();
    gpWindowManager->m_updateFlags = 0;

    originalPalette = NULL;
    effectPalette5 = NULL;
    if (brightenPalette != 0) {
        originalPalette = gpResourceManager->GetPalette("kb.pal");
        effectPalette5 = new palette;
        if (!effectPalette5)
            MemError();
        memcpy(effectPalette5->Data(), originalPalette->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);
        effectData3 = effectPalette5->Data();
        for (index8 = 0; index8 < SPELL_ARMAGEDDON_PALETTE_SIZE; ++index8) {
            effectData3[index8] += BOLT_PALETTE_BRIGHTEN_STEP;
            if (effectData3[index8] > BOLT_PALETTE_COMPONENT_MAX)
                effectData3[index8] = BOLT_PALETTE_COMPONENT_MAX;
        }
        SetPalette(effectData3, 1);
    }

    if (startX > endX) {
        minAngle = -minAngle;
        maxAngle = -maxAngle;
    }
    if (minAngle > maxAngle) {
        index8 = maxAngle;
        maxAngle = minAngle;
        minAngle = index8;
    }

    bolts10 = new SBolt[BOLT_MAX_COUNT];
    extentPadding9 = (startWidth > endWidth ? startWidth : endWidth) >> 1;
    AddBolt(
        bolts10,
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

    boltCount6 = 1;
    while (allFinished9 == 0) {
        for (drawPass0 = 0; drawPass0 < drawPassCount9; ++drawPass0) {
            allFinished9 = 1;
            minY8 = BOLT_EXTENT_SENTINEL;
            minX9 = minY8;
            maxY19 = -1;
            maxX6 = maxY19;
            for (index8 = 0; index8 < boltCount6; ++index8) {
                if (bolts10[index8].finished == 0) {
                    if (bolts10[index8].pixelX > maxX6)
                        maxX6 = bolts10[index8].pixelX;
                    if (bolts10[index8].pixelX < minX9)
                        minX9 = bolts10[index8].pixelX;
                    if (bolts10[index8].pixelY > maxY19)
                        maxY19 = bolts10[index8].pixelY;
                    if (bolts10[index8].pixelY < minY8)
                        minY8 = bolts10[index8].pixelY;
                    DrawBolt(&bolts10[index8], drawDistance3);
                    if (bolts10[index8].pixelX > maxX6)
                        maxX6 = bolts10[index8].pixelX;
                    if (bolts10[index8].pixelX < minX9)
                        minX9 = bolts10[index8].pixelX;
                    if (bolts10[index8].pixelY > maxY19)
                        maxY19 = bolts10[index8].pixelY;
                    if (bolts10[index8].pixelY < minY8)
                        minY8 = bolts10[index8].pixelY;
                }
            }

            maxX6 += extentPadding9;
            minX9 -= extentPadding9;
            maxY19 += extentPadding9;
            minY8 -= extentPadding9;
            if (minX9 < 0)
                minX9 = 0;
            if (minY8 < 0)
                minY8 = 0;
            if (COMBAT_SCREEN_WIDTH - 1 < maxX6)
                maxX6 = COMBAT_SCREEN_WIDTH - 1;
            if (COMBAT_AREA_HEIGHT - 1 < maxY19)
                maxY19 = COMBAT_AREA_HEIGHT - 1;

            DelayTil(&deadline7);
            deadline7 = static_cast<i32>(
                platform::Ticks() + gfCombatSpeedMod[gConfig.combatSpeed] * frameDelay
            );
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                minX9,
                minY8,
                maxX6 - minX9 + 1,
                maxY19 - minY8 + 1,
                minX9,
                minY8
            );
            PollSound();

            for (index8 = 0; index8 < boltCount6; ++index8) {
                if (bolts10[index8].finished == 0)
                    allFinished9 = 0;
            }
            if (allFinished9 != 0)
                goto boltsDone;

            if (branchDistance != 0) {
                oldBoltCount18 = boltCount6;
                for (index8 = 0; index8 < oldBoltCount18; ++index8) {
                    if (bolts10[index8].finished == 0) {
                        remainingDistance36 =
                            abs(bolts10[index8].endX - bolts10[index8].pixelX)
                            + abs(bolts10[index8].endY - bolts10[index8].pixelY);
                        if (boltCount6 < BOLT_MAX_COUNT
                            && remainingDistance36
                                   > angleDistance * BRANCH_MIN_REMAINING_DISTANCE_MULTIPLIER
                            && Random(0, branchChance5) < BOLT_BRANCH_PERCENT_LIMIT) {
                            if (bolts10[index8].lastBranchX != 0) {
                                branchSeparation4 =
                                    abs(bolts10[index8].lastBranchX - bolts10[index8].pixelX)
                                    + abs(bolts10[index8].lastBranchY - bolts10[index8].pixelY);
                                if (branchSeparation4 < branchDistance * BOLT_BRANCH_COOLDOWN_FACTOR)
                                    continue;
                            }
                            bolts10[index8].lastBranchX = bolts10[index8].pixelX;
                            bolts10[index8].lastBranchY = bolts10[index8].pixelY;
                            branchAngle15 =
                                static_cast<float>(
                                    Random(BOLT_BRANCH_RANDOM_LOW, BOLT_BRANCH_RANDOM_HIGH)
                                )
                                / H2EnumIndex(BOLT_ANGLE_PERCENT_SCALE);
                            if (Random(0, 1) != 0)
                                branchAngle15 = -branchAngle15;
                            currentAngle16 = bolts10[index8].angle;
                            currentAngle16 = currentAngle16 + branchAngle15;
                            childDistance2 = Random(branchLength >> 1, branchLength);
                            if (childDistance2 > (remainingDistance36 >> 1))
                                childDistance2 = remainingDistance36 >> 1;
                            childX = static_cast<i32>(
                                bolts10[index8].pixelX
                                + childDistance2 * sin(static_cast<double>(currentAngle16))
                            );
                            childY4 = static_cast<i32>(
                                bolts10[index8].pixelY
                                + childDistance2 * cos(static_cast<double>(currentAngle16))
                            );
                            if (bolts10[index8].endWidth < bolts10[index8].startWidth)
                                childWidth28 = bolts10[index8].width - 1;
                            else
                                childWidth28 = bolts10[index8].width;
                            AddBolt(
                                &bolts10[boltCount6],
                                bolts10[index8].pixelX,
                                bolts10[index8].pixelY,
                                childX,
                                childY4,
                                branchDistance,
                                childWidth28,
                                1,
                                colorMode,
                                static_cast<i32>(
                                    minAngle * BOLT_CHILD_ANGLE_SCALE + BOLT_CHILD_MIN_ANGLE_OFFSET
                                ),
                                static_cast<i32>(
                                    maxAngle * BOLT_CHILD_ANGLE_SCALE + BOLT_CHILD_ANGLE_OFFSET
                                ),
                                angleDistance,
                                bolts10[index8].forceAngle
                            );
                            ++boltCount6;
                        }
                    }
                }
            }
        }

        for (index8 = 0; index8 < boltCount6; ++index8) {
            if (bolts10[index8].finished == 0)
                ResetBoltAngle(&bolts10[index8]);
        }
    }

boltsDone:
    delete[] bolts10;
    if (managePointer != 0) {
        DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
        gpMouseManager->ShowColorPointer();
    }
    if (brightenPalette != 0) {
        SetPalette(originalPalette->Data(), 1);
        gpResourceManager->Dispose(originalPalette);
        delete effectPalette5;
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
    i32 len;
    army* candidate;
    CombatSide sideIndex;
    i32 closest;
    closest = CHAIN_LIGHTNING_DISTANCE_SENTINEL;
    closestCell = COMBAT_HEX_EMPTY;
    sourceX = source->MidX();
    fromY = source->MidY();
    for (sideIndex = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(sideIndex)]; ++armyIndex) {
            candidate = &m_armies[H2EnumIndex(sideIndex)][armyIndex];
            if (gArmyEffected[H2EnumIndex(sideIndex)][armyIndex] == 0) {
                if ((requireWorks != 0 && candidate->SpellCastWorks(SPELL_CHAIN_LIGHTNING))
                    || (requireWorks == 0
                        && candidate->SpellCastWorkChance(SPELL_CHAIN_LIGHTNING) != 0.0f)) {
                    xDelta = abs(candidate->MidX() - sourceX);
                    y = abs(candidate->MidY() - fromY);
                    len = static_cast<i32>(
                        sqrt(static_cast<double>(xDelta * xDelta + y * y))
                    );
                    if (len < closest) {
                        closest = len;
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
    army* target1;
    i32 deltaY5;

    i32 deltaX3;
    i32 targetY;
    i32 startY1;
    i32 targetX9;
    i32 startX0;
    i32 distance7;
    i32 firstBolt2;
    i32 nextTarget10;
    i32 strike18;
    i32 forceAngle4;

    i32 targetDamage9;
    i32 deadline4;

    i32 branchDistance6;

    firstBolt2 = 1;
    damage = spellPower * CHAIN_LIGHTNING_INITIAL_DAMAGE_PER_POWER;
    deadline4 = platform::Ticks();
    startX0 = castX;
    startY1 = castY;
    ClearEffects();
    gpMouseManager->HideColorPointer();
    for (strike18 = 0; strike18 < CHAIN_LIGHTNING_MAX_TARGETS; ++strike18) {
        target1 = &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)]
                            [m_hexCells[targetHex].m_occupantIndex];
        if (strike18 <= CHAIN_LIGHTNING_MAX_TARGETS - 2
            && m_hexCells[targetHex].m_occupantSide == m_currentSide)
            gpCombatManager->m_heroDeathPending[H2EnumIndex(m_currentSide)] = 1;

        targetDamage9 = damage;
        if (target1->m_monsterType == CREATURE_AIR_ELEMENTAL)
            targetDamage9 *= CHAIN_LIGHTNING_AIR_ELEMENTAL_MULTIPLIER;
        if (target1->m_monsterType == CREATURE_IRON_GOLEM
            || target1->m_monsterType == CREATURE_STEEL_GOLEM)
            targetDamage9 = static_cast<i32>(targetDamage9 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
        target1->Damage(targetDamage9, SPELL_NONE);
        damage >>= 1;
        gArmyEffected[H2EnumIndex(target1->m_side)][target1->m_index] = 1;

        targetX9 = target1->MidX();
        targetY = target1->MidY();
        deltaX3 = abs(targetX9 - startX0);
        deltaY5 = abs(targetY - startY1);
        distance7 =
            static_cast<i32>(sqrt(static_cast<double>(deltaX3 * deltaX3 + deltaY5 * deltaY5)));
        branchDistance6 = distance7 / CHAIN_LIGHTNING_DISTANCE_DIVISOR;
        if (branchDistance6 > CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE)
            branchDistance6 = CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE;
        if (branchDistance6 < CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE)
            branchDistance6 = CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE;
        if (branchDistance6 > CHAIN_LIGHTNING_SHORT_BRANCH_MAX)
            forceAngle4 = CHAIN_LIGHTNING_LONG_FORCE_ANGLE;
        else
            forceAngle4 = CHAIN_LIGHTNING_SHORT_FORCE_ANGLE;
        DoBolt(
            0,
            startX0,
            startY1,
            targetX9,
            targetY,
            0,
            CHAIN_LIGHTNING_BOLT_LENGTH,
            CHAIN_LIGHTNING_START_WIDTH,
            CHAIN_LIGHTNING_END_WIDTH,
            BOLT_COLOR_LIGHTNING,
            firstBolt2 ? CHAIN_LIGHTNING_FIRST_MIN_ANGLE : CHAIN_LIGHTNING_MIN_ANGLE,
            firstBolt2 ? CHAIN_LIGHTNING_FIRST_MAX_ANGLE : CHAIN_LIGHTNING_MAX_ANGLE,
            branchDistance6,
            forceAngle4,
            0,
            0,
            strike18 == 0
        );
        firstBolt2 = 0;
        startX0 = targetX9;
        startY1 = targetY;
        DelayMilli(
            static_cast<i32l>(CHAIN_LIGHTNING_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed])
        );
        nextTarget10 = GetNextChainLightningTarget(target1, 1);
        if (nextTarget10 == COMBAT_HEX_EMPTY)
            break;
        targetHex = nextTarget10;
        DrawFrame(1, 0, 0, 0, 0, 1, 1);
        DelayTil(&deadline4);
        deadline4 = static_cast<i32>(
            platform::Ticks() + CHAIN_LIGHTNING_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
        );
    }
    ShowMassSpell(gArmyEffected, gsSpellInfo[H2EnumIndex(SPELL_CHAIN_LIGHTNING)].combatEffect, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpMouseManager->ShowColorPointer();
}

void combatManager::VaporizeCreature(CombatSide side, i32 armyIndex) {
    i32 firstY_j;
    i32 lastY;
    i32 row9;
    i32 rowCount;
    army* target_d;
    i32 topOffset5;

    i32 bottomOffset;

    i32 phase;

    target_d = &m_armies[H2EnumIndex(side)][armyIndex];
    ResetLimitCreature();
    ++m_limitCreatureCount[H2EnumIndex(side)][armyIndex];
    gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gyModify = static_cast<i8*>(H2_ALLOC(SPELL_MODIFIER_ROW_COUNT));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);

    target_d->m_palette = gyModify;
    target_d->m_showQuantity = 0;

    firstY_j = (giMinExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    lastY = (giMaxExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    rowCount = (lastY - firstY_j) / VAPORIZE_STRIPE_WIDTH + 1;
    for (phase = 0; phase < VAPORIZE_PHASE_COUNT; ++phase) {
        switch (phase) {
            case 0:
                topOffset5 = 0;
                bottomOffset = 1;
                break;
            case 1:
                topOffset5 = 1;
                bottomOffset = 3;
                break;
            default:
                topOffset5 = 3;
                bottomOffset = 2;
                break;
        }
        if (phase == VAPORIZE_PHASE_COUNT - 1)
            rowCount = (rowCount - 1) / VAPORIZE_ROW_PAIR_SIZE + 1;
        for (row9 = 0; row9 < rowCount; ++row9) {
            *(row9 * VAPORIZE_STRIPE_WIDTH + gyModify + topOffset5 + firstY_j) = VAPORIZE_MASKED;
            *(gyModify - row9 * VAPORIZE_STRIPE_WIDTH - bottomOffset + lastY) = VAPORIZE_MASKED;
            gbLimitToExtent = true;
            gpCombatManager->DrawFrame(1, 0, 1, 0, VAPORIZE_FRAME_DELAY, 1, 1);
        }
    }
    DelayMilli(static_cast<i32l>(SPELL_VANISH_END_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]));
    target_d->m_palette = NULL;
    target_d->m_showQuantity = 1;
    H2_FREE(gyModify);
    gyModify = NULL;
    gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::RippleCreature(
    CombatSide side,
    i32 armyIndex,
    CombatRippleMode mode
) {
    float amplitudeStep7;
    float amplitude5;
    i32 frameDelay_e;

    i32 phase;
    float amplitudeBase6;
    i32 row_i;
    i32 start;
    i32 end2;
    i32 extentHeight2;
    i32 waveIndex;
    army* target2;
    float* wave;
    i32 phaseStep7;
    i32 skipDistance5;
    i32 amplitudeIndex5;

    target2 = &m_armies[H2EnumIndex(side)][armyIndex];
    switch (mode) {
        case COMBAT_RIPPLE_WAVE:
            phaseStep7 = RIPPLE_WAVE_PHASE_STEP;
            frameDelay_e = RIPPLE_WAVE_FRAME_DELAY;
            amplitudeBase6 = RIPPLE_MODE_ZERO_AMPLITUDE_BASE;
            amplitudeStep7 = RIPPLE_MODE_ZERO_AMPLITUDE_STEP;
            break;
        case COMBAT_RIPPLE_DEATH_RIPPLE:
            phaseStep7 = RIPPLE_DEFAULT_PHASE_STEP;
            frameDelay_e = RIPPLE_DEFAULT_FRAME_DELAY;
            amplitudeBase6 = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep7 = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
        default:
            phaseStep7 = RIPPLE_DEFAULT_PHASE_STEP;
            frameDelay_e = RIPPLE_DEFAULT_FRAME_DELAY;
            amplitudeBase6 = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep7 = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
    }

    ResetLimitCreature();
    ++m_limitCreatureCount[H2EnumIndex(side)][armyIndex];
    if (mode == COMBAT_RIPPLE_DEATH_WAVE)
        gpCombatManager->DrawFrame(0, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    else
        gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    gyModify = static_cast<i8*>(H2_ALLOC(SPELL_MODIFIER_ROW_COUNT));
    wave = static_cast<float*>(H2_ALLOC(sizeof(float) * SPELL_MODIFIER_ROW_COUNT));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    for (row_i = 0; row_i < SPELL_MODIFIER_ROW_COUNT; ++row_i) {
        wave[row_i] = static_cast<float>(
            (sin(static_cast<double>(
                 static_cast<float>(row_i % RIPPLE_WAVE_PERIOD)
                 / static_cast<float>(RIPPLE_WAVE_DIVISOR)
             ))
             - RIPPLE_WAVE_CENTER)
            * RIPPLE_WAVE_RANGE
        );
    }
    target2->m_palette = gyModify;
    target2->m_showQuantity = 0;
    giMinExtentX -= RIPPLE_MARGIN;
    giMaxExtentX += RIPPLE_MARGIN;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_SCREEN_WIDTH - 1)
        giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
    extentHeight2 = giMaxExtentY - giMinExtentY + 1;

    for (phase = RIPPLE_PHASE_START; phase < RIPPLE_PHASE_END; phase += phaseStep7) {
        skipDistance5 =
            abs(RIPPLE_PHASE_CENTER - phase % RIPPLE_PHASE_PERIOD) - RIPPLE_SKIP_CENTER_OFFSET;
        amplitudeIndex5 = (phase - RIPPLE_PHASE_START) / RIPPLE_AMPLITUDE_INDEX_DIVISOR + 1;
        if (mode == COMBAT_RIPPLE_DEATH_WAVE)
            amplitudeIndex5 = RIPPLE_MODE_TWO_AMPLITUDE_START - amplitudeIndex5;
        else if (mode == COMBAT_RIPPLE_WAVE) {
            if (amplitudeIndex5 == 0)
                amplitudeIndex5 = RIPPLE_MODE_ZERO_CENTER_AMPLITUDE;
            else
                amplitudeIndex5 = RIPPLE_MODE_ZERO_AMPLITUDE_START - amplitudeIndex5;
        }
        if (phase > RIPPLE_PHASE_CENTER && phase < RIPPLE_PHASE_END - RIPPLE_SKIP_CENTER_OFFSET
            && (skipDistance5 == RIPPLE_SKIP_DISTANCE_0 || skipDistance5 == RIPPLE_SKIP_DISTANCE_1
                || skipDistance5 == RIPPLE_SKIP_DISTANCE_2
                || skipDistance5 == RIPPLE_SKIP_DISTANCE_3
                || skipDistance5 == RIPPLE_SKIP_DISTANCE_4))
            continue;
        amplitude5 = skipDistance5 * (amplitudeIndex5 * amplitudeStep7 + amplitudeBase6);
        memset(gyModify + giMinExtentY, 0, extentHeight2);
        for (row_i = giMinExtentY; row_i < giMaxExtentY; ++row_i) {
            if (mode == COMBAT_RIPPLE_DEATH_WAVE)
                waveIndex = -giMaxExtentY + row_i
                            + (phase - RIPPLE_PHASE_START) * RIPPLE_WAVE_PHASE_MULTIPLIER;
            else
                waveIndex = giMinExtentY - row_i
                            + (phase - RIPPLE_PHASE_START) * RIPPLE_WAVE_PHASE_MULTIPLIER;
            waveIndex += RIPPLE_WAVE_INDEX_OFFSET;
            if (waveIndex >= 0 && waveIndex < SPELL_MODIFIER_ROW_COUNT)
                gyModify[row_i] = static_cast<i8>((wave[waveIndex]) * amplitude5);
        }
        if (mode == COMBAT_RIPPLE_DEATH_RIPPLE && phase >= RIPPLE_DEATH_RIPPLE_FADE_START) {
            start = giMinExtentY - 1;
            end2 = giMinExtentY
                   + (RIPPLE_DEATH_RIPPLE_FADE_BASE - (RIPPLE_PHASE_END - phase)) * extentHeight2
                         / RIPPLE_FADE_DIVISOR
                   + 1;
            memset(gyModify + start, VAPORIZE_MASKED, end2 - start + 1);
        }
        if (mode == COMBAT_RIPPLE_DEATH_WAVE && phase < RIPPLE_DEATH_WAVE_FADE_END) {
            start = giMinExtentY - 1;
            end2 = giMaxExtentY - 1
                   - (phase - RIPPLE_DEATH_WAVE_FADE_BASE) * extentHeight2 / RIPPLE_FADE_DIVISOR;
            memset(gyModify + start, VAPORIZE_MASKED, end2 - start + 1);
        }
        gbLimitToExtent = true;
        gpCombatManager->DrawFrame(1, 0, 1, 0, frameDelay_e, 1, 1);
    }
    DelayMilli(static_cast<i32l>(SPELL_VANISH_END_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]));
    target2->m_palette = NULL;
    target2->m_showQuantity = 1;
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

    CombatSide side8;
    army* target0;
    i32 returnFrames4;
    i32 creatureDied0;
    i32 frame9;
    i32 effectFrames4;
    i32 armyIndex;
    u32l effectFile4;

    effectFile4 = MAKEFILEID(gCombatFxNames[H2EnumIndex(effect)]);
    effectFrames4 = giNumPowFrames[H2EnumIndex(effect)] - 1;
    returnFrames4 = 0;
    if (gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFile4);
        gCurLoadedSpellEffect = effect;
    }

    for (side8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side8) < COMBAT_SIDE_COUNT; ++side8) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side8)]; ++armyIndex) {
            target0 = &m_armies[H2EnumIndex(side8)][armyIndex];
            if (affected[H2EnumIndex(side8)][armyIndex] != 0)
                target0->m_drawSpellEffect = 1;
            if (animateCreatures != 0 && affected[H2EnumIndex(side8)][armyIndex] != 0
                && target0->m_animationSequence != ARMY_ANIMATION_WINCE
                && target0->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN) {
                if (target0->m_quantity == 0) {
                    if (target0->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)]
                        > effectFrames4)
                        effectFrames4 =
                            target0->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)];
                    gpSoundManager->MemorySample(target0->m_samples[H2EnumIndex(ARMY_SAMPLE_KILL)]);
                } else {
                    if (target0->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)]
                        > effectFrames4)
                        effectFrames4 =
                            target0->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE)];
                    if (target0->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE_RETURN)]
                        > returnFrames4)
                        returnFrames4 = target0->m_frameInfo
                                            .animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WINCE_RETURN)];
                    gpSoundManager->MemorySample(target0->m_samples[H2EnumIndex(ARMY_SAMPLE_WINCE)]);
                }
            }
        }
    }

    for (frame9 = 0; frame9 < effectFrames4; ++frame9) {
        for (side8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side8) < COMBAT_SIDE_COUNT; ++side8) {
            for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side8)]; ++armyIndex) {
                target0 = &m_armies[H2EnumIndex(side8)][armyIndex];
                target0->m_spellEffectYOffset = 0;
                if (animateCreatures != 0 && affected[H2EnumIndex(side8)][armyIndex] != 0) {
                    if (target0->m_quantity == 0) {
                        if (target0->m_animationSequence != ARMY_ANIMATION_DEATH) {
                            target0->m_animationSequence = ARMY_ANIMATION_DEATH;
                            target0->m_animationFrame = 0;
                        } else if (target0->m_animationFrame + 1
                                   < target0->m_frameInfo
                                         .animationFrameCount[H2EnumIndex(target0->m_animationSequence)]) {
                            ++target0->m_animationFrame;
                        }
                    } else if (target0->m_animationSequence != ARMY_ANIMATION_WINCE) {
                        target0->m_animationSequence = ARMY_ANIMATION_WINCE;
                        target0->m_animationFrame = 0;
                    } else if (target0->m_animationFrame + 1
                               < target0->m_frameInfo
                                     .animationFrameCount[H2EnumIndex(target0->m_animationSequence)]) {
                        ++target0->m_animationFrame;
                    }
                }
                if (frame9 + 1 < giNumPowFrames[H2EnumIndex(effect)])
                    gCurSpellEffectFrame = frame9;
            }
        }
        DrawFrame(1, 0, 0, 0, MASS_SPELL_FRAME_DELAY, 1, 1);
    }

    for (side8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side8) < COMBAT_SIDE_COUNT; ++side8) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side8)]; ++armyIndex)
            m_armies[H2EnumIndex(side8)][armyIndex].m_drawSpellEffect = 0;
    }
    for (frame9 = 0; frame9 < returnFrames4 + 1; ++frame9) {
        for (side8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side8) < COMBAT_SIDE_COUNT; ++side8) {
            for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side8)]; ++armyIndex) {
                target0 = &m_armies[H2EnumIndex(side8)][armyIndex];
                if (animateCreatures != 0 && affected[H2EnumIndex(side8)][armyIndex] != 0
                    && target0->m_animationSequence != ARMY_ANIMATION_STAND) {
                    if (target0->m_animationSequence == ARMY_ANIMATION_WINCE) {
                        target0->m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
                        target0->m_animationFrame = 0;
                    } else if (target0->m_animationFrame + 1
                               < target0->m_frameInfo
                                     .animationFrameCount[H2EnumIndex(target0->m_animationSequence)]) {
                        ++target0->m_animationFrame;
                    } else if (target0->m_animationSequence != ARMY_ANIMATION_DEATH) {
                        target0->m_animationSequence = ARMY_ANIMATION_STAND;
                        target0->m_animationFrame = 0;
                    }
                }
            }
        }
        DrawFrame(1, 0, 0, 0, MASS_SPELL_FRAME_DELAY, 1, 1);
    }

    creatureDied0 = 0;
    memset(m_removedArmies, 0, sizeof(m_removedArmies));
    m_removedArmyPresent = 0;
    for (side8 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side8) < COMBAT_SIDE_COUNT; ++side8) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side8)]; ++armyIndex) {
            target0 = &m_armies[H2EnumIndex(side8)][armyIndex];
            if (affected[H2EnumIndex(side8)][armyIndex] != 0 && target0->m_quantity == 0) {
                target0->ProcessDeath(0);
                creatureDied0 = 1;
            }
        }
    }
    if (creatureDied0)
        DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    if (m_removedArmyPresent != 0)
        MakeCreaturesVanish();
}

void combatManager::CastMassSpell(SpellType spell, i32 spellPower) {
    CombatSide side2;
    army* target;
    i32 animateCreatures;
    i32 damage_c;
    i32 anyAffected_i;
    H2SteppedEnumStorage<ArmySpellInfluence, i32> influence_e;
    CombatEffectType effect;
    i32 armyIndex;
    i8 affected2[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];

    target = NULL;
    effect = gsSpellInfo[H2EnumIndex(spell)].combatEffect;
    animateCreatures = 0;
    gpWindowManager->m_updateFlags = 0;
    ShowSpellMessage(0, spell, NULL);
    memset(affected2, 0, sizeof(affected2));

    switch (spell) {
        case SPELL_MASS_SLOW:
        case SPELL_MASS_CURSE:
            side2 = COMBAT_DEFENDER_SIDE - H2EnumIndex(m_currentSide);
            for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                if (m_armies[H2EnumIndex(side2)][armyIndex].SpellCastWorks(spell))
                    affected2[H2EnumIndex(side2)][armyIndex] = 1;
            }
            break;

        case SPELL_MASS_CURE:
        case SPELL_MASS_HASTE:
        case SPELL_MASS_BLESS:
        case SPELL_MASS_SHIELD:
        case SPELL_MASS_FORCE_SHIELD:
            side2 = m_currentSide;
            for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                if (m_armies[H2EnumIndex(side2)][armyIndex].SpellCastWorks(spell))
                    affected2[H2EnumIndex(side2)][armyIndex] = 1;
            }
            break;

        case SPELL_MASS_DISPEL:
            for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
                for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                    if (m_armies[H2EnumIndex(side2)][armyIndex].SpellCastWorks(spell))
                        affected2[H2EnumIndex(side2)][armyIndex] = 1;
                }
            }
            break;

        case SPELL_HOLY_WORD:
        case SPELL_HOLY_SHOUT: {
            animateCreatures = 1;
            damage_c = (spell == SPELL_HOLY_WORD ? HOLY_WORD_DAMAGE_PER_POWER
                                                 : HOLY_SHOUT_DAMAGE_PER_POWER)
                       * spellPower;
            for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
                for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                    if ((H2EnumIndex((m_armies[H2EnumIndex(side2)][armyIndex].m_monster.attributes) & (MONSTER_ATTRIBUTE_UNDEAD)))
                        && m_armies[H2EnumIndex(side2)][armyIndex].SpellCastWorks(spell))
                        affected2[H2EnumIndex(side2)][armyIndex] = 1;
                }
            }
            if (spell == SPELL_HOLY_WORD)
                Blur(0, HOLY_WORD_COLOR_ADJUSTMENT, HOLY_WORD_COLOR_ADJUSTMENT);
            else
                Blur(0, HOLY_SHOUT_COLOR_ADJUSTMENT, HOLY_SHOUT_COLOR_ADJUSTMENT);
            for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
                for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                    if (affected2[H2EnumIndex(side2)][armyIndex] != 0)
                        m_armies[H2EnumIndex(side2)][armyIndex].Damage(damage_c, SPELL_NONE);
                }
            }
            sprintf(
                gText,
                localization::Tr("spell.damage.all_undead"),
                gSpellNames[H2EnumIndex(spell)],
                damage_c
            );
            CombatMessage(gText, 1, 1, 0);
            break;
        }

        case SPELL_DEATH_RIPPLE:
        case SPELL_DEATH_WAVE: {
            animateCreatures = 1;
            for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
                for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                    if (!(H2EnumIndex((m_armies[H2EnumIndex(side2)][armyIndex].m_monster.attributes) & (MONSTER_ATTRIBUTE_UNDEAD)))
                        && m_armies[H2EnumIndex(side2)][armyIndex].SpellCastWorks(spell))
                        affected2[H2EnumIndex(side2)][armyIndex] = 1;
                }
            }
            Ripple(spell == SPELL_DEATH_RIPPLE ? DEATH_RIPPLE_STRENGTH : DEATH_WAVE_STRENGTH);
            damage_c = (spell == SPELL_DEATH_RIPPLE ? DEATH_RIPPLE_DAMAGE_PER_POWER
                                                    : DEATH_WAVE_DAMAGE_PER_POWER)
                       * spellPower;
            for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
                for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
                    if (affected2[H2EnumIndex(side2)][armyIndex] != 0)
                        m_armies[H2EnumIndex(side2)][armyIndex].Damage(damage_c, SPELL_NONE);
                }
            }
            sprintf(
                gText,
                localization::Tr("spell.damage.all_living"),
                damage_c
            );
            CombatMessage(gText, 1, 1, 0);
            break;
        }
    }

    if (gbNoShowCombat)
        goto applySpellInfluence;

    anyAffected_i = 0;
    for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
            if (affected2[H2EnumIndex(side2)][armyIndex] != 0)
                anyAffected_i = 1;
        }
    }
    if (anyAffected_i)
        ShowMassSpell(affected2, effect, animateCreatures);

applySpellInfluence:
    for (side2 = COMBAT_ATTACKER_SIDE; H2EnumIndex(side2) < COMBAT_SIDE_COUNT; ++side2) {
        for (armyIndex = 0; armyIndex < m_armyCount[H2EnumIndex(side2)]; ++armyIndex) {
            if (affected2[H2EnumIndex(side2)][armyIndex] == 0)
                continue;

            target = &m_armies[H2EnumIndex(side2)][armyIndex];
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
                case SPELL_MASS_FORCE_SHIELD:
                    target->SetSpellInfluence(
                        ARMY_SPELL_INFLUENCE_FORCE_SHIELD,
                        spellPower
                    );
                    break;
                case SPELL_MASS_CURE:
                    target->Cure(spellPower);
                    break;
                case SPELL_MASS_DISPEL: {
                    for (influence_e = ARMY_SPELL_INFLUENCE_HASTE;
                         influence_e < ARMY_SPELL_INFLUENCE_COUNT;
                         ++influence_e)
                        target->CancelIndividualSpell(influence_e);
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
    CombatHexDirection searchDirection9;
    i32 xOffset3;
    i32 duration2;
    i32 sourcePart7;
    army* source2;
    i32 searchHex10;
    army* image0;
    i32 step3;
    i32 yOffset6;
    i32 distance0;
    i32 candidateHex;
    i32 frame;
    CombatHexDirection direction;
    i32 deadline1;

    source2 =
        &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];

    distance0 = 1;
    while (distance0 < MIRROR_SEARCH_DISTANCE_LIMIT) {
        for (sourcePart7 = 0; sourcePart7 < MIRROR_SOURCE_PART_COUNT; ++sourcePart7) {
            if (sourcePart7 == 0) {
                searchHex10 = source2->m_hex;
            } else {
                if ((H2EnumIndex((source2->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                    searchHex10 = source2->m_facing == ARMY_FACING_RIGHT ? source2->m_hex + 1
                                : source2->m_hex - 1;
                } else {
                    continue;
                }
            }

            for (direction = COMBAT_DIRECTION_NORTHEAST;
                 H2EnumIndex(direction) < SPELL_ADJACENT_DIRECTION_COUNT;
                 ++direction) {
                if (source2->m_facing == ARMY_FACING_RIGHT)
                    searchDirection9 = direction;
                else
                    searchDirection9 = COMBAT_DIRECTION_NORTHWEST - H2EnumIndex(direction);

                if (source2->m_facing == ARMY_FACING_RIGHT
                    && searchDirection9 == COMBAT_DIRECTION_EAST && sourcePart7 == 0
                    && distance0 == 1)
                    continue;
                if (source2->m_facing == ARMY_FACING_RIGHT
                    && searchDirection9 == COMBAT_DIRECTION_WEST && sourcePart7 == 0
                    && distance0 == 1)
                    continue;
                if (source2->m_facing == ARMY_FACING_RIGHT
                    && searchDirection9 == COMBAT_DIRECTION_WEST && sourcePart7 == 1
                    && distance0 <= 2)
                    continue;
                if (source2->m_facing == ARMY_FACING_LEFT
                    && searchDirection9 == COMBAT_DIRECTION_WEST && sourcePart7 == 0
                    && distance0 == 1)
                    continue;
                if (source2->m_facing == ARMY_FACING_LEFT
                    && searchDirection9 == COMBAT_DIRECTION_EAST && sourcePart7 == 0
                    && distance0 == 1)
                    continue;
                if (source2->m_facing == ARMY_FACING_LEFT
                    && searchDirection9 == COMBAT_DIRECTION_EAST && sourcePart7 == 1
                    && distance0 <= 2)
                    continue;

                candidateHex = searchHex10;
                for (step3 = 0; step3 < distance0; ++step3) {
                    candidateHex = GetAdjacentCellIndexNoArmy(candidateHex, searchDirection9);
                    if (candidateHex < 0 || candidateHex >= COMBAT_HEX_COUNT
                        || candidateHex % HEX_COLUMN_COUNT == 0
                        || candidateHex % HEX_COLUMN_COUNT == HEX_RIGHT_BORDER)
                        continue;
                    if (source2->CanFit(candidateHex, 0, NULL))
                        goto mirror_found;
                }
            }
        }
        ++distance0;
    }
    sprintf(gText, "%s", localization::Tr("spell.mirror_image.failed"));
    NormalDialog(
        gText,
        NORMAL_DIALOG_INFO,
        NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_VALUE,
        0
    );
    return;

mirror_found:
    AddArmy(
        m_currentSide,
        source2->m_monsterType,
        source2->m_quantity,
        candidateHex,
        MONSTER_FLAGS_MIRROR_IMAGE,
        0
    );
    image0 = &m_armies[H2EnumIndex(m_hexCells[candidateHex].m_occupantSide)]
                      [m_hexCells[candidateHex].m_occupantIndex];
    image0->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_SUMMONED;
    duration2 = m_spellPower[H2EnumIndex(m_currentSide)];
    if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        duration2 += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
        duration2 += SPELL_WIZARD_HAT_POWER_BONUS;
    image0->m_roundCounter = duration2;
    source2->m_mirrorImageIndex = image0->m_index;
    image0->m_mirrorSourceIndex = source2->m_index;

    xOffset3 = m_hexCells[source2->m_hex].m_x - m_hexCells[image0->m_hex].m_x;
    yOffset6 = m_hexCells[source2->m_hex].m_y - m_hexCells[image0->m_hex].m_y;
    ResetLimitCreature();
    m_limitCreatureCount[H2EnumIndex(m_hexCells[candidateHex].m_occupantSide)]
                        [m_hexCells[candidateHex].m_occupantIndex]++;
    m_limitCreatureCount[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)]
                        [m_hexCells[targetHex].m_occupantIndex]++;
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    deadline1 = static_cast<i32>(
        platform::Ticks() + MIRROR_SLIDE_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
    );
    for (frame = 0; frame < MIRROR_SLIDE_FRAME_COUNT; ++frame) {
        image0->m_xOffset =
            xOffset3 * (MIRROR_SLIDE_FRAME_COUNT - frame) / MIRROR_SLIDE_FRAME_COUNT;
        image0->m_yOffset = yOffset6 * (MIRROR_SLIDE_FRAME_COUNT - frame) / MIRROR_SLIDE_FRAME_COUNT;
        gbLimitToExtent = true;
        gpCombatManager->DrawFrame(0, 0, 0, 0, 0, 1, 0);
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        gbLimitToExtent = false;
        DelayTil(&deadline1);
        deadline1 = static_cast<i32>(
            platform::Ticks() + MIRROR_SLIDE_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
        );
    }
    image0->m_xOffset = 0;
    image0->m_yOffset = 0;
    UpdateGrid(0, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

void combatManager::SummonElemental(CreatureType monsterType, i32 spellPower) {
    u8 summonHexes[SUMMON_HEX_STORAGE_COUNT];
    u32 summonHex4;
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
    summonHex4 = static_cast<u32>(COMBAT_HEX_EMPTY);
    if (m_heroes[H2EnumIndex(m_currentSide)] != NULL
        && m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;
    for (offset = 0; offset < SUMMON_HEXES_PER_SIDE; ++offset) {
        if (m_hexCells[*(summonHexes + H2EnumIndex(m_currentSide) * SUMMON_HEXES_PER_SIDE
                         + (randomOffset + offset) % SUMMON_HEXES_PER_SIDE)]
                .m_occupantSide
            == COMBAT_SIDE_NONE)
            summonHex4 =
                *(summonHexes + H2EnumIndex(m_currentSide) * SUMMON_HEXES_PER_SIDE
                  + (randomOffset + offset) % SUMMON_HEXES_PER_SIDE);
    }
    m_summonedCreatureType[H2EnumIndex(m_currentSide)] = monsterType;
    AddArmy(
        m_currentSide,
        monsterType,
        spellPower * ELEMENTAL_SUMMON_QUANTITY_PER_POWER,
        summonHex4,
        MONSTER_FLAGS_NONE,
        1
    );
    elementals = &m_armies[H2EnumIndex(m_hexCells[summonHex4].m_occupantSide)]
                          [m_hexCells[summonHex4].m_occupantIndex];
    elementals->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_SUMMONED;
    spellPower = m_spellPower[H2EnumIndex(m_currentSide)];
    if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        spellPower += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_WIZARD_HAT))
        spellPower += SPELL_WIZARD_HAT_POWER_BONUS;
}

void combatManager::DoLuck(CombatSide side, i32 armyIndex) {
    army* target;
    i32 targetX;
    i32 startY;
    i32 startX_b;
    i32 targetY_l;

    target = &m_armies[H2EnumIndex(side)][armyIndex];
    targetX = target->MidX();
    targetY_l = m_hexCells[target->m_hex].m_y;
    targetY_l -=
        GetIconEntry(
            target->m_creatureIcon,
            target->m_frameInfo.animationFrames[H2EnumIndex(ARMY_ANIMATION_STAND)][LUCK_TARGET_FRAME_INDEX]
        )
            ->h
        + LUCK_ICON_Y_PADDING;
    if (targetY_l < LUCK_MIN_TARGET_Y)
        targetY_l = LUCK_MIN_TARGET_Y;
    startY = LUCK_EDGE_START_Y;
    if (target->m_facing == ARMY_FACING_RIGHT) {
        if (targetX < LUCK_RIGHT_FACING_FLIP_X)
            startX_b = targetX + targetY_l / LUCK_DIAGONAL_DIVISOR + LUCK_EDGE_INSET;
        else
            startX_b = targetX - LUCK_EDGE_INSET - targetY_l / LUCK_DIAGONAL_DIVISOR;
    } else if (targetX > LUCK_LEFT_FACING_FLIP_X) {
        startX_b = targetX - LUCK_EDGE_INSET - targetY_l / LUCK_DIAGONAL_DIVISOR;
    } else {
        startX_b = targetX + targetY_l / LUCK_DIAGONAL_DIVISOR + LUCK_EDGE_INSET;
    }
    if (targetY_l < LUCK_SHORT_BOLT_HEIGHT) {
        startX_b = side == COMBAT_ATTACKER_SIDE ? 0 : COMBAT_SCREEN_WIDTH - 1;
        startY = targetY_l + LUCK_MIN_TARGET_Y;
    }
    if (startX_b < 0)
        startX_b = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < startX_b)
        startX_b = COMBAT_SCREEN_WIDTH - 1;
    DoBolt(
        0,
        startX_b,
        startY,
        targetX,
        targetY_l,
        0,
        0,
        LUCK_BOLT_WIDTH,
        LUCK_BOLT_WIDTH,
        startX_b > targetX ? BOLT_COLOR_RAINBOW_FORWARD : BOLT_COLOR_RAINBOW_REVERSE,
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
    i32 frameSpacing4;
    i32 distance8;
    icon* blastIcon_h;
    army* target2;
    i32 startY_d;
    i32 deltaX_a;
    i32 deadline_k;
    i32 segmentCount9;
    i32 segment_h;
    float currentY9;
    float currentX_i;
    i32 targetY9;
    i32 frame_j;
    i32 targetX_a;
    float stepY_e;
    i32 startX_n;
    float stepX_e;
    i32 deltaY_g;

    if (spell == SPELL_COLD_RAY) {
        blastIcon_h = gpResourceManager->GetIcon("coldray.icn");
        frameSpacing4 = BLAST_COLD_RAY_FRAME_SPACING;
    } else {
        blastIcon_h = gpResourceManager->GetIcon("disrray.icn");
        frameSpacing4 = BLAST_DISRUPTING_RAY_FRAME_SPACING;
    }
    target2 =
        &m_armies[H2EnumIndex(m_hexCells[targetHex].m_occupantSide)][m_hexCells[targetHex].m_occupantIndex];
    targetX_a = target2->MidX();
    targetY9 = target2->MidY();
    startX_n = castX;
    startY_d = castY;
    deltaX_a = targetX_a - startX_n;
    deltaY_g = targetY9 - startY_d;
    distance8 =
        static_cast<i32>(sqrt(static_cast<double>(deltaX_a * deltaX_a + deltaY_g * deltaY_g)));
    segmentCount9 = distance8 / frameSpacing4;
    currentX_i = static_cast<float>(startX_n);
    currentY9 = static_cast<float>(startY_d);
    stepX_e = static_cast<float>(deltaX_a) / segmentCount9;
    stepY_e = static_cast<float>(deltaY_g) / segmentCount9;
    deadline_k = 0;
    for (segment_h = 0; segment_h < segmentCount9; ++segment_h) {
        ResetLimitCreature();
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        currentX_i = currentX_i + stepX_e;
        currentY9 = currentY9 + stepY_e;
        frame_j = (segment_h * BLAST_FRAME_COUNT - 1) / segmentCount9;
        blastIcon_h->CombatClipDrawToBuffer(
            static_cast<i32>(currentX_i),
            static_cast<i32>(currentY9),
            frame_j,
            &limits,
            stepX_e < 0.0f ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
            0,
            NULL,
            NULL
        );
        if (giMinExtentX < 0)
            giMinExtentX = 0;
        if (giMinExtentY < 0)
            giMinExtentY = 0;
        if (COMBAT_SCREEN_WIDTH - 1 < giMaxExtentX)
            giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
        if (COMBAT_AREA_HEIGHT - 1 < giMaxExtentY)
            giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
        DelayTil(&deadline_k);
        deadline_k = static_cast<i32>(
            platform::Ticks() + BLAST_FRAME_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
    }
    gbComputeExtent = false;
    gbSaveBiggestExtent = false;
    DrawFrame(1, 0, 0, 0, 0, 1, 0);
    gpResourceManager->Dispose(blastIcon_h);
}

void combatManager::Resurrect(SpellType spell, i32 targetHex, i32 spellPower) {
    army* target;
    i32 otherHex;

    i32 deadHex_j;

    i32 index_o;
    i32 effectY;
    i32 effectX;
    i32 deadIndex;

    i32 oldQuantity_o;
    i32 keepSearching_d;
    i32 armyIndex;
    icon* resurrectIcon;
    i32 processedOtherHex;

    if (m_heroes[H2EnumIndex(m_currentSide)] != NULL
        && m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_ANKH))
        spellPower <<= 1;
    armyIndex = FindResurrectArmyIndex(m_currentSide, spell, targetHex);
    target = &m_armies[H2EnumIndex(m_currentSide)][armyIndex];
    processedOtherHex = 0;
    oldQuantity_o = target->m_quantity;
    target->m_quantity +=
        spellPower * RESURRECT_HIT_POINTS_PER_POWER / target->m_monster.hitPoints;
    if (target->m_quantity > target->m_initialQuantity)
        target->m_quantity = target->m_initialQuantity;
    if (spell == SPELL_RESURRECT)
        target->m_temporaryResurrectionQuantity += target->m_quantity - oldQuantity_o;

    if (oldQuantity_o <= 0) {
        otherHex = COMBAT_HEX_EMPTY;
        deadIndex = COMBAT_HEX_EMPTY;
        keepSearching_d = 1;
        deadHex_j = targetHex;
        while (keepSearching_d) {
            for (index_o = 0; index_o < m_hexCells[deadHex_j].m_deadOccupantCount; ++index_o) {
                if (m_hexCells[deadHex_j].m_deadOccupantSides[index_o] == m_currentSide
                    && m_hexCells[deadHex_j].m_deadOccupantIndices[index_o] == armyIndex) {
                    deadIndex = index_o;
                    if (!processedOtherHex) {
                        if (m_hexCells[deadHex_j].m_deadOccupantFrames[index_o]
                            == ARMY_FACING_RIGHT)
                            otherHex = deadHex_j - 1;
                        else if (m_hexCells[deadHex_j].m_deadOccupantFrames[index_o]
                                 == ARMY_FACING_LEFT)
                            otherHex = deadHex_j + 1;
                    }
                }
                if (deadIndex != COMBAT_HEX_EMPTY) {
                    m_hexCells[deadHex_j].m_occupantSide =
                        m_hexCells[deadHex_j].m_deadOccupantSides[index_o];
                    m_hexCells[deadHex_j].m_occupantIndex =
                        m_hexCells[deadHex_j].m_deadOccupantIndices[index_o];
                    m_hexCells[deadHex_j].m_occupantFrame =
                        m_hexCells[deadHex_j].m_deadOccupantFrames[index_o];
                    if (index_o + 1 == m_hexCells[deadHex_j].m_deadOccupantCount) {
                        m_hexCells[deadHex_j].m_deadOccupantSides[index_o] = COMBAT_SIDE_NONE;
                        m_hexCells[deadHex_j].m_deadOccupantIndices[index_o] = COMBAT_HEX_EMPTY;
                    } else {
                        m_hexCells[deadHex_j].m_deadOccupantSides[index_o] =
                            m_hexCells[deadHex_j].m_deadOccupantSides[index_o + 1];
                        m_hexCells[deadHex_j].m_deadOccupantIndices[index_o] =
                            m_hexCells[deadHex_j].m_deadOccupantIndices[index_o + 1];
                    }
                }
            }
            --m_hexCells[deadHex_j].m_deadOccupantCount;
            if (processedOtherHex) {
                keepSearching_d = 0;
            } else if (otherHex == COMBAT_HEX_EMPTY) {
                keepSearching_d = 0;
            } else {
                deadHex_j = otherHex;
                processedOtherHex = 1;
                deadIndex = COMBAT_HEX_EMPTY;
            }
        }
    }

    effectX = target->MidX();
    effectY = target->MidY();
    if (target->m_quantity - oldQuantity_o > 1)
        sprintf(
            gText,
            localization::TrPlural(
                "spell.resurrect.result",
                target->m_quantity - oldQuantity_o
            ),
            target->m_quantity - oldQuantity_o,
            gArmyNamesPlural[H2EnumIndex(target->m_monsterType)]
        );
    else
        sprintf(
            gText,
            localization::TrPlural(
                "spell.resurrect.result",
                target->m_quantity - oldQuantity_o
            ),
            target->m_quantity - oldQuantity_o,
            gArmyNames[H2EnumIndex(target->m_monsterType)]
        );
    CombatMessage(gText, 1, 1, 0);

    if (!gbNoShowCombat) {
        resurrectIcon = gpResourceManager->GetIcon("yinyang.icn");
        for (index_o = 0; index_o < RESURRECT_ANIMATION_FRAME_COUNT; ++index_o) {
            glTimers[0] = static_cast<i32>(
                platform::Ticks()
                + H2EnumIndex(SPELL_FIZZLE_FRAME_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            IconToBitmap(
                resurrectIcon,
                gpWindowManager->m_screen,
                effectX,
                effectY,
                index_o,
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
                if (index_o >= RESURRECT_DEATH_REVERSE_FRAME) {
                    target->m_animationSequence = ARMY_ANIMATION_STAND;
                    target->m_animationFrame = 0;
                } else {
                    target->m_animationFrame =
                        RESURRECT_DEATH_REVERSE_FRAME - 1 - index_o
                                < target->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)] - 1
                            ? RESURRECT_DEATH_REVERSE_FRAME - 1 - index_o
                            : target->m_frameInfo.animationFrameCount[H2EnumIndex(ARMY_ANIMATION_DEATH)]
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
        localization::TrPlural("spell.resisted", target->m_quantity),
        target->m_quantity == 1 ? gArmyNames[H2EnumIndex(target->m_monsterType)]
                                : gArmyNamesPlural[H2EnumIndex(target->m_monsterType)]
    );
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    WaitEndSample(&fizzleSample, -1);
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
    i32 frame8;
    i32 deadline7;
    i32 width0;
    i32 height9;
    u8* source1;
    u8* destination2;
    i32 row5;
    CombatCastleWallState newWallStates1[EARTHQUAKE_STRUCTURE_COUNT];
    CombatCastleWallState newTowerStates0[EARTHQUAKE_STRUCTURE_COUNT];
    i32 impactPositions6[EARTHQUAKE_MAX_IMPACTS][H2EnumIndex(COORDINATE_AXIS_COUNT)];
    i32 impactCount;
    i32 index7;
    CombatDrawbridgeState newKeepState18;
    i32 impactDelay9[EARTHQUAKE_MAX_IMPACTS];
    icon* cloudIcon1;
    i32 impact;

    i32 shakeOffsets17[EARTHQUAKE_SHAKE_FRAME_COUNT][H2EnumIndex(COORDINATE_AXIS_COUNT)] = {
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

    gpMouseManager->HideColorPointer();
    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    for (pass = 0; pass < EARTHQUAKE_SHAKE_PASS_COUNT; ++pass) {
        for (frame8 = 0; frame8 < EARTHQUAKE_SHAKE_FRAME_COUNT; ++frame8) {
            deadline7 = static_cast<i32>(
                platform::Ticks() + SPELL_ARMAGEDDON_SHAKE_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            PollSound();
            width0 = COMBAT_SCREEN_WIDTH - abs(shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_X)]);
            height9 = COMBAT_AREA_HEIGHT - abs(shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_Y)]) - 1;
            source1 = m_backgroundBuffer->m_pixels
                         + (shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_X)] < 0
                                ? 0
                                : shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_X)])
                         + (shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_Y)] < 0
                                ? 0
                                : shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_Y)])
                               * COMBAT_SCREEN_WIDTH;
            destination2 = gpWindowManager->m_screen->m_pixels
                          + (shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_X)] > 0
                                 ? 0
                                 : -shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_X)])
                          + (shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_Y)] > 0
                                 ? 0
                                 : -shakeOffsets17[frame8][H2EnumIndex(COORDINATE_AXIS_Y)])
                                * COMBAT_SCREEN_WIDTH;
            for (row5 = 0; row5 < height9; ++row5) {
                memcpy(destination2, source1, width0);
                source1 += COMBAT_SCREEN_WIDTH;
                destination2 += COMBAT_SCREEN_WIDTH;
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
            DelayTil(&deadline7);
        }
    }
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    impactCount = 0;
    for (index7 = 0; index7 < EARTHQUAKE_STRUCTURE_COUNT; ++index7) {
        newWallStates1[index7] = m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)];
        if (m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                != COMBAT_WALL_STATE_DESTROYED
            && m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                   != COMBAT_WALL_STATE_SECTION_DESTROYED
            && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_WALL_HIT_CHANCE) {
            ++newWallStates1[index7];
            if (m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_TOWER_STANDING
                && m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED
                && m_wallStates[index7 + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DAMAGE_LAST
                && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_WALL_SECOND_HIT_CHANCE)
                ++newWallStates1[index7];
            impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_X)] =
                wallPos[index7][H2EnumIndex(COORDINATE_AXIS_X)];
            impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_Y)] =
                wallPos[index7][H2EnumIndex(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
            if (newWallStates1[index7] == COMBAT_WALL_STATE_DESTROYED
                || newWallStates1[index7] == COMBAT_WALL_STATE_SECTION_DESTROYED)
                m_hexCells[iWallToHexCell[index7]].m_blocked = 0;
        }

        newTowerStates0[index7] = m_wallStates[index7];
        if (m_wallStates[index7] != COMBAT_WALL_STATE_DESTROYED
            && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_TOWER_HIT_CHANCE) {
            newTowerStates0[index7] = COMBAT_WALL_STATE_DESTROYED;
            impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_X)] =
                towerPos[index7][H2EnumIndex(COORDINATE_AXIS_X)];
            impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_Y)] =
                towerPos[index7][H2EnumIndex(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
        }
    }

    newKeepState18 = m_drawbridgeState;
    if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN
        && SRandom(0, EARTHQUAKE_CHANCE_ROLL_MAX) < EARTHQUAKE_KEEP_HIT_CHANCE) {
        newKeepState18 = COMBAT_CASTLE_GATE_HIDDEN;
        impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_X)] = towerPos[0][H2EnumIndex(COORDINATE_AXIS_X)];
        impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_Y)] =
            towerPos[0][H2EnumIndex(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        ++impactCount;
        impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_X)] = towerPos[1][H2EnumIndex(COORDINATE_AXIS_X)];
        impactPositions6[impactCount][H2EnumIndex(COORDINATE_AXIS_Y)] =
            towerPos[1][H2EnumIndex(COORDINATE_AXIS_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        ++impactCount;
    }

    for (index7 = 0; index7 < EARTHQUAKE_MAX_IMPACTS; ++index7)
        impactDelay9[index7] = Random(0, EARTHQUAKE_MAX_IMPACT_DELAY);
    giMinExtentX = EARTHQUAKE_EXTENT_MIN_X;
    giMinExtentY = 0;
    giMaxExtentX = EARTHQUAKE_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
    if (impactCount != 0) {
        cloudIcon1 = gpResourceManager->GetIcon("lichclod.icn");
        for (index7 = 0; index7 < EARTHQUAKE_CLOUD_FRAME_COUNT; ++index7) {
            glTimers[1] = static_cast<i32>(
                platform::Ticks()
                + H2EnumIndex(SPELL_FIZZLE_FRAME_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
            );
            DrawFrame(0, 0, 1, 0, 0, 1, 0);
            for (impact = 0; impact < impactCount; ++impact) {
                if (index7 >= impactDelay9[impact]
                    && index7 - impactDelay9[impact] < EARTHQUAKE_CLOUD_VISIBLE_FRAME_COUNT) {
                    IconToBitmap(
                        cloudIcon1,
                        gpWindowManager->m_screen,
                        impactPositions6[impact][H2EnumIndex(COORDINATE_AXIS_X)],
                        impactPositions6[impact][H2EnumIndex(COORDINATE_AXIS_Y)],
                        index7 - impactDelay9[impact],
                        ICON_DRAW_CLIP,
                        0,
                        0,
                        COMBAT_SCREEN_WIDTH,
                        COMBAT_AREA_HEIGHT,
                        0
                    );
                }
            }
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1
            );
            DelayTil(&glTimers[1]);
            if (index7 == EARTHQUAKE_APPLY_DAMAGE_FRAME) {
                for (impact = 0; impact < EARTHQUAKE_STRUCTURE_COUNT; ++impact) {
                    m_wallStates[impact + H2EnumIndex(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                        newWallStates1[impact];
                    m_wallStates[impact] = newTowerStates0[impact];
                }
                m_drawbridgeState = newKeepState18;
            }
        }
        gpResourceManager->Dispose(cloudIcon1);
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
    if (target != NULL)
        utf8::Copy(targetName, sizeof(targetName), gArmyNamesPlural[H2EnumIndex(target->m_monsterType)]);
    if (castByCreature != 0) {
        if (spell == SPELL_PARALYZE)
            sprintf(message, localization::Tr("combat.ability.cyclops_paralyze"), targetName);
        else if (spell == SPELL_BLIND)
            sprintf(message, localization::Tr("combat.ability.unicorn_blind"), targetName);
        else if (spell == CREATURE_SPELL_PETRIFY)
            sprintf(message, localization::Tr("combat.ability.medusa_petrify"), targetName);
        else if (spell == SPELL_CURSE)
            sprintf(message, localization::Tr("combat.ability.mummy_curse"), targetName);
        else if (spell == CREATURE_SPELL_DISPEL)
            sprintf(message, localization::Tr("combat.ability.archmage_dispel"), targetName);
        else if (spell == SPELL_SHADOW_MARK)
            sprintf(message, localization::Tr("combat.ability.shadow_mark"), targetName);
    } else {
        if (target != NULL) {
            if (m_heroes[H2EnumIndex(m_currentSide)]->m_isCaptain != 0)
                sprintf(
                    message,
                    localization::Tr("combat.spell.captain_cast_on"),
                    gSpellNames[H2EnumIndex(spell)],
                    targetName
                );
            else
                sprintf(
                    message,
                    localization::Tr("combat.spell.hero_cast_on"),
                    m_heroes[H2EnumIndex(m_currentSide)]->m_name,
                    gSpellNames[H2EnumIndex(spell)],
                    targetName
                );
        } else {
            if (m_heroes[H2EnumIndex(m_currentSide)]->m_isCaptain != 0)
                sprintf(
                    message,
                    localization::Tr("combat.spell.captain_cast"),
                    gSpellNames[H2EnumIndex(spell)]
                );
            else
                sprintf(
                    message,
                    localization::Tr("combat.spell.hero_cast"),
                    m_heroes[H2EnumIndex(m_currentSide)]->m_name,
                    gSpellNames[H2EnumIndex(spell)]
                );
        }
    }
    CombatMessage(message, 1, 1, 0);
}

i32 castX = 0;
i32 castY = 0;
i32 bInTeleportGetDest = 0;
i32 indexToCastOn = -1;
u8 uRedBeam[] = {0x77, 0xb5, 0xbc, 0xc0, 0xc3};
u8 uRainbow[] =
    {0xbc, 0xba, 0xce, 0xca, 0xc8, 0x6f, 0x71, 0x73, 0xde, 0x5a, 0x9f, 0x47, 0x4a, 0x8d, 0x8f};
i8* gyModify = NULL;
