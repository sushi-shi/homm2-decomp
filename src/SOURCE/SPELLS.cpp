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
VA(0x004204c0, 0x86)
int combatManager::HasValidSpellTarget(int spell)
{
    int hex;

    for (hex = 0; hex < COMBAT_HEX_COUNT; ++hex) {
        if (hex % SPELL_HEX_COLUMN_COUNT == 0 ||
            hex % SPELL_HEX_COLUMN_COUNT == SPELL_HEX_RIGHT_BORDER)
            continue;
        if (ValidSpellTarget(spell, hex))
            return 1;
    }
    return 0;
}

// @match-note 93.37%: semantics and CFG agree, including the selected-spell
// positive arm, cleanup path, and retail body order (earthquake, elementals,
// mass spells, mirror/default). The 0x0c frame has elementalType at -0x4,
// this at -0x8, the switch temporary at -0x0c, and the unused argument at +0x8;
// there are no other locals. First residual is the delinked switch dispatch after
// +0x27; manual range audit found no mismatched external call/global target. Tried
// early return, cleanup goto, positive wrapper, and semantic/body ordering. Revisit
// only after total SOURCE fuzzy reaches 95%, or earlier if later same-TU structural
// work changes this function.
VA(0x00420546, 0x44a)
int combatManager::ViewSpells(int)
{
    int elementalType;

    m_selectedSpell =
        gpGame->ViewSpells(m_heroes[giCurGeneral], 0, CombatSpecialHandler, 0);
    if (m_selectedSpell != SPELL_NO_SELECTION) {
        switch (m_selectedSpell) {
    case SPELL_EARTHQUAKE:
        if (m_combatTowns[COMBAT_DEFENDER_SIDE] == 0) {
            NormalDialog("An earthquake will do you no good in this battle.",
                         NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            goto restore_pointer;
        }
        break;

    case SPELL_SUMMON_EARTH_ELEMENTAL:
        elementalType = SPELL_MONSTER_EARTH_ELEMENTAL;
        goto check_elemental;
    case SPELL_SUMMON_AIR_ELEMENTAL:
        elementalType = SPELL_MONSTER_AIR_ELEMENTAL;
        goto check_elemental;
    case SPELL_SUMMON_FIRE_ELEMENTAL:
        elementalType = SPELL_MONSTER_FIRE_ELEMENTAL;
        goto check_elemental;
    case SPELL_SUMMON_WATER_ELEMENTAL:
        elementalType = SPELL_MONSTER_WATER_ELEMENTAL;
check_elemental:
        if (m_unknown351D[m_currentSide] != 0 &&
            m_unknown351D[m_currentSide] != elementalType) {
            NormalDialog("You may only summon one type of elemental per combat.",
                         NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        if (m_armyCount[m_currentSide] >= SPELL_ELEMENTAL_ARMY_LIMIT) {
            sprintf(gText,
                    "You already have %d creatures in your army.  You cannot summon more.",
                    m_armyCount[m_currentSide]);
            NormalDialog(gText, NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        if (!SpaceForElementalExists()) {
            sprintf(gText,
                    "There is no open space adjacent to your hero to summon an elemental to.");
            NormalDialog(gText, NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        break;

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
            NormalDialog("That spell will affect no one!", NORMAL_DIALOG_INFO,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        break;

    case SPELL_MIRROR_IMAGE:
        if (m_armyCount[m_currentSide] >= SPELL_ELEMENTAL_ARMY_LIMIT) {
            sprintf(gText,
                    "You already have %d creatures in your army.  You cannot create a mirror image.",
                    m_armyCount[m_currentSide]);
            NormalDialog(gText, NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        // fall through
    default:
        if (!HasValidSpellTarget(m_selectedSpell)) {
            NormalDialog("That spell will affect no one!", NORMAL_DIALOG_INFO,
                         NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_RESOURCE, 0,
                         NORMAL_DIALOG_NO_VALUE, 0);
            return 0;
        }
        giNextAction = SPELL_ACTION_CAST;
        giNextActionExtra = m_selectedSpell;
        gpMouseManager->SetPointer("spelmous.mse",
                                   gsSpellInfo[m_selectedSpell].iconIndex,
                                   SPELL_POINTER_DEFAULT_ID);
        gpWindowManager->DoDialog(0, HandleCastSpell, 0);
        goto restore_pointer;
        }

        giNextAction = SPELL_ACTION_CAST;
        giNextActionExtra = m_selectedSpell;
    }

restore_pointer:
    gpMouseManager->SetPointer("cmbtmous.mse", 0, SPELL_POINTER_DEFAULT_ID);
    return m_selectedSpell != SPELL_NO_SELECTION;
}

// @match-note 91.43%: semantics and CFG agree, including hover conversion/cache,
// common return, and retail case order. There are no source locals: the message
// reference arrives in ECX and is stored at -0x4; the only other 0x08-frame slot
// is the implicit switch temporary at -0x8. All external calls/globals are
// recovered. First residual is after the switch dispatch at +0x31, where delinked
// help-pointer/table identities truncate the normal diff. Tried early-return and
// retail positive-arm forms. Revisit only after total SOURCE fuzzy reaches 95%,
// or earlier if later same-TU structural work changes this function.
VA(0x00420990, 0x15c)
int CombatSpecialHandler(tag_message &message)
{
    if (message.type == SPELL_MESSAGE_HOVER) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
            return SPELL_HANDLER_CONTINUE;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;

        switch (message.payload.hover.id) {
        case SPELL_CONTROL_PREVIOUS_PAGE:
            gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_PREVIOUS_PAGE],
                                           1, 0, 0);
            break;
        case SPELL_CONTROL_NEXT_PAGE:
            gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_NEXT_PAGE], 1,
                                           0, 0);
            break;
        case SPELL_CONTROL_CLOSE:
            gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_CLOSE], 1, 0,
                                           0);
            break;
        case SPELL_CONTROL_FIRST_MANA:
        case SPELL_CONTROL_FIRST_MANA + 1:
        case SPELL_CONTROL_FIRST_MANA + 2:
        case SPELL_CONTROL_LAST_MANA:
            gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_MANA], 1, 0,
                                           0);
            break;
        default:
            gpCombatManager->CombatMessage(cSpellHelp[SPELL_HELP_DEFAULT], 1, 0,
                                           0);
            break;
        }
    }
    return SPELL_HANDLER_CONTINUE;
}

// @match-note 92.25%: semantics and CFG agree, including the two-stage teleport,
// recursive hover, close/cancel paths, and all message mutations. The message
// reference arrives in ECX and is stored at -0x8; the 0x0c frame also has hex at
// -0x4 and the implicit switch temporary at -0x0c, with no other locals. External
// relocation targets agree. First residual is the switch dispatch after +0x09;
// explicit ranges confirm retail body order is hover, select, then mouse-down/cancel.
// That order and the opposite order were tried. Revisit only after total SOURCE
// fuzzy reaches 95%, or earlier if later same-TU structural work changes this function.
VA(0x00420aec, 0x2aa)
int HandleCastSpell(tag_message &message)
{
    int hex;

    switch (message.type) {
    case SPELL_MESSAGE_HOVER:
        hex = gpCombatManager->GetGridIndex(message.payload.mouse.x, message.payload.mouse.y);
        if (indexToCastOn != hex) {
            if (!gpCombatManager->ValidSpellTarget(
                    gpCombatManager->m_selectedSpell, hex)) {
                indexToCastOn = SPELL_NO_SELECTION;
                gpMouseManager->SetPointer(0);
                if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT &&
                    bInTeleportGetDest) {
                    gpCombatManager->CombatMessage(
                        "Invalid Teleport Destination.", 1, 0, 0);
                } else {
                    gpCombatManager->CombatMessage("Select Spell Target.", 1, 0,
                                                   0);
                }
            } else {
                indexToCastOn = hex;
                gpMouseManager->SetPointer(
                    gsSpellInfo[gpCombatManager->m_selectedSpell].iconIndex);
                gpCombatManager->SpellMessage(
                    gpCombatManager->m_selectedSpell, hex);
            }
        }
        break;

    case SPELL_MESSAGE_SELECT:
        if (indexToCastOn != SPELL_NO_SELECTION) {
            if (!bInTeleportGetDest) {
                giNextActionGridIndex = indexToCastOn;
                if (gpCombatManager->m_selectedSpell == SPELL_TELEPORT) {
                    bInTeleportGetDest = 1;
                    indexToCastOn = SPELL_NO_SELECTION;
                    message.type = SPELL_MESSAGE_HOVER;
                    message.payload.mouse.x = message.payload.mouse.screenX;
                    message.payload.mouse.y = message.payload.mouse.screenY;
                    HandleCastSpell(message);
                    gpCombatManager->CombatMessage(
                        "Select teleport destination.", 1, 0, 0);
                    return SPELL_HANDLER_CONTINUE;
                }
            } else {
                giNextActionGridIndex2 = indexToCastOn;
            }
            bInTeleportGetDest = 0;
            message.type = SPELL_MESSAGE_DIALOG;
            message.payload.widget.command = SPELL_COMMAND_CLOSE;
            return SPELL_HANDLER_CLOSE;
        }
        break;

    case SPELL_MESSAGE_MOUSE_DOWN:
        if (message.payload.keyboard.keyCode == SPELL_COMMAND_CANCEL)
            goto cancel_spell;
        break;

    case SPELL_MESSAGE_CANCEL:
cancel_spell:
        gpCombatManager->m_selectedSpell = SPELL_NO_SELECTION;
        giNextAction = 0;
        message.type = SPELL_MESSAGE_DIALOG;
        message.payload.widget.command = SPELL_COMMAND_CLOSE;
        bInTeleportGetDest = 0;
        return SPELL_HANDLER_CLOSE;
    }
    return SPELL_HANDLER_CONTINUE;
}

// @match-note 91.40%: semantics and CFG agree, including the live-first arm and
// descending corpse scan. The 0x0c frame has target_j at -0x4, corpse at -0x8,
// and this at -0x0c; arguments are side +0x8, spell +0x0c, and hex +0x10, with no
// other locals. Both SpellCastWorkChance relocations match. First code residual is
// the corpse/hex index formation near +0x124: retail forms corpse + 0x61*hex,
// ours forms 0x62*hex + corpse. Tried empty-first do/while, live-first for, direct
// army expressions, an army pointer local, and slot suffixes. Revisit only after
// total SOURCE fuzzy reaches 95%, or earlier if later same-TU structural work
// changes this function.
VA(0x00420d96, 0x2e5)
int combatManager::FindResurrectArmyIndex(int side, int spell, int hex)
{
    army *target_j;
    int corpse;

    if (m_hexCells[hex].m_occupantSide != COMBAT_HEX_EMPTY) {
        if (m_hexCells[hex].m_occupantSide == side) {
            target_j = &m_armies[m_hexCells[hex].m_occupantSide]
                                  [m_hexCells[hex].m_occupantIndex];
            if (target_j->SpellCastWorkChance(spell) > 0.0f)
                return m_hexCells[hex].m_occupantIndex;
        }
        return SPELL_NO_SELECTION;
    }

    for (corpse = m_hexCells[hex].m_deadOccupantCount - 1; corpse >= 0;
         --corpse) {
        if (m_hexCells[hex].m_deadOccupantSides[corpse] !=
                COMBAT_HEX_EMPTY &&
            (m_hexCells[hex].m_deadOccupantFrames[corpse] !=
                 ARMY_FACING_LEFT ||
             m_hexCells[hex + 1].m_occupantSide == COMBAT_HEX_EMPTY) &&
            (m_hexCells[hex].m_deadOccupantFrames[corpse] !=
                 ARMY_FACING_RIGHT ||
             m_hexCells[hex - 1].m_occupantSide == COMBAT_HEX_EMPTY) &&
            m_hexCells[hex].m_deadOccupantSides[corpse] == side) {
            target_j =
                &m_armies[m_hexCells[hex].m_deadOccupantSides[corpse]]
                           [m_hexCells[hex].m_deadOccupantIndices[corpse]];
            if (target_j->SpellCastWorkChance(spell) > 0.0f)
                return m_hexCells[hex].m_deadOccupantIndices[corpse];
        }
    }
    return SPELL_NO_SELECTION;
}

// @match-note 92.04%: semantics and CFG agree, including target filtering, every
// case body, retail case-body order, and singleton teleport-origin access. The
// 0x14 frame has target at -0x8, the implicit teleport hex temporary at -0x0c,
// this at -0x10, the switch temporary at -0x14, and an unused word at -0x4;
// arguments are spell +0x8 and hex +0x0c, with no other source locals. External
// relocs agree. First residual is the equivalent 0.0 constant-pool identity at
// +0x57, followed by the delinked switch boundary at +0x60. Tried a teleport
// pointer local/direct expression, this/global origin access, and both case orders.
// Revisit only after total SOURCE fuzzy reaches 95%, or earlier if later same-TU
// structural work changes this function.
VA(0x0042107b, 0x521)
int combatManager::ValidSpellTarget(int spell, int hex)
{
    army *target = 0;
    if (!ValidHex(hex))
        return 0;

    if (spell != SPELL_FIREBALL && spell != SPELL_FIREBLAST &&
        spell != SPELL_METEOR_SHOWER && spell != SPELL_COLD_RING &&
        spell != SPELL_RESURRECT && spell != SPELL_TRUE_RESURRECT &&
        spell != SPELL_ANIMATE_DEAD &&
        m_hexCells[hex].m_occupantSide != COMBAT_HEX_EMPTY) {
        target = &m_armies[m_hexCells[hex].m_occupantSide]
                            [m_hexCells[hex].m_occupantIndex];
        if (target->m_spellInfluence[SPELL_INFLUENCE_ANTI_MAGIC] != 0 ||
            target->m_monsterType == SPELL_MONSTER_GREEN_DRAGON)
            return 0;
    }

    if (target != 0 && target->SpellCastWorkChance(spell) <= 0.0f)
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
        return FindResurrectArmyIndex(m_currentSide, spell, hex) !=
               SPELL_NO_SELECTION;

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
        if (m_armies[m_hexCells[hex].m_occupantSide]
                     [m_hexCells[hex].m_occupantIndex]
                         .m_mirrorImageIndex != SPELL_NO_SELECTION ||
            m_armies[m_hexCells[hex].m_occupantSide]
                     [m_hexCells[hex].m_occupantIndex]
                         .m_mirrorSourceIndex != SPELL_NO_SELECTION)
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
            if (hex == giNextActionGridIndex ||
                !gpCombatManager
                     ->m_armies[gpCombatManager
                                    ->m_hexCells[giNextActionGridIndex]
                                    .m_occupantSide]
                               [gpCombatManager
                                    ->m_hexCells[giNextActionGridIndex]
                                    .m_occupantIndex]
                     .CanFit(hex, 0, 0))
                return 0;
        }
        break;

    case SPELL_FIREBALL:
    case SPELL_FIREBLAST:
    case SPELL_METEOR_SHOWER:
    case SPELL_COLD_RING:
        if (hex == COMBAT_HEX_EMPTY || hex % SPELL_HEX_COLUMN_COUNT == 0 ||
            hex % SPELL_HEX_COLUMN_COUNT == SPELL_HEX_RIGHT_BORDER)
            return 0;
        break;
    }
    return 1;
}

// @match-note 50.99%: semantics and CFG agree, including area, teleport,
// resurrection/default targeting, and shared formatting. The 0x10 frame has
// target_i at -0x4, armyName at -0x8, this at -0x0c, and the implicit switch
// temporary at -0x10; arguments are spell +0x8 and hex +0x0c, with no other
// locals. All external relocations agree. First residual is an extra five-byte
// switch thunk between the area and teleport bodies (+0x4b ours); retail places
// the equivalent default-target thunk after its table. Tried both label orders,
// explicit occupied goto, repeated expressions, pointer locals, and MSVC
// slot-compatible suffixes. Revisit only after total SOURCE fuzzy reaches 95%,
// or earlier if later same-TU structural work changes this function.
VA(0x0042159c, 0x222)
void combatManager::SpellMessage(int spell, int hex)
{
    army *target_i;
    char *armyName;

    if (gbNoShowCombat)
        return;

    switch (spell) {
    case SPELL_FIREBALL:
    case SPELL_FIREBLAST:
    case SPELL_METEOR_SHOWER:
    case SPELL_COLD_RING:
        sprintf(gText, "Cast %s", gSpellNames[spell]);
        goto show_message;
    case SPELL_TELEPORT:
        if (bInTeleportGetDest) {
            sprintf(gText, "Teleport Here");
            goto show_message;
        }
        goto occupied_target;
    case SPELL_RESURRECT:
    case SPELL_TRUE_RESURRECT:
    case SPELL_ANIMATE_DEAD:
        target_i =
            &m_armies[m_currentSide]
                       [FindResurrectArmyIndex(m_currentSide, spell, hex)];
        break;
    default:
occupied_target:
        target_i = &m_armies[m_hexCells[hex].m_occupantSide]
                              [m_hexCells[hex].m_occupantIndex];
        break;
    }
    if (target_i->m_quantity == 1)
        armyName = gArmyNames[target_i->m_monsterType];
    else
        armyName = gArmyNamesPlural[target_i->m_monsterType];
    sprintf(gText, "Cast %s on %s", gSpellNames[spell], armyName);

show_message:
    CombatMessage(gText, 1, 0, 0);
}

VA(0x004217be, 0x1eca)
void combatManager::CastSpell(int spell, int targetHex, int castByCreature, int teleportDestination)
{
    long damage_m;
    int spellPower_i;
    army *target_i;
    int targetIndex_k;
    int soundSpell_q;
    int armyIndex;
    char sampleName_i[16];
    SAMPLE2 spellSample;
    int targetY_b;
    int targetX_b;
    int side_i;
    int targetSide_i;
    int influence;
    int adjacentHex_q;
    int oldDefense;
    army *teleportArmy_i;
    icon *missileIcon_p;
    char *lightningArmyName_d;
    char *magicArrowArmyName_n;
    char *coldRayArmyName_f;
    float missileAngles[9];

    if (castByCreature == 0 &&
        m_eagleEyeSpell[1 - m_currentSide] == -1 &&
        m_heroes[1 - m_currentSide] != 0 &&
        !m_heroes[1 - m_currentSide]->HasSpell(spell) &&
        m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE] != HERO_SKILL_LEVEL_NONE &&
        m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE] + 1 >=
            gsSpellInfo[spell].level) {
        if (SRandom(0, 9) <=
            m_heroes[1 - m_currentSide]->m_secondarySkills[HERO_SKILL_EAGLE_EYE]) {
            m_eagleEyeSpell[1 - m_currentSide] = static_cast<short>(spell);
        }
    }

    spellSample = GetNullSample();
    if (m_limitCreature != 0) {
        ResetLimitCreature();
        if (ValidHex(m_limitCreatureHex) && m_hexCells[m_limitCreatureHex].m_occupantSide >= 0) {
            m_limitCreatureCount[m_hexCells[m_limitCreatureHex].m_occupantSide]
                                [m_hexCells[m_limitCreatureHex].m_occupantIndex] =
                m_limitCreatureCount[m_hexCells[m_limitCreatureHex].m_occupantSide]
                                    [m_hexCells[m_limitCreatureHex].m_occupantIndex] + 1;
        }
        m_limitCreature = 0;
        m_limitCreatureHex = COMBAT_HEX_EMPTY;
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    }

    if (castByCreature == 0 && m_heroes[m_currentSide] != 0)
        m_heroes[m_currentSide]->UseSpell(spell);

    target_i = 0;
    if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST || spell == SPELL_COLD_RING ||
        spell == SPELL_METEOR_SHOWER || spell == SPELL_SUMMON_EARTH_ELEMENTAL ||
        spell == SPELL_SUMMON_AIR_ELEMENTAL || spell == SPELL_SUMMON_WATER_ELEMENTAL ||
        spell == SPELL_SUMMON_FIRE_ELEMENTAL || spell == SPELL_MASS_BLESS ||
        spell == SPELL_MASS_HASTE || spell == SPELL_EARTHQUAKE || spell == SPELL_MASS_CURSE ||
        spell == SPELL_MASS_CURE || spell == SPELL_HOLY_WORD || spell == SPELL_HOLY_SHOUT ||
        spell == SPELL_DEATH_RIPPLE || spell == SPELL_DEATH_WAVE || spell == SPELL_MASS_SHIELD ||
        spell == SPELL_ARMAGEDDON || spell == SPELL_ELEMENTAL_STORM || spell == SPELL_MASS_DISPEL) {
        target_i = 0;
    } else if (ValidHex(targetHex) && m_hexCells[targetHex].m_occupantSide >= 0) {
        target_i = &m_armies[m_hexCells[targetHex].m_occupantSide]
                            [m_hexCells[targetHex].m_occupantIndex];
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
        if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_ENCHANTED_HOURGLASS) &&
            (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_DURATION)) {
            spellPower_i += SPELL_HOURGLASS_POWER_BONUS;
        }
        if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_WIZARD_HAT) &&
            (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_DURATION)) {
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
            if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST ||
                spell == SPELL_COLD_RING || spell == SPELL_METEOR_SHOWER) {
                targetX_b = m_hexCells[targetHex].m_x;
                targetY_b = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
            }
        }

        if (targetX_b != -1) {
            if (m_currentSide == 0) {
                castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0] + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0] + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
            } else {
                castX = COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[0];
                castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[0] + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
            }
            if ((m_currentSide == 0 ? 1 : -1) * (targetX_b - castX) < targetY_b - castY) {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_HIGH;
                if (m_currentSide == 0) {
                    castX = sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1] + COMBAT_HERO_CAST_LEFT_X_OFFSET;
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1] + COMBAT_HERO_CAST_LEFT_Y_OFFSET;
                } else {
                    castX = COMBAT_HERO_CAST_RIGHT_X - sCmbtHero[m_heroSpriteIndex[m_currentSide]].x[1];
                    castY = sCmbtHero[m_heroSpriteIndex[m_currentSide]].y[1] + COMBAT_HERO_CAST_RIGHT_Y_OFFSET;
                }
            } else {
                m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_LOW;
            }
        } else {
            m_heroAnimationState[m_currentSide] = COMBAT_HERO_CAST_NO_TARGET;
        }
        m_heroAnimationFrame[m_currentSide] = 0;
        while (m_heroAnimationFrame[m_currentSide] <
               sCmbtHero[m_heroSpriteIndex[m_currentSide]]
                   .animationFrameCount[m_heroAnimationState[m_currentSide]]) {
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            m_heroAnimationFrame[m_currentSide]++;
        }
        m_heroAnimationFrame[m_currentSide]--;
    }

    soundSpell_q = spell;
    if (spell == SPELL_PETRIFY)
        soundSpell_q = SPELL_PARALYZE;
    if (spell == SPELL_CREATURE_DISPEL)
        soundSpell_q = SPELL_DISPEL;
    if (strlen(gsSpellInfo[soundSpell_q].soundName) != 0)
        sprintf(sampleName_i, "%s.82M", gsSpellInfo[soundSpell_q].soundName);

    if (castByCreature == 0 && target_i != 0 && !target_i->SpellCastWorks(spell)) {
        ShowSpellCastFailure(target_i, spell);
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
                    adjacentHex_q = teleportArmy_i->GetAdjacentCellIndex(teleportDestination, 1);
                    if (adjacentHex_q == COMBAT_HEX_EMPTY ||
                        (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY &&
                         (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i ||
                          m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k)) ||
                        m_hexCells[adjacentHex_q].m_blocked != 0) {
                        targetHex = teleportDestination - 1;
                    }
                }
                if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                    adjacentHex_q = teleportArmy_i->GetAdjacentCellIndex(adjacentHex_q, 4);
                    if (adjacentHex_q == COMBAT_HEX_EMPTY ||
                        (m_hexCells[adjacentHex_q].m_occupantSide != COMBAT_HEX_EMPTY &&
                         (m_hexCells[adjacentHex_q].m_occupantSide != targetSide_i ||
                          m_hexCells[adjacentHex_q].m_occupantIndex != targetIndex_k)) ||
                        m_hexCells[adjacentHex_q].m_blocked != 0) {
                        targetHex++;
                    }
                }
                teleportArmy_i->m_hex = targetHex;
                if (teleportArmy_i->m_facing == ARMY_FACING_LEFT) {
                    m_hexCells[targetHex].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_RIGHT;
                    m_hexCells[targetHex - 1].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex - 1].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex - 1].m_occupantFrame = ARMY_FACING_LEFT;
                } else if (teleportArmy_i->m_facing == ARMY_FACING_RIGHT) {
                    m_hexCells[targetHex].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex].m_occupantFrame = ARMY_FACING_LEFT;
                    m_hexCells[targetHex + 1].m_occupantSide = static_cast<signed char>(targetSide_i);
                    m_hexCells[targetHex + 1].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                    m_hexCells[targetHex + 1].m_occupantFrame = ARMY_FACING_RIGHT;
                }
                RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
            } else {
                teleportArmy_i->m_hex = teleportDestination;
                m_hexCells[teleportArmy_i->m_hex].m_occupantSide = static_cast<signed char>(targetSide_i);
                m_hexCells[teleportArmy_i->m_hex].m_occupantIndex = static_cast<signed char>(targetIndex_k);
                m_hexCells[teleportArmy_i->m_hex].m_occupantFrame = COMBAT_HEX_EMPTY;
                RippleCreature(teleportArmy_i->m_side, teleportArmy_i->m_index, 2);
            }
            break;
        case SPELL_DISRUPTING_RAY:
            oldDefense = target_i->m_monster.defense;
            target_i->m_monster.defense -= SPELL_DISRUPTING_RAY_DEFENSE_REDUCTION;
            if (target_i->m_monster.defense < SPELL_MINIMUM_DEFENSE)
                target_i->m_monster.defense = SPELL_MINIMUM_DEFENSE;
            sprintf(gText, "The disrupting ray reduces defense by %d.",
                    oldDefense - target_i->m_monster.defense);
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            RippleCreature(target_i->m_side, target_i->m_index, 0);
            break;
        case SPELL_COLD_RAY:
            DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_COLD_RAY_DELAY));
            damage_m = spellPower_i * SPELL_COLD_RAY_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_FIRE_ELEMENTAL)
                damage_m = spellPower_i * SPELL_COLD_RAY_FIRE_ELEMENTAL_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                target_i->m_monsterType == SPELL_MONSTER_STEEL_GOLEM)
                damage_m = static_cast<long>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            coldRayArmyName_f = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                     : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The cold ray does %d damage to the %s.", damage_m, coldRayArmyName_f);
            CombatMessage(gText, 1, 1, 0);
            DoBlast(targetHex, spell);
            target_i->SpellEffect(gsSpellInfo[SPELL_COLD_RAY].combatEffect, 0, 0);
            target_i->Damage(damage_m, -1);
            target_i->PowEffect(-1, 1, -1, -1);
            break;
        case SPELL_CHAIN_LIGHTNING:
            ChainLightning(targetHex, spellPower_i);
            break;
        case SPELL_MAGIC_ARROW:
            DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] * SPELL_MAGIC_ARROW_DELAY));
            damage_m = spellPower_i * SPELL_MAGIC_ARROW_DAMAGE_PER_POWER;
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            magicArrowArmyName_n = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                        : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The magic arrow does %d damage to the %s.", damage_m,
                    magicArrowArmyName_n);
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
            ShootMissile(castX, castY, target_i->MidX(), target_i->MidY(), missileAngles, missileIcon_p);
            gpResourceManager->Dispose(missileIcon_p);
            target_i->Damage(damage_m, -1);
            target_i->PowEffect(-1, 1, -1, -1);
            break;
        case SPELL_LIGHTNING_BOLT:
            damage_m = spellPower_i * SPELL_LIGHTNING_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_AIR_ELEMENTAL)
                damage_m = spellPower_i * SPELL_LIGHTNING_AIR_ELEMENTAL_DAMAGE_PER_POWER;
            if (target_i->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                target_i->m_monsterType == SPELL_MONSTER_STEEL_GOLEM)
                damage_m = static_cast<long>(damage_m * SPELL_GOLEM_DAMAGE_MULTIPLIER);
            ModifyDamageForArtifacts(&damage_m, spell, m_heroes[m_currentSide],
                                     m_heroes[1 - m_currentSide]);
            lightningArmyName_d = target_i->m_quantity < 2 ? gArmyNames[target_i->m_monsterType]
                                                       : gArmyNamesPlural[target_i->m_monsterType];
            sprintf(gText, "The lightning bolt does %d damage to the %s.", damage_m,
                    lightningArmyName_d);
            CombatMessage(gText, 1, 1, 0);
            DoBolt(1, castX, castY, target_i->MidX(), target_i->MidY(), 150, 100, 9, 2, 301,
                   -40, 40, 30, 1, 0, 0, 1);
            target_i->SpellEffect(gsSpellInfo[SPELL_LIGHTNING_BOLT].combatEffect, 0, 0);
            target_i->Damage(damage_m, -1);
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
            CastMassSpell(spell, spellPower_i);
            break;
        case SPELL_MIRROR_IMAGE:
            MirrorImage(targetHex);
            break;
        case SPELL_SUMMON_EARTH_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_EARTH_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_AIR_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_AIR_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_FIRE_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_FIRE_ELEMENTAL, spellPower_i);
            break;
        case SPELL_SUMMON_WATER_ELEMENTAL:
            SummonElemental(SPELL_MONSTER_WATER_ELEMENTAL, spellPower_i);
            break;
        case SPELL_RESURRECT:
        case SPELL_TRUE_RESURRECT:
        case SPELL_ANIMATE_DEAD:
            Resurrect(spell, targetHex, spellPower_i);
            break;
        case SPELL_CURE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_CURE].combatEffect, 0, 0);
            target_i->Cure(spellPower_i);
            DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            break;
        case SPELL_SLOW:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_SLOW, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_SLOW].combatEffect, 0, 0);
            break;
        case SPELL_HASTE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_HASTE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_HASTE].combatEffect, 0, 0);
            break;
        case SPELL_SHIELD:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_SHIELD, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_SHIELD].combatEffect, 0, 0);
            break;
        case SPELL_DRAGON_SLAYER:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_DRAGON_SLAYER, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_DRAGON_SLAYER].combatEffect, 0, 0);
            break;
        case SPELL_BLESS:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLESS, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BLESS].combatEffect, 0, 0);
            break;
        case SPELL_STONE_SKIN:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_STONE_SKIN, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_STONE_SKIN].combatEffect, 0, 0);
            break;
        case SPELL_STEEL_SKIN:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_STEEL_SKIN, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_STEEL_SKIN].combatEffect, 0, 0);
            break;
        case SPELL_CURSE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_CURSE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_CURSE].combatEffect, 0, 0);
            break;
        case SPELL_BERSERKER:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BERSERKER, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BERSERKER].combatEffect, 0, 0);
            break;
        case SPELL_HYPNOTIZE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_HYPNOTIZE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_HYPNOTIZE].combatEffect, 0, 0);
            break;
        case SPELL_PARALYZE:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_PARALYZE, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_PARALYZE].combatEffect, 0, 0);
            break;
        case SPELL_CREATURE_DISPEL:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->DispelGood();
            target_i->SpellEffect(gsSpellInfo[SPELL_DISPEL].combatEffect, 0, 1);
            break;
        case SPELL_DISPEL:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->DispelGood();
            target_i->SpellEffect(gsSpellInfo[SPELL_DISPEL].combatEffect, 0, 0);
            for (influence = 0; influence < SPELL_INFLUENCE_COUNT; influence++)
                target_i->CancelIndividualSpell(influence);
            break;
        case SPELL_BLIND:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLIND, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_BLIND].combatEffect, 0, 0);
            break;
        case SPELL_BLOOD_LUST:
            ShowSpellMessage(castByCreature, spell, target_i);
            BloodLustEffect(target_i, SPELL_BLOOD_LUST_EFFECT);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_BLOOD_LUST, SPELL_BLOOD_LUST_DURATION);
            break;
        case SPELL_ANTI_MAGIC:
            ShowSpellMessage(castByCreature, spell, target_i);
            target_i->SetSpellInfluence(SPELL_INFLUENCE_ANTI_MAGIC, spellPower_i);
            target_i->SpellEffect(gsSpellInfo[SPELL_ANTI_MAGIC].combatEffect, 0, 0);
            break;
        case SPELL_PETRIFY:
            ShowSpellMessage(castByCreature, spell, target_i);
            TurnToStone(target_i);
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
    }

    for (side_i = 0; side_i < COMBAT_SIDE_COUNT; side_i++) {
        for (armyIndex = 0; armyIndex < m_armyCount[side_i]; armyIndex++) {
            army &combatArmy = m_armies[side_i][armyIndex];
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
        while (m_heroAnimationFrame[m_currentSide] <
               sCmbtHero[m_heroSpriteIndex[m_currentSide]]
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

// @match-note retained 99.86%, combined live 91.17%: semantics, 0x08 frame,
// CFG, and all 3 relocation
// targets agree. After masking the relocation union, the only raw residuals
// are branch-displacement bytes at +0x18 and +0x40; the normalized stream then
// differs only in the typed gsSpellInfo relocation versus retail's interior
// combatEffect label. Tried a combined guard, nested positive guard, empty
// negative arm, and a shared empty invalid/negative arm. Revisit at 95% or if
// earlier SPELLS work changes TU branch layout.
VA(0x00423688, 0xda)
void combatManager::DefaultSpell(int targetHex)
{
    if (ValidHex(targetHex)) {
        if (m_hexCells[targetHex].m_occupantSide < 0) {
        } else {
            army *target =
                &m_armies[m_hexCells[targetHex].m_occupantSide]
                         [m_hexCells[targetHex].m_occupantIndex];
            target->SpellEffect(gsSpellInfo[m_selectedSpell].combatEffect, 0,
                                1);
        }
    }
}

// @match-note retained 99.71%, combined live 99.42%: complete animation,
// 19-cell selection, duplicate-effect
// filtering, elemental/golem damage, message, and PowEffect CFG. All 51
// relocation targets agree and the normalized instruction stream is otherwise
// aligned. Retail uses a 0x5c frame while this semantic-name layout uses 0x60;
// the relocation-union raw audit retains 123 stack/branch bytes. Tried cached
// versus direct hex fields, removing side/index temporaries, typed short
// storage, and in-place damage shifts. Revisit at 95% for an od_slots semantic
// suffix pass, or earlier after predecessor/header changes.
VA(0x00423762, 0x623)
void combatManager::Fireball(int targetHex, int spell)
{
    if (!ValidHex(targetHex))
        return;

    int targetX = m_hexCells[targetHex].m_x;
    int targetY = m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET;
    if (!gbNoShowCombat) {
        int frameCount = SPELL_FIREBALL_FRAME_COUNT;
        icon *spellIcon;
        if (spell == SPELL_FIREBALL)
            spellIcon = gpResourceManager->GetIcon("fireball.icn");
        else if (spell == SPELL_FIREBLAST)
            spellIcon = gpResourceManager->GetIcon("firebal2.icn");
        else {
            spellIcon = gpResourceManager->GetIcon("coldring.icn");
            frameCount = SPELL_COLD_RING_FRAME_COUNT;
        }

        int frame;
        for (frame = 0; frame < frameCount; ++frame) {
            glTimers[0] = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    SPELL_AREA_ANIMATION_DELAY);
            IconToBitmap(spellIcon, gpWindowManager->m_screen, targetX,
                         targetY, frame, 1, 0, 0, COMBAT_SCREEN_WIDTH,
                         COMBAT_AREA_HEIGHT, 0);
            if (spell == SPELL_COLD_RING) {
                FlipIconToBitmap(spellIcon, gpWindowManager->m_screen,
                                 targetX, targetY, frame, 1, 0, 0,
                                 COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
            }
            UpdateCombatArea();
            DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
            DelayTil(&glTimers[0]);
        }
        gpResourceManager->Dispose(spellIcon);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    army *target = &m_armies[m_currentSide][m_currentArmyIndex];
    short affectedHexes[SPELL_FIREBALL_AFFECTED_HEX_COUNT];
    int frame;
    for (frame = 0; frame < SPELL_FIREBALL_AFFECTED_HEX_COUNT; ++frame)
        affectedHexes[frame] = COMBAT_HEX_EMPTY;
    if (spell != SPELL_COLD_RING)
        affectedHexes[0] = static_cast<short>(targetHex);

    for (frame = 0; frame < SPELL_ADJACENT_DIRECTION_COUNT; ++frame) {
        affectedHexes[frame + 1] = static_cast<short>(
            GetAdjacentCellIndexNoArmy(targetHex, frame));
        if (spell == SPELL_FIREBLAST) {
            affectedHexes[frame + SPELL_FIREBLAST_SECOND_RING_FIRST] =
                static_cast<short>(
                target->GetAdjacentCellIndex(affectedHexes[frame + 1], frame));
        }
    }
    if (spell == SPELL_FIREBLAST) {
        affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] = static_cast<short>(
            targetHex - SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] < 0)
            affectedHexes[SPELL_FIREBLAST_AXIAL_FIRST] = COMBAT_HEX_EMPTY;
        affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] = static_cast<short>(
            targetHex + SPELL_FIREBLAST_HEX_ROW_STRIDE);
        if (affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] >= COMBAT_HEX_COUNT)
            affectedHexes[SPELL_FIREBLAST_AXIAL_SECOND] = COMBAT_HEX_EMPTY;
        affectedHexes[SPELL_FIREBLAST_CORNER_FIRST] = static_cast<short>(
            GetAdjacentCellIndexNoArmy(affectedHexes[2], 0));
        affectedHexes[SPELL_FIREBLAST_CORNER_FIRST + 1] = static_cast<short>(
            GetAdjacentCellIndexNoArmy(affectedHexes[2], 2));
        affectedHexes[SPELL_FIREBLAST_CORNER_FIRST + 2] = static_cast<short>(
            GetAdjacentCellIndexNoArmy(affectedHexes[5], 5));
        affectedHexes[SPELL_FIREBLAST_CORNER_FIRST + 3] = static_cast<short>(
            GetAdjacentCellIndexNoArmy(affectedHexes[5], 3));
    }

    long baseDamage =
        m_spellPower[m_currentSide] * SPELL_FIREBALL_DAMAGE_PER_POWER;
    ClearEffects();
    int anyAffected = 0;
    int affectedCount = SPELL_FIREBALL_AFFECTED_HEX_COUNT;
    for (frame = 0; frame < affectedCount; ++frame) {
        if (affectedHexes[frame] != COMBAT_HEX_EMPTY &&
            m_hexCells[affectedHexes[frame]].m_occupantSide !=
                COMBAT_HEX_EMPTY) {
            target =
                &m_armies[m_hexCells[affectedHexes[frame]].m_occupantSide]
                         [m_hexCells[affectedHexes[frame]].m_occupantIndex];
            if (target->SpellCastWorks(spell) &&
                !gArmyEffected
                    [m_hexCells[affectedHexes[frame]].m_occupantSide]
                    [m_hexCells[affectedHexes[frame]].m_occupantIndex]) {
                gArmyEffected
                    [m_hexCells[affectedHexes[frame]].m_occupantSide]
                    [m_hexCells[affectedHexes[frame]].m_occupantIndex] = 1;
                if (target->m_damagePending == 0) {
                    long damage = baseDamage;
                    if (spell == SPELL_COLD_RING &&
                        target->m_monsterType == SPELL_MONSTER_FIRE_ELEMENTAL)
                        damage <<= 1;
                    if ((spell == SPELL_FIREBALL ||
                         spell == SPELL_FIREBLAST) &&
                        target->m_monsterType == SPELL_MONSTER_WATER_ELEMENTAL)
                        damage *= 2;
                    if (target->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                        target->m_monsterType == SPELL_MONSTER_STEEL_GOLEM) {
                        damage = static_cast<long>(
                            damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                    }
                    target->Damage(damage, spell);
                    anyAffected = 1;
                }
            }
        }
    }
    if (anyAffected) {
        ModifyDamageForArtifacts(&baseDamage, spell, m_heroes[m_currentSide],
                                 m_heroes[1 - m_currentSide]);
        if (spell == SPELL_COLD_RING)
            sprintf(gText, "The cold ring does %d damage.", baseDamage);
        else
            sprintf(gText, "The fireball does %d damage.", baseDamage);
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(-1, 1, -1, -1);
    }
}

// @match-note retained 99.88%, combined live 96.91%: semantics, 0x40 frame
// size, animation/damage CFG, and all
// 30 relocation targets agree. The normalized instruction stream differs only
// by delinked string/config/constant identities, but the relocation-union raw
// audit retains 33 stack-slot and dependent branch bytes: retail places target,
// frame, base damage, affected flag, and damage at -0x8/-0x10/-0x4/-0x30/-0x34;
// current semantic names place them at -0x2c/-0x34/-0x8/-0x4/-0x38. Tried both
// multidimensional and row-plus-index army addressing plus in-place doubling.
// Revisit at 95% for the od_slots suffix pass or after predecessor changes.
VA(0x00423d85, 0x3c9)
void combatManager::MeteorShower(int targetHex)
{
    if (!ValidHex(targetHex))
        return;

    army *target = &m_armies[m_currentSide][0] + m_currentArmyIndex;
    int affectedHexes[SPELL_METEOR_AFFECTED_HEX_COUNT];
    affectedHexes[0] = targetHex;
    int direction;
    for (direction = 0; direction < SPELL_ADJACENT_DIRECTION_COUNT;
         ++direction) {
        affectedHexes[direction + 1] =
            GetAdjacentCellIndexNoArmy(targetHex, direction);
    }

    if (!gbNoShowCombat) {
        icon *meteorIcon = gpResourceManager->GetIcon("meteor.icn");
        for (direction = 0; direction < SPELL_METEOR_PASS_COUNT;
             ++direction) {
            int frame;
            for (frame = 0; frame < SPELL_METEOR_FRAME_COUNT; ++frame) {
                glTimers[0] = static_cast<int>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                        SPELL_METEOR_ANIMATION_DELAY);
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                meteorIcon->CombatClipDrawToBuffer(
                    m_hexCells[targetHex].m_x,
                    m_hexCells[targetHex].m_y - COMBAT_SPELL_TARGET_Y_OFFSET,
                    frame, 0, 0, 0, 0, 0);
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(meteorIcon);
    }

    int baseDamage =
        m_spellPower[m_currentSide] * SPELL_METEOR_DAMAGE_PER_POWER;
    ClearEffects();
    int anyAffected = 0;
    for (direction = 0; direction < SPELL_METEOR_AFFECTED_HEX_COUNT;
         ++direction) {
        if (affectedHexes[direction] != COMBAT_HEX_EMPTY &&
            m_hexCells[affectedHexes[direction]].m_occupantSide !=
                COMBAT_HEX_EMPTY) {
            target =
                &m_armies[m_hexCells[affectedHexes[direction]].m_occupantSide]
                         [m_hexCells[affectedHexes[direction]].m_occupantIndex];
            if (target->SpellCastWorks(SPELL_METEOR_SHOWER) &&
                !gArmyEffected
                    [m_hexCells[affectedHexes[direction]].m_occupantSide]
                    [m_hexCells[affectedHexes[direction]].m_occupantIndex]) {
                gArmyEffected
                    [m_hexCells[affectedHexes[direction]].m_occupantSide]
                    [m_hexCells[affectedHexes[direction]].m_occupantIndex] = 1;
                if (target->m_damagePending == 0) {
                    long damage = baseDamage;
                    if (target->m_monsterType ==
                        SPELL_MONSTER_EARTH_ELEMENTAL)
                        damage <<= 1;
                    target->Damage(damage, SPELL_METEOR_SHOWER);
                    anyAffected = 1;
                }
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, "The meteor shower does %d damage.", baseDamage);
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(-1, 1, -1, -1);
    }
}

// @match-note retained 99.75%, combined live 96.80%: complete tiled animation,
// global
// damage, artifact/golem handling, message, and PowEffect CFG. The 0x4c frame
// size and all 31 relocation targets agree. Retail slots side/index/target/
// damage at -0x24/-0x8/-0xc/-0x28; current semantic names use
// -0xc/-0x38/-0x10/-0x3c, with the first structural divergence at retail
// +0x1bc in the army-address evaluation. Tried direct indexing, row-plus-index,
// reversed pointer addition, uninitialized target, and in-place doubling.
// Revisit at 95% for slot suffixing; do not repeat these spellings beforehand.
VA(0x0042414e, 0x2fb)
void combatManager::ElementalStorm(void)
{
    if (!gbNoShowCombat) {
        SLimitData limits;
        icon *stormIcon = gpResourceManager->GetIcon("storm.icn");
        int pass;
        for (pass = 0; pass < SPELL_STORM_PASS_COUNT; ++pass) {
            int frame;
            for (frame = 0; frame < SPELL_STORM_FRAME_COUNT; ++frame) {
                glTimers[0] = static_cast<int>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                        SPELL_AREA_ANIMATION_DELAY);
                DrawFrame(0, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
                int row;
                for (row = 0; row < SPELL_STORM_ROW_COUNT; ++row) {
                    int column;
                    for (column = 0; column < SPELL_STORM_COLUMN_COUNT;
                         ++column) {
                        stormIcon->CombatClipDrawToBuffer(
                            column * SPELL_STORM_TILE_SIZE,
                            row * SPELL_STORM_TILE_SIZE,
                            (column * SPELL_STORM_FRAME_COLUMN_STEP + frame +
                             row) %
                                SPELL_STORM_FRAME_COUNT,
                            &limits, 0, 0, 0, 0);
                    }
                }
                UpdateCombatArea();
                DelayTil(&glTimers[0]);
            }
        }
        gpResourceManager->Dispose(stormIcon);
    }

    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    int anyAffected = 0;
    int baseDamage =
        m_spellPower[m_currentSide] * SPELL_ELEMENTAL_STORM_DAMAGE_PER_POWER;
    army *target;
    int side;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        int armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            target = armyIndex + m_armies[side];
            if (target->SpellCastWorks(SPELL_ELEMENTAL_STORM)) {
                long damage = baseDamage;
                if (target->m_monsterType == SPELL_MONSTER_AIR_ELEMENTAL)
                    damage <<= 1;
                if (target->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                    target->m_monsterType == SPELL_MONSTER_STEEL_GOLEM) {
                    damage = static_cast<long>(
                        damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side] && m_heroes[side]->HasArtifact(
                                          SPELL_ARTIFACT_BROACH_SHIELDING)) {
                    damage = static_cast<long>(
                        damage * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                target->Damage(damage, SPELL_ELEMENTAL_STORM);
                anyAffected = 1;
            }
        }
    }
    if (anyAffected) {
        sprintf(gText, "The elemental storm does %d damage.", baseDamage);
        CombatMessage(gText, 1, 1, 0);
        target->PowEffect(-1, 1, -1, -1);
    }
}

// @match-note retained 98.53%, combined live 98.38%: complete damage, palette
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
void combatManager::Armageddon(void)
{
    int baseDamage2 =
        m_spellPower[m_currentSide] * SPELL_ARMAGEDDON_DAMAGE_PER_POWER;
    int anyAffected6 = 0;
    army *target18;
    int side11;
    for (side11 = 0; side11 < COMBAT_SIDE_COUNT; ++side11) {
        int armyIndex3;
        for (armyIndex3 = 0; armyIndex3 < m_armyCount[side11];
             ++armyIndex3) {
            target18 = armyIndex3 + m_armies[side11];
            if (target18->SpellCastWorks(SPELL_ARMAGEDDON)) {
                long damage9 = baseDamage2;
                if (target18->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
                    target18->m_monsterType == SPELL_MONSTER_STEEL_GOLEM) {
                    damage9 = static_cast<long>(
                        damage9 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
                }
                if (m_heroes[side11] && m_heroes[side11]->HasArtifact(
                                          SPELL_ARTIFACT_BROACH_SHIELDING)) {
                    damage9 = static_cast<long>(
                        damage9 * SPELL_GOLEM_DAMAGE_MULTIPLIER);
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

    palette *originalPalette5 = 0;
    palette *effectPalette4 = 0;
    if (!gbNoShowCombat) {
        gpWindowManager->m_updateFlags = 0;
        originalPalette5 = gpResourceManager->GetPalette("kb.pal");
        effectPalette4 = new palette;
        if (!effectPalette4)
            MemError();
        memcpy(effectPalette4->Data(), originalPalette5->Data(),
               SPELL_ARMAGEDDON_PALETTE_SIZE);

        glTimers[0] = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                SPELL_AREA_ANIMATION_DELAY);
        signed char *effectData = effectPalette4->Data();
        int pass;
        for (pass = 0; pass < SPELL_ARMAGEDDON_BRIGHTEN_PASS_COUNT;
             ++pass) {
            int color;
            for (color = 0; color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT;
                 ++color) {
                if (effectData[color *
                                   SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT] <
                    SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color *
                                     SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT];
                if (effectData[color *
                                       SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                   1] <
                    SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color *
                                         SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                     1];
                if (effectData[color *
                                       SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                   2] <
                    SPELL_ARMAGEDDON_CHANNEL_MAX)
                    ++effectData[color *
                                         SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                     2];
            }
            DelayTil(&glTimers[0]);
            SetPalette(effectData, 1);
            glTimers[0] = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    SPELL_ARMAGEDDON_PALETTE_DELAY);
        }

        int shakeOffsets[SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT][2] = {
            {2, 2},   {4, 1},   {3, -2},  {0, -6},  {2, -2},
            {-1, 3},  {-5, 4},  {-8, 6},  {-4, 2},  {-1, 1},
            {-3, -3}, {-7, -5}, {-5, -7}, {-2, -3}, {0, 0}
        };
        gpMouseManager->HideColorPointer();
        memcpy(m_backgroundBuffer->m_pixels, gpWindowManager->m_screen->m_pixels,
               COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT);

        for (pass = 0; pass < SPELL_ARMAGEDDON_SHAKE_PASS_COUNT; ++pass) {
            float scale = static_cast<float>(
                (SPELL_ARMAGEDDON_SHAKE_PASS_COUNT - pass) *
                SPELL_ARMAGEDDON_SHAKE_SCALE);
            int frame;
            for (frame = 0; frame < SPELL_ARMAGEDDON_SHAKE_FRAME_COUNT;
                 ++frame) {
                if (pass == 0) {
                    int color;
                    for (color = 0;
                         color < SPELL_ARMAGEDDON_PALETTE_COLOR_COUNT;
                         ++color) {
                        if (effectData
                                [color *
                                     SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                 1] >
                            SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData
                                [color *
                                     SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                 1] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                        if (effectData
                                [color *
                                     SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                 2] >
                            SPELL_ARMAGEDDON_CHANNEL_STEP)
                            effectData
                                [color *
                                     SPELL_ARMAGEDDON_PALETTE_CHANNEL_COUNT +
                                 2] -=
                                SPELL_ARMAGEDDON_CHANNEL_STEP;
                    }
                    SetPalette(effectData, 1);
                }

                int offsetX = static_cast<int>(shakeOffsets[frame][0] * scale);
                int offsetY = static_cast<int>(shakeOffsets[frame][1] * scale);
                int timer = static_cast<int>(
                    KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                        SPELL_ARMAGEDDON_SHAKE_DELAY);
                PollSound();
                int copyWidth = COMBAT_SCREEN_WIDTH -
                                abs(static_cast<int>(
                                    shakeOffsets[frame][0] * scale));
                int copyHeight = COMBAT_AREA_HEIGHT -
                                 abs(static_cast<int>(
                                     shakeOffsets[frame][1] * scale)) -
                                 1;
                unsigned char *source =
                    m_backgroundBuffer->m_pixels +
                    (offsetY > 0 ? offsetY : 0) * COMBAT_SCREEN_WIDTH +
                    (offsetX > 0 ? offsetX : 0);
                unsigned char *destination =
                    gpWindowManager->m_screen->m_pixels +
                    (offsetY > 0 ? 0 : -offsetY) * COMBAT_SCREEN_WIDTH +
                    (offsetX > 0 ? 0 : -offsetX);
                int row;
                for (row = 0; row < copyHeight; ++row) {
                    memcpy(destination, source, copyWidth);
                    source += COMBAT_SCREEN_WIDTH;
                    destination += COMBAT_SCREEN_WIDTH;
                }
                BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                                   COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT,
                                   0, 0);
                DelayTil(&timer);
            }
        }
    }

    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_DRAW_DELAY, 1, 1);
    target18->PowEffect(-1, 1, -1, -1);

    signed char *effectDataRestore7 = effectPalette4->Data();
    signed char *originalData18 = originalPalette5->Data();
    int restorePass9;
    for (restorePass9 = 0;
         restorePass9 < SPELL_ARMAGEDDON_RESTORE_PASS_COUNT; ++restorePass9) {
        int component5;
        for (component5 = 0; component5 < SPELL_ARMAGEDDON_PALETTE_SIZE;
             ++component5) {
            if (static_cast<unsigned char>(effectDataRestore7[component5]) ==
                static_cast<unsigned char>(originalData18[component5])) {
            } else {
                if (static_cast<unsigned char>(originalData18[component5]) <
                    static_cast<unsigned char>(effectDataRestore7[component5])) {
                    if (static_cast<unsigned char>(originalData18[component5]) <
                        static_cast<unsigned char>(effectDataRestore7[component5]) -
                            SPELL_ARMAGEDDON_CHANNEL_STEP)
                        effectDataRestore7[component5] =
                            static_cast<unsigned char>(
                                effectDataRestore7[component5]) -
                            SPELL_ARMAGEDDON_CHANNEL_STEP;
                    else
                        effectDataRestore7[component5] =
                            originalData18[component5];
                } else if (static_cast<unsigned char>(
                                   effectDataRestore7[component5]) +
                                   SPELL_ARMAGEDDON_CHANNEL_STEP <
                           static_cast<unsigned char>(
                               originalData18[component5])) {
                    effectDataRestore7[component5] =
                        static_cast<unsigned char>(
                            effectDataRestore7[component5]) +
                        SPELL_ARMAGEDDON_CHANNEL_STEP;
                } else {
                    effectDataRestore7[component5] =
                        originalData18[component5];
                }
            }
        }
        SetPalette(effectDataRestore7, 1);
        DelayMilli(static_cast<long>(
            gfCombatSpeedMod[gConfig.combatSpeed] *
            SPELL_ARMAGEDDON_PALETTE_DELAY));
    }
    SetPalette(originalPalette5->Data(), 1);
    gpWindowManager->m_updateFlags = 1;
    gpResourceManager->Dispose(originalPalette5);
    delete effectPalette4;
    gpMouseManager->ShowColorPointer();
}

VA(0x00424e48, 0x101)
void combatManager::TurnToStone(army *target)
{
    ResetLimitCreature();
    ++m_limitCreatureCount[target->m_side][target->m_index];
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1);
    DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->SetSpellInfluence(ARMY_SPELL_INFLUENCE_PETRIFIED,
                              SPELL_PETRIFY_DURATION);
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1, SPELL_PETRIFY_FIZZLE_STEPS, 0, 0);
}

VA(0x00424f49, 0x192)
void combatManager::BloodLustEffect(army *target, int effect)
{
    ResetLimitCreature();
    ++m_limitCreatureCount[target->m_side][target->m_index];
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1);
    DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->m_monster.flags.abilityFlags |= effect;
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1, SPELL_BLOOD_LUST_FIZZLE_STEPS, 0, 0);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1);
    target->m_monster.flags.abilityFlags -= effect;
    gpCombatManager->DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1, SPELL_BLOOD_LUST_FIZZLE_STEPS, 0, 0);
}

// @early-stop
// All 48 relocation-masked instructions are identical and all five relocation
// targets agree. The reported 99.86% is solely delinked relocation identity;
// there is no code-byte residual.
VA(0x004250db, 0x80)
void combatManager::Ripple(int strength)
{
    memcpy(m_backgroundBuffer->m_pixels, gpWindowManager->m_screen->m_pixels,
           COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT);
    DoRipple(m_backgroundBuffer, gpWindowManager->m_screen,
             COMBAT_AREA_HEIGHT, strength);
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

VA(0x0042515b, 0x72)
void combatManager::Blur(int redAdjust, int greenAdjust, int blueAdjust)
{
    memcpy(m_backgroundBuffer->m_pixels, gpWindowManager->m_screen->m_pixels,
           COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT);
    DoBlur(m_backgroundBuffer, gpWindowManager->m_screen, COMBAT_AREA_HEIGHT,
           redAdjust, greenAdjust, blueAdjust);
    m_backgroundDrawn = 0;
}

// @match-note 98.42%: complete distance/width interpolation and both angle
// perturbation paths; all 30 relocation targets agree. The first raw differing
// byte is +0x05, the sub esp immediate: retail 0x58, ours 0x54. The first
// normalized-stream difference is retail +0x1c8 `fld qword ptr
// [const_000eb1a8]` (DIR32 operand at +0x1ca) versus the same opcode/value using
// ours $T8146 identity. The first opcode/CFG divergence is retail +0x21c
// `jne +0x5` versus ours `je +0xf2` for the second zero-angle guard. Tried
// direct/compound intermediate angle assignments and nested zero-angle guards.
// Revisit at 95% for od_slots; do not repeat these spellings beforehand.
VA(0x004251cd, 0x320)
void combatManager::ResetBoltAngle(SBolt *bolt)
{
    if (bolt->nearTarget != 0)
        return;

    int deltaX = abs(bolt->endX - bolt->pixelX);
    int deltaY = abs(bolt->endY - bolt->pixelY);
    int distance = static_cast<int>(
        sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
    if (bolt->totalDistance < distance)
        bolt->distanceRatio = 0;
    else
        bolt->distanceRatio = static_cast<float>(bolt->totalDistance - distance) /
                              bolt->totalDistance;

    if (bolt->startWidth != bolt->endWidth) {
        int widthDelta = bolt->endWidth - bolt->startWidth;
        if (widthDelta > 0)
            ++widthDelta;
        else
            --widthDelta;
        int width = bolt->startWidth +
                    static_cast<int>(widthDelta * bolt->distanceRatio);
        if (width < 1)
            width = 1;
        bolt->width = width;
    }
    bolt->widthFirst = -(bolt->width >> 1);
    bolt->widthLast = bolt->width + bolt->widthFirst - 1;

    int angleX = bolt->endX - bolt->pixelX;
    int angleY = bolt->endY - bolt->pixelY;
    bolt->baseAngle = static_cast<float>(
        atan2(static_cast<double>(angleX), static_cast<double>(angleY)));
    float averageAngle = static_cast<float>(
        (bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR);
    averageAngle = static_cast<float>(
        (BOLT_INITIAL_ANGLE_BIAS - bolt->distanceRatio) /
        BOLT_INITIAL_ANGLE_DIVISOR) * averageAngle;
    bolt->angle = averageAngle + bolt->baseAngle;

    if (bolt->minAngle != 0 || bolt->maxAngle != 0) {
        if (!(bolt->angleDistance * BOLT_ANGLE_DISTANCE_FACTOR < distance ||
              bolt->forceAngle != 0))
            return;
        float randomAngle;
        if (bolt->minAngle == bolt->maxAngle)
            randomAngle = static_cast<float>(bolt->minAngle) /
                          BOLT_ANGLE_PERCENT_SCALE;
        else
            randomAngle = static_cast<float>(
                Random(bolt->minAngle, bolt->maxAngle)) /
                BOLT_ANGLE_PERCENT_SCALE;
        randomAngle = static_cast<float>(
            (BOLT_RANDOM_ANGLE_BIAS - bolt->distanceRatio) /
            BOLT_RANDOM_ANGLE_DIVISOR) * randomAngle;
        bolt->baseAngle = randomAngle + bolt->baseAngle;
    }
}

// @match-note 88.07%: complete stepping, clipping, width drawing, all six color
// modes, target approach, and finish CFG with the exact 0x40 frame. Retail is
// 0x4f0 bytes and ours 0x503. The first raw differing byte is +0x19, the local
// displacement in `mov [ebp-local], eax` for oldX: retail 0xd4 (-0x2c), ours
// 0xfc (-0x04). The first normalized instruction divergence is retail +0x6a
// `fld dword ptr [eax+0x38]`; ours starts `mov eax,[eax+0x38]`, stores a local,
// emits two `jmp $+0` continuations, then loads that local. The delinked switch
// boundary truncates homm2 relocs: explicit object ranges contain 28 relocations
// on both sides, with every external target agreeing and eight local entries
// targeting their respective DrawBolt labels. Tried direct and compound sin/cos
// assignment, both loop polarities, direct per-case pixel expressions, and
// retail case-body order. Revisit at 95% or after predecessor slot changes; do
// not grind the switch beforehand.
VA(0x004254ed, 0x4f0)
void combatManager::DrawBolt(SBolt *bolt, int stepCount)
{
    int oldX = static_cast<int>(bolt->currentX);
    int oldY = static_cast<int>(bolt->currentY);
    int widthFirst = bolt->widthFirst;
    int widthLast = bolt->widthLast;
    int unusedRandom = Random(BOLT_RANDOM_WIDTH_LOW, BOLT_RANDOM_WIDTH_HIGH);
    int step;
    for (step = 0; stepCount > step; ++step) {
        bolt->currentX =
            static_cast<float>(sin(bolt->baseAngle)) + bolt->currentX;
        bolt->currentY =
            static_cast<float>(cos(bolt->baseAngle)) + bolt->currentY;
        bolt->pixelX = static_cast<int>(bolt->currentX);
        bolt->pixelY = static_cast<int>(bolt->currentY);
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

        int drawX = bolt->pixelX;
        int drawY = bolt->pixelY;
        if (bolt->pixelX == oldX) {
            if (bolt->pixelY == oldY)
                continue;
        }
        {
            oldX = bolt->pixelX;
            oldY = bolt->pixelY;
            int widthIndex;
            for (widthIndex = widthFirst;; ++widthIndex) {
                if (widthLast < widthIndex)
                    break;
                if (bolt->drawVertically != 0)
                    drawY = bolt->pixelY + widthIndex;
                else
                    drawX = bolt->pixelX + widthIndex;
                if (drawX >= 0 && drawX < COMBAT_SCREEN_WIDTH && drawY >= 0 &&
                    drawY < COMBAT_AREA_HEIGHT) {
                    int edgeDistance;
                    if (widthIndex < 0)
                        edgeDistance = -(widthFirst - widthIndex);
                    else
                        edgeDistance = widthLast - widthIndex;
                    switch (bolt->colorMode) {
                    case BOLT_COLOR_RED_TABLE:
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] =
                            gColorTableRed[static_cast<signed char>(
                                gpWindowManager->m_screen->m_pixels
                                    [drawY * COMBAT_SCREEN_WIDTH + drawX])];
                        break;
                    case BOLT_COLOR_RED_BEAM:
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] =
                            uRedBeam[edgeDistance];
                        break;
                    case BOLT_COLOR_RAINBOW_FORWARD:
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] =
                            uRainbow[widthIndex - widthFirst];
                        break;
                    case BOLT_COLOR_RAINBOW_REVERSE:
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] =
                            uRainbow[BOLT_RAINBOW_LAST_INDEX -
                                     (widthIndex - widthFirst)];
                        break;
                    case BOLT_COLOR_LIGHTNING: {
                        unsigned char color;
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
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] = color;
                        break;
                    }
                    default:
                        gpWindowManager->m_screen->m_pixels
                            [drawY * COMBAT_SCREEN_WIDTH + drawX] =
                            static_cast<unsigned char>(bolt->colorMode);
                        break;
                    }
                }
            }

            int distance = abs(bolt->endY - bolt->pixelY) +
                           abs(bolt->endX - bolt->pixelX);
            if (bolt->nearTarget == 0) {
                if (distance < BOLT_NEAR_TARGET_DISTANCE) {
                    bolt->nearTarget = 1;
                    bolt->nearestDistance = distance;
                }
            } else if (bolt->nearestDistance + 1 < distance ||
                       distance < BOLT_FINISHED_DISTANCE) {
                bolt->finished = 1;
                return;
            } else if (distance < bolt->nearestDistance) {
                bolt->nearestDistance = distance;
            }
        }
    }
}

// @match-note 99.59%: complete initialization/clamping/orientation/distance CFG;
// the 0x18 frame, 0x26f byte size, and all seven relocation targets agree. The
// first raw and normalized instruction difference starts at retail +0x1c3:
// retail `mov eax,[ebp+0x14]`, ours `mov eax,[ebp+0x18]`; the first differing
// byte is its +0x1c5 displacement, retail 0x14 versus ours 0x18. Retail evaluates
// horizontal abs first and retains it in ebx; ours evaluates vertical abs first,
// producing the later equivalent `jle`/`jge` polarity difference. Tried reversed
// inequality, explicit distance locals, and direct abs expressions. Revisit at
// 95% or after predecessor compiler-state changes.
VA(0x004259dd, 0x26f)
void combatManager::AddBolt(SBolt *bolt, int startX, int startY, int endX,
                            int endY, int branchDistance, int startWidth,
                            int endWidth, int colorMode, int minAngle,
                            int maxAngle, int angleDistance, int forceAngle)
{
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

    if (colorMode == BOLT_COLOR_RAINBOW_FORWARD ||
        colorMode == BOLT_COLOR_RAINBOW_REVERSE) {
        if (startX <= 0 || startX >= COMBAT_SCREEN_WIDTH - 1)
            bolt->drawVertically = 1;
        else
            bolt->drawVertically = 0;
    } else if (abs(endX - startX) > abs(endY - startY))
        bolt->drawVertically = 1;
    else
        bolt->drawVertically = 0;

    int deltaX = abs(endX - startX);
    int deltaY = abs(endY - startY);
    bolt->totalDistance = static_cast<int>(
        sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
    ResetBoltAngle(bolt);
}

// @match-note 98.52%: complete palette, 25-record bolt pool, extent/timer/blit,
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
void combatManager::DoBolt(int managePointer, int startX, int startY, int endX,
                           int endY, int branchDistance, int branchLength,
                           int startWidth, int endWidth, int colorMode,
                           int minAngle, int maxAngle, int angleDistance,
                           int unusedParameter, int forceAngle, int frameDelay,
                           int brightenPalette)
{
    if (managePointer != 0)
        gpMouseManager->HideColorPointer();

    int drawDistance = angleDistance;
    int allFinished = 0;
    int drawPassCount = (angleDistance - 1) / drawDistance + 1;
    int branchChance = branchDistance * BOLT_ANGLE_PERCENT_SCALE /
                       angleDistance;
    int deadline = KBTickCount();
    gpWindowManager->m_updateFlags = 0;

    palette *originalPalette = 0;
    palette *effectPalette = 0;
    if (brightenPalette != 0) {
        originalPalette = gpResourceManager->GetPalette("kb.pal");
        effectPalette = new palette;
        if (!effectPalette)
            MemError();
        memcpy(effectPalette->Data(), originalPalette->Data(),
               SPELL_ARMAGEDDON_PALETTE_SIZE);
        signed char *effectData = effectPalette->Data();
        int component;
        for (component = 0; component < SPELL_ARMAGEDDON_PALETTE_SIZE;
             ++component) {
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
        int savedAngle = maxAngle;
        maxAngle = minAngle;
        minAngle = savedAngle;
    }

    SBolt *bolts = new SBolt[BOLT_MAX_COUNT];
    int extentPadding =
        (endWidth <= startWidth ? startWidth : endWidth) >> 1;
    AddBolt(bolts, startX, startY, endX, endY, branchDistance, startWidth,
            endWidth, colorMode, minAngle, maxAngle, angleDistance,
            forceAngle);

    int boltCount = 1;
    while (allFinished == 0) {
        int drawPass;
        for (drawPass = 0; drawPass < drawPassCount; ++drawPass) {
            allFinished = 1;
            int minY = BOLT_EXTENT_SENTINEL;
            int minX = minY;
            int maxY = -1;
            int maxX = maxY;
            int boltIndex;
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
            deadline = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    frameDelay);
            BlitBitmapToScreen(gpWindowManager->m_screen, minX, minY,
                               maxX - minX + 1, maxY - minY + 1, minX, minY);
            PollSound();

            for (boltIndex = 0; boltIndex < boltCount; ++boltIndex) {
                if (bolts[boltIndex].finished == 0)
                    allFinished = 0;
            }
            if (allFinished != 0)
                break;

            if (branchDistance != 0) {
                int oldBoltCount = boltCount;
                for (boltIndex = 0; boltIndex < oldBoltCount; ++boltIndex) {
                    if (bolts[boltIndex].finished == 0) {
                        int remainingDistance =
                            abs(bolts[boltIndex].endX -
                                bolts[boltIndex].pixelX) +
                            abs(bolts[boltIndex].endY -
                                bolts[boltIndex].pixelY);
                        if (boltCount < BOLT_MAX_COUNT &&
                            angleDistance * 2 < remainingDistance &&
                            Random(0, branchChance) <
                                BOLT_BRANCH_PERCENT_LIMIT) {
                            if (bolts[boltIndex].lastBranchX != 0) {
                                int branchSeparation =
                                    abs(bolts[boltIndex].lastBranchX -
                                        bolts[boltIndex].pixelX) +
                                    abs(bolts[boltIndex].lastBranchY -
                                        bolts[boltIndex].pixelY);
                                if (branchSeparation <
                                    branchDistance *
                                        BOLT_BRANCH_COOLDOWN_FACTOR)
                                    goto skipBranch;
                            }
                            bolts[boltIndex].lastBranchX =
                                bolts[boltIndex].pixelX;
                            bolts[boltIndex].lastBranchY =
                                bolts[boltIndex].pixelY;
                            float branchAngle = static_cast<float>(
                                Random(BOLT_BRANCH_RANDOM_LOW,
                                       BOLT_BRANCH_RANDOM_HIGH)) /
                                                BOLT_ANGLE_PERCENT_SCALE;
                            if (Random(0, 1) != 0)
                                branchAngle = -branchAngle;
                            float currentAngle = bolts[boltIndex].angle;
                            float childAngle = branchAngle + currentAngle;
                            int childDistance =
                                Random(branchLength >> 1, branchLength);
                            if ((remainingDistance >> 1) < childDistance)
                                childDistance = remainingDistance >> 1;
                            int childX = static_cast<int>(
                                sin(childAngle) * childDistance +
                                bolts[boltIndex].pixelX);
                            int childY = static_cast<int>(
                                cos(childAngle) * childDistance +
                                bolts[boltIndex].pixelY);
                            int childWidth;
                            if (bolts[boltIndex].endWidth <
                                bolts[boltIndex].startWidth)
                                childWidth = bolts[boltIndex].width - 1;
                            else
                                childWidth = bolts[boltIndex].width;
                            AddBolt(&bolts[boltCount],
                                    bolts[boltIndex].pixelX,
                                    bolts[boltIndex].pixelY, childX, childY,
                                    branchDistance, childWidth, 1, colorMode,
                                    static_cast<int>(
                                        minAngle * BOLT_CHILD_ANGLE_SCALE +
                                        BOLT_CHILD_MIN_ANGLE_OFFSET),
                                    static_cast<int>(
                                        maxAngle * BOLT_CHILD_ANGLE_SCALE +
                                        BOLT_CHILD_ANGLE_OFFSET),
                                    angleDistance,
                                    bolts[boltIndex].forceAngle);
                            ++boltCount;
                        }
                    }
skipBranch:
                    ;
                }
            }
        }

        int boltIndex;
        for (boltIndex = 0; boltIndex < boltCount; ++boltIndex) {
            if (bolts[boltIndex].finished == 0)
                ResetBoltAngle(&bolts[boltIndex]);
        }
    }

    delete bolts;
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

// @match-note retained 99.68%, combined live 94.16%: semantics, CFG, frame,
// and all 12 relocation targets agree. Later TU/header state changed the two
// side/index multiply sequences; the remaining relocation-only residual is the
// zero-float label ($T8947 versus retail const_000eb150). Revisit after shared
// layout stabilization; do not repeat the already exact source spelling.
VA(0x004266ce, 0x18c)
int combatManager::GetNextChainLightningTarget(army *source, int requireWorks)
{
    int closestDistance = 999999;
    int closestHex = COMBAT_HEX_EMPTY;
    int sourceX = source->MidX();
    int sourceY = source->MidY();

    int side;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        int armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            army *candidate = &m_armies[side][armyIndex];
            if (gArmyEffected[side][armyIndex] == 0) {
                if ((requireWorks != 0 &&
                     candidate->SpellCastWorks(SPELL_CHAIN_LIGHTNING)) ||
                    (requireWorks == 0 &&
                     candidate->SpellCastWorkChance(SPELL_CHAIN_LIGHTNING) !=
                         0.0f)) {
                    int deltaX = abs(candidate->MidX() - sourceX);
                    int deltaY = abs(candidate->MidY() - sourceY);
                    int distance = static_cast<int>(sqrt(static_cast<double>(
                        deltaX * deltaX + deltaY * deltaY)));
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestHex = candidate->m_hex;
                    }
                }
            }
        }
    }
    return closestHex;
}

// @match-note 99.41%: complete four-target damage/selection/bolt CFG; all 37
// relocation targets agree. Retail has a 0x5c frame versus ours 0x50. The first
// code divergence is the branch-distance clamp (`jg`/`jl` versus retail
// `jge`/`jle`). Tried both assignment orders, a nextTarget local, `<`, and
// `<= MIN-1`; revisit at 95% for slot/layout steering, not those spellings.
VA(0x0042685a, 0x361)
void combatManager::ChainLightning(int targetHex, int spellPower)
{
    int firstBolt = 1;
    int damage = spellPower * CHAIN_LIGHTNING_INITIAL_DAMAGE_PER_POWER;
    int deadline = KBTickCount();
    int startX = castX;
    int startY = castY;

    ClearEffects();
    gpMouseManager->HideColorPointer();
    int strike;
    for (strike = 0; strike < CHAIN_LIGHTNING_MAX_TARGETS; ++strike) {
        army *target = &m_armies[m_hexCells[targetHex].m_occupantSide]
                                 [m_hexCells[targetHex].m_occupantIndex];
        if (strike <= CHAIN_LIGHTNING_MAX_TARGETS - 2 &&
            m_hexCells[targetHex].m_occupantSide == m_currentSide)
            gpCombatManager->m_heroDeathPending[m_currentSide] = 1;

        int targetDamage = damage;
        if (target->m_monsterType == SPELL_MONSTER_AIR_ELEMENTAL)
            targetDamage *= CHAIN_LIGHTNING_AIR_ELEMENTAL_MULTIPLIER;
        if (target->m_monsterType == SPELL_MONSTER_IRON_GOLEM ||
            target->m_monsterType == SPELL_MONSTER_STEEL_GOLEM)
            targetDamage = static_cast<int>(targetDamage *
                                            SPELL_GOLEM_DAMAGE_MULTIPLIER);
        target->Damage(targetDamage, COMBAT_HEX_EMPTY);
        damage >>= 1;
        gArmyEffected[target->m_side][target->m_index] = 1;

        int targetX = target->MidX();
        int targetY = target->MidY();
        int deltaX = abs(targetX - startX);
        int deltaY = abs(targetY - startY);
        int distance = static_cast<int>(
            sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
        int branchDistance = distance / CHAIN_LIGHTNING_DISTANCE_DIVISOR;
        if (branchDistance > CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MAX_BRANCH_DISTANCE;
        if (branchDistance < CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE)
            branchDistance = CHAIN_LIGHTNING_MIN_BRANCH_DISTANCE;
        int forceAngle = branchDistance < CHAIN_LIGHTNING_SHORT_BRANCH_THRESHOLD
                             ? 2
                             : 3;
        DoBolt(0, startX, startY, targetX, targetY, 0,
               CHAIN_LIGHTNING_BOLT_LENGTH, CHAIN_LIGHTNING_START_WIDTH,
               CHAIN_LIGHTNING_END_WIDTH, CHAIN_LIGHTNING_COLOR_MODE,
               CHAIN_LIGHTNING_MIN_ANGLE, CHAIN_LIGHTNING_MAX_ANGLE,
               branchDistance, forceAngle, 0, 0, strike == 0);
        firstBolt = 0;
        startX = targetX;
        startY = targetY;
        DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] *
                                     SPELL_FIZZLE_FRAME_DELAY));
        int nextTarget = GetNextChainLightningTarget(target, 1);
        if (nextTarget == COMBAT_HEX_EMPTY)
            break;
        targetHex = nextTarget;
        DrawFrame(1, 0, 0, 0, 0, 1, 1);
        DelayTil(&deadline);
        deadline = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                SPELL_FIZZLE_FRAME_DELAY);
    }
    ShowMassSpell(gArmyEffected, gsSpellInfo[SPELL_CHAIN_LIGHTNING].combatEffect,
                  1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpMouseManager->ShowColorPointer();
}

// @match-note 94.56%: complete three-phase vapor mask and cleanup CFG. Retail
// has a 0x30 frame and 31 relocations versus ours 0x28 and 29; the first
// divergence is BaseAlloc's retail source-line expression, followed by the
// bottom-stripe address evaluation order. Tried extent calculation before and
// after palette assignment and explicit first/last stripe locals. Revisit at
// 95% after source-line metadata/od_slots work; do not repeat those orderings.
VA(0x00426bbb, 0x292)
void combatManager::VaporizeCreature(int side, int armyIndex)
{
    army *target = &m_armies[side][armyIndex];
    ResetLimitCreature();
    ++m_limitCreatureCount[side][armyIndex];
    gpCombatManager->DrawFrame(1, 1, 1, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gyModify = static_cast<signed char *>(
        BaseAlloc(SPELL_MODIFIER_ROW_COUNT, SPELLS_SOURCE_FILE, 0));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    int rowCount = giMaxExtentY - giMinExtentY + 1;
    target->m_palette = gyModify;
    target->m_drawEnabled = 0;

    int firstY = (giMinExtentY / VAPORIZE_STRIPE_WIDTH) *
                 VAPORIZE_STRIPE_WIDTH;
    int lastY = (giMaxExtentY / VAPORIZE_STRIPE_WIDTH) *
                VAPORIZE_STRIPE_WIDTH;
    rowCount = (lastY - firstY) / VAPORIZE_STRIPE_WIDTH + 1;
    int phase;
    for (phase = 0; phase < VAPORIZE_PHASE_COUNT; ++phase) {
        int topOffset;
        int bottomOffset;
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
        int row;
        for (row = 0; row < rowCount; ++row) {
            gyModify[row * VAPORIZE_STRIPE_WIDTH +
                     firstY + topOffset] =
                VAPORIZE_MASKED;
            gyModify[lastY +
                     (row * -VAPORIZE_STRIPE_WIDTH - bottomOffset)] =
                VAPORIZE_MASKED;
            gbLimitToExtent = 1;
            gpCombatManager->DrawFrame(1, 0, 1, 0, 30, 1, 1);
        }
    }
    DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] *
                                 SPELL_FIZZLE_FRAME_DELAY));
    target->m_palette = 0;
    target->m_drawEnabled = 1;
    BaseFree(gyModify, SPELLS_SOURCE_FILE, 0);
    gyModify = 0;
    gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

// @match-note 92.18%: complete mode parameters, sine table, phase filtering,
// amplitude scaling, both fade masks, draw, and cleanup CFG. Retail has a 0x54
// frame/68 relocs versus ours 0x58/62; the first code divergence after the
// switch is side/index multiplication order, then BaseAlloc source-line
// metadata. The old unscaled-wave/union-filter formulation was wrong and is
// exhausted. Revisit at 95% for slot placement and allocation metadata only.
VA(0x00426e4d, 0x592)
void combatManager::RippleCreature(int side, int armyIndex, int mode)
{
    army *target = &m_armies[side][armyIndex];
    int phaseStep;
    int frameDelay;
    float amplitudeBase;
    float amplitudeStep;
    switch (mode) {
    case SPELL_RIPPLE_MODE_WAVE:
        phaseStep = 2;
        frameDelay = 20;
        amplitudeBase = RIPPLE_MODE_ZERO_AMPLITUDE_BASE;
        amplitudeStep = RIPPLE_MODE_ZERO_AMPLITUDE_STEP;
        break;
    case SPELL_RIPPLE_MODE_DEATH_RIPPLE:
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

    int extentHeight;
    gyModify = static_cast<signed char *>(
        BaseAlloc(SPELL_MODIFIER_ROW_COUNT, SPELLS_SOURCE_FILE, 0));
    float *wave = static_cast<float *>(
        BaseAlloc(sizeof(float) * SPELL_MODIFIER_ROW_COUNT,
                  SPELLS_SOURCE_FILE, 0));
    memset(gyModify, 0, SPELL_MODIFIER_ROW_COUNT);
    int row;
    for (row = 0; row < SPELL_MODIFIER_ROW_COUNT; ++row) {
        wave[row] = static_cast<float>(
            (sin(static_cast<float>(row % RIPPLE_WAVE_PERIOD) /
                 static_cast<float>(RIPPLE_WAVE_DIVISOR)) - 0.5) * 2.0);
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

    int phase;
    for (phase = RIPPLE_PHASE_START; phase < RIPPLE_PHASE_END;
         phase += phaseStep) {
        int skipDistance = abs(RIPPLE_PHASE_CENTER -
                               phase % RIPPLE_PHASE_PERIOD) -
                           RIPPLE_SKIP_CENTER_OFFSET;
        int amplitudeIndex =
            (phase - RIPPLE_PHASE_START) / RIPPLE_AMPLITUDE_INDEX_DIVISOR + 1;
        if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE)
            amplitudeIndex = RIPPLE_MODE_TWO_AMPLITUDE_START - amplitudeIndex;
        else if (mode == SPELL_RIPPLE_MODE_WAVE) {
            if (amplitudeIndex == 0)
                amplitudeIndex = RIPPLE_MODE_ZERO_CENTER_AMPLITUDE;
            else
                amplitudeIndex = RIPPLE_MODE_ZERO_AMPLITUDE_START -
                                 amplitudeIndex;
        }
        if (phase > RIPPLE_PHASE_CENTER &&
            phase < RIPPLE_PHASE_END - RIPPLE_SKIP_CENTER_OFFSET &&
            (skipDistance != RIPPLE_SKIP_DISTANCE_0 &&
             skipDistance != RIPPLE_SKIP_DISTANCE_1 &&
             skipDistance != RIPPLE_SKIP_DISTANCE_2 &&
             skipDistance != RIPPLE_SKIP_DISTANCE_3 &&
             skipDistance != RIPPLE_SKIP_DISTANCE_4)) {
            float amplitude =
                (amplitudeIndex * amplitudeStep + amplitudeBase) *
                skipDistance;
            memset(gyModify + giMinExtentY, 0, extentHeight);
            for (row = giMinExtentY; row < giMaxExtentY; ++row) {
                int waveIndex;
                if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE)
                    waveIndex = -RIPPLE_PHASE_CENTER - giMaxExtentY +
                                phase * 2 + row;
                else
                    waveIndex = phase * 2 - RIPPLE_PHASE_CENTER - row +
                                giMinExtentY;
                waveIndex += RIPPLE_WAVE_INDEX_OFFSET;
                if (waveIndex >= 0 && waveIndex < SPELL_MODIFIER_ROW_COUNT)
                    gyModify[row] = static_cast<signed char>(
                        wave[waveIndex] * amplitude);
            }
            if (mode == SPELL_RIPPLE_MODE_DEATH_RIPPLE &&
                phase >= RIPPLE_DEATH_RIPPLE_FADE_START) {
                int start = giMinExtentY - 1;
                int end = giMinExtentY +
                          (RIPPLE_DEATH_RIPPLE_FADE_BASE -
                           (RIPPLE_PHASE_END - phase)) *
                              extentHeight / RIPPLE_FADE_DIVISOR +
                          1;
                memset(gyModify + start, VAPORIZE_MASKED, end - start + 1);
            }
            if (mode == SPELL_RIPPLE_MODE_DEATH_WAVE &&
                phase < RIPPLE_DEATH_WAVE_FADE_END) {
                int start = giMinExtentY - 1;
                int end = giMaxExtentY - 1 -
                          (phase - RIPPLE_DEATH_WAVE_FADE_BASE) *
                              extentHeight / RIPPLE_FADE_DIVISOR;
                memset(gyModify + start, VAPORIZE_MASKED, end - start + 1);
            }
            gbLimitToExtent = 1;
            gpCombatManager->DrawFrame(1, 0, 1, 0, frameDelay, 1, 1);
        }
    }
    DelayMilli(static_cast<long>(gfCombatSpeedMod[gConfig.combatSpeed] *
                                 SPELL_FIZZLE_FRAME_DELAY));
    target->m_palette = 0;
    target->m_drawEnabled = 1;
    BaseFree(gyModify, SPELLS_SOURCE_FILE, 0);
    BaseFree(wave, SPELLS_SOURCE_FILE, 0);
    gyModify = 0;
    if (mode != SPELL_RIPPLE_MODE_DEATH_RIPPLE)
        gpCombatManager->DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

// @match-note retained 94.20%, live 88.08% after shared-header changes:
// complete effect loading, creature animation, death processing, and vanish
// CFG; all 23 relocation targets agree. Retail has a 0x28 frame versus ours
// 0x40. The first normalized divergence is affected[side][armyIndex] address
// multiplication order, followed by wince/death arm layout. Tried target
// pointer locals, direct indexing, and both frame-loop spellings. Revisit at
// 95% for od_slots/header stabilization; do not restore the transient live max.
VA(0x004273df, 0x6b2)
void combatManager::ShowMassSpell(signed char (* const affected)[20],
                                  int effect, int animateCreatures)
{
    unsigned long effectFile = MAKEFILEID(gCombatFxNames[effect]);
    int effectFrames = giNumPowFrames[effect] - 1;
    int returnFrames = 0;
    if (gCurLoadedSpellEffect != effect) {
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
        gCurLoadedSpellIcon = gpResourceManager->GetIcon(effectFile);
        gCurLoadedSpellEffect = effect;
    }

    int side;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        int armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            army *target = &m_armies[side][armyIndex];
            if (affected[side][armyIndex] != 0)
                target->m_drawSpellEffect = 1;
            if (animateCreatures != 0 && affected[side][armyIndex] != 0 &&
                target->m_animationSequence != ARMY_ANIMATION_WINCE &&
                target->m_animationSequence != ARMY_ANIMATION_WINCE_RETURN) {
                if (target->m_quantity == 0) {
                    if (effectFrames < target->m_frameInfo.animationFrameCount[
                                           ARMY_ANIMATION_DEATH])
                        effectFrames = target->m_frameInfo.animationFrameCount[
                            ARMY_ANIMATION_DEATH];
                    gpSoundManager->MemorySample(target->m_samples[ARMY_SAMPLE_KILL]);
                } else {
                    if (effectFrames < target->m_frameInfo.animationFrameCount[
                                           ARMY_ANIMATION_WINCE])
                        effectFrames = target->m_frameInfo.animationFrameCount[
                            ARMY_ANIMATION_WINCE];
                    if (returnFrames < target->m_frameInfo.animationFrameCount[
                                           ARMY_ANIMATION_WINCE_RETURN])
                        returnFrames = target->m_frameInfo.animationFrameCount[
                            ARMY_ANIMATION_WINCE_RETURN];
                    gpSoundManager->MemorySample(target->m_samples[ARMY_SAMPLE_WINCE]);
                }
            }
        }
    }

    int frame;
    for (frame = 0; frame < effectFrames; ++frame) {
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            int armyIndex;
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                army *target = &m_armies[side][armyIndex];
                target->m_spellEffectYOffset = 0;
                if (animateCreatures != 0 && affected[side][armyIndex] != 0) {
                    if (target->m_quantity == 0) {
                        if (target->m_animationSequence == ARMY_ANIMATION_DEATH) {
                            if (target->m_animationFrame + 1 <
                                target->m_frameInfo.animationFrameCount[
                                    target->m_animationSequence])
                                ++target->m_animationFrame;
                        } else {
                            target->m_animationSequence = ARMY_ANIMATION_DEATH;
                            target->m_animationFrame = 0;
                        }
                    } else if (target->m_animationSequence ==
                               ARMY_ANIMATION_WINCE) {
                        if (target->m_animationFrame + 1 <
                            target->m_frameInfo.animationFrameCount[
                                target->m_animationSequence])
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
        int armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex)
            m_armies[side][armyIndex].m_drawSpellEffect = 0;
    }
    for (frame = 0; frame < returnFrames + 1; ++frame) {
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            int armyIndex;
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                army *target = &m_armies[side][armyIndex];
                if (animateCreatures != 0 && affected[side][armyIndex] != 0 &&
                    target->m_animationSequence != ARMY_ANIMATION_STAND) {
                    if (target->m_animationSequence ==
                        ARMY_ANIMATION_WINCE) {
                        target->m_animationSequence = ARMY_ANIMATION_WINCE_RETURN;
                        target->m_animationFrame = 0;
                    } else if (target->m_animationFrame + 1 <
                               target->m_frameInfo.animationFrameCount[
                                   target->m_animationSequence]) {
                        ++target->m_animationFrame;
                    } else if (target->m_animationSequence !=
                               ARMY_ANIMATION_DEATH) {
                        target->m_animationSequence = ARMY_ANIMATION_STAND;
                        target->m_animationFrame = 0;
                    }
                }
            }
        }
        DrawFrame(1, 0, 0, 0, 110, 1, 1);
    }

    int creatureDied = 0;
    memset(m_removedArmies, 0, sizeof(m_removedArmies));
    m_removedArmyPresent = 0;
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        int armyIndex;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            army *target = &m_armies[side][armyIndex];
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

// @match-note 84.73%: complete mass-target selection, damage, presentation,
// influence application, and final draw CFG. Retail has a 0x54 frame versus
// ours 0x58. The delink helper stops at a local label (4/59); explicit
// llvm-objdump range review shows all external callees/globals agree and one
// extra local-label relocation remains. Tried retail enemy-body-before-friendly
// switch order. Revisit structural block order at 95%, not expression grinding.
VA(0x00427a91, 0x8f8)
void combatManager::CastMassSpell(int spell, int spellPower)
{
    army *target = 0;
    unsigned int effect = gsSpellInfo[spell].combatEffect;
    int animateCreatures = 0;
    gpWindowManager->m_updateFlags = 0;
    ShowSpellMessage(0, spell, 0);
    signed char affected[COMBAT_SIDE_COUNT][20];
    memset(affected, 0, sizeof(affected));

    int side;
    int armyIndex;
    switch (spell) {
    case SPELL_MASS_SLOW:
    case SPELL_MASS_CURSE:
        side = 1 - m_currentSide;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            if (m_armies[side][armyIndex].SpellCastWorks(spell))
                affected[side][armyIndex] = 1;
        }
        break;

    case SPELL_MASS_CURE:
    case SPELL_MASS_HASTE:
    case SPELL_MASS_BLESS:
    case SPELL_MASS_SHIELD:
        side = m_currentSide;
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            if (m_armies[side][armyIndex].SpellCastWorks(spell))
                affected[side][armyIndex] = 1;
        }
        break;

    case SPELL_HOLY_WORD:
    case SPELL_HOLY_SHOUT: {
        animateCreatures = 1;
        int damage = (spell == SPELL_HOLY_WORD ? 10 : 20) * spellPower;
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                target = &m_armies[side][armyIndex];
                if ((target->m_monster.flags.bytes.attributes & 4) != 0 &&
                    target->SpellCastWorks(spell))
                    affected[side][armyIndex] = 1;
            }
        }
        if (spell == SPELL_HOLY_WORD)
            Blur(0, -2, -2);
        else
            Blur(0, -4, -4);
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                if (affected[side][armyIndex] != 0)
                    m_armies[side][armyIndex].Damage(damage, COMBAT_HEX_EMPTY);
            }
        }
        sprintf(gText, "The %s spell does %d damage", gSpellNames[spell], damage);
        CombatMessage(gText, 1, 1, 0);
        break;
    }

    case SPELL_MASS_DISPEL:
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                if (m_armies[side][armyIndex].SpellCastWorks(spell))
                    affected[side][armyIndex] = 1;
            }
        }
        break;

    case SPELL_DEATH_RIPPLE:
    case SPELL_DEATH_WAVE: {
        animateCreatures = 1;
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                target = &m_armies[side][armyIndex];
                if ((target->m_monster.flags.bytes.attributes & 4) == 0 &&
                    target->SpellCastWorks(spell))
                    affected[side][armyIndex] = 1;
            }
        }
        Ripple(spell == SPELL_DEATH_RIPPLE ? 1 : 2);
        int damage = (spell == SPELL_DEATH_RIPPLE ? 5 : 10) * spellPower;
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                if (affected[side][armyIndex] != 0)
                    m_armies[side][armyIndex].Damage(damage, COMBAT_HEX_EMPTY);
            }
        }
        sprintf(gText, "The Death spell does %d damage", damage);
        CombatMessage(gText, 1, 1, 0);
        break;
    }
    }

    if (!gbNoShowCombat) {
        int anyAffected = 0;
        for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
            for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
                if (affected[side][armyIndex] != 0)
                    anyAffected = 1;
            }
        }
        if (anyAffected)
            ShowMassSpell(affected, effect, animateCreatures);
    }

    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        for (armyIndex = 0; armyIndex < m_armyCount[side]; ++armyIndex) {
            if (affected[side][armyIndex] == 0)
                continue;
            target = &m_armies[side][armyIndex];
            switch (spell) {
            case SPELL_MASS_CURE:
                target->Cure(spellPower);
                break;
            case SPELL_MASS_HASTE:
                target->SetSpellInfluence(SPELL_INFLUENCE_HASTE, spellPower);
                break;
            case SPELL_MASS_SLOW:
                target->SetSpellInfluence(SPELL_INFLUENCE_SLOW, spellPower);
                break;
            case SPELL_MASS_BLESS:
                target->SetSpellInfluence(SPELL_INFLUENCE_BLESS, spellPower);
                break;
            case SPELL_MASS_CURSE:
                target->SetSpellInfluence(SPELL_INFLUENCE_CURSE, spellPower);
                break;
            case SPELL_MASS_DISPEL: {
                int influence;
                for (influence = 0; influence < SPELL_INFLUENCE_COUNT; ++influence)
                    target->CancelIndividualSpell(influence);
                break;
            }
            case SPELL_HOLY_WORD:
            case SPELL_HOLY_SHOUT:
            case SPELL_DEATH_RIPPLE:
            case SPELL_DEATH_WAVE:
                break;
            case SPELL_MASS_SHIELD:
                target->SetSpellInfluence(SPELL_INFLUENCE_SHIELD, spellPower);
                break;
            }
        }
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    gpWindowManager->m_updateFlags = 1;
}

// @match-note retained 91.94%, live 89.43%: complete search, failure dialog,
// army creation, duration artifacts, links, and slide animation; the 0x44
// frame matches. Retail has 38 relocs versus ours 34, with no wrong external
// target. The first divergence is search-loop initialization/trampoline order.
// Tried success inline and retail failure-before-success via goto. Revisit at
// 95% for loop CFG/slots; do not repeat either body placement.
VA(0x00428389, 0x5c8)
void combatManager::MirrorImage(int targetHex)
{
    army *source = &m_armies[m_hexCells[targetHex].m_occupantSide]
                             [m_hexCells[targetHex].m_occupantIndex];
    int mirrorHex = COMBAT_HEX_EMPTY;
    int distance;
    for (distance = 1; distance <= MIRROR_SEARCH_MAX_DISTANCE; ++distance) {
        int sourcePart;
        for (sourcePart = 0; sourcePart < 2; ++sourcePart) {
            int searchHex;
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

            int direction;
            for (direction = 0; direction < SPELL_ADJACENT_DIRECTION_COUNT;
                 ++direction) {
                int searchDirection = source->m_facing == ARMY_FACING_RIGHT
                                          ? direction
                                          : SPELL_ADJACENT_DIRECTION_COUNT - 1 - direction;
                int candidateHex = searchHex;
                int step;
                for (step = 0; step < distance; ++step) {
                    candidateHex = GetAdjacentCellIndexNoArmy(candidateHex,
                                                               searchDirection);
                    if (candidateHex >= 0 && candidateHex < COMBAT_HEX_COUNT &&
                        candidateHex % SPELL_HEX_COLUMN_COUNT != 0 &&
                        candidateHex % SPELL_HEX_COLUMN_COUNT !=
                            SPELL_HEX_RIGHT_BORDER &&
                        source->CanFit(candidateHex, 0, 0)) {
                        mirrorHex = candidateHex;
                        goto mirror_found;
                    }
                }
            }
        }
    }
    sprintf(gText, "Mirror Image spell failed!");
    NormalDialog(gText, NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE,
                 NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                 NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_VALUE, 0);
    return;

mirror_found:
    AddArmy(m_currentSide, source->m_monsterType, source->m_quantity,
            mirrorHex, MIRROR_ARMY_FLAG, 0);
    army *image = &m_armies[m_hexCells[mirrorHex].m_occupantSide]
                            [m_hexCells[mirrorHex].m_occupantIndex];
    image->m_monster.flags.abilityFlags |= MIRROR_MONSTER_ABILITY;
    int duration = m_spellPower[m_currentSide];
    if (m_heroes[m_currentSide]->HasArtifact(
            SPELL_ARTIFACT_ENCHANTED_HOURGLASS))
        duration += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_WIZARD_HAT))
        duration += SPELL_WIZARD_HAT_POWER_BONUS;
    image->m_roundCounter = duration;
    source->m_mirrorImageIndex = image->m_index;
    image->m_mirrorSourceIndex = source->m_index;

    int xOffset = m_hexCells[source->m_hex].m_x -
                  m_hexCells[image->m_hex].m_x;
    int yOffset = m_hexCells[source->m_hex].m_y -
                  m_hexCells[image->m_hex].m_y;
    ResetLimitCreature();
    ++m_limitCreatureCount[image->m_side][image->m_index];
    ++m_limitCreatureCount[m_hexCells[targetHex].m_occupantSide]
                          [m_hexCells[targetHex].m_occupantIndex];
    gpCombatManager->DrawFrame(0, 1, 0, 1, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    int deadline = static_cast<int>(KBTickCount());
    int frame;
    for (frame = 0; frame < MIRROR_SLIDE_FRAME_COUNT; ++frame) {
        image->m_xOffset = (MIRROR_SLIDE_FRAME_COUNT - frame) * xOffset /
                           MIRROR_SLIDE_FRAME_COUNT;
        image->m_yOffset = (MIRROR_SLIDE_FRAME_COUNT - frame) * yOffset /
                           MIRROR_SLIDE_FRAME_COUNT;
        gbLimitToExtent = 1;
        gpCombatManager->DrawFrame(0, 0, 0, 0, 0, 1, 0);
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        gbLimitToExtent = 0;
        DelayTil(&deadline);
        deadline = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                SPELL_FIZZLE_FRAME_DELAY);
    }
    image->m_xOffset = 0;
    image->m_yOffset = 0;
    UpdateGrid(0, 1);
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
}

// @match-note retained 99.88%, live 90.97% after later named-header constants:
// complete hex selection, quantity artifact, AddArmy, ability flag, and duration
// artifact semantics; the 0x1c frame and all 5 relocation targets agree. Before
// the later header additions all 157 masked instructions were identical. The
// current first divergence is modulo/index evaluation order. Do not tune this
// affected function before 95%; accept the retained maximum with the new hash.
VA(0x00428951, 0x218)
void combatManager::SummonElemental(int monsterType, int spellPower)
{
    unsigned char summonHexes[8];
    summonHexes[0] = 14;
    summonHexes[1] = 27;
    summonHexes[2] = 40;
    summonHexes[3] = 11;
    summonHexes[4] = 24;
    summonHexes[5] = 37;
    int randomOffset = SRandom(0, 2);
    unsigned int summonHex = static_cast<unsigned int>(COMBAT_HEX_EMPTY);
    if (m_heroes[m_currentSide] != 0 &&
        m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_BOOK_ELEMENTS))
        spellPower <<= 1;
    int offset;
    for (offset = 0; offset < 3; ++offset) {
        if (m_hexCells[summonHexes[m_currentSide * 3 +
                                   (randomOffset + offset) % 3]]
                .m_occupantSide == COMBAT_HEX_EMPTY)
            summonHex = summonHexes[m_currentSide * 3 +
                                    (randomOffset + offset) % 3];
    }
    m_unknown351D[m_currentSide] = static_cast<unsigned char>(monsterType);
    AddArmy(m_currentSide, monsterType,
            spellPower * ELEMENTAL_SUMMON_QUANTITY_PER_POWER, summonHex, 0, 1);
    army *elementals = &m_armies[m_hexCells[summonHex].m_occupantSide]
                                  [m_hexCells[summonHex].m_occupantIndex];
    elementals->m_monster.flags.abilityFlags |= MIRROR_MONSTER_ABILITY;
    spellPower = m_spellPower[m_currentSide];
    if (m_heroes[m_currentSide]->HasArtifact(
            SPELL_ARTIFACT_ENCHANTED_HOURGLASS))
        spellPower += SPELL_HOURGLASS_POWER_BONUS;
    if (m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_WIZARD_HAT))
        spellPower += SPELL_WIZARD_HAT_POWER_BONUS;
}

// @match-note live 87.70% after named-header state changes: complete fixed
// frame-info lookup, edge selection, clamping, and
// rainbow-bolt call; the 0x18 frame and all 3 relocation targets agree. The
// current first divergence is side/index multiplication; the first local CFG
// divergence is facing/targetX arm order after the exact army+0x2ae signed-byte
// frame load. Tried m_animationFrame, a dynamic
// standing-frame lookup, and the fixed reconstructed table entry. Revisit at
// 95% for condition polarity; do not retry the two incorrect frame sources.
VA(0x00428b69, 0x1e6)
void combatManager::DoLuck(int side, int armyIndex)
{
    army *target = &m_armies[side][armyIndex];
    int targetX = target->MidX();
    int targetY = m_hexCells[target->m_hex].m_y;
    targetY -= GetIconEntry(
                   target->m_creatureIcon,
                   target->m_frameInfo.animationFrames
                       [ARMY_ANIMATION_STANDING_END - 1]
                       [LUCK_TARGET_FRAME_INDEX])->h +
               LUCK_ICON_Y_PADDING;
    if (targetY < LUCK_MIN_TARGET_Y)
        targetY = LUCK_MIN_TARGET_Y;
    int startY = LUCK_EDGE_START_Y;
    int startX;
    if (target->m_facing == ARMY_FACING_RIGHT) {
        if (targetX < 480)
            startX = targetX + targetY / 2 + LUCK_EDGE_INSET;
        else
            startX = targetX - LUCK_EDGE_INSET - targetY / 2;
    } else if (targetX < 201) {
        startX = targetX + targetY / 2 + LUCK_EDGE_INSET;
    } else {
        startX = targetX - LUCK_EDGE_INSET - targetY / 2;
    }
    if (targetY < LUCK_SHORT_BOLT_HEIGHT) {
        startX = side == 0 ? 0 : COMBAT_SCREEN_WIDTH - 1;
        startY = targetY + LUCK_MIN_TARGET_Y;
    }
    if (startX < 0)
        startX = 0;
    if (COMBAT_SCREEN_WIDTH - 1 < startX)
        startX = COMBAT_SCREEN_WIDTH - 1;
    DoBolt(0, startX, startY, targetX, targetY, 0, 0, LUCK_BOLT_WIDTH,
           LUCK_BOLT_WIDTH,
           startX <= targetX ? BOLT_COLOR_RAINBOW_FORWARD
                             : BOLT_COLOR_RAINBOW_REVERSE,
           LUCK_BOLT_ANGLE, LUCK_BOLT_ANGLE,
           LUCK_BOLT_DISTANCE, LUCK_BOLT_FORCE_ANGLE, 1,
           LUCK_BOLT_FRAME_DELAY, 0);
}

// @match-note retained 99.75%, live 99.49% after shared-header changes:
// complete blast interpolation/draw/update CFG; the 0x7c frame and all 50
// relocation targets agree. A prior stabilized build differed only in local
// string/constant identities; current TU state first differs at the segment
// loop branch (`jle` versus retail `jge`). Tried `count > segment`, `<`, and an
// explicit break (worse). Revisit at 95% after headers settle, not before.
VA(0x00428d4f, 0x33a)
void combatManager::DoBlast(int targetHex, int spell)
{
    icon *blastIcon;
    int frameSpacing;
    if (spell == SPELL_COLD_RAY) {
        blastIcon = gpResourceManager->GetIcon("coldray.icn");
        frameSpacing = BLAST_COLD_RAY_FRAME_SPACING;
    } else {
        blastIcon = gpResourceManager->GetIcon("disrray.icn");
        frameSpacing = BLAST_DISRUPTING_RAY_FRAME_SPACING;
    }
    army *target = &m_armies[m_hexCells[targetHex].m_occupantSide]
                             [m_hexCells[targetHex].m_occupantIndex];
    int targetX = target->MidX();
    int targetY = target->MidY();
    int startX = castX;
    int startY = castY;
    int deltaX = targetX - startX;
    int deltaY = targetY - startY;
    int distance = static_cast<int>(
        sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
    int segmentCount = distance / frameSpacing;
    float currentX = static_cast<float>(startX);
    float currentY = static_cast<float>(startY);
    float stepX = static_cast<float>(deltaX) / segmentCount;
    float stepY = static_cast<float>(deltaY) / segmentCount;
    int deadline = 0;
    int segment;
    for (segment = 0; segmentCount > segment; ++segment) {
        ResetLimitCreature();
        gbComputeExtent = 1;
        gbSaveBiggestExtent = 1;
        currentX += stepX;
        currentY += stepY;
        int frame = (segment * BLAST_FRAME_COUNT - 1) / segmentCount;
        SLimitData limits;
        blastIcon->CombatClipDrawToBuffer(
            static_cast<int>(currentX), static_cast<int>(currentY), frame,
            &limits, stepX < 0.0f, 0, 0, 0);
        if (giMinExtentX < 0)
            giMinExtentX = 0;
        if (giMinExtentY < 0)
            giMinExtentY = 0;
        if (COMBAT_SCREEN_WIDTH - 1 < giMaxExtentX)
            giMaxExtentX = COMBAT_SCREEN_WIDTH - 1;
        if (COMBAT_AREA_HEIGHT - 1 < giMaxExtentY)
            giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
        DelayTil(&deadline);
        deadline = static_cast<int>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                SPELL_FIZZLE_FRAME_DELAY);
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
    }
    gbComputeExtent = 0;
    gbSaveBiggestExtent = 0;
    DrawFrame(1, 0, 0, 0, 0, 1, 0);
    gpResourceManager->Dispose(blastIcon);
}

// @match-note 85.51%: complete quantity/artifact handling, both dead-hex record
// removals, message, reverse-death animation, and cleanup; the 0x44 frame and
// all 33 relocation targets agree. The first code divergence is quantity-clamp
// polarity, then processedOtherHex initialization and the dead-occupant loop
// CFG. Tried moving that initialization across the oldQuantity arm and explicit
// break/keepSearching forms. Revisit structural loop order at 95%.
VA(0x00429089, 0x655)
void combatManager::Resurrect(int spell, int targetHex, int spellPower)
{
    if (m_heroes[m_currentSide] != 0 &&
        m_heroes[m_currentSide]->HasArtifact(SPELL_ARTIFACT_ANKH))
        spellPower <<= 1;
    int armyIndex = FindResurrectArmyIndex(m_currentSide, spell, targetHex);
    army *target = &m_armies[m_currentSide][armyIndex];
    int processedOtherHex = 0;
    int oldQuantity = target->m_quantity;
    target->m_quantity +=
        spellPower * RESURRECT_HIT_POINTS_PER_POWER /
        static_cast<unsigned short>(target->m_monster.hitPoints);
    if (target->m_quantity > target->m_initialQuantity)
        target->m_quantity = target->m_initialQuantity;
    if (spell == SPELL_RESURRECT)
        target->m_temporaryResurrectionQuantity +=
            target->m_quantity - oldQuantity;

    if (oldQuantity <= 0) {
        int otherHex = COMBAT_HEX_EMPTY;
        int keepSearching = 1;
        int deadHex = targetHex;
        while (keepSearching) {
            int deadIndex = COMBAT_HEX_EMPTY;
            int index;
            for (index = 0; index < m_hexCells[deadHex].m_deadOccupantCount;
                 ++index) {
                if (m_hexCells[deadHex].m_deadOccupantSides[index] ==
                        m_currentSide &&
                    m_hexCells[deadHex].m_deadOccupantIndices[index] ==
                        armyIndex) {
                    deadIndex = index;
                    if (!processedOtherHex) {
                        if (m_hexCells[deadHex].m_deadOccupantFrames[index] == 1)
                            otherHex = deadHex - 1;
                        else if (m_hexCells[deadHex].m_deadOccupantFrames[index] == 0)
                            otherHex = deadHex + 1;
                    }
                    m_hexCells[deadHex].m_occupantSide =
                        m_hexCells[deadHex].m_deadOccupantSides[index];
                    m_hexCells[deadHex].m_occupantIndex =
                        m_hexCells[deadHex].m_deadOccupantIndices[index];
                    m_hexCells[deadHex].m_occupantFrame =
                        m_hexCells[deadHex].m_deadOccupantFrames[index];
                    if (m_hexCells[deadHex].m_deadOccupantCount == index + 1) {
                        m_hexCells[deadHex].m_deadOccupantSides[index] =
                            COMBAT_HEX_EMPTY;
                        m_hexCells[deadHex].m_deadOccupantIndices[index] =
                            COMBAT_HEX_EMPTY;
                    } else {
                        m_hexCells[deadHex].m_deadOccupantSides[index] =
                            m_hexCells[deadHex].m_deadOccupantSides[index + 1];
                        m_hexCells[deadHex].m_deadOccupantIndices[index] =
                            m_hexCells[deadHex].m_deadOccupantIndices[index + 1];
                    }
                    break;
                }
            }
            --m_hexCells[deadHex].m_deadOccupantCount;
            if (processedOtherHex || otherHex == COMBAT_HEX_EMPTY) {
                keepSearching = 0;
            } else {
                deadHex = otherHex;
                processedOtherHex = 1;
                deadIndex = COMBAT_HEX_EMPTY;
            }
        }
    }

    int effectX = target->MidX();
    int effectY = target->MidY();
    int resurrected = target->m_quantity - oldQuantity;
    if (resurrected < 2)
        sprintf(gText, "%d %s rises from the dead!", resurrected,
                gArmyNames[target->m_monsterType]);
    else
        sprintf(gText, "%d %s rise from the dead!", resurrected,
                gArmyNamesPlural[target->m_monsterType]);
    CombatMessage(gText, 1, 1, 0);

    if (!gbNoShowCombat) {
        icon *resurrectIcon = gpResourceManager->GetIcon("yinyang.icn");
        int frame;
        for (frame = 0; frame < RESURRECT_ANIMATION_FRAME_COUNT; ++frame) {
            glTimers[0] = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    SPELL_FIZZLE_FRAME_DELAY);
            IconToBitmap(resurrectIcon, gpWindowManager->m_screen, effectX,
                         effectY, frame, 1, 0, 0, COMBAT_SCREEN_WIDTH,
                         COMBAT_AREA_HEIGHT, 0);
            UpdateCombatArea();
            target->m_facing = 1 - target->m_side;
            if (target->m_animationSequence == ARMY_ANIMATION_DEATH) {
                if (frame < RESURRECT_DEATH_REVERSE_FRAME) {
                    int reverseFrame =
                        target->m_frameInfo.animationFrameCount[
                            ARMY_ANIMATION_DEATH] - 1;
                    if (RESURRECT_DEATH_REVERSE_FRAME - 1 - frame <=
                        reverseFrame)
                        reverseFrame = RESURRECT_DEATH_REVERSE_FRAME - 1 - frame;
                    target->m_animationFrame = reverseFrame;
                } else {
                    target->m_animationSequence = ARMY_ANIMATION_STAND;
                    target->m_animationFrame = 0;
                }
            }
            DrawFrame(0, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
            DelayTil(glTimers);
        }
        gpResourceManager->Dispose(resurrectIcon);
    }
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);
    target->m_monster.flags.abilityFlags &= ~RESURRECT_MONSTER_ABILITY;
}

// @match-note 97.87%: complete side-specific three-cell test; the 0x04 frame
// matches and neither side has relocations. The only normalized residual is one
// retail continuation `jmp` before the true return. Tried nested side arms and
// the combined early-return condition. Revisit at 95% for inline continuation
// shaping; do not repeat those equivalent condition forms.
VA(0x004296de, 0xb9)
int combatManager::SpaceForElementalExists(void)
{
    if ((m_currentSide == 0 &&
         m_hexCells[14].m_occupantSide != COMBAT_HEX_EMPTY &&
         m_hexCells[27].m_occupantSide != COMBAT_HEX_EMPTY &&
         m_hexCells[40].m_occupantSide != COMBAT_HEX_EMPTY) ||
        (m_currentSide == 1 &&
         m_hexCells[11].m_occupantSide != COMBAT_HEX_EMPTY &&
         m_hexCells[24].m_occupantSide != COMBAT_HEX_EMPTY &&
         m_hexCells[37].m_occupantSide != COMBAT_HEX_EMPTY))
        return 0;
    return 1;
}

// @early-stop
// All instructions are identical after relocation masking and all 15 targets
// agree. The 99.89% residual is only delinked identities for NULL_SAMPLE2's
// second word and four strings; the 0x18 frame and all code bytes agree.
VA(0x00429797, 0xd9)
void combatManager::ShowSpellCastFailure(army *target, int)
{
    SAMPLE2 sample = NULL_SAMPLE2;
    sample = LoadPlaySample("rsbryfzl.82m");
    char *armyName;
    if (target->m_quantity == 1)
        armyName = gArmyNames[target->m_monsterType];
    else
        armyName = gArmyNamesPlural[target->m_monsterType];
    sprintf(gText, "The %s %s the spell!", armyName,
            target->m_quantity == 1 ? "resists" : "resist");
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    WaitEndSample(sample, -1);
}

VA(0x00429870, 0x270)
void combatManager::ModifyDamageForArtifacts(long *damage, int spell,
                                              hero *attacker, hero *defender)
{
    if (attacker != 0) {
        if (attacker->HasArtifact(SPELL_ARTIFACT_EVERCOLD_ICICLE) &&
            (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
        if (attacker->HasArtifact(SPELL_ARTIFACT_EVERHOT_LAVA_ROCK) &&
            (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
        if (attacker->HasArtifact(SPELL_ARTIFACT_LIGHTNING_ROD) &&
            (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_BONUS);
    }
    if (defender != 0) {
        if (defender->HasArtifact(SPELL_ARTIFACT_ICE_CLOAK) &&
            (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(SPELL_ARTIFACT_FIRE_CLOAK) &&
            (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(SPELL_ARTIFACT_LIGHTNING_HELM) &&
            (spell == SPELL_LIGHTNING_BOLT || spell == SPELL_CHAIN_LIGHTNING))
            *damage = static_cast<long>(*damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        if (defender->HasArtifact(SPELL_ARTIFACT_HEART_FIRE)) {
            if (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)
                *damage <<= 1;
            else if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                *damage = static_cast<long>(
                    *damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
        }
        if (defender->HasArtifact(SPELL_ARTIFACT_HEART_ICE)) {
            if (spell == SPELL_COLD_RAY || spell == SPELL_COLD_RING)
                *damage = static_cast<long>(
                    *damage * SPELL_ARTIFACT_DAMAGE_REDUCTION);
            else if (spell == SPELL_FIREBALL || spell == SPELL_FIREBLAST)
                *damage <<= 1;
        }
    }
}

// @match-note 97.20%: complete shake, randomized wall/tower/keep damage, cloud
// animation, state application, and input restore; the 0x160 frame and all 62
// relocation targets agree. After constant identity, the first code divergence
// is the screen-copy row loop branch (`jge` versus retail `jle`); later residuals
// are packed wallPos/towerPos local-label identities and keep impact ordering.
// Tried cached/direct offsets, both height association orders, <= zero tests,
// and both loop polarities. Revisit at 95% after layout/header stabilization.
VA(0x00429ae0, 0x931)
void combatManager::Earthquake(void)
{
    int shakeOffsets[30];
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
    memcpy(m_backgroundBuffer->m_pixels, gpWindowManager->m_screen->m_pixels,
           COMBAT_SCREEN_WIDTH * COMBAT_AREA_HEIGHT);
    int pass;
    for (pass = 0; pass < EARTHQUAKE_SHAKE_PASS_COUNT; ++pass) {
        int frame;
        for (frame = 0; frame < EARTHQUAKE_SHAKE_FRAME_COUNT; ++frame) {
            int deadline = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    SPELL_ARMAGEDDON_SHAKE_DELAY);
            PollSound();
            int width = COMBAT_SCREEN_WIDTH -
                        abs(shakeOffsets[frame *
                                         EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                         COMBAT_COORDINATE_X]);
            int height = COMBAT_AREA_HEIGHT -
                         abs(shakeOffsets[frame *
                                          EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                          COMBAT_COORDINATE_Y]) -
                         1;
            unsigned char *source =
                m_backgroundBuffer->m_pixels +
                (shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                              COMBAT_COORDINATE_Y] > 0
                     ? shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                    COMBAT_COORDINATE_Y]
                     : 0) *
                    COMBAT_SCREEN_WIDTH +
                (shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                              COMBAT_COORDINATE_X] > 0
                     ? shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                    COMBAT_COORDINATE_X]
                     : 0);
            int destinationX =
                shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                             COMBAT_COORDINATE_X] <= 0
                    ? -shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                    COMBAT_COORDINATE_X]
                    : 0;
            int destinationY =
                shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                             COMBAT_COORDINATE_Y] <= 0
                    ? -shakeOffsets[frame * EARTHQUAKE_SHAKE_COORDINATE_COUNT +
                                    COMBAT_COORDINATE_Y]
                    : 0;
            unsigned char *destination =
                gpWindowManager->m_screen->m_pixels +
                destinationY * COMBAT_SCREEN_WIDTH + destinationX;
            int row;
            for (row = 0; height > row; ++row) {
                memcpy(destination, source, width);
                source += COMBAT_SCREEN_WIDTH;
                destination += COMBAT_SCREEN_WIDTH;
            }
            BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                               COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0, 0);
            DelayTil(&deadline);
        }
    }
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, SPELL_FIZZLE_FRAME_DELAY, 1, 1);

    int newWallStates[EARTHQUAKE_STRUCTURE_COUNT];
    int newTowerStates[EARTHQUAKE_STRUCTURE_COUNT];
    int impactPositions[EARTHQUAKE_MAX_IMPACTS][2];
    int impactCount = 0;
    int structure;
    for (structure = 0; structure < EARTHQUAKE_STRUCTURE_COUNT; ++structure) {
        newWallStates[structure] =
            m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST];
        if (m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                COMBAT_WALL_STATE_DESTROYED &&
            m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                COMBAT_WALL_STATE_SECTION_DESTROYED &&
            SRandom(0, 100) < EARTHQUAKE_WALL_HIT_CHANCE) {
            ++newWallStates[structure];
            if (m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_DESTROYED &&
                m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_TOWER_STANDING &&
                m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_SECTION_DESTROYED &&
                m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_SECTION_DAMAGE_LAST &&
                SRandom(0, 100) < EARTHQUAKE_WALL_SECOND_HIT_CHANCE)
                ++newWallStates[structure];
            impactPositions[impactCount][COMBAT_COORDINATE_X] =
                wallPos[structure][COMBAT_COORDINATE_X];
            impactPositions[impactCount][COMBAT_COORDINATE_Y] =
                wallPos[structure][COMBAT_COORDINATE_Y] +
                EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
            if (newWallStates[structure] == COMBAT_WALL_STATE_DESTROYED ||
                newWallStates[structure] == COMBAT_WALL_STATE_SECTION_DESTROYED)
                m_hexCells[iWallToHexCell[structure]].m_blocked = 0;
        }

        newTowerStates[structure] = m_wallStates[structure];
        if (m_wallStates[structure] != COMBAT_WALL_STATE_DESTROYED &&
            SRandom(0, 100) < EARTHQUAKE_TOWER_HIT_CHANCE) {
            newTowerStates[structure] = COMBAT_WALL_STATE_DESTROYED;
            impactPositions[impactCount][COMBAT_COORDINATE_X] =
                towerPos[structure][COMBAT_COORDINATE_X];
            impactPositions[impactCount][COMBAT_COORDINATE_Y] =
                towerPos[structure][COMBAT_COORDINATE_Y] +
                EARTHQUAKE_CLOUD_Y_OFFSET;
            ++impactCount;
        }
    }

    int newKeepState = m_drawbridgeState;
    if (m_drawbridgeState != 3 &&
        SRandom(0, 100) < EARTHQUAKE_KEEP_HIT_CHANCE) {
        newKeepState = 3;
        impactPositions[impactCount][COMBAT_COORDINATE_X] =
            towerPos[0][COMBAT_COORDINATE_X];
        impactPositions[impactCount][COMBAT_COORDINATE_Y] =
            towerPos[0][COMBAT_COORDINATE_Y] + EARTHQUAKE_CLOUD_Y_OFFSET;
        impactPositions[impactCount + 1][COMBAT_COORDINATE_X] =
            towerPos[1][COMBAT_COORDINATE_X];
        impactPositions[impactCount + 1][COMBAT_COORDINATE_Y] =
            towerPos[1][COMBAT_COORDINATE_Y] + EARTHQUAKE_CLOUD_Y_OFFSET;
        impactCount += 2;
    }

    int impactDelay[EARTHQUAKE_MAX_IMPACTS];
    for (structure = 0; structure < EARTHQUAKE_MAX_IMPACTS; ++structure)
        impactDelay[structure] = Random(0, 2);
    giMinExtentX = EARTHQUAKE_EXTENT_MIN_X;
    giMinExtentY = 0;
    giMaxExtentX = EARTHQUAKE_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_AREA_HEIGHT - 1;
    if (impactCount != 0) {
        icon *cloudIcon = gpResourceManager->GetIcon("lichclod.icn");
        int frame;
        for (frame = 0; frame < EARTHQUAKE_CLOUD_FRAME_COUNT; ++frame) {
            glTimers[1] = static_cast<int>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                    SPELL_FIZZLE_FRAME_DELAY);
            DrawFrame(0, 0, 1, 0, 0, 1, 0);
            int impact;
            for (impact = 0; impact < impactCount; ++impact) {
                int cloudFrame = frame - impactDelay[impact];
                if (impactDelay[impact] <= frame &&
                    cloudFrame < EARTHQUAKE_CLOUD_VISIBLE_FRAME_COUNT) {
                    IconToBitmap(cloudIcon, gpWindowManager->m_screen,
                                 impactPositions[impact][COMBAT_COORDINATE_X],
                                 impactPositions[impact][COMBAT_COORDINATE_Y],
                                 cloudFrame, 1, 0, 0, COMBAT_SCREEN_WIDTH,
                                 COMBAT_AREA_HEIGHT, 0);
                }
            }
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX, giMinExtentY, giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1);
            DelayTil(&glTimers[1]);
            if (frame == EARTHQUAKE_APPLY_DAMAGE_FRAME) {
                for (structure = 0; structure < EARTHQUAKE_STRUCTURE_COUNT;
                     ++structure) {
                    m_wallStates[structure + COMBAT_WALL_SLOT_SECTION_FIRST] =
                        static_cast<unsigned char>(newWallStates[structure]);
                    m_wallStates[structure] =
                        static_cast<unsigned char>(newTowerStates[structure]);
                }
                m_drawbridgeState = newKeepState;
            }
        }
        gpResourceManager->Dispose(cloudIcon);
    }
    gpMouseManager->ShowColorPointer();
}

// @match-note 97.79%: complete creature-first and hero-second message selection;
// all 26 relocation targets agree. Retail has a 0x1d8 frame versus ours 0x1d4.
// The first code residual after Archmages sprintf is a retail `jmp`, dead-local
// clear/increment, then another `jmp`; later differences are string identities.
// Tried hero-first order, plural polarity, direct hero access, and the extra
// unused targetName variadic argument. Revisit dead-local scope at 95%.
VA(0x0042a411, 0x2b1)
void combatManager::ShowSpellMessage(int castByCreature, int spell,
                                     army *target)
{
    char targetName[60];
    char message[400];
    if (target != 0) {
        char *name;
        if (target->m_quantity > 1)
            name = gArmyNamesPlural[target->m_monsterType];
        else
            name = gArmyNames[target->m_monsterType];
        sprintf(targetName, name);
    }
    if (castByCreature != 0) {
        if (spell == SPELL_PARALYZE)
            sprintf(message, "The %s are paralyzed by the Cyclopes!", targetName);
        else if (spell == SPELL_BLIND)
            sprintf(message, "The Unicorns' attack blinds the %s!", targetName);
        else if (spell == SPELL_PETRIFY)
            sprintf(message, "The Medusas' gaze turns the %s to stone!", targetName);
        else if (spell == SPELL_CURSE)
            sprintf(message, "The Mummies' curse falls upon the %s!", targetName);
        else if (spell == SPELL_CREATURE_DISPEL)
            sprintf(message, "The Archmages dispel all good spells", targetName);
    } else {
        if (target != 0) {
            if (m_heroes[m_currentSide]->m_isCaptain != 0)
                sprintf(message, "The captain casts '%s' on the %s.",
                        gSpellNames[spell], targetName);
            else
                sprintf(message, "%s casts '%s' on the %s.",
                        m_heroes[m_currentSide]->m_name, gSpellNames[spell],
                        targetName);
        } else {
            if (m_heroes[m_currentSide]->m_isCaptain != 0)
                sprintf(message, "The captain casts '%s'.", gSpellNames[spell]);
            else
                sprintf(message, "%s casts '%s'.",
                        m_heroes[m_currentSide]->m_name, gSpellNames[spell]);
        }
    }
    CombatMessage(message, 1, 1, 0);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f00b0) int castX;
DATA(0x004f00b4) int castY;
DATA(0x004f00b8) int bInTeleportGetDest;
DATA(0x004f0278) int indexToCastOn;
DATA(0x004f04c0) unsigned char uRedBeam[] = {
    0x77, 0xb5, 0xbc, 0xc0, 0xc3
};
DATA(0x004f04c8) unsigned char uRainbow[] = {
    0xbc, 0xba, 0xce, 0xca, 0xc8, 0x6f, 0x71, 0x73,
    0xde, 0x5a, 0x9f, 0x47, 0x4a, 0x8d, 0x8f
};
DATA(0x004f04e0) signed char *gyModify;
