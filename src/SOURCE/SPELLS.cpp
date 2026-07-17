// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SPELLS.OBJ   from: (directly linked into exe)
// functions: 37   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
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

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\SPELLS.CPP"
VA(0x004204c0, 0x86)
i32 combatManager::HasValidSpellTarget(SpellType spell) {
    i32 hex;

    for (hex = 0; hex < COMBAT_HEX_COUNT; ++hex) {
        if (hex % SPELL_HEX_COLUMN_COUNT == 0
            || hex % SPELL_HEX_COLUMN_COUNT == SPELL_HEX_RIGHT_BORDER)
            continue;
        if (ValidSpellTarget(spell, hex))
            return 1;
    }
    return 0;
}

// @semantic: recovered retail body order and CFG, including the
// in-switch common action block, all four elemental continuation jumps, ordinary
// switch-break cleanup, and the no-selection scope. The 0x0c frame has
// elementalType at -0x4, this at -0x8, and the switch temporary at -0x0c. Explicit
// objdiff ranges align every switch instruction and the embedded tables relative
// to the function; the first 41 ordered external relocations agree, and the
// remaining 18 table entries use equivalent delinked local-label targets (62
// sites total, with three earlier function-local sites). Outside
// the +0x383..+0x3c3 switch data, raw bytes differ only at +0x45 and +0x41a:
// the entry no-selection branch and its paired terminal target. Retail jumps
// directly to the false result, while the
// equal-size canonical form jumps to the final comparison. Explicit early label,
// nested return, explicit terminal false label, selected-arm constant returns,
// reversed comparison operands, and OR-zero comparison spellings added 1-18 bytes,
// kept the same two-byte residual, or reversed the branch. Revisit only after a
// material TU-state change.
VA(0x00420546, 0x44a)
SpellType combatManager::ViewSpells(i32) {
    CreatureType elementalType;

    m_selectedSpell = gpGame->ViewSpells(m_heroes[giCurGeneral], 0, CombatSpecialHandler, 0);
    if (m_selectedSpell != SPELL_NONE) {
        switch (m_selectedSpell) {
            case SPELL_EARTHQUAKE:
                if (m_combatTowns[COMBAT_DEFENDER_SIDE] == 0) {
                    NormalDialog(
                        "An earthquake will do you no good unless there are town walls to damage.",
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
                if (m_summonedCreatureType[m_currentSide] != 0
                    && m_summonedCreatureType[m_currentSide] != elementalType) {
                    NormalDialog(
                        "You may only summon one type of elemental per combat.",
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
                if (m_armyCount[m_currentSide] >= SPELL_ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        "You already have %d creatures groups in combat and cannot add any more.",
                        m_armyCount[m_currentSide]
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
                        "There is no open space adjacent to your hero to summon an Elemental to."
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
                if (!HasValidSpellTarget(IDX(m_selectedSpell))) {
                    NormalDialog(
                        "That spell will affect no one!",
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
                giNextAction = SPELL_ACTION_CAST;
                giNextActionExtra = m_selectedSpell;
                break;

            case SPELL_MIRROR_IMAGE:
                if (m_armyCount[m_currentSide] >= SPELL_ELEMENTAL_ARMY_LIMIT) {
                    sprintf(
                        gText,
                        "You already have %d creatures groups in combat and cannot add any more.",
                        m_armyCount[m_currentSide]
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
                // fall through
            default:
                if (!HasValidSpellTarget(IDX(m_selectedSpell))) {
                    NormalDialog(
                        "That spell will affect no one!",
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
                giNextAction = SPELL_ACTION_CAST;
                giNextActionExtra = m_selectedSpell;
                gpMouseManager->SetPointer(
                    "spelmous.mse",
                    gsSpellInfo[IDX(m_selectedSpell)].iconIndex,
                    SPELL_POINTER_DEFAULT_ID
                );
                gpWindowManager->DoDialog(0, HandleCastSpell, 0);
                break;
        }

    restore_pointer:
        gpMouseManager->SetPointer("cmbtmous.mse", 0, SPELL_POINTER_DEFAULT_ID);
    }
    return m_selectedSpell != SPELL_NONE;
}

// @semantic: semantics and CFG agree, including hover conversion/cache,
// common return, and retail case order. There are no source locals: the message
// reference arrives in ECX and is stored at -0x4; the only other 0x08-frame slot
// is the implicit switch temporary at -0x8. All external calls/globals are
// recovered. First residual is after the switch dispatch at +0x31, where delinked
// help-pointer/table identities truncate the normal diff. Tried early-return and
// retail positive-arm forms. Revisit only after total SOURCE fuzzy reaches 95%,
// or earlier if later same-TU structural work changes this function.
VA(0x00420990, 0x15c)
i32 CombatSpecialHandler(tag_message& message) {
    if (message.type == SPELL_MESSAGE_HOVER) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
            return SPELL_HANDLER_CONTINUE;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;

        switch (message.payload.hover.id) {
            case SPELL_CONTROL_PREVIOUS_PAGE:
                gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_PREVIOUS_PAGE], 1, 0, 0);
                break;
            case SPELL_CONTROL_NEXT_PAGE:
                gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_NEXT_PAGE], 1, 0, 0);
                break;
            case SPELL_CONTROL_CLOSE:
                gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_CLOSE], 1, 0, 0);
                break;
            case SPELL_CONTROL_FIRST_MANA:
            case SPELL_CONTROL_FIRST_MANA + 1:
            case SPELL_CONTROL_FIRST_MANA + 2:
            case SPELL_CONTROL_LAST_MANA:
                gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_MANA], 1, 0, 0);
                break;
            default:
                gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_DEFAULT], 1, 0, 0);
                break;
        }
    }
    return SPELL_HANDLER_CONTINUE;
}

// @semantic: complete two-stage teleport, recursive hover,
// select, and mouse-down/cancel CFG. Reordering the teleport-destination arm and
// replacing the cancel goto with case fallthrough recovered every middle/tail
// structural difference. The message reference is at -0x8, hex at -0x4, and the
// switch temporary at -0x0c. Explicit objdiff ranges align relative instruction
// positions through the embedded table. The first residual at +0x35 is load order:
// retail loads hex then compares indexToCastOn, while MSVC canonicalizes both
// operand spellings to the reverse order. The target delinker also represents the
// recursive self-call as an intra-function label; external relocation identities
// otherwise agree. Revisit only after a TU-state change.
VA(0x00420aec, 0x2aa)
i32 HandleCastSpell(tag_message& message) {
    i32 hex;

    switch (message.type) {
        case SPELL_MESSAGE_HOVER:
            hex = gpCombatManager->GetGridIndex(message.payload.mouse.x, message.payload.mouse.y);
            if (indexToCastOn != hex) {
                if (!gpCombatManager->ValidSpellTarget(gpCombatManager->m_selectedSpell, hex)) {
                    indexToCastOn = SPELL_NO_SELECTION;
                    gpMouseManager->SetPointer(0);
                    if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT && bInTeleportGetDest) {
                        gpCombatManager->CombatMessage("Invalid Teleport Destination", 1, 0, 0);
                    } else {
                        gpCombatManager->CombatMessage("Select Spell Target", 1, 0, 0);
                    }
                } else {
                    indexToCastOn = hex;
                    gpMouseManager->SetPointer(
                        gsSpellInfo[IDX(gpCombatManager->m_selectedSpell)].iconIndex
                    );
                    gpCombatManager->SpellMessage(IDX(gpCombatManager->m_selectedSpell), hex);
                }
            }
            break;

        case SPELL_MESSAGE_SELECT:
            if (indexToCastOn != SPELL_NO_SELECTION) {
                if (bInTeleportGetDest) {
                    giNextActionGridIndex2 = indexToCastOn;
                } else {
                    giNextActionGridIndex = indexToCastOn;
                    if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT) {
                        bInTeleportGetDest = 1;
                        indexToCastOn = SPELL_NO_SELECTION;
                        message.type = SPELL_MESSAGE_HOVER;
                        message.payload.mouse.x = message.payload.mouse.screenX;
                        message.payload.mouse.y = message.payload.mouse.screenY;
                        HandleCastSpell(message);
                        gpCombatManager->CombatMessage("Select teleport destination.", 1, 0, 0);
                        return SPELL_HANDLER_CONTINUE;
                    }
                }
                bInTeleportGetDest = 0;
                message.type = SPELL_MESSAGE_DIALOG;
                message.payload.widget.command = SPELL_COMMAND_CLOSE;
                return SPELL_HANDLER_CLOSE;
            }
            break;

        case SPELL_MESSAGE_MOUSE_DOWN:
            if (message.payload.keyboard.keyCode != SPELL_COMMAND_CANCEL)
                break;
            // fall through

        case SPELL_MESSAGE_CANCEL:
            gpCombatManager->m_selectedSpell = SPELL_NONE;
            giNextAction = 0;
            message.type = SPELL_MESSAGE_DIALOG;
            message.payload.widget.command = SPELL_COMMAND_CLOSE;
            bInTeleportGetDest = 0;
            return SPELL_HANDLER_CLOSE;
    }
    return SPELL_HANDLER_CONTINUE;
}

// @semantic: semantics and CFG agree, including the live-first arm and
// descending corpse scan. The 0x0c frame has target_j at -0x4, corpse at -0x8,
// and this at -0x0c; arguments are side +0x8, spell +0x0c, and hex +0x10, with no
// other locals. Both SpellCastWorkChance relocations match. First code residual is
// the corpse/hex index formation near +0x124: retail forms corpse + 0x61*hex,
// ours forms 0x62*hex + corpse. Tried empty-first do/while, live-first for, direct
// army expressions, an army pointer local, and slot suffixes. Revisit only after
// total SOURCE fuzzy reaches 95%, or earlier if later same-TU structural work
// changes this function.
VA(0x00420d96, 0x2e5)
i32 combatManager::FindResurrectArmyIndex(i32 side, i32 spell, i32 hex) {
    army* target_j;
    i32 corpse;

    if (m_hexCells[hex].m_occupantSide != COMBAT_HEX_EMPTY) {
        if (m_hexCells[hex].m_occupantSide == side) {
            target_j = &m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex];
            if (target_j->SpellCastWorkChance(SpellType(spell)) > 0.0f)
                return m_hexCells[hex].m_occupantIndex;
        }
        return SPELL_NO_SELECTION;
    }

    for (corpse = m_hexCells[hex].m_deadOccupantCount - 1; corpse >= 0; --corpse) {
        if (m_hexCells[hex].m_deadOccupantSides[corpse] != COMBAT_HEX_EMPTY
            && (m_hexCells[hex].m_deadOccupantFrames[corpse] != ARMY_FACING_LEFT
                || m_hexCells[hex + 1].m_occupantSide == COMBAT_HEX_EMPTY)
            && (m_hexCells[hex].m_deadOccupantFrames[corpse] != ARMY_FACING_RIGHT
                || m_hexCells[hex - 1].m_occupantSide == COMBAT_HEX_EMPTY)
            && m_hexCells[hex].m_deadOccupantSides[corpse] == side) {
            target_j = &m_armies[m_hexCells[hex].m_deadOccupantSides[corpse]]
                                [m_hexCells[hex].m_deadOccupantIndices[corpse]];
            if (target_j->SpellCastWorkChance(SpellType(spell)) > 0.0f)
                return m_hexCells[hex].m_deadOccupantIndices[corpse];
        }
    }
    return SPELL_NO_SELECTION;
}

// @semantic: all target filters and retail case-body order agree, as do all 37
// ordered relocation targets. The unused bucket-0 word, target_j, and the real
// teleportBlocked temporary restore retail's 0x14 frame at -0x4/-0x8/-0x0c,
// followed by this and the switch temporary. The first executable residual is
// equivalent occupant side/index multiplication order; direct/pointer forms,
// cached indices, and both case-body orders were already exhausted.
VA(0x0042107b, 0x521)
i32 combatManager::ValidSpellTarget(SpellType spell, i32 hex) {
    army* target_j = 0;
    i32 unusedSpellWord5;
    i32 teleportBlocked;
    if (!ValidHex(hex))
        return 0;

    if (spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST && spell != SPELL_METEOR_SHOWER
        && spell != SPELL_COLD_RING && spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT
        && spell != SPELL_ANIMATE_DEAD && m_hexCells[hex].m_occupantSide != COMBAT_HEX_EMPTY) {
        target_j = &m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex];
        if (target_j->m_spellInfluence[IDX(SPELL_INFLUENCE_ANTI_MAGIC)] != 0
            || target_j->m_monsterType == CREATURE_GREEN_DRAGON)
            return 0;
    }

    if (target_j != 0 && target_j->SpellCastWorkChance(spell) <= 0.0f)
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
            if (m_hexCells[hex].m_occupantSide == COMBAT_HEX_EMPTY)
                return 0;
            break;

        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            return FindResurrectArmyIndex(m_currentSide, IDX(spell), hex) != SPELL_NO_SELECTION;

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
            if (m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex]
                        .m_mirrorImageIndex
                    != SPELL_NO_SELECTION
                || m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex]
                           .m_mirrorSourceIndex
                       != SPELL_NO_SELECTION)
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
            if (m_hexCells[hex].m_occupantSide != 1 - m_currentSide)
                return 0;
            break;

        case SPELL_TELEPORT:
            if (!bInTeleportGetDest) {
                if (m_hexCells[hex].m_occupantSide != m_currentSide)
                    return 0;
            } else {
                teleportBlocked =
                    hex == giNextActionGridIndex
                    || !m_armies[gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide]
                                [gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantIndex]
                                    .CanFit(hex, 0, 0);
                if (teleportBlocked)
                    return 0;
            }
            break;

        case SPELL_FIREBALL:
        case SPELL_FIREBLAST:
        case SPELL_METEOR_SHOWER:
        case SPELL_COLD_RING:
            if (hex == COMBAT_HEX_EMPTY || hex % SPELL_HEX_COLUMN_COUNT == 0
                || hex % SPELL_HEX_COLUMN_COUNT == SPELL_HEX_RIGHT_BORDER)
                return 0;
            break;
    }
    return 1;
}

// @semantic: the 0x10 frame, switch CFG, 27 ordered relocations, and all code
// outside +0xed..+0x128 agree after current compiler-local normalization. The
// residual is the occupied-target army address: retail evaluates occupantIndex
// before occupantSide, while VC4.2 evaluates the equivalent two-dimensional
// subscript in the reverse order. Flattened index-first addition, OR-zero, and
// reversed pointer addition compile identically. Revisit after TU-state changes.
VA(0x0042159c, 0x222)
void combatManager::SpellMessage(i32 spell, i32 hex) {
    army* target_i;
    char* armyName;

    if (gbNoShowCombat)
        return;

    switch (spell) {
        case IDX(SPELL_FIREBALL):
        case IDX(SPELL_FIREBLAST):
        case IDX(SPELL_METEOR_SHOWER):
        case IDX(SPELL_COLD_RING):
            sprintf(gText, "Cast %s", gSpellNames[spell]);
            break;
        case IDX(SPELL_TELEPORT):
            if (bInTeleportGetDest) {
                sprintf(gText, "Teleport Here");
                break;
            }
            goto occupied_target;
        case IDX(SPELL_RESURRECT):
        case IDX(SPELL_TRUE_RESURRECT):
        case IDX(SPELL_ANIMATE_DEAD):
            target_i = &m_armies[m_currentSide][FindResurrectArmyIndex(m_currentSide, spell, hex)];
            goto format_target;
        default:
        occupied_target:
            target_i = &m_armies[m_hexCells[hex].m_occupantSide][m_hexCells[hex].m_occupantIndex];
        format_target:
            if (target_i->m_quantity == 1)
                armyName = gArmyNames[IDX(target_i->m_monsterType)];
            else
                armyName = gArmyNamesPlural[IDX(target_i->m_monsterType)];
            sprintf(gText, "Cast %s on %s", gSpellNames[spell], armyName);
            break;
    }

    CombatMessage(gText, 1, 0, 0);
}

// @semantic: The complete dispatch, creature-cast handling, spell-specific
// bodies, sound/hero animation, cleanup, and final selector CFG agree with retail.
// All 270 external relocation targets agree. Retail reserves a 0xb4 frame and
// stores this at -0xa4; ours reserves 0xa8 and stores this at -0x9c. Retail has
// unreferenced interior words at -0x10 and -0x3c and places the three
// case-specific army-name pointers at -0x98..-0xa0; ours places those pointers
// at -0x7c..-0x84, ahead of the switch temporaries. The recovered sample buffer,
// SAMPLE2, and float[9] ranges otherwise account for every named aggregate. The
// first normalized residual is the typed
// gsSpellInfo field relocation versus retail's interior label, followed by one
// inline continuation jump and side/index multiplication order. Local scopes,
// direct member expressions, the aggregate shapes, and flattening the final
// Eagle Eye predicate were audited; flattening was byte-neutral, and fake
// padding would not recover the interior layout. Revisit for switch/case scope
// and compiler-slot shaping.
VA(0x004217be, 0x1eca)
void combatManager::CastSpell(
    SpellType spell,
    i32 targetHex,
    i32 castByCreature,
    i32 teleportDestination
) {
    i32l damage_m;
    i32 spellPower_i;
    army* target_i;
    i32 targetIndex_k;
    SpellType soundSpell_q;
    i32 armyIndex;
    char sampleName_i[16];
    SAMPLE2 spellSample;
    i32 targetY_b;
    i32 targetX_b;
    i32 side_i;
    i32 targetSide_i;
    i32 influence;
    i32 adjacentHex_q;
    i32 oldDefense;
    army* teleportArmy_i;
    icon* missileIcon_p;
    char* lightningArmyName_d;
    char* magicArrowArmyName_n;
    char* coldRayArmyName_f;
    float missileAngles[9];

    if (castByCreature == 0 && m_eagleEyeSpell[1 - m_currentSide] == -1
        && m_heroes[1 - m_currentSide] != 0 && !m_heroes[1 - m_currentSide]->HasSpell(spell)
        && m_heroes[1 - m_currentSide]->m_secondarySkills[IDX(HERO_SKILL_EAGLE_EYE)]
               != IDX(HERO_SKILL_LEVEL_NONE)
        && m_heroes[1 - m_currentSide]->m_secondarySkills[IDX(HERO_SKILL_EAGLE_EYE)] + 1
               >= gsSpellInfo[IDX(spell)].level) {
        if (SRandom(0, 9)
            <= m_heroes[1 - m_currentSide]->m_secondarySkills[IDX(HERO_SKILL_EAGLE_EYE)]) {
            m_eagleEyeSpell[1 - m_currentSide] = static_cast<i16>(spell);
        }
    }

    spellSample = GetNullSample();
    if (m_limitCreature != 0) {
        ResetLimitCreature();
        if (ValidHex(m_limitCreatureHex) && m_hexCells[m_limitCreatureHex].m_occupantSide >= 0) {
            m_limitCreatureCount
                [m_hexCells[m_limitCreatureHex].m_occupantSide]
                [m_hexCells[m_limitCreatureHex].m_occupantIndex] =
                    m_limitCreatureCount[m_hexCells[m_limitCreatureHex].m_occupantSide]
                                        [m_hexCells[m_limitCreatureHex].m_occupantIndex]
                    + 1;
        }
        m_limitCreature = 0;
        m_limitCreatureHex = COMBAT_HEX_EMPTY;
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }

    if (castByCreature == 0 && m_heroes[m_currentSide] != 0)
        m_heroes[m_currentSide]->UseSpell(spell);

    target_i = 0;
    if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
        || spell == SPELL_METEOR_SHOWER || spell == SPELL_SUMMON_EARTH_ELEMENTAL
        || spell == SPELL_SUMMON_AIR_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL
        || spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_MASS_BLESS
        || spell == SPELL_MASS_HASTE || spell == SPELL_EARTHQUAKE || spell == SPELL_MASS_CURSE
        || spell == SPELL_MASS_CURE || spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT
        || spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE || spell == SPELL_MASS_SHIELD
        || spell == SPELL_ARMAGEDDON || spell == SPELL_ELEMENTAL_STORM
        || spell == SPELL_MASS_DISPEL) {
        target_i = 0;
    } else if (ValidHex(targetHex) && m_hexCells[targetHex].m_occupantSide >= 0) {
        target_i =
            &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];
        targetSide_i = m_hexCells[targetHex].m_occupantSide;
        targetIndex_k = m_hexCells[targetHex].m_occupantIndex;
    } else {
        target_i = 0;
    }

    if (castByCreature == 0)
        m_heroCastSpell[m_currentSide] = 1;

    if (castByCreature != 0) {
        spellPower_i = SPELL_DEFAULT_CREATURE_POWER;
    } else {
        spellPower_i = m_spellPower[m_currentSide];
        if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS)
            && (gsSpellInfo[IDX(spell)].attributes & SPELL_ATTRIBUTE_DURATION)) {
            spellPower_i += SPELL_HOURGLASS_POWER_BONUS;
        }
        if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_WIZARD_HAT)
            && (gsSpellInfo[IDX(spell)].attributes & SPELL_ATTRIBUTE_DURATION)) {
            spellPower_i += SPELL_WIZARD_HAT_POWER_BONUS;
        }
    }

    if (castByCreature == 0) {
        targetX_b = -1;
        targetY_b = -1;
        if (target_i != 0) {
            targetX_b = target_i->MidX();
            targetY_b = target_i->MidY();
        } else {
            if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING
                || spell == SPELL_METEOR_SHOWER) {
                targetX_b = m_hexCells[targetHex].m_x;
                targetY_b = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
            }
        }

        if (targetX_b != -1) {
            if (m_currentSide == 0) {
                castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0]
                        + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0]
                        + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
            } else {
                castX = COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0];
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0]
                        + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
            }
            if ((m_currentSide == 0 ? 1 : -1) * (targetX_b - castX) < targetY_b - castY) {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_HIGH;
                if (m_currentSide == 0) {
                    castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1]
                            + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1]
                            + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
                } else {
                    castX =
                        COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1];
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1]
                            + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
                }
            } else {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_LOW;
            }
        } else {
            m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_NO_TARGET;
        }
        m_heroAnimationFrame[m_currentSide] = 0;
        while (m_heroAnimationFrame[m_currentSide]
               < sCmbtHero[m_heroSpriteIndex[m_currentSide]]
                     .animationFrameCount[m_heroAnimationState[m_currentSide]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[m_currentSide]++;
        }
        m_heroAnimationFrame[m_currentSide]--;
    }

    soundSpell_q = spell;
    if (spell == CREATURE_SPELL_PETRIFY)
        soundSpell_q = SPELL_PARALYZE;
    if (spell == CREATURE_SPELL_DISPEL)
        soundSpell_q = SPELL_DISPEL;
    if (strlen(gsSpellInfo[IDX(soundSpell_q)].soundName) != 0)
        sprintf(sampleName_i, "%s.82M", gsSpellInfo[IDX(soundSpell_q)].soundName);

    if (castByCreature == 0 && target_i != 0 && !target_i->SpellCastWorks(spell)) {
        ShowSpellCastFailure(target_i, IDX(spell));
    } else {
        spellSample = LoadPlaySample(sampleName_i);
        switch (spell) {
            case SPELL_TELEPORT:
                teleportArmy_i = target_i;
                targetHex = teleportDestination;
                RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 1);
                m_hexCells[teleportArmy_i->m_hex].m_occupantSide = COMBAT_HEX_EMPTY;
                m_hexCells[teleportArmy_i->m_hex].m_occupantIndex = COMBAT_HEX_EMPTY;
                if (m_hexCells[teleportArmy_i->m_hex].m_occupantFrame == ARMY_FACING_LEFT) {
                    m_hexCells[teleportArmy_i->m_hex + 1].m_occupantSide = COMBAT_HEX_EMPTY;
                    m_hexCells[teleportArmy_i->m_hex + 1].m_occupantIndex = COMBAT_HEX_EMPTY;
                } else if (m_hexCells[teleportArmy_i->m_hex].m_occupantFrame == ARMY_FACING_RIGHT) {
                    m_hexCells[teleportArmy_i->m_hex - 1].m_occupantSide = COMBAT_HEX_EMPTY;
                    m_hexCells[teleportArmy_i->m_hex - 1].m_occupantIndex = COMBAT_HEX_EMPTY;
                }
                if (gbNoShowCombat == 0)
                    WaitEndSample(spellSample, -1);
                if (gbNoShowCombat == 0) {
                    sprintf(gText, "telptin.82m");
                    spellSample = LoadPlaySample(gText);
                }
                if ((teleportArmy_i->m_monster.flags.all & ARMY_FLAG_WIDE) != 0) {
                    adjacentHex_q = teleportDestination;
                    if (teleportArmy_i->m_facing == ARMY_FACING_RIGHT) {
                        adjacentHex_q =
                            teleportArmy_i->GetAdjacentCellIndex(teleportDestination, 1);
                        if (adjacentHex_q == COMBAT_HEX_EMPTY
                            || (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY
                                && (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i
                                    || m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k))
                            || m_hexCells[adjacentHex_q].m_blocked != 0) {
                            targetHex = teleportDestination - 1;
                        }
                    }
                    if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                        adjacentHex_q = teleportArmy_i->GetAdjacentCellIndex(adjacentHex_q, 4);
                        if (adjacentHex_q == COMBAT_HEX_EMPTY
                            || (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY
                                && (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i
                                    || m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k))
                            || m_hexCells[adjacentHex_q].m_blocked != 0) {
                            targetHex++;
                        }
                    }
                    teleportArmy_i->m_hex = targetHex;
                    if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                        m_hexCells[targetHex].m_occupantSide = static_cast<i8>(targetSide_i);
                        m_hexCells[targetHex].m_occupantIndex = static_cast<i8>(targetIndex_k);
                        m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_RIGHT;
                        m_hexCells[targetHex - 1].m_occupantSide = static_cast<i8>(targetSide_i);
                        m_hexCells[targetHex - 1].m_occupantIndex = static_cast<i8>(targetIndex_k);
                        m_hexCells[targetHex - 1].m_occupantFrame = ARMY_FACING_LEFT;
                    } else if (teleportArmy_i->m_facing == ARMY_FACING_RIGHT) {
                        m_hexCells[targetHex].m_occupantSide = static_cast<i8>(targetSide_i);
                        m_hexCells[targetHex].m_occupantIndex = static_cast<i8>(targetIndex_k);
                        m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_LEFT;
                        m_hexCells[targetHex + 1].m_occupantSide = static_cast<i8>(targetSide_i);
                        m_hexCells[targetHex + 1].m_occupantIndex = static_cast<i8>(targetIndex_k);
                        m_hexCells[targetHex + 1].m_occupantFrame = ARMY_FACING_RIGHT;
                    }
                    RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
                } else {
                    teleportArmy_i->m_hex = teleportDestination;
                    m_hexCells[teleportArmy_i->m_hex].m_occupantSide =
                        static_cast<i8>(targetSide_i);
                    m_hexCells[teleportArmy_i->m_hex].m_occupantIndex =
                        static_cast<i8>(targetIndex_k);
                    m_hexCells[teleportArmy_i->m_hex].m_occupantFrame = COMBAT_HEX_EMPTY;
                    RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
                }
                break;
            case SPELL_DISRUPTING_RAY:
                oldDefense = target_i->m_monster.defense;
                target_i->m_monster.defense -= SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
                if (target_i->m_monster.defense < SPELL_MINIMUM_DEFENSE)
                    target_i->m_monster.defense = SPELL_MINIMUM_DEFENSE;
                sprintf(
                    gText,
                    "The disrupting ray reduces defense by %d.",
                    oldDefense - target_i->m_monster.defense
                );
                CombatMessage(gText, 1, 1, 0);
                DoBlast(targetHex, IDX(spell));
                RippleCreature(target_i->m_side, target_i->m_index, 0);
                break;
            case SPELL_COLD_RAY:
                DelayMilli(
                    static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_COLD_RAY_DELAY)
                );
                damage_m = spellPower_i * SPELL_COLD_RAY_DAMAGE_PER_POWER;
                if (target_i->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                    damage_m = spellPower_i * SPELL_COLD_RAY_FIRE_ELEMENTAL_DAMAGE_PER_POWER;
                if (target_i->m_monsterType == CREATURE_IRON_GOLEM
                    || target_i->m_monsterType == CREATURE_STEEL_GOLEM)
                    damage_m = static_cast<i32l>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                ModifyDamageForArtifacts(
                    &damage_m,
                    IDX(spell),
                    m_heroes[m_currentSide],
                    m_heroes[1 - m_currentSide]
                );
                coldRayArmyName_f = target_i->m_quantity < 2
                                        ? gArmyNames[IDX(target_i->m_monsterType)]
                                        : gArmyNamesPlural[IDX(target_i->m_monsterType)];
                sprintf(
                    gText,
                    "The cold ray does %d\n damage to the %s.",
                    damage_m,
                    coldRayArmyName_f
                );
                CombatMessage(gText, 1, 1, 0);
                DoBlast(targetHex, IDX(spell));
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_COLD_RAY)].combatEffect, 0, 0);
                target_i->Damage(damage_m, SPELL_NONE);
                target_i->PowEffect(-1, 1, -1, -1);
                break;
            case SPELL_CHAIN_LIGHTNING:
                ChainLightning(targetHex, spellPower_i);
                break;
            case SPELL_MAGIC_ARROW:
                DelayMilli(
                    static_cast<i32l>(
                        gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_MAGIC_ARROW_DELAY
                    )
                );
                damage_m = spellPower_i * SPELL_MAGIC_ARROW_DAMAGE_PER_POWER;
                ModifyDamageForArtifacts(
                    &damage_m,
                    IDX(spell),
                    m_heroes[m_currentSide],
                    m_heroes[1 - m_currentSide]
                );
                magicArrowArmyName_n = target_i->m_quantity < 2
                                           ? gArmyNames[IDX(target_i->m_monsterType)]
                                           : gArmyNamesPlural[IDX(target_i->m_monsterType)];
                sprintf(
                    gText,
                    "The magic arrow does %d\n damage to the %s.",
                    damage_m,
                    magicArrowArmyName_n
                );
                CombatMessage(gText, 1, 1, 0);
                missileIcon_p = gpResourceManager->GetIcon("keep.icn");
                missileAngles[0] = 90.0f;
                missileAngles[1] = 68.5f;
                missileAngles[2] = 45.0f;
                missileAngles[3] = 20.8f;
                missileAngles[4] = 0.0f;
                missileAngles[5] = -20.8f;
                missileAngles[6] = -45.0f;
                missileAngles[7] = -68.5f;
                missileAngles[8] = -90.0f;
                ShootMissile(
                    castX,
                    castY,
                    target_i->MidX(),
                    target_i->MidY(),
                    missileAngles,
                    missileIcon_p
                );
                gpResourceManager->Dispose(missileIcon_p);
                target_i->Damage(damage_m, SPELL_NONE);
                target_i->PowEffect(-1, 1, -1, -1);
                break;
            case SPELL_LIGHTNING_BOLT:
                damage_m = spellPower_i * SPELL_LIGHTNING_DAMAGE_PER_POWER;
                if (target_i->m_monsterType == CREATURE_AIR_ELEMENTAL)
                    damage_m = spellPower_i * SPELL_LIGHTNING_AIR_ELEMENTAL_DAMAGE_PER_POWER;
                if (target_i->m_monsterType == CREATURE_IRON_GOLEM
                    || target_i->m_monsterType == CREATURE_STEEL_GOLEM)
                    damage_m = static_cast<i32l>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                ModifyDamageForArtifacts(
                    &damage_m,
                    IDX(spell),
                    m_heroes[m_currentSide],
                    m_heroes[1 - m_currentSide]
                );
                lightningArmyName_d = target_i->m_quantity < 2
                                          ? gArmyNames[IDX(target_i->m_monsterType)]
                                          : gArmyNamesPlural[IDX(target_i->m_monsterType)];
                sprintf(
                    gText,
                    "The lightning bolt does %d\n damage to the %s.",
                    damage_m,
                    lightningArmyName_d
                );
                CombatMessage(gText, 1, 1, 0);
                DoBolt(
                    1,
                    castX,
                    castY,
                    target_i->MidX(),
                    target_i->MidY(),
                    150,
                    100,
                    9,
                    2,
                    301,
                    -40,
                    40,
                    30,
                    1,
                    0,
                    0,
                    1
                );
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_LIGHTNING_BOLT)].combatEffect, 0, 0);
                target_i->Damage(damage_m, SPELL_NONE);
                target_i->PowEffect(-1, 1, -1, -1);
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
                CastMassSpell(IDX(spell), spellPower_i);
                break;
            case SPELL_MIRROR_IMAGE:
                MirrorImage(targetHex);
                break;
            case SPELL_SUMMON_EARTH_ELEMENTAL:
                SummonElemental(IDX(CREATURE_EARTH_ELEMENTAL), spellPower_i);
                break;
            case SPELL_SUMMON_AIR_ELEMENTAL:
                SummonElemental(IDX(CREATURE_AIR_ELEMENTAL), spellPower_i);
                break;
            case SPELL_SUMMON_FIRE_ELEMENTAL:
                SummonElemental(IDX(CREATURE_FIRE_ELEMENTAL), spellPower_i);
                break;
            case SPELL_SUMMON_WATER_ELEMENTAL:
                SummonElemental(IDX(CREATURE_WATER_ELEMENTAL), spellPower_i);
                break;
            case SPELL_RESURRECT:
            case SPELL_TRUE_RESURRECT:
            case SPELL_ANIMATE_DEAD:
                Resurrect(IDX(spell), targetHex, spellPower_i);
                break;
            case SPELL_CURE:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_CURE)].combatEffect, 0, 0);
                target_i->Cure(spellPower_i);
                DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                break;
            case SPELL_SLOW:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_SLOW, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_SLOW)].combatEffect, 0, 0);
                break;
            case SPELL_HASTE:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_HASTE, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_HASTE)].combatEffect, 0, 0);
                break;
            case SPELL_SHIELD:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_SHIELD, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_SHIELD)].combatEffect, 0, 0);
                break;
            case SPELL_DRAGON_SLAYER:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_DRAGON_SLAYER, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_DRAGON_SLAYER)].combatEffect, 0, 0);
                break;
            case SPELL_BLESS:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_BLESS, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_BLESS)].combatEffect, 0, 0);
                break;
            case SPELL_STONE_SKIN:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_STONE_SKIN, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_STONE_SKIN)].combatEffect, 0, 0);
                break;
            case SPELL_STEEL_SKIN:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_STEEL_SKIN, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_STEEL_SKIN)].combatEffect, 0, 0);
                break;
            case SPELL_CURSE:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_CURSE, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_CURSE)].combatEffect, 0, 0);
                break;
            case SPELL_BERSERKER:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_BERSERKER, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_BERSERKER)].combatEffect, 0, 0);
                break;
            case SPELL_HYPNOTIZE:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_HYPNOTIZE, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_HYPNOTIZE)].combatEffect, 0, 0);
                break;
            case SPELL_PARALYZE:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_PARALYZE, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_PARALYZE)].combatEffect, 0, 0);
                break;
            case CREATURE_SPELL_DISPEL:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->DispelGood();
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_DISPEL)].combatEffect, 0, 1);
                break;
            case SPELL_DISPEL:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->DispelGood();
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_DISPEL)].combatEffect, 0, 0);
                for (influence = 0; influence < IDX(SPELL_INFLUENCE_COUNT); influence++)
                    target_i->CancelIndividualSpell(ArmySpellInfluence(influence));
                break;
            case SPELL_BLIND:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_BLIND, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_BLIND)].combatEffect, 0, 0);
                break;
            case SPELL_BLOOD_LUST:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                BloodLustEffect(target_i, SPELL_BLOOD_LUST_EFFECT);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_BLOOD_LUST, SPELL_BLOOD_LUST_DURATION);
                break;
            case SPELL_ANTI_MAGIC:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                target_i->SetSpellInfluence(SPELL_INFLUENCE_ANTI_MAGIC, spellPower_i);
                target_i->SpellEffect(gsSpellInfo[IDX(SPELL_ANTI_MAGIC)].combatEffect, 0, 0);
                break;
            case CREATURE_SPELL_PETRIFY:
                ShowSpellMessage(castByCreature, IDX(spell), target_i);
                TurnToStone(target_i);
                break;
            case SPELL_COLD_RING:
                Fireball(targetHex, IDX(SPELL_COLD_RING));
                break;
            case SPELL_FIREBALL:
                Fireball(targetHex, IDX(SPELL_FIREBALL));
                break;
            case SPELL_FIREBLAST:
                Fireball(targetHex, IDX(SPELL_FIREBLAST));
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
    }

    for (side_i = 0; side_i < COMBAT_SIDE_COUNT; side_i++) {
        for (armyIndex = 0; armyIndex < m_armyCount[side_i]; armyIndex++) {
            army& combatArmy = m_armies[side_i][armyIndex];
            combatArmy.m_killPending = 0;
            combatArmy.m_deathPending = combatArmy.m_killPending;
            combatArmy.m_damagePending = combatArmy.m_deathPending;
            combatArmy.m_drawState = 1;
            combatArmy.m_animationState = 0;
            combatArmy.m_lastTargetHex = COMBAT_HEX_EMPTY;
        }
    }

    if (castByCreature == 0) {
        m_heroAnimationState[m_currentSide]++;
        m_heroAnimationFrame[m_currentSide] = 0;
        while (m_heroAnimationFrame[m_currentSide]
               < sCmbtHero[m_heroSpriteIndex[m_currentSide]]
                     .animationFrameCount[m_heroAnimationState[m_currentSide]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[m_currentSide]++;
        }
        m_heroAnimationState[m_currentSide] = 0;
        m_heroAnimationFrame[m_currentSide] = 0;
        DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }
    WaitEndSample(spellSample, -1);
    CheckChangeSelector();
}

// @semantic: the 0x08 frame, body operands, CFG semantics, and all three ordered
// relocations agree. The only raw residual is the branch byte at +0x18: retail routes a
// failed ValidHex test to the shared empty-cell return trampoline at +0x3f,
// while VC4.2 targets the common epilogue directly. Nested empty/else, two early
// guards, positive-arm goto, explicit common-tail goto, and scoped label variants
// were tried; the retained nested early return removes the former +0x40 residual.
// Revisit only after a material TU-state change.
VA(0x00423688, 0xda)
void combatManager::DefaultSpell(i32 targetHex) {
    if (ValidHex(targetHex)) {
        if (m_hexCells[targetHex].m_occupantSide < 0)
            return;
        army* target =
            &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];
        target->SpellEffect(gsSpellInfo[IDX(m_selectedSpell)].combatEffect, 0, 1);
    }
}

// @early-stop
// Complete 0x623 body, 0x5c frame/slots, 422-instruction CFG, and all 51 ordered
// relocations align. The only executable residual at +0x366..+0x371 is the
// equivalent affectedCount[-0xc]/frame[-0x18] loop comparison: retail loads the
// bound and uses `jge`, while ours loads the index and uses `jle`. Reversed
// relational and OD_STEER(affectedCount) forms were byte-neutral; revisit on TU change.
VA(0x00423762, 0x623)
void combatManager::Fireball(i32 targetHex, i32 spell) {
    if (!ValidHex(targetHex))
        return;

    i32l baseDamage_w;
    icon* spellIcon_n;
    i32 affectedCount_n;
    army* target_n;
    i32 targetX_n;
    i32 frame_i;
    i32 targetY_p;
    i16 affectedHexes_e[SPELL_FIREBALL_AFFECTED_HEX_COUNT];
    i32 anyAffected_g;
    i32l damage_f;
    i32 frameCount_i;

    targetX_n = m_hexCells[targetHex].m_x;
    targetY_p = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
    if (!gbNoShowCombat) {
        frameCount_i = SPELL_FIREBALL_FRAME_COUNT;
        if (spell == SPELL_FIREBALL)
            spellIcon_n = gpResourceManager->GetIcon("fireball.icn");
        else if (spell == SPELL_FIREBLAST)
            spellIcon_n = gpResourceManager->GetIcon("firebal2.icn");
        else {
            spellIcon_n = gpResourceManager->GetIcon("coldring.icn");
            frameCount_i = SPELL_COLD_RING_FRAME_COUNT;
        }

        for (frame_i = 0; frame_i < frameCount_i; ++frame_i) {
            glTimers[0] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_AREA_ANIMATION_DELAY
            );
            IconToBitmap(
                spellIcon_n,
                gpWindowManager->m_screen,
                targetX_n,
                targetY_p,
                frame_i,
                1,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
            if (spell == SPELL_COLD_RING) {
                FlipIconToBitmap(
                    spellIcon_n,
                    gpWindowManager->m_screen,
                    targetX_n,
                    targetY_p,
                    frame_i,
                    1,
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
        gpResourceManager->Dispose(spellIcon_n);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target_n = m_armies[0] + m_currentSide * COMBAT_ARMY_STORAGE_SLOT_COUNT + m_currentArmyIndex;
    for (frame_i = 0; frame_i < SPELL_FIREBALL_AFFECTED_HEX_COUNT; ++frame_i)
        affectedHexes_e[frame_i] = COMBAT_HEX_EMPTY;
    if (spell != SPELL_COLD_RING)
        affectedHexes_e[0] = static_cast<i16>(targetHex);

    for (frame_i = 0; frame_i < SPELL_ADJACENT_DIRECTION_COUNT; ++frame_i) {
        affectedHexes_e[frame_i + 1] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(targetHex, frame_i));
        if (spell == SPELL_FIREBLAST) {
            affectedHexes_e[frame_i + SPELL_FIREBLAST_SECOND_RING_FIRST] = static_cast<i16>(
                target_n->GetAdjacentCellIndex(affectedHexes_e[frame_i + 1], frame_i)
            );
        }
    }
    if (spell == SPELL_FIREBLAST) {
        affectedHexes_e[SPELL_FIREBLAST_AXIAL_FIRST] =
            static_cast<i16>(targetHex - SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes_e[SPELL_FIREBLAST_AXIAL_FIRST] < 0)
            affectedHexes_e[SPELL_FIREBLAST_AXIAL_FIRST] = COMBAT_HEX_EMPTY;
        affectedHexes_e[SPELL_FIREBLAST_AXIAL_SECOND] =
            static_cast<i16>(targetHex + SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes_e[SPELL_FIREBLAST_AXIAL_SECOND] >= COMBAT_HEX_COUNT)
            affectedHexes_e[SPELL_FIREBLAST_AXIAL_SECOND] = COMBAT_HEX_EMPTY;
        affectedHexes_e[SPELL_FIREBLAST_CORNER_FIRST] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(affectedHexes_e[2], 0));
        affectedHexes_e[SPELL_FIREBLAST_CORNER_FIRST + 1] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(affectedHexes_e[2], 2));
        affectedHexes_e[SPELL_FIREBLAST_CORNER_FIRST + 2] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(affectedHexes_e[5], 5));
        affectedHexes_e[SPELL_FIREBLAST_CORNER_FIRST + 3] =
            static_cast<i16>(GetAdjacentCellIndexNoArmy(affectedHexes_e[5], 3));
    }

    baseDamage_w = m_spellPower[m_currentSide] * SPELL_FIREBALL_DAMAGE_PER_POWER;
    ClearEffects();
    anyAffected_g = 0;
    affectedCount_n = SPELL_FIREBALL_AFFECTED_HEX_COUNT;
    for (frame_i = 0; frame_i < affectedCount_n; ++frame_i) {
        if (affectedHexes_e[frame_i] != COMBAT_HEX_EMPTY
            && m_hexCells[affectedHexes_e[frame_i]].m_occupantSide != COMBAT_HEX_EMPTY) {
            target_n = &m_armies[m_hexCells[affectedHexes_e[frame_i]].m_occupantSide]
                                [m_hexCells[affectedHexes_e[frame_i]].m_occupantIndex];
            if (target_n->SpellCastWorks(SpellType(spell))
                && !*(
                    gArmyEffected[0]
                    + m_hexCells[affectedHexes_e[frame_i]].m_occupantSide * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[affectedHexes_e[frame_i]].m_occupantIndex
                )) {
                *(gArmyEffected[0]
                  + m_hexCells[affectedHexes_e[frame_i]].m_occupantSide * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[affectedHexes_e[frame_i]].m_occupantIndex) = 1;
                if (target_n->m_damagePending == 0) {
                    damage_f = baseDamage_w;
                    if (spell == SPELL_COLD_RING
                        && target_n->m_monsterType == CREATURE_FIRE_ELEMENTAL)
                        damage_f <<= 1;
                    if ((spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                        && target_n->m_monsterType == CREATURE_WATER_ELEMENTAL)
                        damage_f *= 2;
                    if (target_n->m_monsterType == CREATURE_IRON_GOLEM
                        || target_n->m_monsterType == CREATURE_STEEL_GOLEM) {
                        damage_f = static_cast<i32l>(damage_f * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                    }
                    target_n->Damage(damage_f, SpellType(spell));
                    anyAffected_g = 1;
                }
            }
        }
    }
    if (anyAffected_g) {
        ModifyDamageForArtifacts(
            &baseDamage_w,
            spell,
            m_heroes[m_currentSide],
            m_heroes[1 - m_currentSide]
        );
        if (spell == SPELL_COLD_RING)
            sprintf(gText, "The cold ring does %d damage.", baseDamage_w);
        else
            sprintf(gText, "The fireball does %d damage.", baseDamage_w);
        CombatMessage(gText, 1, 1, 0);
        target_n->PowEffect(-1, 1, -1, -1);
    }
}

VA(0x00423d85, 0x3c9)
void combatManager::MeteorShower(i32 targetHex) {
    if (!ValidHex(targetHex))
        return;

    army* target_k = &m_armies[m_currentSide][0] + m_currentArmyIndex;
    i32 affectedHexes_f[SPELL_METEOR_AFFECTED_HEX_COUNT];
    affectedHexes_f[0] = targetHex;
    i32 direction;
    i32 frame;
    for (direction = 0; direction < SPELL_ADJACENT_DIRECTION_COUNT; ++direction) {
        affectedHexes_f[direction + 1] = GetAdjacentCellIndexNoArmy(targetHex, direction);
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
                    0,
                    0,
                    0,
                    0,
                    0
                );
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(meteorIcon);
    }

    i32 baseDamage_w = m_spellPower[m_currentSide] * SPELL_METEOR_DAMAGE_PER_POWER;
    ClearEffects();
    i32 anyAffected_h = 0;
    i32l damage_c;
    for (direction = 0; direction < SPELL_METEOR_AFFECTED_HEX_COUNT; ++direction) {
        if (affectedHexes_f[direction] != COMBAT_HEX_EMPTY
            && m_hexCells[affectedHexes_f[direction]].m_occupantSide != COMBAT_HEX_EMPTY) {
            target_k = &m_armies[m_hexCells[affectedHexes_f[direction]].m_occupantSide]
                                [m_hexCells[affectedHexes_f[direction]].m_occupantIndex];
            if (target_k->SpellCastWorks(SPELL_METEOR_SHOWER)
                && !*(
                    gArmyEffected[0]
                    + m_hexCells[affectedHexes_f[direction]].m_occupantSide * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[affectedHexes_f[direction]].m_occupantIndex
                )) {
                *(gArmyEffected[0]
                  + m_hexCells[affectedHexes_f[direction]].m_occupantSide * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[affectedHexes_f[direction]].m_occupantIndex) = 1;
                if (target_k->m_damagePending == 0) {
                    damage_c = baseDamage_w;
                    if (target_k->m_monsterType == CREATURE_EARTH_ELEMENTAL)
                        damage_c <<= 1;
                    target_k->Damage(damage_c, SPELL_METEOR_SHOWER);
                    anyAffected_h = 1;
                }
            }
        }
    }
    if (anyAffected_h) {
        sprintf(gText, "The meteor shower does %d damage.", baseDamage_w);
        CombatMessage(gText, 1, 1, 0);
        target_k->PowEffect(-1, 1, -1, -1);
    }
}

VA(0x0042414e, 0x2fb)
void combatManager::ElementalStorm(void) {
    i32 baseDamage_w;
    i32 column_e;
    army* target_m;
    i32 armyIndex_e;
    i32 frame_i;
    i32 row_b;
    i32 pass_c;
    i32 side_h;
    i32 anyAffected_f;
    i32l damage_e;
    icon* stormIcon_i;
    SLimitData limits_n;

    if (!gbNoShowCombat) {
        stormIcon_i = gpResourceManager->GetIcon("storm.icn");
        for (pass_c = 0; pass_c < SPELL_STORM_PASS_COUNT; ++pass_c) {
            for (frame_i = 0; frame_i < SPELL_STORM_FRAME_COUNT; ++frame_i) {
                glTimers[0] = static_cast<i32>(
                    KBTickCount()
                    + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_AREA_ANIMATION_DELAY
                );
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                for (row_b = 0; row_b < SPELL_STORM_ROW_COUNT; ++row_b) {
                    for (column_e = 0; column_e < SPELL_STORM_COLUMN_COUNT; ++column_e) {
                        stormIcon_i->CombatClipDrawToBuffer(
                            column_e * SPELL_STORM_TILE_SIZE,
                            row_b * SPELL_STORM_TILE_SIZE,
                            (column_e * SPELL_STORM_FRAME_COLUMN_STEP + (frame_i | 0) + row_b)
                                % SPELL_STORM_FRAME_COUNT,
                            &limits_n,
                            0,
                            0,
                            0,
                            0
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
    anyAffected_f = 0;
    baseDamage_w = m_spellPower[m_currentSide] * SPELL_ELEMENTAL_STORM_DAMAGE_PER_POWER;
    for (side_h = 0; side_h < COMBAT_SIDE_COUNT; ++side_h) {
        for (armyIndex_e = 0; armyIndex_e < m_armyCount[side_h]; ++armyIndex_e) {
            target_m = m_armies[0] + side_h * COMBAT_ARMY_STORAGE_SLOT_COUNT + armyIndex_e;
            if (target_m->SpellCastWorks(SPELL_ELEMENTAL_STORM)) {
                damage_e = baseDamage_w;
                if (target_m->m_monsterType == CREATURE_AIR_ELEMENTAL)
                    damage_e <<= 1;
                if (target_m->m_monsterType == CREATURE_IRON_GOLEM
                    || target_m->m_monsterType == CREATURE_STEEL_GOLEM) {
                    damage_e = static_cast<i32l>(damage_e * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side_h] && m_heroes[side_h]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    damage_e = static_cast<i32l>(damage_e * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                target_m->Damage(damage_e, SPELL_ELEMENTAL_STORM);
                anyAffected_f = 1;
            }
        }
    }
    if (anyAffected_f) {
        sprintf(gText, "The elemental storm does %d damage.", baseDamage_w);
        CombatMessage(gText, 1, 1, 0);
        target_m->PowEffect(-1, 1, -1, -1);
    }
}

// @semantic: complete damage, palette
// brighten/fade, 15-step shake,
// buffer copy, redraw, cleanup, and PowEffect CFG. Retail and ours both use a
// 0x120 frame, emit 573 instructions, and have the same 79 relocation targets.
// Retail slots base/affected/side/index/target/damage at
// -0x8/-0x24/-0x2c/-0x10/-0x14/-0x30; the current semantic suffix pass has
// -0x8/-0x1c/-0x24/-0x28/-0x10/-0x2c. First non-relocation stream divergence
// is retail +0x6d4 in source-buffer address evaluation; a later unsigned fade
// comparison has opposite block polarity. Tried cached and repeated palette
// components, signed/unsigned empty-arm fades, pointer-addition order, and one
// od_slots suffix pass. Retail +0x1c0 branches straight to +0x7e5 when
// gbNoShowCombat is set; from there PowEffect at +0x81e and both palette Data
// calls at +0x826/+0x831 are unguarded, followed by restore/dispose/delete and
// ShowColorPointer. This preserves retail's unsafe no-show path deliberately.
// The army pointer is assigned before SpellCastWorks on every nonempty combat
// stack iteration, and retail likewise calls PowEffect without the affected
// flag guarding it. Revisit at 95%; do not add guards or permute these paths.
VA(0x00424449, 0x9ff)
void combatManager::Armageddon(void) {
    i32 baseDamage2 = m_spellPower[m_currentSide] * SPELL_ARMAGEDDON_DAMAGE_PER_POWER;
    i32 anyAffected6 = 0;
    army* target18;
    i32 side11;
    for (side11 = 0; side11 < COMBAT_SIDE_COUNT; ++side11) {
        i32 armyIndex3;
        for (armyIndex3 = 0; armyIndex3 < m_armyCount[side11]; ++armyIndex3) {
            target18 = &m_armies[side11][armyIndex3];
            if (target18->SpellCastWorks(SPELL_ARMAGEDDON)) {
                i32l damage9 = baseDamage2;
                if (target18->m_monsterType == CREATURE_IRON_GOLEM
                    || target18->m_monsterType == CREATURE_STEEL_GOLEM) {
                    damage9 = static_cast<i32l>(damage9 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side11] && m_heroes[side11]->HasArtifact(ARTIFACT_BROACH_SHIELDING)) {
                    damage9 = static_cast<i32l>(damage9 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                target18->Damage(damage9, SPELL_ARMAGEDDON);
                anyAffected6 = 1;
            }
        }
    }
    if (anyAffected6) {
        sprintf(gText, "The armaggedon does %d damage.", baseDamage2);
        CombatMessage(gText, 1, 1, 0);
    }

    palette* originalPalette5 = 0;
    palette* effectPalette4 = 0;
    if (!gbNoShowCombat) {
        gpWindowManager->m_updateFlags = 0;
        originalPalette5 = gpResourceManager->GetPalette("kb.pal");
        effectPalette4 = new palette;
        if (!effectPalette4)
            MemError();
        memcpy(effectPalette4->Data(), originalPalette5->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);

        glTimers[0] = static_cast<i32>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_AREA_ANIMATION_DELAY
        );
        i8* effectData = effectPalette4->Data();
        i32 pass;
        for (pass = 0; pass < SPELL_ARMAGEDDON_BRIGHTEN_PASS_COUNT; ++pass) {
            i32 color;
            for (color = 0; color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color) {
                if (effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT];
                if (effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 1]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 1];
                if (effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 2]
                    < SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 2];
            }
            DelayTil(&glTimers[0]);
            SetPalette(effectData, 1);
            glTimers[0] = static_cast<i32>(
                KBTickCount()
                + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_ARMAGEDDON_PALETTE_DELAY
            );
        }

        i32 shakeOffsets[SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT][2] = {
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

        for (pass = 0; pass < SPELL_ARMAGEDDON_SHAKE_PASS_COUNT; ++pass) {
            float scale = static_cast<float>(
                (SPELL_ARMAGEDDON_SHAKE_PASS_COUNT - pass) * SPELL_ARMAGEDDON_SHAKE_SCALE
            );
            i32 frame;
            for (frame = 0; frame < SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT; ++frame) {
                if (pass == 0) {
                    i32 color;
                    for (color = 0; color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT; ++color) {
                        if (effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 1]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 1] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                        if (effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 2]
                            > SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData[color * SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT + 2] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                    }
                    SetPalette(effectData, 1);
                }

                i32 offsetX = static_cast<i32>(shakeOffsets[frame][0] * scale);
                i32 offsetY = static_cast<i32>(shakeOffsets[frame][1] * scale);
                i32 timer = static_cast<i32>(
                    KBTickCount()
                    + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_ARMAGEDDON_SHAKE_DELAY
                );
                PollSound();
                i32 copyWidth =
                    COMBAT_SCREEN_WIDTH - abs(static_cast<i32>(shakeOffsets[frame][0] * scale));
                i32 copyHeight =
                    COMBAT_AREA_HEIGHT - abs(static_cast<i32>(shakeOffsets[frame][1] * scale)) - 1;
                u8* source = m_backgroundBuffer->m_pixels
                             + (offsetY > 0 ? offsetY : 0) * COMBAT_SCREEN_WIDTH
                             + (offsetX > 0 ? offsetX : 0);
                u8* destination = gpWindowManager->m_screen->m_pixels
                                  + (offsetY > 0 ? 0 : -offsetY) * COMBAT_SCREEN_WIDTH
                                  + (offsetX > 0 ? 0 : -offsetX);
                i32 row;
                for (row = 0; row < copyHeight; ++row) {
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

    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target18->PowEffect(-1, 1, -1, -1);

    i8* effectDataRestore7 = effectPalette4->Data();
    i8* originalData18 = originalPalette5->Data();
    i32 restorePass9;
    for (restorePass9 = 0; restorePass9 < SPELL_ARMAGEDDON_RESTORE_PASS_COUNT; ++restorePass9) {
        i32 component5;
        for (component5 = 0; component5 < SPELL_ARMAGEDDON_PALETTE_SIZE; ++component5) {
            if (static_cast<u8>(effectDataRestore7[component5])
                == static_cast<u8>(originalData18[component5])) {
            } else {
                if (static_cast<u8>(originalData18[component5])
                    < static_cast<u8>(effectDataRestore7[component5])) {
                    if (static_cast<u8>(originalData18[component5])
                        < static_cast<u8>(effectDataRestore7[component5])
                              - SPELL_ARMAGEDDON_CHANNEL_STEP)
                        effectDataRestore7[component5] =
                            static_cast<u8>(effectDataRestore7[component5])
                            - SPELL_ARMAGEDDON_CHANNEL_STEP;
                    else
                        effectDataRestore7[component5] = originalData18[component5];
                } else if (static_cast<u8>(effectDataRestore7[component5])
                               + SPELL_ARMAGEDDON_CHANNEL_STEP
                           < static_cast<u8>(originalData18[component5])) {
                    effectDataRestore7[component5] = static_cast<u8>(effectDataRestore7[component5])
                                                     + SPELL_ARMAGEDDON_CHANNEL_STEP;
                } else {
                    effectDataRestore7[component5] = originalData18[component5];
                }
            }
        }
        SetPalette(effectDataRestore7, 1);
        DelayMilli(
            static_cast<i32l>(
                gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_ARMAGEDDON_PALETTE_DELAY
            )
        );
    }
    SetPalette(originalPalette5->Data(), 1);
    gpWindowManager->m_updateFlags = 1;
    gpResourceManager->Dispose(originalPalette5);
    delete effectPalette4;
    gpMouseManager->ShowColorPointer();
}

// @early-stop
// Complete 0x101 body, 0x4 frame/slot, 86-instruction CFG, and all 23 ordered
// relocations align. The only executable residual at +0x17..+0x2f computes the
// same m_limitCreatureCount element: retail loads target index (+0xea) before
// side (+0xe6), while ours loads side first. Reversed indexing and three flat/
// pointer-addition spellings were byte-neutral or worse; revisit on TU change.
VA(0x00424e48, 0x101)
void combatManager::TurnToStone(army* target) {
    ResetLimitCreature();
    ++m_limitCreatureCount[target->m_side][target->m_index];
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
        0,
        0
    );
}

VA(0x00424f49, 0x192)
void combatManager::BloodLustEffect(army* target, i32 effect) {
    ResetLimitCreature();
    ++m_limitCreatureCount[target->m_side][target->m_index];
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
        0,
        0
    );
    gpWindowManager->SaveFizzleSource(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    target->m_monster.flags.abilityFlags -= effect;
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1,
        SPELL_BLOOD_LUST_FIZZLE_STEPS,
        0,
        0
    );
}

VA(0x004250db, 0x80)
void combatManager::Ripple(i32 strength) {
    i32 unusedRippleWord1;
    i32 unusedRippleWord2;

    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    DoRipple(m_backgroundBuffer, gpWindowManager->m_screen, COMBAT_AREA_HEIGHT, strength);
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

VA(0x0042515b, 0x72)
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

// @semantic: complete distance/width interpolation and both angle
// perturbation paths; all 30 relocation targets agree. A bucket-11 unused word
// restores the retail 0x58 frame and mapped slots. The first normalized-stream
// difference is retail +0x1c8 `fld qword ptr
// [const_000eb1a8]` (DIR32 operand at +0x1ca) versus the same opcode/value using
// ours $T8146 identity. The first opcode/CFG divergence is retail +0x21c
// `jne +0x5` versus ours `je +0xf2` for the second zero-angle guard. Tried
// direct/compound intermediate angle assignments and nested zero-angle guards.
// Revisit at 95% for od_slots; do not repeat these spellings beforehand.
VA(0x004251cd, 0x320)
void combatManager::ResetBoltAngle(SBolt* bolt) {
    i32 unusedBoltAngleWord1;

    if (bolt->nearTarget != 0)
        return;

    i32 deltaX = abs(bolt->endX - bolt->pixelX);
    i32 deltaY = abs(bolt->endY - bolt->pixelY);
    i32 distance = static_cast<i32>(sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
    if (bolt->totalDistance < distance)
        bolt->distanceRatio = 0;
    else
        bolt->distanceRatio =
            static_cast<float>(bolt->totalDistance - distance) / bolt->totalDistance;

    if (bolt->startWidth != bolt->endWidth) {
        i32 widthDelta = bolt->endWidth - bolt->startWidth;
        if (widthDelta > 0)
            ++widthDelta;
        else
            --widthDelta;
        i32 width = bolt->startWidth + static_cast<i32>(widthDelta * bolt->distanceRatio);
        if (width < 1)
            width = 1;
        bolt->width = width;
    }
    bolt->widthFirst = -(bolt->width >> 1);
    bolt->widthLast = bolt->width + bolt->widthFirst - 1;

    i32 angleX = bolt->endX - bolt->pixelX;
    i32 angleY = bolt->endY - bolt->pixelY;
    bolt->baseAngle =
        static_cast<float>(atan2(static_cast<double>(angleX), static_cast<double>(angleY)));
    float averageAngle =
        static_cast<float>((bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR);
    averageAngle = static_cast<float>(
                       (BOLT_INITIAL_ANGLE_BIAS - bolt->distanceRatio) / BOLT_INITIAL_ANGLE_DIVISOR
                   )
                   * averageAngle;
    bolt->angle = averageAngle + bolt->baseAngle;

    if (bolt->minAngle != 0 || bolt->maxAngle != 0) {
        if (!(bolt->angleDistance * BOLT_ANGLE_DISTANCE_FACTOR < distance || bolt->forceAngle != 0))
            return;
        float randomAngle;
        if (bolt->minAngle == bolt->maxAngle)
            randomAngle = static_cast<float>(bolt->minAngle) / BOLT_ANGLE_PERCENT_SCALE;
        else
            randomAngle = static_cast<float>(Random(bolt->minAngle, bolt->maxAngle))
                          / BOLT_ANGLE_PERCENT_SCALE;
        randomAngle = static_cast<float>(
                          (BOLT_RANDOM_ANGLE_BIAS - bolt->distanceRatio) / BOLT_RANDOM_ANGLE_DIVISOR
                      )
                      * randomAngle;
        bolt->baseAngle = randomAngle + bolt->baseAngle;
    }
}

// @semantic: complete stepping, clipping, width
// drawing, all six color
// modes, target approach, and finish CFG with the exact 0x40 frame. Retail is
// 0x4f0 bytes and ours 0x503. The first raw differing byte is +0x19, the local
// displacement in `mov [ebp-local], eax` for oldX: retail 0xd4 (-0x2c), ours
// 0xfc (-0x04). The first normalized instruction divergence is retail +0x6a
// `fld dword ptr [eax+0x38]`; ours starts `mov eax,[eax+0x38]`, stores a local,
// emits two `jmp $+0` continuations, then loads that local. The delinked switch
// boundary truncates homm2 relocs: explicit object ranges contain 28 relocations
// on both sides, with every external target agreeing and eight local entries
// targeting their respective DrawBolt labels. Tried direct and compound sin/cos
// assignment, both loop polarities, direct per-case pixel expressions, the
// positive movement wrapper, a nested lightning switch, and removing the random
// result slot. Horizontal-first distance evaluation is byte-neutral and retained
// because it matches retail. Revisit at 95% or after predecessor slot changes.
VA(0x004254ed, 0x4f0)
void combatManager::DrawBolt(SBolt* bolt, i32 stepCount) {
    i32 oldX = static_cast<i32>(bolt->currentX);
    i32 oldY = static_cast<i32>(bolt->currentY);
    i32 widthFirst = bolt->widthFirst;
    i32 widthLast = bolt->widthLast;
    i32 unusedRandom = Random(BOLT_RANDOM_WIDTH_LOW, BOLT_RANDOM_WIDTH_HIGH);
    i32 step;
    for (step = 0; stepCount > step; ++step) {
        bolt->currentX = static_cast<float>(sin(bolt->baseAngle)) + bolt->currentX;
        bolt->currentY = static_cast<float>(cos(bolt->baseAngle)) + bolt->currentY;
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

        i32 drawX = bolt->pixelX;
        i32 drawY = bolt->pixelY;
        if (bolt->pixelX == oldX) {
            if (bolt->pixelY == oldY)
                continue;
        }
        {
            oldX = bolt->pixelX;
            oldY = bolt->pixelY;
            i32 widthIndex;
            for (widthIndex = widthFirst;; ++widthIndex) {
                if (widthLast < widthIndex)
                    break;
                if (bolt->drawVertically != 0)
                    drawY = bolt->pixelY + widthIndex;
                else
                    drawX = bolt->pixelX + widthIndex;
                if (drawX >= 0 && drawX < COMBAT_SCREEN_WIDTH && drawY >= 0
                    && drawY < COMBAT_AREA_HEIGHT) {
                    i32 edgeDistance;
                    if (widthIndex < 0)
                        edgeDistance = -(widthFirst - widthIndex);
                    else
                        edgeDistance = widthLast - widthIndex;
                    switch (bolt->colorMode) {
                        case IDX(BOLT_COLOR_RED_TABLE):
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] =
                                gColorTableRed[static_cast<i8>(
                                    gpWindowManager->m_screen
                                        ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX]
                                )];
                            break;
                        case IDX(BOLT_COLOR_RED_BEAM):
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] =
                                uRedBeam[edgeDistance];
                            break;
                        case IDX(BOLT_COLOR_RAINBOW_FORWARD):
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] =
                                uRainbow[widthIndex - widthFirst];
                            break;
                        case IDX(BOLT_COLOR_RAINBOW_REVERSE):
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] =
                                uRainbow[BOLT_RAINBOW_LAST_INDEX - (widthIndex - widthFirst)];
                            break;
                        case IDX(BOLT_COLOR_LIGHTNING): {
                            u8 color;
                            if (edgeDistance == 0)
                                color = BOLT_LIGHTNING_SHADE_0;
                            else if (edgeDistance == 1)
                                color = BOLT_LIGHTNING_SHADE_1;
                            else if (edgeDistance == 2)
                                color = BOLT_LIGHTNING_SHADE_2;
                            else if (edgeDistance == 3)
                                color = BOLT_LIGHTNING_SHADE_3;
                            else if (edgeDistance == 4)
                                color = BOLT_LIGHTNING_SHADE_4;
                            else
                                color = BOLT_LIGHTNING_SHADE_5;
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] = color;
                            break;
                        }
                        default:
                            gpWindowManager->m_screen
                                ->m_pixels[drawY * COMBAT_SCREEN_WIDTH + drawX] =
                                static_cast<u8>(bolt->colorMode);
                            break;
                    }
                }
            }

            i32 distance = abs(bolt->endX - bolt->pixelX) + abs(bolt->endY - bolt->pixelY);
            if (bolt->nearTarget == 0) {
                if (distance < BOLT_NEAR_TARGET_DISTANCE) {
                    bolt->nearTarget = 1;
                    bolt->nearestDistance = distance;
                }
            } else if (bolt->nearestDistance + 1 < distance || distance < BOLT_FINISHED_DISTANCE) {
                bolt->finished = 1;
                return;
            } else if (distance < bolt->nearestDistance) {
                bolt->nearestDistance = distance;
            }
        }
    }
}

VA(0x004259dd, 0x26f)
void combatManager::AddBolt(
    SBolt* bolt,
    i32 startX,
    i32 startY,
    i32 endX,
    i32 endY,
    i32 branchDistance,
    i32 startWidth,
    i32 endWidth,
    i32 colorMode,
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
    } else if ((abs(endX - startX) | 0) > abs(endY - startY))
        bolt->drawVertically = 1;
    else
        bolt->drawVertically = 0;

    i32 deltaX = abs(endX - startX);
    i32 deltaY = abs(endY - startY);
    bolt->totalDistance =
        static_cast<i32>(sqrt(static_cast<double>((deltaX * deltaX | 0) + deltaY * deltaY)));
    ResetBoltAngle(bolt);
}

// @semantic: complete palette, 25-record bolt pool, extent/timer/blit,
// branching, angle reset, and cleanup CFG; all 60 relocation targets agree. The
// first raw differing byte is +0x05, the sub esp immediate: retail 0xac, ours
// 0xbc. After masking frame/local slots, the first relocation-identity difference
// is the retail +0x7d DIR32 operand for "kb.pal" versus ours equivalent string
// symbol. The first normalized instruction divergence is retail +0x1a1
// `mov eax,[ebp+0x28]` versus ours +0x1a4 `mov eax,[ebp+0x24]`; the following
// loads reverse the startWidth/endWidth operands of the equivalent maximum.
// Retail's executable size is 0xa82 through the `ret`; its 0xa84 object span
// additionally contains NOPs at +0xa82 and +0xa83. Ours uses a 0xac0 object span.
// Later residuals are loop-slot allocation and constant-pool identities. Tried
// inline/pointer record access, top/tail cleanup, positive/continue branch arms,
// ternary width selection, and stored/inline child angles. Retail itself leaves
// parameter 14 unused and uses scalar delete for the POD bolt allocation; both
// are preserved. Revisit at 95% for od_slots and AST permutation, not before.
VA(0x00425c4c, 0xa82)
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
    i32 colorMode,
    i32 minAngle,
    i32 maxAngle,
    i32 angleDistance,
    i32 unusedParameter,
    i32 forceAngle,
    i32 frameDelay,
    i32 brightenPalette
) {
    if (managePointer != 0)
        gpMouseManager->HideColorPointer();

    i32 drawDistance = angleDistance;
    i32 allFinished = 0;
    i32 drawPassCount = (angleDistance - 1) / drawDistance + 1;
    i32 branchChance = branchDistance * BOLT_ANGLE_PERCENT_SCALE / angleDistance;
    i32 deadline = KBTickCount();
    gpWindowManager->m_updateFlags = 0;

    palette* originalPalette = 0;
    palette* effectPalette = 0;
    if (brightenPalette != 0) {
        originalPalette = gpResourceManager->GetPalette("kb.pal");
        effectPalette = new palette;
        if (!effectPalette)
            MemError();
        memcpy(effectPalette->Data(), originalPalette->Data(), SPELL_ARMAGEDDON_PALETTE_SIZE);
        i8* effectData = effectPalette->Data();
        i32 component;
        for (component = 0; component < SPELL_ARMAGEDDON_PALETTE_SIZE; ++component) {
            effectData[component] += BOLT_PALETTE_BRIGHTEN_STEP;
            if (effectData[component] > BOLT_PALETTE_COMPONENT_MAX)
                effectData[component] = BOLT_PALETTE_COMPONENT_MAX;
        }
        SetPalette(effectData, 1);
    }

    if (endX < startX) {
        minAngle = -minAngle;
        maxAngle = -maxAngle;
    }
    if (maxAngle < minAngle) {
        i32 savedAngle = maxAngle;
        maxAngle = minAngle;
        minAngle = savedAngle;
    }

    SBolt* bolts = new SBolt[BOLT_MAX_COUNT];
    i32 extentPadding = (endWidth <= startWidth ? startWidth : endWidth) >> 1;
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

    i32 boltCount = 1;
    while (allFinished == 0) {
        i32 drawPass;
        for (drawPass = 0; drawPass < drawPassCount; ++drawPass) {
            allFinished = 1;
            i32 minY = BOLT_EXTENT_SENTINEL;
            i32 minX = minY;
            i32 maxY = -1;
            i32 maxX = maxY;
            i32 boltIndex;
            for (boltIndex = 0; boltIndex < boltCount; ++boltIndex) {
                if (bolts[boltIndex].finished == 0) {
                    if (maxX < bolts[boltIndex].pixelX)
                        maxX = bolts[boltIndex].pixelX;
                    if (bolts[boltIndex].pixelX < minX)
                        minX = bolts[boltIndex].pixelX;
                    if (maxY < bolts[boltIndex].pixelY)
                        maxY = bolts[boltIndex].pixelY;
                    if (bolts[boltIndex].pixelY < minY)
                        minY = bolts[boltIndex].pixelY;
                    DrawBolt(&bolts[boltIndex], drawDistance);
                    if (maxX < bolts[boltIndex].pixelX)
                        maxX = bolts[boltIndex].pixelX;
                    if (bolts[boltIndex].pixelX < minX)
                        minX = bolts[boltIndex].pixelX;
                    if (maxY < bolts[boltIndex].pixelY)
                        maxY = bolts[boltIndex].pixelY;
                    if (bolts[boltIndex].pixelY < minY)
                        minY = bolts[boltIndex].pixelY;
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

            for (boltIndex = 0; boltIndex < boltCount; ++boltIndex) {
                if (bolts[boltIndex].finished == 0)
                    allFinished = 0;
            }
            if (allFinished != 0)
                break;

            if (branchDistance != 0) {
                i32 oldBoltCount = boltCount;
                for (boltIndex = 0; boltIndex < oldBoltCount; ++boltIndex) {
                    if (bolts[boltIndex].finished == 0) {
                        i32 remainingDistance =
                            abs(bolts[boltIndex].endX - bolts[boltIndex].pixelX)
                            + abs(bolts[boltIndex].endY - bolts[boltIndex].pixelY);
                        if (boltCount < BOLT_MAX_COUNT && angleDistance * 2 < remainingDistance
                            && Random(0, branchChance) < BOLT_BRANCH_PERCENT_LIMIT) {
                            if (bolts[boltIndex].lastBranchX != 0) {
                                i32 branchSeparation =
                                    abs(bolts[boltIndex].lastBranchX - bolts[boltIndex].pixelX)
                                    + abs(bolts[boltIndex].lastBranchY - bolts[boltIndex].pixelY);
                                if (branchSeparation < branchDistance * BOLT_BRANCH_COOLDOWN_FACTOR)
                                    goto skipBranch;
                            }
                            bolts[boltIndex].lastBranchX = bolts[boltIndex].pixelX;
                            bolts[boltIndex].lastBranchY = bolts[boltIndex].pixelY;
                            float branchAngle =
                                static_cast<float>(
                                    Random(BOLT_BRANCH_RANDOM_LOW, BOLT_BRANCH_RANDOM_HIGH)
                                )
                                / BOLT_ANGLE_PERCENT_SCALE;
                            if (Random(0, 1) != 0)
                                branchAngle = -branchAngle;
                            float currentAngle = bolts[boltIndex].angle;
                            float childAngle = branchAngle + currentAngle;
                            i32 childDistance = Random(branchLength >> 1, branchLength);
                            if ((remainingDistance >> 1) < childDistance)
                                childDistance = remainingDistance >> 1;
                            i32 childX = static_cast<i32>(
                                sin(childAngle) * childDistance + bolts[boltIndex].pixelX
                            );
                            i32 childY = static_cast<i32>(
                                cos(childAngle) * childDistance + bolts[boltIndex].pixelY
                            );
                            i32 childWidth;
                            if (bolts[boltIndex].endWidth < bolts[boltIndex].startWidth)
                                childWidth = bolts[boltIndex].width - 1;
                            else
                                childWidth = bolts[boltIndex].width;
                            AddBolt(
                                &bolts[boltCount],
                                bolts[boltIndex].pixelX,
                                bolts[boltIndex].pixelY,
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
                                bolts[boltIndex].forceAngle
                            );
                            ++boltCount;
                        }
                    }
                skipBranch:;
                }
            }
        }

        i32 boltIndex;
        for (boltIndex = 0; boltIndex < boltCount; ++boltIndex) {
            if (bolts[boltIndex].finished == 0)
                ResetBoltAngle(&bolts[boltIndex]);
        }
    }

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

// @semantic
// Complete 0x18c body, 0x2c frame/slots, CFG, and all 12 ordered relocations
// agree. Qualifying distance fixes the final minimum branch. The remaining
// +0x12e/+0x132/+0x135/+0x139 bytes reverse the commutative deltaX/deltaY
// square loads; writing/qualifying deltaY first was byte-neutral.
VA(0x004266ce, 0x18c)
i32 combatManager::GetNextChainLightningTarget(army* source, i32 requireWorks) {
    army* candidate_p;
    i32 closestHex_f;
    i32 distance;
    i32 sourceX;
    i32 sourceY;
    i32 deltaX_e;
    i32 side;
    i32 deltaY_n;
    i32 closestDistance;
    i32 armyIndex_g;
    closestDistance = 999999;
    closestHex_f = COMBAT_HEX_EMPTY;
    sourceX = source->MidX();
    sourceY = source->MidY();
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex_g = 0; armyIndex_g < m_armyCount[side]; ++armyIndex_g) {
            candidate_p = &m_armies[side][armyIndex_g];
            if (gArmyEffected[side][armyIndex_g] == 0) {
                if ((requireWorks != 0 && candidate_p->SpellCastWorks(SPELL_CHAIN_LIGHTNING))
                    || (requireWorks == 0
                        && candidate_p->SpellCastWorkChance(SPELL_CHAIN_LIGHTNING) != 0.0f)) {
                    deltaX_e = abs(candidate_p->MidX() - sourceX);
                    deltaY_n = abs(candidate_p->MidY() - sourceY);
                    distance = static_cast<i32>(
                        sqrt(static_cast<double>(deltaX_e * deltaX_e + deltaY_n * deltaY_n))
                    );
                    if (OD_STEER(distance) < closestDistance) {
                        closestDistance = distance;
                        closestHex_f = candidate_p->m_hex;
                    }
                }
            }
        }
    }
    return closestHex_f;
}

// @semantic: complete four-target damage/selection/bolt CFG, exact 0x5c frame
// and slots, and all 37 ordered relocation targets align. Three evidenced unused
// bucket-12/13/14 words restore retail slots -0x2c, -0x3c, and -0x44. The only
// executable residual is the branch-distance clamp: retail lowers inclusive
// jge/jle tests while VC4.2 canonicalizes both equivalent assignments to jg/jl.
// Inclusive and MIN-1 spellings were byte-neutral or perturbed only TU state.
VA(0x0042685a, 0x361)
void combatManager::ChainLightning(i32 targetHex, i32 spellPower) {
    i32 firstBolt = 1;
    i32 damage = spellPower * CHAIN_LIGHTNING_INITIAL_DAMAGE_PER_POWER;
    i32 deadline = KBTickCount();
    i32 startX = castX;
    i32 startY = castY;
    i32 unusedChainWord6;
    i32 unusedChainWord5;
    i32 unusedChainWord8;

    ClearEffects();
    gpMouseManager->HideColorPointer();
    i32 strike;
    for (strike = 0; strike < CHAIN_LIGHTNING_MAX_TARGETS; ++strike) {
        army* target =
            &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];
        if (strike <= CHAIN_LIGHTNING_MAX_TARGETS - 2
            && m_hexCells[targetHex].m_occupantSide == m_currentSide)
            gpCombatManager->m_heroDeathPending[m_currentSide] = 1;

        i32 targetDamage = damage;
        if (target->m_monsterType == CREATURE_AIR_ELEMENTAL)
            targetDamage *= CHAIN_LIGHTNING_AIR_ELEMENTAL_MULTIPLIER;
        if (target->m_monsterType == CREATURE_IRON_GOLEM
            || target->m_monsterType == CREATURE_STEEL_GOLEM)
            targetDamage = static_cast<i32>(targetDamage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
        target->Damage(targetDamage, COMBAT_HEX_EMPTY);
        damage >>= 1;
        gArmyEffected[target->m_side][target->m_index] = 1;

        i32 targetX = target->MidX();
        i32 targetY = target->MidY();
        i32 deltaX = abs(targetX - startX);
        i32 deltaY = abs(targetY - startY);
        i32 distance =
            static_cast<i32>(sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
        i32 branchDistance = distance / CHAIN_LIGHTNING_DISTANCE_DIVISOR;
        if (branchDistance > CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE;
        if (branchDistance < CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE;
        i32 forceAngle = branchDistance < CHAIN_LIGHTNING_SHORT_BRANCH_THRESHOLD ? 2 : 3;
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
            CHAIN_LIGHTNING_COLOR_MODE,
            CHAIN_LIGHTNING_MIN_ANGLE,
            CHAIN_LIGHTNING_MAX_ANGLE,
            branchDistance,
            forceAngle,
            0,
            0,
            strike == 0
        );
        firstBolt = 0;
        startX = targetX;
        startY = targetY;
        DelayMilli(
            static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_FIZZLE_FRAME_DELAY)
        );
        i32 nextTarget = GetNextChainLightningTarget(target, 1);
        if (nextTarget == COMBAT_HEX_EMPTY)
            break;
        targetHex = nextTarget;
        DrawFrame(1, 0, 0, 0, 0, 1, 1);
        DelayTil(&deadline);
        deadline = static_cast<i32>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_FIZZLE_FRAME_DELAY
        );
    }
    ShowMassSpell(gArmyEffected, gsSpellInfo[IDX(SPELL_CHAIN_LIGHTNING)].combatEffect, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpMouseManager->ShowColorPointer();
}

// @semantic: The complete three-phase vapor mask and cleanup CFG agree. Retail
// stores the initial extent height, then replaces it with a stripe count based on
// the scaled first Y, 5 * (giMinExtentY / 5); both operations are preserved.
// Retail has a 0x30 frame versus ours 0x28. All 31 relocations now agree after
// recovering the 0x09dc source-line owner and its +9/+0x38 allocator offsets.
// The first normalized residual is stripe address evaluation order. Tried
// extent initialization before/after palette assignment and semantic slot
// suffixes. Revisit for slot shaping.
VA(0x00426bbb, 0x292)
void combatManager::VaporizeCreature(i32 side, i32 armyIndex) {
    DATA(0x004f04e4) static i16 vaporizeSourceLineBase = 2524;
    army* target = &m_armies[side][armyIndex];
    ResetLimitCreature();
    ++m_limitCreatureCount[side][armyIndex];
    gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gyModify = static_cast<i8*>(
        H2_ALLOC(SPELL_MODIFIER_ROW_COUNT, 2524 + SPELL_VAPORIZE_ALLOC_LINE_OFFSET)
    );
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    i32 rowCount = giMaxExtentY - giMinExtentY + 1;
    target->m_palette = gyModify;
    target->m_drawEnabled = 0;

    i32 firstY = (giMinExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    i32 lastY = (giMaxExtentY / VAPORIZE_STRIPE_WIDTH) * VAPORIZE_STRIPE_WIDTH;
    rowCount = (lastY - firstY) / VAPORIZE_STRIPE_WIDTH + 1;
    i32 phase;
    for (phase = 0; phase < VAPORIZE_PHASE_COUNT; ++phase) {
        i32 topOffset;
        i32 bottomOffset;
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
            rowCount = (rowCount - 1) / 2 + 1;
        i32 row;
        for (row = 0; row < rowCount; ++row) {
            gyModify[row * VAPORIZE_STRIPE_WIDTH + firstY + topOffset] = VAPORIZE_MASKED;
            gyModify[lastY + (row * -VAPORIZE_STRIPE_WIDTH - bottomOffset)] = VAPORIZE_MASKED;
            gbLimitToExtent = true;
            gpCombatManager->DrawFrame(1, 0, 1, 0, 30, 1, 1);
        }
    }
    DelayMilli(static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_VANISH_END_DELAY));
    target->m_palette = 0;
    target->m_drawEnabled = 1;
    H2_FREE(gyModify, 2524 + SPELL_VAPORIZE_FREE_LINE_OFFSET);
    gyModify = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

// @semantic: complete mode parameters, sine table, amplitude scaling,
// both fade masks, draw, and cleanup CFG agree. The effect predicate matches
// retail: phases outside the center band always draw, while center-band phases
// skip only five distances. Restoring the otherwise-unused initial extent height
// closed both missing extent-global targets. Retail has a 0x54 frame versus
// ours 0x58. All 68 relocations now agree after recovering the 0x0a1b
// source-line owner and its +0x2c/+0x2d/+0x8e/+0x8f allocator offsets. The
// first normalized residual is side/index multiplication order. Tried both
// wave scalings and direct/pointer army access.
VA(0x00426e4d, 0x592)
void combatManager::RippleCreature(i32 side, i32 armyIndex, i32 mode) {
    DATA(0x004f0540) static i16 rippleSourceLineBase = 2587;
    army* target = &m_armies[side][armyIndex];
    i32 phaseStep;
    i32 frameDelay;
    float amplitudeBase;
    float amplitudeStep;
    switch (mode) {
        case IDX(SPELL_RIPPLE_MODE_WAVE):
            phaseStep = 2;
            frameDelay = 20;
            amplitudeBase = RIPPLE_MODE_ZERO_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_MODE_ZERO_AMPLITUDE_STEP;
            break;
        case IDX(SPELL_RIPPLE_MODE_DEATH_RIPPLE):
            phaseStep = 1;
            frameDelay = 30;
            amplitudeBase = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
        default:
            phaseStep = 1;
            frameDelay = 30;
            amplitudeBase = RIPPLE_OTHER_AMPLITUDE_BASE;
            amplitudeStep = RIPPLE_OTHER_AMPLITUDE_STEP;
            break;
    }

    ResetLimitCreature();
    ++m_limitCreatureCount[side][armyIndex];
    if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE)
        gpCombatManager->DrawFrame(0, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    else
        gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    i32 extentHeight = giMaxExtentY - giMinExtentY + 1;
    gyModify = static_cast<i8*>(
        H2_ALLOC(SPELL_MODIFIER_ROW_COUNT, 2587 + SPELL_RIPPLE_MODIFIER_ALLOC_LINE_OFFSET)
    );
    float* wave = static_cast<float*>(H2_ALLOC(
        sizeof(float) * SPELL_MODIFIER_ROW_COUNT,
        2587 + SPELL_RIPPLE_WAVE_ALLOC_LINE_OFFSET
    ));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    i32 row;
    for (row = 0; row < SPELL_MODIFIER_ROW_COUNT; ++row) {
        wave[row] = static_cast<float>(
            (sin(static_cast<float>(row % RIPPLE_WAVE_PERIOD)
                 / static_cast<float>(RIPPLE_WAVE_DIVISOR))
             - 0.5)
            * 2.0
        );
    }
    target->m_palette = gyModify;
    target->m_drawEnabled = 0;
    giMinExtentX -= RIPPLE_MARGIN;
    giMaxExtentX += RIPPLE_MARGIN;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX >= COMBAT_SCREEN_WIDTH)
        giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
    extentHeight = giMaxExtentY - giMinExtentY + 1;

    i32 phase;
    for (phase = RIPPLE_PHASE_START; phase < RIPPLE_PHASE_END; phase += phaseStep) {
        i32 skipDistance =
            abs(RIPPLE_PHASE_CENTER - phase % RIPPLE_PHASE_PERIOD) - RIPPLE_SKIP_CENTER_OFFSET;
        i32 amplitudeIndex = (phase - RIPPLE_PHASE_START) / RIPPLE_AMPLITUDE_INDEX_DIVISOR + 1;
        if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE)
            amplitudeIndex = RIPPLE_MODE_TWO_AMPLITUDE_START - amplitudeIndex;
        else if (mode == SPELL_RIPPLE_MODE_WAVE) {
            if (amplitudeIndex == 0)
                amplitudeIndex = RIPPLE_MODE_ZERO_CENTER_AMPLITUDE;
            else
                amplitudeIndex = RIPPLE_MODE_ZERO_AMPLITUDE_START - amplitudeIndex;
        }
        if (phase <= RIPPLE_PHASE_CENTER || phase >= RIPPLE_PHASE_END - RIPPLE_SKIP_CENTER_OFFSET
            || (skipDistance != RIPPLE_SKIP_DISTANCE_0 && skipDistance != RIPPLE_SKIP_DISTANCE_1
                && skipDistance != RIPPLE_SKIP_DISTANCE_2 && skipDistance != RIPPLE_SKIP_DISTANCE_3
                && skipDistance != RIPPLE_SKIP_DISTANCE_4)) {
            float amplitude = (amplitudeIndex * amplitudeStep + amplitudeBase) * skipDistance;
            memset(gyModify + giMinExtentY, 0, extentHeight);
            for (row = giMinExtentY; row < giMaxExtentY; ++row) {
                i32 waveIndex;
                if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE)
                    waveIndex = -RIPPLE_PHASE_CENTER - giMaxExtentY + phase * 2 + row;
                else
                    waveIndex = phase * 2 - RIPPLE_PHASE_CENTER - row + giMinExtentY;
                waveIndex += RIPPLE_WAVE_INDEX_OFFSET;
                if (waveIndex >= 0 && waveIndex < SPELL_MODIFIER_ROW_COUNT)
                    gyModify[row] = static_cast<i8>(wave[waveIndex] * amplitude);
            }
            if (mode == SPELL_RIPPLE_MODE_DEATH_RIPPLE && phase >= RIPPLE_DEATH_RIPPLE_FADE_START) {
                i32 start = giMinExtentY - 1;
                i32 end = giMinExtentY
                          + (RIPPLE_DEATH_RIPPLE_FADE_BASE - (RIPPLE_PHASE_END - phase))
                                * extentHeight / RIPPLE_FADE_DIVISOR
                          + 1;
                memset(gyModify + start, VAPORIZE_MASKED, end - start + 1);
            }
            if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE && phase < RIPPLE_DEATH_WAVE_FADE_END) {
                i32 start = giMinExtentY - 1;
                i32 end =
                    giMaxExtentY - 1
                    - (phase - RIPPLE_DEATH_WAVE_FADE_BASE) * extentHeight / RIPPLE_FADE_DIVISOR;
                memset(gyModify + start, VAPORIZE_MASKED, end - start + 1);
            }
            gbLimitToExtent = true;
            gpCombatManager->DrawFrame(1, 0, 1, 0, frameDelay, 1, 1);
        }
    }
    DelayMilli(static_cast<i32l>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_VANISH_END_DELAY));
    target->m_palette = 0;
    target->m_drawEnabled = 1;
    H2_FREE(gyModify, 2587 + SPELL_RIPPLE_MODIFIER_FREE_LINE_OFFSET);
    H2_FREE(wave, 2587 + SPELL_RIPPLE_WAVE_FREE_LINE_OFFSET);
    gyModify = 0;
    if (mode != SPELL_RIPPLE_MODE_DEATH_RIPPLE)
        gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

// @semantic:
// complete effect loading, creature animation, death processing, and vanish
// CFG; all 23 relocation targets agree. Retail has a 0x28 frame versus ours
// 0x40. The first normalized divergence is affected[side][armyIndex] address
// multiplication order, followed by wince/death arm layout. Tried target
// pointer locals, direct indexing, and both frame-loop spellings. Revisit at
// 95% for od_slots/header stabilization; do not restore the transient live max.
VA(0x004273df, 0x6b2)
void combatManager::ShowMassSpell(i8 (*const affected)[20], i32 effect, i32 animateCreatures) {
    u32l effectFile = MAKEFILEID(gCombatFxNames[effect]);
    i32 effectFrames = giNumPowFrames[effect] - 1;
    i32 returnFrames = 0;
    if (gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFile);
        gCurLoadedSpellEffect = effect;
    }

    i32 side;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        i32 armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            army* target = &m_armies[side][armyIndex];
            if (affected[side][armyIndex] != 0)
                target->m_drawSpellEffect = 1;
            if (animateCreatures != 0 && affected[side][armyIndex] != 0
                && target->m_animationSequence != ARMY_ANIMATION_WINCE
                && target->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN) {
                if (target->m_quantity == 0) {
                    if (effectFrames
                        < target->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)])
                        effectFrames =
                            target->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)];
                    gpSoundManager->MemorySample(target->m_samples[ARMY_SAMPLE_KILL]);
                } else {
                    if (effectFrames
                        < target->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)])
                        effectFrames =
                            target->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE)];
                    if (returnFrames
                        < target->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)])
                        returnFrames = target->m_frameInfo
                                           .animationFrameCount[IDX(ARMY_ANIMATION_WINCE_RETURN)];
                    gpSoundManager->MemorySample(target->m_samples[ARMY_SAMPLE_WINCE]);
                }
            }
        }
    }

    i32 frame;
    for (frame = 0; frame < effectFrames; ++frame) {
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            i32 armyIndex;
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                army* target = &m_armies[side][armyIndex];
                target->m_spellEffectYOffset = 0;
                if (animateCreatures != 0 && affected[side][armyIndex] != 0) {
                    if (target->m_quantity == 0) {
                        if (target->m_animationSequence == ARMY_ANIMATION_DEATH) {
                            if (target->m_animationFrame + 1
                                < target->m_frameInfo
                                      .animationFrameCount[IDX(target->m_animationSequence)])
                                ++target->m_animationFrame;
                        } else {
                            target->m_animationSequence = ARMY_ANIMATION_DEATH;
                            target->m_animationFrame = 0;
                        }
                    } else if (target->m_animationSequence == ARMY_ANIMATION_WINCE) {
                        if (target->m_animationFrame + 1
                            < target->m_frameInfo
                                  .animationFrameCount[IDX(target->m_animationSequence)])
                            ++target->m_animationFrame;
                    } else {
                        target->m_animationSequence = ARMY_ANIMATION_WINCE;
                        target->m_animationFrame = 0;
                    }
                }
                if (frame + 1 < giNumPowFrames[effect])
                    gCurSpellEffectFrame = frame;
            }
        }
        DrawFrame(1, 0, 0, 0, 110, 1, 1);
    }

    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        i32 armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex)
            m_armies[side][armyIndex].m_drawSpellEffect = 0;
    }
    for (frame = 0; frame < returnFrames + 1; ++frame) {
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            i32 armyIndex;
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                army* target = &m_armies[side][armyIndex];
                if (animateCreatures != 0 && affected[side][armyIndex] != 0
                    && target->m_animationSequence != ARMY_ANIMATION_STAND) {
                    if (target->m_animationSequence == ARMY_ANIMATION_WINCE) {
                        target->m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
                        target->m_animationFrame = 0;
                    } else if (target->m_animationFrame + 1
                               < target->m_frameInfo
                                     .animationFrameCount[IDX(target->m_animationSequence)]) {
                        ++target->m_animationFrame;
                    } else if (target->m_animationSequence != ARMY_ANIMATION_DEATH) {
                        target->m_animationSequence = ARMY_ANIMATION_STAND;
                        target->m_animationFrame = 0;
                    }
                }
            }
        }
        DrawFrame(1, 0, 0, 0, 110, 1, 1);
    }

    i32 creatureDied = 0;
    memset(m_removedArmies, 0, sizeof(m_removedArmies));
    m_removedArmyPresent = 0;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        i32 armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            army* target = &m_armies[side][armyIndex];
            if (affected[side][armyIndex] != 0 && target->m_quantity == 0) {
                target->ProcessDeath(0);
                creatureDied = 1;
            }
        }
    }
    if (creatureDied)
        DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    if (m_removedArmyPresent != 0)
        MakeCreaturesVanish();
}

// @semantic: complete mass-target selection, damage, presentation,
// influence application, and final draw CFG agree with the retail 0x54 frame.
// The first normalized residual is gsSpellInfo's typed field relocation versus
// retail's interior constant label; the delink helper then stops at the switch.
// Explicit llvm-objdump range review shows every external callee/global agrees
// and only one extra local-label attribution remains. The recovered slot order,
// MASS_DISPEL-before-damage body order, positive affected wrapper, and final
// influence-call order are required. A case-scoped spelling raised fuzzy to
// 92.14% but incorrectly expanded the frame to 0x88 and was rejected. Revisit
// switch-label shaping after the pre-95 structural campaign.
VA(0x00427a91, 0x8f8)
void combatManager::CastMassSpell(i32 spell, i32 spellPower) {
    army* target_i = 0;
    u32 effect = gsSpellInfo[spell].combatEffect;
    i32 animateCreatures_k = 0;
    gpWindowManager->m_updateFlags = 0;
    ShowSpellMessage(0, spell, 0);
    i8 affected[COMBAT_SIDE_COUNT][20];
    memset(affected, 0, sizeof(affected));

    i32 side_i;
    i32 damage_c;
    i32 armyIndex_k;
    i32 anyAffected_i;
    CombatSpellInfluence influence_e;
    switch (spell) {
        case IDX(SPELL_MASS_SLOW):
        case IDX(SPELL_MASS_CURSE):
            side_i = 1 - m_currentSide;
            for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                if (m_armies[side_i][armyIndex_k].SpellCastWorks(spell))
                    affected[side_i][armyIndex_k] = 1;
            }
            break;

        case IDX(SPELL_MASS_CURE):
        case IDX(SPELL_MASS_HASTE):
        case IDX(SPELL_MASS_BLESS):
        case IDX(SPELL_MASS_SHIELD):
            side_i = m_currentSide;
            for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                if (m_armies[side_i][armyIndex_k].SpellCastWorks(spell))
                    affected[side_i][armyIndex_k] = 1;
            }
            break;

        case IDX(SPELL_MASS_DISPEL):
            for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
                for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                    if (m_armies[side_i][armyIndex_k].SpellCastWorks(spell))
                        affected[side_i][armyIndex_k] = 1;
                }
            }
            break;

        case IDX(SPELL_HOLY_WORD):
        case IDX(SPELL_HOLY_SHOUT): {
            animateCreatures_k = 1;
            damage_c = (spell == SPELL_HOLY_WORD ? 10 : 20) * spellPower;
            for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
                for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                    if ((m_armies[side_i][armyIndex_k].m_monster.flags.bytes.attributeFlags & 4)
                            != 0
                        && m_armies[side_i][armyIndex_k].SpellCastWorks(spell))
                        affected[side_i][armyIndex_k] = 1;
                }
            }
            if (spell == SPELL_HOLY_WORD)
                Blur(0, -2, -2);
            else
                Blur(0, -4, -4);
            for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
                for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                    if (affected[side_i][armyIndex_k] != 0)
                        m_armies[side_i][armyIndex_k].Damage(damage_c, COMBAT_HEX_EMPTY);
                }
            }
            sprintf(
                gText,
                "The %s spell does %d damage\nto all undead creatures.",
                gSpellNames[spell],
                damage_c
            );
            CombatMessage(gText, 1, 1, 0);
            break;
        }

        case IDX(SPELL_DEATH_RIPPLE):
        case IDX(SPELL_DEATH_WAVE): {
            animateCreatures_k = 1;
            for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
                for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                    if ((m_armies[side_i][armyIndex_k].m_monster.flags.bytes.attributeFlags & 4)
                            == 0
                        && m_armies[side_i][armyIndex_k].SpellCastWorks(spell))
                        affected[side_i][armyIndex_k] = 1;
                }
            }
            Ripple(2 - (spell == SPELL_DEATH_RIPPLE));
            damage_c = (spell == SPELL_DEATH_RIPPLE ? 5 : 10) * spellPower;
            for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
                for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                    if (affected[side_i][armyIndex_k] != 0)
                        m_armies[side_i][armyIndex_k].Damage(damage_c, COMBAT_HEX_EMPTY);
                }
            }
            sprintf(gText, "The Death spell does %d damage\nto all living creatures.", damage_c);
            CombatMessage(gText, 1, 1, 0);
            break;
        }
    }

    if (!gbNoShowCombat) {
        anyAffected_i = 0;
        for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
            for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
                if (affected[side_i][armyIndex_k] != 0)
                    anyAffected_i = 1;
            }
        }
        if (anyAffected_i)
            ShowMassSpell(affected, effect, animateCreatures_k);
    }

    for (side_i = 0; side_i < COMBAT_SIDE_COUNT; ++side_i) {
        for (armyIndex_k = 0; armyIndex_k < m_armyCount[side_i]; ++armyIndex_k) {
            if (affected[side_i][armyIndex_k] != 0) {
                target_i = &m_armies[side_i][armyIndex_k];
                switch (spell) {
                    case IDX(SPELL_MASS_CURSE):
                        target_i->SetSpellInfluence(SPELL_INFLUENCE_CURSE, spellPower);
                        break;
                    case IDX(SPELL_MASS_SLOW):
                        target_i->SetSpellInfluence(SPELL_INFLUENCE_SLOW, spellPower);
                        break;
                    case IDX(SPELL_MASS_HASTE):
                        target_i->SetSpellInfluence(SPELL_INFLUENCE_HASTE, spellPower);
                        break;
                    case IDX(SPELL_MASS_BLESS):
                        target_i->SetSpellInfluence(SPELL_INFLUENCE_BLESS, spellPower);
                        break;
                    case IDX(SPELL_MASS_SHIELD):
                        target_i->SetSpellInfluence(SPELL_INFLUENCE_SHIELD, spellPower);
                        break;
                    case IDX(SPELL_MASS_CURE):
                        target_i->Cure(spellPower);
                        break;
                    case IDX(SPELL_MASS_DISPEL): {
                        for (influence_e = 0; influence_e < SPELL_INFLUENCE_COUNT; ++influence_e)
                            target_i->CancelIndividualSpell(influence_e);
                        break;
                    }
                    case IDX(SPELL_HOLY_WORD):
                    case IDX(SPELL_HOLY_SHOUT):
                    case IDX(SPELL_DEATH_RIPPLE):
                    case IDX(SPELL_DEATH_WAVE):
                        break;
                }
            }
        }
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->m_updateFlags = 1;
}

// @semantic: complete search, failure dialog, army creation, duration
// artifacts, links, and slide animation; the 0x44 frame and all 38 relocations
// match. Restoring the combat-speed deadline before the first slide frame closed
// four missing targets. The first residual is search-loop initialization and
// trampoline order. Tried success inline and retail failure-before-success via
// goto. The recovered 50.0f slide delay and DoBlast's 10.0f delay make SPELLS
// .rdata exact-sized; its only raw pool residual is those final two values in
// reverse `$T` order. Swapping their header declaration order did not change
// emission. Revisit at 95% for loop CFG/slots; do not repeat either body placement.
VA(0x00428389, 0x5c8)
void combatManager::MirrorImage(i32 targetHex) {
    army* source =
        &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];
    i32 mirrorHex = COMBAT_HEX_EMPTY;
    i32 distance;
    for (distance = 1; distance <= MIRROR_SEARCH_MAX_DISTANCE; ++distance) {
        i32 sourcePart;
        for (sourcePart = 0; sourcePart < 2; ++sourcePart) {
            i32 searchHex;
            if (sourcePart == 0) {
                searchHex = source->m_hex;
            } else {
                if ((source->m_monster.flags.all & ARMY_FLAG_WIDE) == 0)
                    continue;
                if (source->m_facing == ARMY_FACING_RIGHT)
                    searchHex = source->m_hex + 1;
                else
                    searchHex = source->m_hex - 1;
            }

            i32 direction;
            for (direction = 0; direction < SPELL_ADJACENT_DIRECTION_COUNT; ++direction) {
                i32 searchDirection = source->m_facing == ARMY_FACING_RIGHT
                                          ? direction
                                          : SPELL_ADJACENT_DIRECTION_COUNT - 1 - direction;
                i32 candidateHex = searchHex;
                i32 step;
                for (step = 0; step < distance; ++step) {
                    candidateHex = GetAdjacentCellIndexNoArmy(candidateHex, searchDirection);
                    if (candidateHex >= 0 && candidateHex < COMBAT_HEX_COUNT
                        && candidateHex % SPELL_HEX_COLUMN_COUNT != 0
                        && candidateHex % SPELL_HEX_COLUMN_COUNT != SPELL_HEX_RIGHT_BORDER
                        && source->CanFit(candidateHex, 0, 0)) {
                        mirrorHex = candidateHex;
                        goto mirror_found;
                    }
                }
            }
        }
    }
    sprintf(gText, "Mirror Image spell failed!");
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
        IDX(source->m_monsterType),
        source->m_quantity,
        mirrorHex,
        MIRROR_ARMY_FLAG,
        0
    );
    army* image =
        &m_armies[m_hexCells[mirrorHex].m_occupantSide][m_hexCells[mirrorHex].m_occupantIndex];
    image->m_monster.flags.abilityFlags |= MIRROR_MONSTER_ABILITY;
    i32 duration = m_spellPower[m_currentSide];
    if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        duration += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_WIZARD_HAT))
        duration += SPELL_WIZARD_HAT_POWER_BONUS;
    image->m_roundCounter = duration;
    source->m_mirrorImageIndex = image->m_index;
    image->m_mirrorSourceIndex = source->m_index;

    i32 xOffset = m_hexCells[source->m_hex].m_x - m_hexCells[image->m_hex].m_x;
    i32 yOffset = m_hexCells[source->m_hex].m_y - m_hexCells[image->m_hex].m_y;
    ResetLimitCreature();
    ++m_limitCreatureCount[image->m_side][image->m_index];
    ++m_limitCreatureCount[m_hexCells[targetHex].m_occupantSide]
                          [m_hexCells[targetHex].m_occupantIndex];
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    i32 deadline = static_cast<i32>(
        KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * MIRROR_SLIDE_FRAME_DELAY
    );
    i32 frame;
    for (frame = 0; frame < MIRROR_SLIDE_FRAME_COUNT; ++frame) {
        image->m_xOffset = (MIRROR_SLIDE_FRAME_COUNT - frame) * xOffset / MIRROR_SLIDE_FRAME_COUNT;
        image->m_yOffset = (MIRROR_SLIDE_FRAME_COUNT - frame) * yOffset / MIRROR_SLIDE_FRAME_COUNT;
        gbLimitToExtent = true;
        gpCombatManager->DrawFrame(0, 0, 0, 0, 0, 1, 0);
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        gbLimitToExtent = false;
        DelayTil(&deadline);
        deadline = static_cast<i32>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * MIRROR_SLIDE_FRAME_DELAY
        );
    }
    image->m_xOffset = 0;
    image->m_yOffset = 0;
    UpdateGrid(0, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

VA(0x00428951, 0x218)
void combatManager::SummonElemental(i32 monsterType, i32 spellPower) {
    u8 summonHexes_l[8];
    summonHexes_l[0] = 14;
    summonHexes_l[1] = 27;
    summonHexes_l[2] = 40;
    summonHexes_l[3] = 11;
    summonHexes_l[4] = 24;
    summonHexes_l[5] = 37;
    i32 randomOffset_a = SRandom(0, 2);
    u32 summonHex = static_cast<u32>(COMBAT_HEX_EMPTY);
    if (m_heroes[m_currentSide] != 0
        && m_heroes[m_currentSide]->HasArtifact(ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;
    i32 offset;
    for (offset = 0; offset < 3; ++offset) {
        if (m_hexCells[summonHexes_l[m_currentSide * 3 + ((randomOffset_a | 0) + offset) % 3]]
                .m_occupantSide
            == COMBAT_HEX_EMPTY)
            summonHex = summonHexes_l[m_currentSide * 3 + ((randomOffset_a | 0) + offset) % 3];
    }
    m_summonedCreatureType[m_currentSide] = static_cast<u8>(monsterType);
    AddArmy(
        m_currentSide,
        monsterType,
        spellPower * ELEMENTAL_SUMMON_QUANTITY_PER_POWER,
        summonHex,
        0,
        1
    );
    army* elementals =
        &m_armies[m_hexCells[summonHex].m_occupantSide][m_hexCells[summonHex].m_occupantIndex];
    elementals->m_monster.flags.abilityFlags |= MIRROR_MONSTER_ABILITY;
    spellPower = m_spellPower[m_currentSide];
    if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ENCHANTED_HOURGLASS))
        spellPower += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_WIZARD_HAT))
        spellPower += SPELL_WIZARD_HAT_POWER_BONUS;
}

VA(0x00428b69, 0x1e6)
void combatManager::DoLuck(i32 side, i32 armyIndex) {
    army* target_i = &m_armies[side][armyIndex];
    i32 targetX_k = target_i->MidX();
    i32 targetY_l = m_hexCells[target_i->m_hex].m_y;
    targetY_l -= GetIconEntry(
                     target_i->m_creatureIcon,
                     target_i->m_frameInfo
                         .animationFrames[IDX(ARMY_ANIMATION_STAND)][LUCK_TARGET_FRAME_INDEX]
                 )
                     ->h
                 + LUCK_ICON_Y_PADDING;
    if (targetY_l < LUCK_MIN_TARGET_Y)
        targetY_l = LUCK_MIN_TARGET_Y;
    i32 startY_n = LUCK_EDGE_START_Y;
    i32 startX_b;
    if (target_i->m_facing == ARMY_FACING_RIGHT) {
        if (targetX_k < 480)
            startX_b = targetX_k + targetY_l / 2 + LUCK_EDGE_INSET;
        else
            startX_b = targetX_k - LUCK_EDGE_INSET - targetY_l / 2;
    } else if (targetX_k > 200) {
        startX_b = targetX_k - LUCK_EDGE_INSET - targetY_l / 2;
    } else {
        startX_b = targetX_k + targetY_l / 2 + LUCK_EDGE_INSET;
    }
    if (targetY_l < LUCK_SHORT_BOLT_HEIGHT) {
        startX_b = side == 0 ? 0 : COMBAT_SCREEN_WIDTH - 1;
        startY_n = targetY_l + LUCK_MIN_TARGET_Y;
    }
    if (startX_b < 0)
        startX_b = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < startX_b)
        startX_b = COMBAT_SCREEN_WIDTH - 1;
    DoBolt(
        0,
        startX_b,
        startY_n,
        targetX_k,
        targetY_l,
        0,
        0,
        LUCK_BOLT_WIDTH,
        LUCK_BOLT_WIDTH,
        BOLT_COLOR_RAINBOW_REVERSE
            + ((((targetX_k | 0) >= startX_b) - 1)
               & (BOLT_COLOR_RAINBOW_FORWARD - BOLT_COLOR_RAINBOW_REVERSE)),
        LUCK_BOLT_ANGLE,
        LUCK_BOLT_ANGLE,
        LUCK_BOLT_DISTANCE,
        LUCK_BOLT_FORCE_ANGLE,
        1,
        LUCK_BOLT_FRAME_DELAY,
        0
    );
}

// @early-stop
// @early-stop-reloc-only
// All 0x33a bytes match after masking 50 relocation sites; the 0x7c frame/slots,
// CFG, and effective targets align. OD_STEER(deltaX_a) fixes the commutative squared-
// distance load order, and OD_STEER(currentX_i)/OD_STEER(currentY_d) fix both float updates.
// The retained report residual is only compiler-local strings/constants and the
// retail iLeftRightSave+0x10 identity for __adjust_fdiv. Revisit after relocation
// identity normalization or a material SPELLS source/header change.
VA(0x00428d4f, 0x33a)
void combatManager::DoBlast(i32 targetHex, i32 spell) {
    float stepY_e;
    i32 frameSpacing_c;
    army* target_i;
    i32 targetY_a;
    i32 targetX_a;
    i32 frame_j;
    i32 deltaX_a;
    i32 deltaY_a;
    float currentX_i;
    i32 distance_d;
    SLimitData limits_a;
    i32 segment_h;
    i32 startY_d;
    i32 startX_d;
    i32 deadline_j;
    float currentY_d;
    icon* blastIcon_h;
    float stepX_a;
    i32 segmentCount_f;

    if (spell == SPELL_COLD_RAY) {
        blastIcon_h = gpResourceManager->GetIcon("coldray.icn");
        frameSpacing_c = BLAST_COLD_RAY_FRAME_SPACING;
    } else {
        blastIcon_h = gpResourceManager->GetIcon("disrray.icn");
        frameSpacing_c = BLAST_DISRUPTING_RAY_FRAME_SPACING;
    }
    target_i =
        &m_armies[m_hexCells[targetHex].m_occupantSide][m_hexCells[targetHex].m_occupantIndex];
    targetX_a = target_i->MidX();
    targetY_a = target_i->MidY();
    startX_d = castX;
    startY_d = castY;
    deltaX_a = targetX_a - startX_d;
    deltaY_a = targetY_a - startY_d;
    distance_d = static_cast<i32>(
        sqrt(static_cast<double>(OD_STEER(deltaX_a) * deltaX_a + deltaY_a * deltaY_a))
    );
    segmentCount_f = distance_d / frameSpacing_c;
    currentX_i = static_cast<float>(startX_d);
    currentY_d = static_cast<float>(startY_d);
    stepX_a = static_cast<float>(deltaX_a) / segmentCount_f;
    stepY_e = static_cast<float>(deltaY_a) / segmentCount_f;
    deadline_j = 0;
    for (segment_h = 0; segmentCount_f > (segment_h | 0); ++segment_h) {
        ResetLimitCreature();
        gbComputeExtent = true;
        gbSaveBiggestExtent = true;
        currentX_i = OD_STEER(currentX_i) + stepX_a;
        currentY_d = OD_STEER(currentY_d) + stepY_e;
        frame_j = (segment_h * BLAST_FRAME_COUNT - 1) / segmentCount_f;
        blastIcon_h->CombatClipDrawToBuffer(
            static_cast<i32>(currentX_i),
            static_cast<i32>(currentY_d),
            frame_j,
            &limits_a,
            stepX_a < 0.0f,
            0,
            0,
            0
        );
        if (giMinExtentX < 0)
            giMinExtentX = 0;
        if (giMinExtentY < 0)
            giMinExtentY = 0;
        if (COMBAT_SCREEN_WIDTH - 1 < giMaxExtentX)
            giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
        if (COMBAT_AREA_HEIGHT - 1 < giMaxExtentY)
            giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
        DelayTil(&deadline_j);
        deadline_j = static_cast<i32>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * BLAST_FRAME_DELAY
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

// @semantic: quantity/artifact handling, both dead-hex removals, message,
// reverse-death animation, and cleanup agree; all 33 ordered relocation targets
// match. Bucket-4 and bucket-10 unused words restore retail's -0x18/-0x34 gaps
// and exact 0x44 frame. The first executable residual is corpse address
// evaluation: retail forms index + 0x61*hex while ours forms 0x62*hex + index.
// Reversed subscripts and a materialized resurrected pointer were exhausted.
VA(0x00429089, 0x655)
void combatManager::Resurrect(i32 spell, i32 targetHex, i32 spellPower) {
    i32 armyIndex_f;
    army* target_i;
    i32 processedOtherHex_p;
    i32 oldQuantity_b;
    i32 otherHex_i;
    i32 deadIndex_e;
    i32 keepSearching_m;
    i32 deadHex_k;
    i32 index_o;
    i32 effectX_p;
    i32 effectY_j;
    icon* resurrectIcon;
    i32 unusedResurrectWord3;
    i32 unusedResurrectWord9;

    if (m_heroes[m_currentSide] != 0 && m_heroes[m_currentSide]->HasArtifact(ARTIFACT_ANKH))
        spellPower <<= 1;
    armyIndex_f = FindResurrectArmyIndex(m_currentSide, spell, targetHex);
    target_i = &m_armies[m_currentSide][armyIndex_f];
    processedOtherHex_p = 0;
    oldQuantity_b = target_i->m_quantity;
    target_i->m_quantity += spellPower * RESURRECT_HIT_POINTS_PER_POWER
                            / static_cast<u16>(target_i->m_monster.hitPoints);
    if (target_i->m_initialQuantity < target_i->m_quantity)
        target_i->m_quantity = target_i->m_initialQuantity;
    if (spell == SPELL_RESURRECT)
        target_i->m_temporaryResurrectionQuantity += target_i->m_quantity - oldQuantity_b;

    if (oldQuantity_b <= 0) {
        otherHex_i = COMBAT_HEX_EMPTY;
        deadIndex_e = COMBAT_HEX_EMPTY;
        keepSearching_m = 1;
        deadHex_k = targetHex;
        while (keepSearching_m) {
            for (index_o = 0; index_o < m_hexCells[deadHex_k].m_deadOccupantCount; ++index_o) {
                if (m_hexCells[deadHex_k].m_deadOccupantSides[index_o] == m_currentSide
                    && m_hexCells[deadHex_k].m_deadOccupantIndices[index_o] == armyIndex_f) {
                    deadIndex_e = index_o;
                    if (!processedOtherHex_p) {
                        if (m_hexCells[deadHex_k].m_deadOccupantFrames[index_o] == 1)
                            otherHex_i = deadHex_k - 1;
                        else if (m_hexCells[deadHex_k].m_deadOccupantFrames[index_o] == 0)
                            otherHex_i = deadHex_k + 1;
                    }
                }
                if (deadIndex_e != COMBAT_HEX_EMPTY) {
                    m_hexCells[deadHex_k].m_occupantSide =
                        m_hexCells[deadHex_k].m_deadOccupantSides[index_o];
                    m_hexCells[deadHex_k].m_occupantIndex =
                        m_hexCells[deadHex_k].m_deadOccupantIndices[index_o];
                    m_hexCells[deadHex_k].m_occupantFrame =
                        m_hexCells[deadHex_k].m_deadOccupantFrames[index_o];
                    if (m_hexCells[deadHex_k].m_deadOccupantCount == index_o + 1) {
                        m_hexCells[deadHex_k].m_deadOccupantSides[index_o] = COMBAT_HEX_EMPTY;
                        m_hexCells[deadHex_k].m_deadOccupantIndices[index_o] = COMBAT_HEX_EMPTY;
                    } else {
                        m_hexCells[deadHex_k].m_deadOccupantSides[index_o] =
                            m_hexCells[deadHex_k].m_deadOccupantSides[index_o + 1];
                        m_hexCells[deadHex_k].m_deadOccupantIndices[index_o] =
                            m_hexCells[deadHex_k].m_deadOccupantIndices[index_o + 1];
                    }
                }
            }
            --m_hexCells[deadHex_k].m_deadOccupantCount;
            if (processedOtherHex_p) {
                keepSearching_m = 0;
            } else if (otherHex_i == COMBAT_HEX_EMPTY) {
                keepSearching_m = 0;
            } else {
                deadHex_k = otherHex_i;
                processedOtherHex_p = 1;
                deadIndex_e = COMBAT_HEX_EMPTY;
            }
        }
    }

    effectX_p = target_i->MidX();
    effectY_j = target_i->MidY();
    if (target_i->m_quantity - oldQuantity_b > 1)
        sprintf(
            gText,
            "%d %s rise from the dead!",
            target_i->m_quantity - oldQuantity_b,
            gArmyNamesPlural[IDX(target_i->m_monsterType)]
        );
    else
        sprintf(
            gText,
            "%d %s rises from the dead!",
            target_i->m_quantity - oldQuantity_b,
            gArmyNames[IDX(target_i->m_monsterType)]
        );
    CombatMessage(gText, 1, 1, 0);

    if (!gbNoShowCombat) {
        resurrectIcon = gpResourceManager->GetIcon("yinyang.icn");
        for (index_o = 0; index_o < RESURRECT_ANIMATION_FRAME_COUNT; ++index_o) {
            glTimers[0] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_FIZZLE_FRAME_DELAY
            );
            IconToBitmap(
                resurrectIcon,
                gpWindowManager->m_screen,
                effectX_p,
                effectY_j,
                index_o,
                1,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
            UpdateCombatArea();
            target_i->m_facing = 1 - target_i->m_side;
            if (target_i->m_animationSequence == ARMY_ANIMATION_DEATH) {
                if (index_o >= RESURRECT_DEATH_REVERSE_FRAME) {
                    target_i->m_animationSequence = ARMY_ANIMATION_STAND;
                    target_i->m_animationFrame = 0;
                } else {
                    register i32 reverseFrame =
                        target_i->m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_DEATH)] - 1;
                    if (RESURRECT_DEATH_REVERSE_FRAME - 1 - index_o <= reverseFrame)
                        reverseFrame = RESURRECT_DEATH_REVERSE_FRAME - 1 - index_o;
                    target_i->m_animationFrame = reverseFrame;
                }
            }
            DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
            DelayTil(glTimers);
        }
        gpResourceManager->Dispose(resurrectIcon);
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target_i->m_monster.flags.abilityFlags &= ~RESURRECT_MONSTER_ABILITY;
}

VA(0x004296de, 0xb9)
i32 combatManager::SpaceForElementalExists(void) {
    if ((m_currentSide == 0 && m_hexCells[14].m_occupantSide != COMBAT_HEX_EMPTY
         && m_hexCells[27].m_occupantSide != COMBAT_HEX_EMPTY
         && m_hexCells[40].m_occupantSide != COMBAT_HEX_EMPTY)
        || (m_currentSide == 1 && m_hexCells[11].m_occupantSide != COMBAT_HEX_EMPTY
            && m_hexCells[24].m_occupantSide != COMBAT_HEX_EMPTY
            && m_hexCells[37].m_occupantSide != COMBAT_HEX_EMPTY))
        return 0;
    else
        return 1;
}

// @semantic: normalized compiler-local strings, NULL_SAMPLE2 addends, all 15
// relocation sites, CFG, and semantics agree. The real residual is stack layout:
// retail stores the aggregate return temporary at -0x10/-0x0c and armyName at
// -0x14; VC4.2 uses -0x14/-0x10 and -0x0c. Semantic bucket-13/bucket-15 names,
// declaration order, nested/separate scopes, cached result assignment, and direct
// result initialization all failed to improve or added an extra aggregate copy.
// Seven raw stack-displacement bytes remain. Revisit after an earlier SPELLS edit
// or relevant SAMPLE2/header change perturbs aggregate-temporary placement.
VA(0x00429797, 0xd9)
void combatManager::ShowSpellCastFailure(army* target, i32) {
    SAMPLE2 sample = NULL_SAMPLE2;
    sample = LoadPlaySample("rsbryfzl.82m");
    char* armyName;
    if (target->m_quantity == 1)
        armyName = gArmyNames[IDX(target->m_monsterType)];
    else
        armyName = gArmyNamesPlural[IDX(target->m_monsterType)];
    sprintf(
        gText,
        "The %s %s the spell!",
        armyName,
        target->m_quantity == 1 ? "resists" : "resist"
    );
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    WaitEndSample(sample, -1);
}

VA(0x00429870, 0x270)
void combatManager::ModifyDamageForArtifacts(
    i32l* damage,
    i32 spell,
    hero* attacker,
    hero* defender
) {
    if (attacker != 0) {
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
    if (defender != 0) {
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

// @semantic: complete shake, randomized wall/tower/keep damage, cloud
// animation, state application, and input restore; the 0x160 frame and all 62
// relocation targets agree. After constant identity, the first code divergence
// is the screen-copy row loop branch (`jge` versus retail `jle`); later residuals
// are packed wallPos/towerPos local-label identities and keep impact ordering.
// Tried cached/direct offsets, both height association orders, <= zero tests,
// both loop polarities, and an 80-walk syntax-aware AST permutation pass after
// the 95% phase switch; the AST pass was byte-neutral. Revisit only for a new
// frame/layout discovery, not further generic permutation.
VA(0x00429ae0, 0x931)
void combatManager::Earthquake(void) {
    i32 shakeOffsets[30];
    shakeOffsets[0] = 2;
    shakeOffsets[1] = 2;
    shakeOffsets[2] = 4;
    shakeOffsets[3] = 1;
    shakeOffsets[4] = 3;
    shakeOffsets[5] = -2;
    shakeOffsets[6] = 0;
    shakeOffsets[7] = -6;
    shakeOffsets[8] = 2;
    shakeOffsets[9] = -2;
    shakeOffsets[10] = -1;
    shakeOffsets[11] = 3;
    shakeOffsets[12] = -5;
    shakeOffsets[13] = 4;
    shakeOffsets[14] = -8;
    shakeOffsets[15] = 6;
    shakeOffsets[16] = -4;
    shakeOffsets[17] = 2;
    shakeOffsets[18] = -1;
    shakeOffsets[19] = 1;
    shakeOffsets[20] = -3;
    shakeOffsets[21] = -3;
    shakeOffsets[22] = -7;
    shakeOffsets[23] = -5;
    shakeOffsets[24] = -5;
    shakeOffsets[25] = -7;
    shakeOffsets[26] = -2;
    shakeOffsets[27] = -3;
    shakeOffsets[28] = 0;
    shakeOffsets[29] = 0;

    gpMouseManager->HideColorPointer();
    memcpy(
        m_backgroundBuffer->m_pixels,
        gpWindowManager->m_screen->m_pixels,
        COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT
    );
    i32 pass;
    for (pass = 0; pass < EARTHQUAKE_SHAKE_PASS_COUNT; ++pass) {
        i32 frame;
        for (frame = 0; frame < EARTHQUAKE_SHAKE_FRAME_COUNT; ++frame) {
            i32 deadline = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_ARMAGEDDON_SHAKE_DELAY
            );
            PollSound();
            i32 width =
                COMBAT_SCREEN_WIDTH
                - abs(
                    shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_X)]
                );
            i32 height =
                COMBAT_AREA_HEIGHT
                - abs(shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_Y)])
                - 1;
            u8* source =
                m_backgroundBuffer->m_pixels
                + (shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_Y)] > 0
                       ? shakeOffsets
                             [frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_Y)]
                       : 0)
                      * COMBAT_SCREEN_WIDTH
                + (shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_X)] > 0
                       ? shakeOffsets
                             [frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_X)]
                       : 0);
            i32 destinationX =
                shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_X)] <= 0
                    ? -shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_X)]
                    : 0;
            i32 destinationY =
                shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_Y)] <= 0
                    ? -shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT + IDX(COMBAT_COORDINATE_Y)]
                    : 0;
            u8* destination = gpWindowManager->m_screen->m_pixels
                              + destinationY * COMBAT_SCREEN_WIDTH + destinationX;
            i32 row;
            for (row = 0; height > row; ++row) {
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
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    i32 newWallStates[EARTHQUAKE_STRUCTURE_COUNT];
    i32 newTowerStates[EARTHQUAKE_STRUCTURE_COUNT];
    i32 impactPositions[EARTHQUAKE_MAX_IMPACTS][2];
    i32 impactCount = 0;
    i32 structure;
    for (structure = 0; structure < EARTHQUAKE_STRUCTURE_COUNT; ++structure) {
        newWallStates[structure] = m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST];
        if (m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] != IDX(COMBAT_WALL_STATE_DESTROYED)
            && m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST]
                   != IDX(COMBAT_WALL_STATE_SECTION_DESTROYED)
            && SRandom(0, 100) < EARTHQUAKE_WALL_HIT_CHANCE) {
            ++newWallStates[structure];
            if (m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST]
                    != IDX(COMBAT_WALL_STATE_DESTROYED)
                && m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST]
                       != IDX(COMBAT_WALL_STATE_TOWER_STANDING)
                && m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST]
                       != IDX(COMBAT_WALL_STATE_SECTION_DESTROYED)
                && m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST]
                       != IDX(COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                && SRandom(0, 100) < EARTHQUAKE_WALL_SECOND_HIT_CHANCE)
                ++newWallStates[structure];
            impactPositions[impactCount][IDX(COMBAT_COORDINATE_X)] =
                wallPos[structure][IDX(COMBAT_COORDINATE_X)];
            impactPositions[impactCount][IDX(COMBAT_COORDINATE_Y)] =
                wallPos[structure][IDX(COMBAT_COORDINATE_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
            if (newWallStates[structure] == COMBAT_WALL_STATE_DESTROYED
                || newWallStates[structure] == COMBAT_WALL_STATE_SECTION_DESTROYED)
                m_hexCells[iWallToHexCell[structure]].m_blocked = 0;
        }

        newTowerStates[structure] = m_wallStates[structure];
        if (m_wallStates[structure] != IDX(COMBAT_WALL_STATE_DESTROYED)
            && SRandom(0, 100) < EARTHQUAKE_TOWER_HIT_CHANCE) {
            newTowerStates[structure] = COMBAT_WALL_STATE_DESTROYED;
            impactPositions[impactCount][IDX(COMBAT_COORDINATE_X)] =
                towerPos[structure][IDX(COMBAT_COORDINATE_X)];
            impactPositions[impactCount][IDX(COMBAT_COORDINATE_Y)] =
                towerPos[structure][IDX(COMBAT_COORDINATE_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
        }
    }

    i32 newKeepState = m_drawbridgeState;
    if (m_drawbridgeState != 3 && SRandom(0, 100) < EARTHQUAKE_KEEP_HIT_CHANCE) {
        newKeepState = 3;
        impactPositions[impactCount][IDX(COMBAT_COORDINATE_X)] =
            towerPos[0][IDX(COMBAT_COORDINATE_X)];
        impactPositions[impactCount][IDX(COMBAT_COORDINATE_Y)] =
            towerPos[0][IDX(COMBAT_COORDINATE_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        impactPositions[impactCount + 1][IDX(COMBAT_COORDINATE_X)] =
            towerPos[1][IDX(COMBAT_COORDINATE_X)];
        impactPositions[impactCount + 1][IDX(COMBAT_COORDINATE_Y)] =
            towerPos[1][IDX(COMBAT_COORDINATE_Y)] + EARTHQUAKE_CLOUD_Y_OFFSET;
        impactCount += 2;
    }

    i32 impactDelay[EARTHQUAKE_MAX_IMPACTS];
    for (structure = 0; structure < EARTHQUAKE_MAX_IMPACTS; ++structure)
        impactDelay[structure] = Random(0, 2);
    giMinExtentX = EARTHQUAKE_EXTENT_MIN_X;
    giMinExtentY = 0;
    giMaxExtentX = EARTHQUAKE_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
    if (impactCount != 0) {
        icon* cloudIcon = gpResourceManager->GetIcon("lichclod.icn");
        i32 frame;
        for (frame = 0; frame < EARTHQUAKE_CLOUD_FRAME_COUNT; ++frame) {
            glTimers[1] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_FIZZLE_FRAME_DELAY
            );
            DrawFrame(0, 0, 1, 0, 0, 1, 0);
            i32 impact;
            for (impact = 0; impact < impactCount; ++impact) {
                i32 cloudFrame = frame - impactDelay[impact];
                if (impactDelay[impact] <= frame
                    && cloudFrame < EARTHQUAKE_CLOUD_VISIBLE_FRAME_COUNT) {
                    IconToBitmap(
                        cloudIcon,
                        gpWindowManager->m_screen,
                        impactPositions[impact][IDX(COMBAT_COORDINATE_X)],
                        impactPositions[impact][IDX(COMBAT_COORDINATE_Y)],
                        cloudFrame,
                        1,
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
            if (frame == EARTHQUAKE_APPLY_DAMAGE_FRAME) {
                for (structure = 0; structure < EARTHQUAKE_STRUCTURE_COUNT; ++structure) {
                    m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] =
                        static_cast<u8>(newWallStates[structure]);
                    m_wallStates[structure] = static_cast<u8>(newTowerStates[structure]);
                }
                m_drawbridgeState = newKeepState;
            }
        }
        gpResourceManager->Dispose(cloudIcon);
    }
    gpMouseManager->ShowColorPointer();
}

VA(0x0042a411, 0x2b1)
void combatManager::ShowSpellMessage(i32 castByCreature, i32 spell, army* target) {
    char targetName_b[60];
    char message_m[400];
    i32 unhandledSpell_j;
    char* name_k;
    if (target != 0) {
        if (target->m_quantity > 1)
            name_k = gArmyNamesPlural[IDX(target->m_monsterType)];
        else
            name_k = gArmyNames[IDX(target->m_monsterType)];
        sprintf(targetName_b, name_k);
    }
    if (castByCreature != 0) {
        if (spell == SPELL_PARALYZE)
            sprintf(message_m, "The %s are paralyzed by the Cyclopes!", targetName_b);
        else if (spell == SPELL_BLIND)
            sprintf(message_m, "The Unicorns' attack blinds the %s!", targetName_b);
        else if (spell == CREATURE_SPELL_PETRIFY)
            sprintf(message_m, "The Medusas' gaze turns the %s to stone!", targetName_b);
        else if (spell == SPELL_CURSE)
            sprintf(message_m, "The Mummies' curse falls upon the %s!", targetName_b);
        else if (spell == CREATURE_SPELL_DISPEL)
            sprintf(message_m, "The Archmages dispel all good spells\non your %s!", targetName_b);
        else {
            unhandledSpell_j = 0;
            ++unhandledSpell_j;
        }
    } else {
        if (target != 0) {
            if (m_heroes[m_currentSide]->m_isCaptain != 0)
                sprintf(
                    message_m,
                    "The captain casts '%s' on the %s.",
                    gSpellNames[spell],
                    targetName_b
                );
            else
                sprintf(
                    message_m,
                    "%s casts '%s' on the %s.",
                    m_heroes[m_currentSide]->m_name,
                    gSpellNames[spell],
                    targetName_b
                );
        } else {
            if (m_heroes[m_currentSide]->m_isCaptain != 0)
                sprintf(message_m, "The captain casts '%s'.", gSpellNames[spell]);
            else
                sprintf(
                    message_m,
                    "%s casts '%s'.",
                    m_heroes[m_currentSide]->m_name,
                    gSpellNames[spell]
                );
        }
    }
    CombatMessage(message_m, 1, 1, 0);
}

// @data-layout-note
// Fresh VC 4.2 storage contains 104 real definitions: 71 initialized owners
// in a 0x7af-byte .data section and 33 constant-pool owners in a 0xb4-byte
// .rdata section. Retail contributes 0x7b0 bytes at 0xf00b0..0xf0860 and
// 0xb8 bytes at 0xeb150..0xeb208. All 104 logical extents are disjoint and
// byte-exact. The only uncovered data bytes are alignment at 0xf04c5..0xf04c8,
// 0xf04d7, 0xf04e6..0xf04e8, 0xf0542..0xf0544, and the 0xf085f tail byte;
// rdata has four bytes of tail alignment. Candidate section SHA-256 values are
// f1d571be911cbefa4a7e71a321c94eb2066288bf93972fed8b6ff79f2a196b85
// for data and b34aa88d87447a6f3ecc02cc0ec32e87b57466a46817b252b16af407ca052173
// for rdata.
//
// The 62 compiler-local data owners each have one zero-addend code reference,
// and all 62 pair to exact retail targets. Sixty use equal-count function
// sequences. CastMassSpell has one later candidate-only jump-table relocation;
// before it, $SG37516 is bracketed by gSpellNames/gText at candidate
// +0x3fb and retail +0x3f9 -> 0xf05f4, while $SG37532 is bracketed by gText
// sites at candidate +0x5d6 and retail +0x5d0 -> 0xf062c.
//
// The rdata bijection uses 65 exact sites across 19 referenced owners, then
// payload/extent compatibility uniquely forces the other 14; removing any
// chosen edge prevents a perfect matching. Candidate pooling still combines
// two byte-identical uses that retail splits: $T38049 maps to its unique pool
// slot at 0xeb184 while ChainLightning refers to the equal value at 0xeb160,
// and $T38127 maps to 0xeb200 while RippleCreature refers to the equal value at
// 0xeb190. These are reference-pooling walls, not aliases or missing storage.
//
// Retail disassembly also proved the function-local source-line owners at
// 0xf04e4 and 0xf0540 and all six offset uses. Exact owner payload comparison
// recovered twelve incorrect user-visible literals, including allocator-path
// line records, embedded damage-message newlines, mass-spell target text, and
// Archmage target substitution. Do not restore the old strings or replace the
// proven topology with synthetic aliases, padding owners, or cursor replay.
// ---- globals (definitions, RVA order) ----
DATA(0x004f00b0) i32 castX = 0;
DATA(0x004f00b4) i32 castY = 0;
DATA(0x004f00b8) i32 bInTeleportGetDest = 0;
DATA(0x004f0278) i32 indexToCastOn = -1;
DATA(0x004f04c0) u8 uRedBeam[] = {0x77, 0xb5, 0xbc, 0xc0, 0xc3};
DATA(0x004f04c8) u8 uRainbow[] =
    {0xbc, 0xba, 0xce, 0xca, 0xc8, 0x6f, 0x71, 0x73, 0xde, 0x5a, 0x9f, 0x47, 0x4a, 0x8d, 0x8f};
DATA(0x004f04e0) i8* gyModify = 0;
