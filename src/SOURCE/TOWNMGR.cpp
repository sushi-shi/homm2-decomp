// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWNMGR.OBJ   from: (directly linked into exe)
// functions: 33   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
#include <_types.h>
#include <BASE/Misc.h>
#include <BASE/border.h>
#include <BASE/executive.h>
#include <BASE/font.h>
#include <BASE/inputManager.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/iconWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/textWidget.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Castle.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/RECRUIT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/game.h>
#include <SOURCE/HERO.h>
#include <SOURCE/hero.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/philAI.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/strip.h>
#include <SOURCE/town.h>
#include <SOURCE/townObject.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/tradpost.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VA(0x00413900, 0x16a)
townObject::townObject(int, int, char *) {}

VA(0x00413a6a, 0x60)
townObject::~townObject() {}

VA(0x00413aca, 0x437)
void townObject::Draw(int) {}

VA(0x00413f01, 0x68)
townManager::townManager(void) {}

VA(0x00413f69, 0x1a0)
void townManager::SetupExtraStuff(void) {}

VA(0x00414109, 0x1ef)
int townManager::Open(int) { return 0; }

VA(0x004142f8, 0x77)
void townManager::ChangeTown(void) {}

VA(0x0041436f, 0x95a)
void townManager::SetupTown(void) {}

VA(0x00414cc9, 0x1cf)
void townManager::UnloadTown(void) {}

VA(0x00414e98, 0xca)
void townManager::Close(void) {}

VA(0x00414f62, 0x3b9)
void townManager::SetArmyCommand(int) {}

VA(0x0041531b, 0x5c5)
void townManager::SetCommandAndText(struct tag_message &) {}

VA(0x004158e0, 0x7d)
void townManager::ShowText(char *) {}

VA(0x0041595d, 0x1830)
int townManager::Main(tag_message &message)
{
    char description_b[400];
    int exitTown_i = 0;
    int quickView_k = (message.payload.widget.parameter & TOWN_QUICK_VIEW_MODIFIER) != 0;
    int debugBuilding_e;
    int index_i;
    int marketplaceCount_m;
    baseManager *dialogManager_d;
    SAMPLE2 buildSample_m;
    int total;
    int unusedTownValue;

    if (giDebugBuildingToBuild != -1) {
        debugBuilding_e = giDebugBuildingToBuild;
        giDebugBuildingToBuild = -1;
        if (debugBuilding_e == TOWN_DEBUG_BUILD_ALL) {
            for (index_i = 0; index_i < TOWN_BUILDING_COUNT; ++index_i) {
                if ((gTownEligibleBuildMask[m_town->m_type] & (1L << index_i)) ||
                    index_i == TOWN_COMMAND_CASTLE)
                    BuildObj(index_i);
            }
        } else if ((gTownEligibleBuildMask[m_town->m_type] &
                    (1L << static_cast<unsigned char>(debugBuilding_e))) ||
                   debugBuilding_e == TOWN_COMMAND_CASTLE) {
            BuildObj(debugBuilding_e);
        }
    }

    if (KBTickCount() > glTimers[0]) {
        DrawTown(1, 1);
        glTimers[0] = KBTickCount() + TOWN_REDRAW_INTERVAL;
    }

    switch (message.type) {
    case TOWN_MESSAGE_SELECT:
        switch (message.payload.widget.command) {
        case TOWN_INPUT_SELECT:
        case TOWN_INPUT_ALTERNATE_SELECT:
        switch (message.payload.widget.id) {
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            if (quickView_k) {
                QuickViewRecruit(
                    m_town, message.payload.widget.id - TOWN_COMMAND_FIRST_DWELLING);
            } else {
                DrawTown(1, 1);
                dialogManager_d = new recruitUnit(
                    m_town,
                    message.payload.widget.id - TOWN_COMMAND_FIRST_DWELLING, 1);
                if (dialogManager_d == 0)
                    MemError();
                gpExec->DoDialog(dialogManager_d);
                delete dialogManager_d;
            }
            break;

        case TOWN_COMMAND_CASTLE:
            if (!quickView_k) {
                m_heroWindow0 = new heroWindow(0, 0, "caslwind.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetupCastle(m_heroWindow0, 0);
                m_castleDialogActive = 1;
                m_recruitResult = 0;
                gpWindowManager->DoDialog(
                    m_heroWindow0, CastleHandler, 0);
                m_castleDialogActive = 0;
                delete m_heroWindow0;

                if (m_recruitResult != 0) {
                    RedrawTownScreen();
                    gpWindowManager->SaveFizzleSource(0, 0x100,
                                                       0x228, 0xcc);
                    if (m_heroStrip != 0)
                        delete m_heroStrip;
                    sprintf(gText, "port%04d.icn",
                            m_recruitHero->m_portrait);
                    m_heroStrip = new strip(
                        0, 0x163, 3,
                        gpResourceManager->MakeId(gText, 1),
                        0, &m_recruitHero->m_army, 0x7a, 0, -1);
                    if (m_heroStrip == 0)
                        MemError();
                    buildSample_m = NULL_SAMPLE2;
                    buildSample_m = LoadPlaySample("buildtwn.82M");
                    hero *townHero = gpGame->GetHero(
                        m_town->m_occupyingHeroId);
                    int fizzleWidth = 0x228;
                    m_townWindow->DrawWindow(0);
                    m_garrisonStrip->DrawIcons(0);
                    m_heroStrip->DrawIcons(0);
                    gpWindowManager->FizzleForward(
                        0, 0x100, fizzleWidth, 0xcc, -1, 0, 0);
                    WaitEndSample(buildSample_m, -1);
                    m_recruitResult = 0;
                    gpWindowManager->ReleaseFizzleSource();
                } else {
                    if (m_selectedBuilding == 15 &&
                        m_town->m_occupyingHeroId == -1) {
                        if (m_heroStrip != 0)
                            delete m_heroStrip;
                        m_heroStrip = 0;
                        sprintf(gText, "port%04d.icn",
                                m_town->m_type + 0x5a);
                        m_heroStrip = new strip(
                            0, 0x163, 3,
                            gpResourceManager->MakeId(gText, 1),
                            0, 0, -1, 0, gpCurPlayer->m_color);
                        if (m_heroStrip == 0)
                            MemError();
                    }
                    RedrawTownScreen();
                    if (m_selectedBuilding != -1)
                        BuildObj(m_selectedBuilding);
                }
            }
            break;

        case TOWN_COMMAND_MAGE_GUILD:
            if (!quickView_k) {
                if (m_town->m_occupyingHeroId != -1 &&
                    !gpGame->GetHero(m_town->m_occupyingHeroId)
                         ->HasArtifact(TOWN_SPELL_BOOK_ARTIFACT)) {
                    if (gpGame->GetHero(m_town->m_occupyingHeroId)
                            ->NumArtifacts() == TOWN_MAX_ARTIFACTS) {
                        NormalDialog(
                            "You must purchase a spell book to use the mage guild, but you currently have no room for a spell book.  Try giving one of your artifacts to another hero.",
                            1, -1, -1, -1, 0, -1, 0, -1, 0);
                    } else if (gpCurPlayer->m_resources[RES_GOLD] <
                               TOWN_SPELL_BOOK_COST) {
                        NormalDialog(
                            "To cast spells, you must first buy a spell book for 500 gold.  Unfortunately, you seem to be a little short of cash at the moment.",
                            1, -1, -1, 7, TOWN_SPELL_BOOK_ARTIFACT,
                            -1, 0, -1, 0);
                    } else {
                        NormalDialog(
                            "To cast spells, you must first buy a spell book for 500 gold.  Do you wish to buy one?",
                            2, -1, -1, 7, TOWN_SPELL_BOOK_ARTIFACT,
                            -1, 0, -1, 0);
                        if (gpWindowManager->m_dialogResult ==
                            TOWN_DIALOG_BUY_SPELL_BOOK) {
                            GiveArtifact(
                                gpGame->GetHero(m_town->m_occupyingHeroId),
                                TOWN_SPELL_BOOK_ARTIFACT, 1, -1);
                            gpCurPlayer->m_resources[RES_GOLD] -=
                                TOWN_SPELL_BOOK_COST;
                            m_bankBox->Update(1);
                            m_townWindow->DrawWindow();
                            m_town->GiveSpells(0);
                        }
                    }
                } else {
                    m_heroWindow0 = new heroWindow(0, 0, "magewind.bin");
                    if (m_heroWindow0 == 0)
                        MemError();
                    SetWinText(m_heroWindow0, 17);
                    SetupMage(m_heroWindow0);
                    gpWindowManager->DoDialog(
                        m_heroWindow0, MageGuildHandler, 0);
                    delete m_heroWindow0;
                }
                m_town->GiveSpells(0);
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_WELL:
            if (!quickView_k) {
                m_heroWindow0 = new heroWindow(0, 0, "wellwind.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetupWell(m_heroWindow0);
                gpWindowManager->DoDialog(
                    m_heroWindow0, TrueFalseDialogHandler, 0);
                delete m_heroWindow0;
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_THIEVES_GUILD:
            if (!quickView_k) {
                m_heroWindow0 = new heroWindow(0, 0, "thiefwin.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetWinText(m_heroWindow0, 14);
                SetupThievesGuild(m_heroWindow0, -1);
                gpWindowManager->DoDialog(
                    m_heroWindow0, TrueFalseDialogHandler, 0);
                delete m_heroWindow0;
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_TAVERN:
            if (!quickView_k) {
                if (m_town->m_type == TOWN_TYPE_NECROMANCER) {
                    sprintf(description_b,
                            GetBuildingInfo(m_town->m_type,
                                            message.payload.widget.id, 1));
                    NormalDialog(description_b, 1, -1, -1,
                                 m_town->m_type + 19, message.payload.widget.id,
                                 -1, 0, -1, 0);
                } else {
                    DoTavern();
                }
            }
            break;

        case TOWN_COMMAND_UPGRADE_CASTLE:
            if (!quickView_k) {
                if (m_town->m_unknown37 != 0) {
                    NormalDialog(
                        "This town may not be upgraded to a castle.",
                        1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    if (BuyBuild(TOWN_COMMAND_CASTLE,
                                 CanBuy(m_town, TOWN_COMMAND_CASTLE) == 0,
                                 quickView_k))
                        BuildObj(TOWN_COMMAND_CASTLE);
                }
            }
            break;

        case TOWN_COMMAND_DOCK:
            if (!quickView_k) {
                gpWindowManager->BroadcastMessage(
                    TOWN_MESSAGE_SELECT, 5, TOWN_CONTROL_CLOSE,
                    TOWN_INTERFACE_BROADCAST_FLAGS);
                if (gpGame->GetBoatsBuilt() < TOWN_MAX_BOATS &&
                    gpAdvManager->GetCell(m_town->m_boatX,
                                          m_town->m_boatY)->triggerType == 0) {
                    m_heroWindow0 =
                        new heroWindow(0xb1, 0x14, "shipwind.bin");
                    if (m_heroWindow0 == 0)
                        MemError();
                    SetWinText(m_heroWindow0, 12);
                    if (gpGame->m_players[giCurPlayer].resources[RES_GOLD] <
                            TOWN_BOAT_GOLD_COST ||
                        gpGame->m_players[giCurPlayer].resources[RES_WOOD] <
                            TOWN_BOAT_WOOD_COST) {
                        message.type = TOWN_MESSAGE_SELECT;
                        message.payload.widget.command = 5;
                        message.payload.widget.id = TOWN_DIALOG_BUILD_BOAT;
                        message.payload.widget.data.value = 0x1000;
                        m_heroWindow0->BroadcastMessage(message);
                        message.payload.widget.command = 6;
                        message.payload.widget.data.value = 2;
                        m_heroWindow0->BroadcastMessage(message);
                    }
                    gpWindowManager->DoDialog(
                        m_heroWindow0, TrueFalseDialogHandler, 0);
                    delete m_heroWindow0;
                    if (gpWindowManager->m_dialogResult ==
                        TOWN_DIALOG_BUILD_BOAT) {
                        if (gpGame->CreateBoat(m_town->m_boatX,
                                               m_town->m_boatY, 0) != -1) {
                            BuildObj(14);
                            gpGame->m_players[giCurPlayer]
                                .resources[RES_GOLD] -= TOWN_BOAT_GOLD_COST;
                            gpGame->m_players[giCurPlayer]
                                .resources[RES_WOOD] -= TOWN_BOAT_WOOD_COST;
                            m_bankBox->Update(1);
                        } else {
                            LogStr("Can't create boat!");
                        }
                    }
                } else {
                    NormalDialog("Cannot build another boat.",
                                 1, 0xd0, 0x28, -1, 0, -1, 0, -1, 0);
                }
                gpWindowManager->BroadcastMessage(
                    TOWN_MESSAGE_SELECT, 6, TOWN_CONTROL_CLOSE,
                    TOWN_INTERFACE_BROADCAST_FLAGS);
            }
            break;

        case TOWN_COMMAND_TRADING_POST:
            if (!quickView_k) {
                marketplaceCount_m = 0;
                for (index_i = 0; index_i < gpCurPlayer->m_townCount; ++index_i) {
                    if (gpGame->GetTown(gpCurPlayer->TownId(index_i))
                            ->m_buildings & TOWN_BUILDING_MARKETPLACE)
                        ++marketplaceCount_m;
                }
                if (marketplaceCount_m > 10)
                    marketplaceCount_m = 10;
                DoTradingPost(1, fTradingPostEfficency[marketplaceCount_m]);
                RedrawTownScreen();
            }
            break;

        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
        case 15:
            if (quickView_k) {
                sprintf(description_b,
                        GetBuildingInfo(m_town->m_type,
                                        message.payload.widget.id, 1));
                NormalDialog(description_b, 4, -1, -1,
                             m_town->m_type + 19, message.payload.widget.id,
                             -1, 0, -1, 0);
            } else {
                sprintf(description_b,
                        GetBuildingInfo(m_town->m_type, message.payload.widget.id, 1));
                NormalDialog(description_b, 1, -1, -1,
                             m_town->m_type + 19, message.payload.widget.id,
                             -1, 0, -1, 0);
            }
            break;

        case TOWN_CONTROL_CLOSE:
            if (!quickView_k)
                SetCommandAndText(message);
            break;

        default:
            if (quickView_k) {
                int armySelected = 0;
                hero *viewedHero;
                if (message.payload.widget.id >= TOWN_GARRISON_SLOT_FIRST &&
                    message.payload.widget.id <= TOWN_GARRISON_SLOT_LAST) {
                    m_selectedStrip = m_garrisonStrip;
                    m_selectedArmySlot =
                        message.payload.widget.id - TOWN_GARRISON_SLOT_FIRST;
                    armySelected = 1;
                }
                if (message.payload.widget.id >= TOWN_HERO_SLOT_FIRST &&
                    message.payload.widget.id <= TOWN_HERO_SLOT_LAST) {
                    m_selectedStrip = m_heroStrip;
                    m_selectedArmySlot =
                        message.payload.widget.id - TOWN_HERO_SLOT_FIRST;
                    armySelected = 1;
                }
                if (armySelected &&
                    m_selectedStrip->m_army
                            ->m_creatureTypes[m_selectedArmySlot] != -1) {
                    if (m_selectedStrip == m_heroStrip)
                        viewedHero = gpGame->GetHero(
                            m_town->m_occupyingHeroId);
                    else
                        viewedHero = 0;
                    gpGame->ViewArmy(
                        TOWN_ARMY_VIEW_X, TOWN_ARMY_VIEW_Y,
                        m_selectedStrip->m_army
                            ->m_creatureTypes[m_selectedArmySlot],
                        m_selectedStrip->m_army
                            ->m_creatureCounts[m_selectedArmySlot],
                        m_town, 1, 1, 1, viewedHero, 0,
                        m_selectedStrip->m_army, m_selectedArmySlot);
                    m_bankBox->Update(1);
                }
            } else {
                DoCommand(m_command);
                SetCommandAndText(message);
            }
            break;
        }
        break;

        case TOWN_INPUT_DESELECT:
            switch (message.payload.widget.id) {
            case TOWN_CONTROL_PREVIOUS_TOWN:
            case TOWN_CONTROL_NEXT_TOWN:
                if (m_town->m_owner == giCurPlayer &&
                    gpCurPlayer->m_townCount > 1) {
                    int townPosition =
                        gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
                    townPosition = (townPosition +
                                    (message.payload.widget.id == TOWN_CONTROL_PREVIOUS_TOWN
                                         ? -1
                                         : 1) +
                                    gpCurPlayer->m_townCount) %
                                   gpCurPlayer->m_townCount;
                    m_town = gpGame->GetTown(
                        gpCurPlayer->TownId(townPosition));
                    ChangeTown();
                }
                break;

            case TOWN_CONTROL_CLOSE:
                if (!quickView_k)
                    ++exitTown_i;
                break;
            }
            break;
        }
        break;

    case TOWN_MESSAGE_KEY_DOWN:
        switch (message.payload.keyboard.keyCode) {
        case 1:
            ++exitTown_i;
            break;

        case TOWN_KEY_SHIFT_LEFT:
        case TOWN_KEY_SHIFT_RIGHT:
            ShiftQualChange();
            break;
        }
        break;

    case TOWN_MESSAGE_KEY_UP:
        switch (message.payload.keyboard.keyCode) {
        case TOWN_KEY_SHIFT_LEFT:
        case TOWN_KEY_SHIFT_RIGHT:
            ShiftQualChange();
            break;
        }
        break;

    case TOWN_MESSAGE_HOVER:
        gpWindowManager->ConvertToHover(message);
        if (message.payload.hover.id == m_lastHoverId &&
            message.payload.hover.modifiers == m_lastHoverSubId)
            return 1;
        m_lastHoverId = message.payload.hover.id;
        m_lastHoverSubId = message.payload.hover.modifiers;
        SetCommandAndText(message);
        return 1;
    }

    if (exitTown_i == 1) {
        message.type = TOWN_MESSAGE_EXIT;
        message.payload.widget.command = 1;
        return 2;
    }
    return 1;
}

VA(0x0041718d, 0x4e3)
void townManager::DoCommand(int command)
{
    hero *viewedHero;
    int canDismiss;
    int slot;

    switch (command) {
    case TOWN_ARMY_COMMAND_SELECT:
        m_swapStrip = m_selectedStrip;
        m_swapArmySlot = m_selectedArmySlot;
        m_swapStrip->m_selectedSlot = m_swapArmySlot;
        m_swapStrip->Draw();
        break;

    case TOWN_ARMY_COMMAND_VIEW:
        if (m_selectedStrip == m_heroStrip)
            viewedHero = gpGame->GetHero(m_town->m_occupyingHeroId);
        else
            viewedHero = 0;
        if (m_castleDialogActive == 1 ||
            (m_selectedStrip == m_heroStrip &&
             m_selectedStrip->m_army->GetNumArmies() == 1))
            canDismiss = 1;
        else
            canDismiss = 0;
        gpGame->ViewArmy(
            TOWN_ARMY_VIEW_X, TOWN_ARMY_VIEW_Y,
            m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot],
            m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot],
            m_town, canDismiss, 1, 0, viewedHero, 0,
            m_selectedStrip->m_army, m_selectedArmySlot);
        m_bankBox->Update(1);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedStrip->m_army
                ->m_creatureTypes[m_selectedArmySlot] = -1;
            m_selectedStrip->m_army
                ->m_creatureCounts[m_selectedArmySlot] = 0;
        }
        ResetStrips();
        break;

    case TOWN_ARMY_COMMAND_MERGE:
        if (m_pendingStrip != m_swapStrip &&
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] !=
                m_pendingStrip->m_army
                    ->m_creatureTypes[m_pendingArmySlot]) {
            for (slot = 0;
                 slot < TOWN_ARMY_SLOT_COUNT &&
                 m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] !=
                     m_pendingStrip->m_army->m_creatureTypes[slot];
                 ++slot) {
            }
            if (slot < TOWN_ARMY_SLOT_COUNT)
                m_pendingArmySlot = slot;
        }
        m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] +=
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = -1;
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = 0;
        ResetStrips();
        break;

    case TOWN_ARMY_COMMAND_SWAP: {
        short count = m_pendingStrip->m_army
                          ->m_creatureCounts[m_pendingArmySlot];
        m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = count;
        signed char type = m_pendingStrip->m_army
                               ->m_creatureTypes[m_pendingArmySlot];
        m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = type;
        ResetStrips();
        break;
    }

    case TOWN_ARMY_COMMAND_VIEW_HERO:
        HeroView(m_town->m_occupyingHeroId, 1, 0);
        RedrawTownScreen();
        gpWindowManager->FadeScreen(0, 8, 0);
        break;

    case TOWN_ARMY_COMMAND_SPLIT:
        SplitArmy();
        ResetStrips();
        break;
    }
    m_lastHoverId = -1;
}

VA(0x00417670, 0xad)
void townManager::RedrawTownScreen(void)
{
    tag_message message;

    DrawTown(0, 1);
    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 3;
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(0);
    m_garrisonStrip->DrawIcons(0);
    m_heroStrip->DrawIcons(0);
    m_bankBox->Update(0);
    gpWindowManager->UpdateScreenRegion(0, 0, 0x280, 0x1e0);
}

VA(0x0041771d, 0x374)
void townManager::SplitArmy(void)
{
    short unusedValue = 1;
    short unusedDialogValue = 4;
    int sameCreature;
    tag_message message;

    m_heroWindow1 = new heroWindow(0xb1, 0x14, "splitwin.bin");
    if (m_heroWindow1 == 0)
        MemError();
    m_splitAmount = 0;
    m_splitMaximum = m_swapStrip->m_army
                         ->m_creatureCounts[m_swapArmySlot];
    message.type = TOWN_MESSAGE_SELECT;
    if (m_pendingStrip->m_army == m_swapStrip->m_army) {
        sprintf(gText, "Move how many troops?");
    } else {
        sprintf(gText, "Move how many %s troops from %s to %s?",
                gArmyNames[m_swapStrip->m_army
                               ->m_creatureTypes[m_swapArmySlot]],
                m_swapStrip == m_heroStrip ? "Hero's Army" : "Garrison",
                m_pendingStrip == m_heroStrip ? "Hero's Army" : "Garrison");
    }
    message.payload.widget.command = 3;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", m_splitAmount);
    message.payload.widget.id = 4;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow1, SplitArmyHandler, 0);
    delete m_heroWindow1;
    if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
        sameCreature = 0;
        if (m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] ==
            m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot])
            sameCreature = 1;
        if (sameCreature != 0) {
            m_pendingStrip->m_army
                ->m_creatureCounts[m_pendingArmySlot] +=
                m_splitAmount;
        } else {
            m_pendingStrip->m_army
                ->m_creatureTypes[m_pendingArmySlot] =
                m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
            m_pendingStrip->m_army
                ->m_creatureCounts[m_pendingArmySlot] =
                m_splitAmount;
        }
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] -=
            m_splitAmount;
    }
}

VA(0x00417a91, 0x21)
void townManager::ShiftQualChange(void)
{
    gpInputManager->ForceMouseMove();
}

VA(0x00417ab2, 0xb0)
void townManager::ResetStrips(void)
{
    if (m_swapStrip != 0)
        m_swapStrip->m_selectedSlot = -1;
    if (m_pendingStrip != 0)
        m_pendingStrip->m_selectedSlot = -1;
    m_heroStrip->Draw();
    m_garrisonStrip->Draw();
    m_swapStrip = m_pendingStrip = 0;
    m_swapArmySlot = m_pendingArmySlot = -1;
}

VA(0x00417b62, 0x7b)
void townManager::Toggle(int building)
{
    int index;

    if (m_town->m_buildings & (1L << building)) {
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_townObjects[index]->m_buildingId == building)
                m_townObjects[index]->m_visible ^= 1;
        }
    }
}

VA(0x00417bdd, 0xc0)
void townManager::DrawTown(int updateScreen, int drawFlags)
{
    int index;

    PollSound();
    m_backgroundIcon->DrawToBuffer(0, 0, 0, 0);
    PollSound();
    for (index = 0; index < m_townObjectCount; ++index) {
        m_townObjects[index]->Draw(drawFlags);
        PollSound();
    }
    m_townWindow->DrawWindow(0, 0x88, 0x89);
    PollSound();
    if (updateScreen != 0)
        BlitBitmapToScreen(gpWindowManager->m_screen,
                           0, 0, 0x280, 0x100, 0, 0);
    PollSound();
}

VA(0x00417c9d, 0xf35)
int townManager::BuyBuild(int building, int cannotBuy, int quickView)
{
    int mageLevel = 0;
    int index = 0;
    int costCount = 0;
    char *description = static_cast<char *>(BaseAlloc(
        400, "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
        *reinterpret_cast<short *>("\x48\x09") + 8));
    short costs[8];
    signed char resourceTypes[8];
    int dwelling;
    short dialogWidth;
    short dialogHeight;
    short dialogLeft;
    short dialogControl;
    short dialogResult;
    short dialogButtonWidth;
    short dialogButtonCount;
    int widgetIndex;
    int resourceCount;
    int topRowCount;
    int bottomRowCount;
    int prerequisiteCount;
    unsigned long prerequisiteMask;
    int lineCount;
    int windowY;
    int windowHeight;
    int windowRows;
    heroWindow *window;
    tag_message message;
    char iconName[16];
    widget *descriptionWidget;
    icon *resourceIcon;
    int row;
    int rowY;
    int resourcesInRow;
    int rowWidth;
    int nextResource;
    int rowResourceTypes[4];
    int spacing;
    int xStart;
    int x;
    int entryWidth;
    char *amountText[TOWN_RESOURCE_COUNT];
    textWidget *amountWidgets[TOWN_RESOURCE_COUNT];
    iconWidget *resourceWidgets[TOWN_RESOURCE_COUNT];

    for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
        costs[index] = -1;
        resourceTypes[index] = static_cast<signed char>(costs[index]);
    }

    dwelling = -1;
    if (building >= 19 && building <= 30)
        dwelling = building - 19;

    if (building == TOWN_COMMAND_TAVERN &&
        m_town->m_type == TOWN_TYPE_NECROMANCER) {
        for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
            if (xShrineBuildingCost[index] > 0) {
                resourceTypes[costCount] = static_cast<signed char>(index);
                costs[costCount] = static_cast<short>(xShrineBuildingCost[index]);
                ++costCount;
            }
        }
    } else if (building == TOWN_COMMAND_MAGE_GUILD) {
        mageLevel = gpTownManager->m_town->m_buildState;
        for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
            if (gMageBuildingCosts[
                    mageLevel + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                        ? mageLevel + 1
                        : TOWN_MAGE_GUILD_MAX_LEVEL][index] > 0) {
                resourceTypes[costCount] = static_cast<signed char>(index);
                costs[costCount] = static_cast<short>(
                    gMageBuildingCosts[
                        mageLevel + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                            ? mageLevel + 1
                            : TOWN_MAGE_GUILD_MAX_LEVEL][index]);
                ++costCount;
            }
        }
    } else if (building == TOWN_COMMAND_SPECIAL_BUILDING) {
        for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
            if (gSpecialBuildingCosts[gpTownManager->m_town->m_type][index] > 0) {
                resourceTypes[costCount] = static_cast<signed char>(index);
                costs[costCount] = static_cast<short>(
                    gSpecialBuildingCosts[gpTownManager->m_town->m_type][index]);
                ++costCount;
            }
        }
    } else if (building <= TOWN_COMMAND_LAST_NEUTRAL_BUILDING) {
        for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
            if (gNeutralBuildingCosts[building][index] > 0) {
                resourceTypes[costCount] = static_cast<signed char>(index);
                costs[costCount] = static_cast<short>(
                    gNeutralBuildingCosts[building][index]);
                ++costCount;
            }
        }
    } else {
        for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
            if (gDwellingCosts[gpTownManager->m_town->m_type]
                              [dwelling][index] > 0) {
                resourceTypes[costCount] = static_cast<signed char>(index);
                costs[costCount] = static_cast<short>(
                    gDwellingCosts[gpTownManager->m_town->m_type]
                                  [dwelling][index]);
                ++costCount;
            }
        }
    }

    dialogWidth = 0x50;
    dialogHeight = 0x28;
    dialogLeft = 0x20;
    dialogControl = 0x121;
    dialogResult = 0;
    dialogButtonWidth = 2;
    dialogButtonCount = 3;
    widgetIndex = 0;
    resourceCount = 0;
    topRowCount = 0;
    bottomRowCount = 0;

    for (index = 0; index < TOWN_RESOURCE_COUNT; ++index) {
        if (resourceTypes[index] != -1)
            ++resourceCount;
    }
    if (resourceCount < 5) {
        topRowCount = resourceCount;
    } else if (resourceCount == 5) {
        topRowCount = 2;
        bottomRowCount = 3;
    } else if (resourceCount == 6) {
        topRowCount = 3;
        bottomRowCount = 3;
    } else if (resourceCount == TOWN_RESOURCE_COUNT) {
        topRowCount = 3;
        bottomRowCount = 4;
    }

    sprintf(description, GetBuildingInfo(m_town->m_type, building, 0));
    if (dwelling >= 0) {
        prerequisiteCount = 0;
        prerequisiteMask = gHierarchyMask[m_town->m_type][dwelling];
        for (index = 0; index < 32; ++index) {
            if (prerequisiteMask & (1L << index)) {
                if (prerequisiteCount == 0)
                    strcat(description, "\n\nRequires:");
                ++prerequisiteCount;
                strcat(description, "\n");
                strcat(description, GetBuildingName(m_town->m_type, index));
            }
        }
        if (m_town->m_type == TOWN_TYPE_NECROMANCER &&
            building == TOWN_COMMAND_NECROMANCER_MAGE_GUILD_PREREQUISITE &&
            m_town->m_buildState <= 2)
            strcat(description, "\nLevel 2 Mage Guild");
    }
    strcat(description, "\n ");

    lineCount = bigFont->LineLength(description, 0xf0);
    windowY = 0x97;
    windowHeight = windowY;
    windowHeight += lineCount << 4;
    if (resourceCount <= 4)
        windowHeight += 0x2c;
    else
        windowHeight += 0x58;
    if (quickView == 0)
        windowHeight += 0x27;
    windowRows = (windowHeight - 0x45) / 0x2d;
    if (windowRows < 3)
        windowRows = 3;
    if (windowRows > 6)
        windowRows = 6;

    sprintf(gText, "buybuil%d.bin", windowRows);
    window = new heroWindow(0x9e, 0x10, gText);
    if (window == 0)
        MemError();

    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 9;
    message.payload.widget.id = 2;
    sprintf(iconName, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message.payload.widget.data.text = iconName;
    window->BroadcastMessage(message);
    message.payload.widget.command = 4;
    message.payload.widget.id = 2;
    message.payload.widget.data.value = building;
    window->BroadcastMessage(message);

    if (building == TOWN_COMMAND_MAGE_GUILD) {
        sprintf(gText, "Mage Guild, Level %d",
                mageLevel + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                    ? mageLevel + 1
                    : TOWN_MAGE_GUILD_MAX_LEVEL);
    } else {
        strcpy(gText, GetBuildingName(m_town->m_type, building));
    }
    message.payload.widget.command = 3;
    message.payload.widget.id = 3;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    descriptionWidget = new textWidget(
        0x2b, static_cast<short>(windowY + 0x18), 0xf0,
        static_cast<short>(lineCount << 4), description, "bigfont.fnt",
        1, -1, 8, 1);
    if (descriptionWidget == 0)
        MemError();
    window->AddWidget(descriptionWidget, -1);

    widgetIndex = 0;
    resourceIcon = gpResourceManager->GetIcon("resource.icn");
    for (row = 0; row < 2; ++row) {
        rowY = row * 0x2c + lineCount * 0x10 + windowY + 0xc;
        if (row == 0)
            resourcesInRow = topRowCount;
        else
            resourcesInRow = bottomRowCount;
        if (resourcesInRow > 0) {
            rowWidth = 0;
            nextResource = widgetIndex;
            for (index = 0; index < 4; ++index) {
                if (index < resourcesInRow) {
                    while (resourceTypes[nextResource] == -1)
                        ++nextResource;
                    rowResourceTypes[index] = resourceTypes[nextResource];
                    ++nextResource;
                } else {
                    rowResourceTypes[index] = -1;
                }
            }
            for (index = 0; index < resourcesInRow; ++index) {
                rowWidth += GetIconEntry(
                    resourceIcon, rowResourceTypes[index])->w;
            }
            spacing = (0x100 - rowWidth) / (resourcesInRow + 1);
            xStart = spacing + 0x20;
            x = xStart;
            for (index = 0; index < resourcesInRow; ++index) {
                entryWidth = GetIconEntry(
                    resourceIcon, rowResourceTypes[index])->w;
                amountText[widgetIndex] = static_cast<char *>(BaseAlloc(
                    10, "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                    *reinterpret_cast<short *>("\x48\x09") + 0x128));
                sprintf(amountText[widgetIndex], "%d", costs[widgetIndex]);
                amountWidgets[widgetIndex] = new textWidget(
                    static_cast<short>(x), static_cast<short>(rowY + 0x23),
                    static_cast<short>(entryWidth), 0xc,
                    amountText[widgetIndex], "smalfont.fnt", 1, -1, 8, 1);
                if (amountWidgets[widgetIndex] == 0)
                    MemError();
                resourceWidgets[widgetIndex] = new iconWidget(
                    static_cast<short>(
                        x - GetIconEntry(resourceIcon,
                                         rowResourceTypes[index])->x),
                    static_cast<short>(rowY),
                    static_cast<short>(entryWidth), 0xc, "resource.icn",
                    resourceTypes[widgetIndex], 0, -1, 0x10, 1);
                if (resourceWidgets[widgetIndex] == 0)
                    MemError();
                window->AddWidget(amountWidgets[widgetIndex], -1);
                window->AddWidget(resourceWidgets[widgetIndex], -1);
                ++widgetIndex;
                x += spacing + entryWidth;
            }
        }
    }
    gpResourceManager->Dispose(resourceIcon);

    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            TOWN_MESSAGE_SELECT, 5, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
    m_selectedBuilding = -1;
    if (quickView != 0) {
        message.payload.widget.command = 6;
        message.payload.widget.data.value = 6;
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        window->BroadcastMessage(message);
        message.payload.widget.command = 6;
        message.payload.widget.data.value = 6;
        message.payload.widget.id = 0x7801;
        window->BroadcastMessage(message);
        message.payload.widget.command = 6;
        message.payload.widget.data.value = 6;
        message.payload.widget.id = 0;
        window->BroadcastMessage(message);
        gpWindowManager->AddWindow(window, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(window);
    } else {
        if (cannotBuy != 0) {
            message.payload.widget.command = 6;
            message.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message.payload.widget.data.value = 2;
            window->BroadcastMessage(message);
            message.payload.widget.command = 5;
            message.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message.payload.widget.data.value = 0x1000;
            window->BroadcastMessage(message);
        }
        gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedBuilding = building;
            for (index = 0; index < resourceCount; ++index)
                gpCurPlayer->m_resources[resourceTypes[index]] -= costs[index];
        }
    }
    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            TOWN_MESSAGE_SELECT, 6, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
    delete window;
    if (quickView != 0)
        return 0;
    return gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM;
}

VA(0x00418bd2, 0x3e9)
void townManager::BuildObj(int building)
{
    int objectIndex;
    SLimitData limits;
    int index;
    SAMPLE2 buildSample;
    int frame;

    if (((!(m_town->m_buildings & (1L << building))) ||
         (building == TOWN_COMMAND_MAGE_GUILD &&
          m_town->m_buildState != TOWN_MAGE_GUILD_MAX_LEVEL)) &&
        (building != TOWN_COMMAND_DOCK || m_town->CanBuildDock())) {
        DrawTown(1, 1);
        m_town->BuildBuilding(building);
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_town->m_buildings &
                (1L << m_townObjects[index]->m_buildingId)) {
                m_townObjects[index]->m_visible = 1;
                m_townObjects[index]->m_border->m_flags |=
                    TOWN_OBJECT_BORDER_ENABLED;
            } else {
                m_townObjects[index]->m_visible = 0;
                m_townObjects[index]->m_border->m_flags &=
                    ~TOWN_OBJECT_BORDER_ENABLED;
            }
        }

        objectIndex = -1;
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_townObjects[index]->m_buildingId == building)
                objectIndex = index;
        }

        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        giMinExtentX = 0x27f;
        giMinExtentY = 0xff;
        gbComputeExtent = 1;
        gbSaveBiggestExtent = 1;
        gbReturnAfterComputeExtent = 1;
        if (building == TOWN_COMMAND_MAGE_GUILD) {
            if (gpTownManager->m_town->m_type == TOWN_TYPE_NECROMANCER)
                frame = (gpTownManager->m_town->m_buildState * 3 - 3) << 1;
            else
                frame = gpTownManager->m_town->m_buildState - 1;
            m_townObjects[objectIndex]->m_icon->CombatClipDrawToBuffer(
                0, 0, frame, &limits, 0, 0, 0, 0);
            if (m_townObjects[objectIndex]->m_animationFrameCount != 0)
                m_townObjects[objectIndex]->m_icon->CombatClipDrawToBuffer(
                    0, 0, frame + 1, &limits, 0, 0, 0, 0);
        } else {
            m_townObjects[objectIndex]->m_icon->CombatClipDrawToBuffer(
                0, 0, 0, &limits, 0, 0, 0, 0);
            if (m_townObjects[objectIndex]->m_animationFrameCount != 0)
                m_townObjects[objectIndex]->m_icon->CombatClipDrawToBuffer(
                    0, 0, 1, &limits, 0, 0, 0, 0);
        }
        gbComputeExtent = 0;
        gbSaveBiggestExtent = 0;
        gbReturnAfterComputeExtent = 0;

        gpWindowManager->SaveFizzleSource(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        DrawTown(0, 1);
        buildSample = NULL_SAMPLE2;
        buildSample = LoadPlaySample("buildtwn.82M");
        gpWindowManager->FizzleForward(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1, -1, 0, 0);
        WaitEndSample(buildSample, -1);
        PollSound();
        m_selectedBuilding = -1;
        gpWindowManager->BroadcastMessage(
            TOWN_MESSAGE_SELECT, 6, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
        RedrawTownScreen();
    }
}

VA(0x00418fbb, 0x3d8)
void townManager::SetupMage(heroWindow *window)
{
    short unusedZero = 0;
    short unusedAvailable = 1;
    short unusedInvalid = TOWN_MAGE_SPELL_UNAVAILABLE;
    short unusedIconState = 2;
    short unusedFirstSpell = TOWN_MAGE_FIRST_SPELL_CONTROL;
    short unusedFirstIcon = TOWN_MAGE_FIRST_ICON_CONTROL;
    short unusedFirstDescription = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    short unusedGuildIcon = TOWN_MAGE_GUILD_ICON_CONTROL;
    short unusedDescription = TOWN_MAGE_DESCRIPTION_CONTROL;
    tag_message message;
    int level;
    int slot;
    int spellState;
    int hasLibrary;
    int lineCount;
    int unusedGuildFrame;

    message.type = TOWN_MESSAGE_SELECT;
    if (m_town->m_occupyingHeroId == -1) {
        strcpy(gText, "The above spells are available here.");
        message.payload.widget.command = 3;
        message.payload.widget.id = TOWN_MAGE_DESCRIPTION_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }

    for (level = 0; level < TOWN_MAGE_GUILD_MAX_LEVEL; ++level) {
        for (slot = 0; slot < TOWN_MAGE_SPELLS_PER_LEVEL; ++slot) {
            if (m_town->m_type == TOWN_TYPE_WIZARD &&
                (m_town->m_buildings & TOWN_WIZARD_LIBRARY_BUILDING_FLAG))
                hasLibrary = 1;
            else
                hasLibrary = 0;

            if (gSpellLimits[level] + hasLibrary <= slot) {
                spellState = TOWN_MAGE_SPELL_UNAVAILABLE;
            } else if (m_town->m_spellCounts[level + 1] <= slot) {
                spellState = 1;
            } else {
                spellState = 0;
            }

            if (spellState == TOWN_MAGE_SPELL_UNAVAILABLE)
                message.payload.widget.command = 6;
            else
                message.payload.widget.command = 5;
            message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                             TOWN_MAGE_FIRST_SPELL_CONTROL;
            message.payload.widget.data.value = 4;
            window->BroadcastMessage(message);

            if (spellState != TOWN_MAGE_SPELL_UNAVAILABLE) {
                message.payload.widget.command = 4;
                message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                                 TOWN_MAGE_FIRST_SPELL_CONTROL;
                message.payload.widget.data.value = spellState;
                window->BroadcastMessage(message);
            }

            if (spellState != 0) {
                message.payload.widget.command = 6;
                message.payload.widget.data.value = 4;
                message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                                 TOWN_MAGE_FIRST_ICON_CONTROL;
                window->BroadcastMessage(message);
                message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                                 TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = 4;
                message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                                 TOWN_MAGE_FIRST_ICON_CONTROL;
                message.payload.widget.data.value = gsSpellInfo[
                    m_town->m_spells[level][slot]].iconIndex;
                window->BroadcastMessage(message);
                lineCount = smallFont->LineLength(
                    gSpellNames[m_town->m_spells[level][slot]], 0x4a);
                if (lineCount == 1)
                    sprintf(gText, "%s\n[%d]",
                            gSpellNames[m_town->m_spells[level][slot]],
                            GetManaCost(m_town->m_spells[level][slot], 0));
                else
                    sprintf(gText, "%s  [%d]",
                            gSpellNames[m_town->m_spells[level][slot]],
                            GetManaCost(m_town->m_spells[level][slot], 0));
                message.payload.widget.command = 3;
                message.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot +
                                 TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                message.payload.widget.data.text = gText;
                window->BroadcastMessage(message);
            }
        }
    }

    message.payload.widget.data.value = m_town->m_buildState - 1;
    message.payload.widget.command = 4;
    message.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    unusedGuildFrame = message.payload.widget.data.value;
    window->BroadcastMessage(message);
    sprintf(gText, "magegld%c.icn", cHeroTypeInitial[m_town->m_type]);
    message.payload.widget.command = 9;
    message.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
}

VA(0x00419393, 0x190)
int MageGuildHandler(tag_message &message)
{
    short unusedFirstSpell = TOWN_MAGE_FIRST_SPELL_CONTROL;
    short unusedFirstIcon = TOWN_MAGE_FIRST_ICON_CONTROL;
    short unusedFirstDescription = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    int action;
    int quickView;
    int spellSlot;
    int level;
    int slot;
    int spell;

    if (message.type == TOWN_MESSAGE_SELECT) {
        action = message.payload.widget.command;
        switch (action) {
        case TOWN_INPUT_SELECT:
        case TOWN_INPUT_ALTERNATE_SELECT:
            quickView = message.payload.widget.parameter & TOWN_QUICK_VIEW_MODIFIER;
            spellSlot = -1;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_SPELL_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_SPELL_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_SPELL_CONTROL;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_ICON_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_ICON_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_ICON_CONTROL;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_DESCRIPTION_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_DESCRIPTION_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id -
                            TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
            if (spellSlot != -1) {
                level = spellSlot / TOWN_MAGE_SPELLS_PER_LEVEL;
                slot = spellSlot % TOWN_MAGE_SPELLS_PER_LEVEL;
                if (gpTownManager->m_town->m_spellCounts[level + 1] <= slot)
                    return 1;
                spell = gpTownManager->m_town->m_spells[level][slot];
                NormalDialog(gSpellDesc[spell],
                             quickView < 1 ? 1 : 4,
                             -1, -1, 8, spell, -1, 0, -1, 0);
                return 1;
            }
            break;
        }
    }
    return EventWindowHandler(message);
}

VA(0x00419523, 0x706)
int townManager::RecruitHero(int availableHeroIndex, int cannotRecruit)
{
    short unusedTextControl = 3;
    short unusedPortraitControl = 7;
    short unusedTextState = 1;
    short unusedPortraitState = 2;
    short unusedIconState = 4;
    short unusedDimState = 6;
    short unusedButtonText = 8;
    short unusedButtonIcon = 9;
    tag_message message;
    int artifactCount;
    int index;
    int townX;
    int townY;
    int newHeroClass;

    m_heroWindow1 = new heroWindow(0xb1, 0x10, "rcrthero.bin");
    if (m_heroWindow1 == 0)
        MemError();
    SetWinText(m_heroWindow1, 0x14);
    m_recruitHero = &gpGame->m_heroRecs[
        gpCurPlayer->AvailableHeroId(availableHeroIndex)];
    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    message.type = TOWN_MESSAGE_SELECT;

    if (cannotRecruit != 0) {
        message.payload.widget.command = 6;
        message.payload.widget.data.value = 2;
        message.payload.widget.id = 8;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = 9;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.command = 5;
        message.payload.widget.data.value = 0x1000;
        message.payload.widget.id = 8;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = 9;
        m_heroWindow1->BroadcastMessage(message);
        message.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(message);
    }

    artifactCount = 0;
    for (index = 0; index < TOWN_MAX_ARTIFACTS; ++index) {
        if (m_recruitHero->m_artifacts[index] != -1 &&
            m_recruitHero->m_artifacts[index] != TOWN_SPELL_BOOK_ARTIFACT)
            ++artifactCount;
    }
    sprintf(gText, "%s is a level %d %s with %d artifacts.",
            m_recruitHero->m_name, m_recruitHero->m_level,
            gAlignmentNames[m_recruitHero->m_cursorType], artifactCount);
    message.payload.widget.command = 3;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "port%04d.icn", m_recruitHero->m_portrait);
    message.payload.widget.command = 9;
    message.payload.widget.id = 2;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);

    m_recruitState = -1;
    gpWindowManager->DoDialog(m_heroWindow1, RecruitHeroHandler, 0);
    delete m_heroWindow1;
    if (m_recruitState != -1) {
        m_recruitState = availableHeroIndex;
        gpCurPlayer->m_resources[RES_GOLD] -= gHeroGoldCost;
        gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        ++gpCurPlayer->m_heroCount;

        townX = m_town->m_x;
        townY = m_town->m_y;
        m_recruitHero->m_x = townX;
        m_recruitHero->m_y = townY;
        m_recruitHero->m_eventFlags = 0;
        m_recruitHero->m_direction = 2;
        m_recruitHero->m_remainingMobility = m_recruitHero->CalcMobility();
        m_recruitHero->m_mobility = m_recruitHero->m_remainingMobility;
        m_recruitHero->m_locationType =
            gpGame->m_worldMap.GetCell(townX, townY)->triggerType;
        m_recruitHero->m_occupiedTown =
            gpGame->m_worldMap.GetCell(townX, townY)->w4hi;
        gpGame->m_worldMap.GetCell(townX, townY)->triggerType =
            AI_OBJECT_HERO;
        gpGame->m_worldMap.GetCell(townX, townY)->w4hi =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        SendMapChange(3, m_recruitHero->m_id, townX, townY,
                      TOWN_MAP_CHANGE_UNUSED, 0, 0);
        m_recruitResult = 1;
        m_town->m_occupyingHeroId = m_recruitHero->m_id;
        gpGame->m_availableHeroes[
            gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            static_cast<signed char>(giCurPlayer);
        CheckValidAvailableHeroes();
        if (m_town->m_buildings & 1)
            m_town->GiveSpells(0);

        newHeroClass =
            gpCurPlayer->m_availableHeroIds[1 - m_recruitState] / 9;
        newHeroClass = Random(1, 5) + newHeroClass;
        newHeroClass %= TOWN_HERO_CLASS_COUNT;
        gpCurPlayer->m_availableHeroIds[m_recruitState] =
            static_cast<signed char>(
                gpGame->GetNewHeroId(giCurPlayer, newHeroClass, 0));
        gpGame->m_availableHeroes[
            gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            AI_HERO_AVAILABLE_FLAG;
    } else {
        if (m_castleDialogActive != 0)
            SetupCastle(m_heroWindow0, 0);
        if (m_castleDialogActive != 0)
            m_heroWindow0->DrawWindow();
    }

    m_bankBox->Update(1);
    gpWindowManager->BroadcastMessage(
        TOWN_MESSAGE_SELECT, 6, TOWN_CONTROL_CLOSE,
        TOWN_INTERFACE_BROADCAST_FLAGS);
    m_recruitHero->m_owner = -1;
    if (m_recruitState != -1)
        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    return m_recruitState != -1;
}

VA(0x00419c29, 0x153)
int TavernHandler(tag_message &message)
{
    int unusedDelay = TOWN_TAVERN_ANIMATION_DELAY;
    short unusedFirstFrame = TOWN_TAVERN_FIRST_ANIMATION_FRAME;
    int action;
    int control;

    if (message.type == TOWN_MESSAGE_SELECT) {
        action = message.payload.widget.command;
        switch (action) {
        case TOWN_INPUT_DESELECT:
            control = message.payload.widget.id;
            switch (control) {
            case 0x7800:
            case 0x7801:
            case TOWN_DIALOG_CONFIRM:
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                message.payload.widget.id = 10;
                message.payload.widget.command = message.payload.widget.id;
                return 2;
            }
            break;
        }
    }
    if (static_cast<long>(KBTickCount()) > glTimers[0]) {
        message.type = TOWN_MESSAGE_SELECT;
        message.payload.widget.command = 4;
        message.payload.widget.id = 2;
        ++gpGame->m_viewArmyResult;
        message.payload.widget.data.value =
            gpGame->m_viewArmyResult % TOWN_TAVERN_ANIMATION_FRAME_COUNT +
            TOWN_TAVERN_FIRST_ANIMATION_FRAME;
        gpTownManager->m_heroWindow0->BroadcastMessage(message);
        gpTownManager->m_heroWindow0->MoveWindow(0, 0);
        glTimers[0] = static_cast<int>(KBTickCount() +
                                       TOWN_TAVERN_ANIMATION_DELAY);
    }
    return 1;
}

VA(0x00419d7c, 0x110)
void townManager::DoTavern(void)
{
    int unusedValue = 0;
    tag_message message;

    m_heroWindow0 = new heroWindow(0xa2, 10, "tavwin.bin");
    if (m_heroWindow0 == 0)
        MemError();
    SetWinText(m_heroWindow0, 0x16);
    sprintf(gText,
            "A generous tip for the barkeep yields the following rumor:\n\n%s",
            gpGame->m_rumour);
    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 3;
    message.payload.widget.id = TOWN_TAVERN_RUMOUR_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow0->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow0, TavernHandler, 0);
    delete m_heroWindow0;
}

VA(0x00419e8c, 0x328)
int SplitArmyHandler(tag_message &message)
{
    short unusedAmountControl = TOWN_SPLIT_AMOUNT_CONTROL;
    short unusedIncreaseControl = TOWN_SPLIT_INCREASE_CONTROL;
    short unusedDecreaseControl = TOWN_SPLIT_DECREASE_CONTROL;
    int handled = 0;
    int action;
    int control;

    if (message.type == TOWN_MESSAGE_SELECT) {
        action = message.payload.widget.command;
        switch (action) {
        case TOWN_INPUT_SELECT:
            control = message.payload.widget.id;
            switch (control) {
            case TOWN_SPLIT_AMOUNT_CONTROL:
                message.payload.widget.command = 7;
                gpTownManager->m_heroWindow1->BroadcastMessage(message);
                gpTownManager->m_splitAmount = atoi(message.payload.widget.data.text);
                if (gpTownManager->m_splitAmount < 0)
                    gpTownManager->m_splitAmount = 0;
                if (gpTownManager->m_splitAmount >=
                    gpTownManager->m_splitMaximum)
                    gpTownManager->m_splitAmount =
                        gpTownManager->m_splitMaximum - 1;
                goto update_amount;
            }
            break;
        case TOWN_INPUT_DESELECT:
            control = message.payload.widget.id;
            switch (control) {
            case TOWN_SPLIT_INCREASE_CONTROL:
                ++gpTownManager->m_splitAmount;
                if (gpTownManager->m_splitAmount >=
                    gpTownManager->m_splitMaximum)
                    gpTownManager->m_splitAmount =
                        gpTownManager->m_splitMaximum - 1;
                goto update_amount;
            case TOWN_SPLIT_DECREASE_CONTROL:
                --gpTownManager->m_splitAmount;
                if (gpTownManager->m_splitAmount < 0)
                    gpTownManager->m_splitAmount = 0;
                goto update_amount;
            case 0x7800:
            case 0x7801:
                gpTownManager->m_splitAmount = 0;
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                handled = 1;
                break;
            case TOWN_DIALOG_CONFIRM:
                if (gpTownManager->m_splitAmount == 0)
                    gpWindowManager->m_dialogResult = 0x7801;
                else
                    gpWindowManager->m_dialogResult = TOWN_DIALOG_CONFIRM;
                handled = 1;
                break;
            default:
                break;
            }
            break;
        }
    }

    if (handled != 0) {
        message.payload.widget.id = 10;
        message.payload.widget.command = message.payload.widget.id;
        return 2;
    }
    return 1;

update_amount:
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 3;
    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpTownManager->m_heroWindow1->DrawWindow(
        1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
    return 1;
}

// @early-stop
// reloc-masked: instruction streams identical; only local-string/interior-global relocation identities differ
VA(0x0041a1b4, 0x5cf)
void townManager::SetupWell(heroWindow *window)
{
    short unusedFirstIcon = 1;
    short unusedFirstName = TOWN_WELL_FIRST_NAME_CONTROL;
    short unusedFirstMonsterIcon = TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
    short unusedFirstCreature = TOWN_WELL_FIRST_CREATURE_CONTROL;
    short unusedFirstDetail = TOWN_WELL_FIRST_DETAIL_CONTROL;
    short unusedFirstAvailable = TOWN_WELL_FIRST_AVAILABLE_CONTROL;
    short unusedFirstAvailableCount = TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
    unsigned char dwellingTypes[8];
    int available;
    int dwelling;
    tag_message message;
    char iconName[16];
    char detailText[40];
    tag_monsterInfo monsterInfo;
    int growth;

    for (dwelling = 0; dwelling < TOWN_WELL_DWELLING_COUNT; ++dwelling) {
        if (dwelling == TOWN_WELL_DWELLING_COUNT - 1 &&
            (m_town->m_buildings &
             (1L << TOWN_WELL_LAST_UPGRADE_BUILDING))) {
            dwellingTypes[dwelling] = TOWN_WELL_DWELLING_COUNT * 2 - 1;
        } else if (dwelling >= 1 &&
                   (m_town->m_buildings &
                    (1L << (dwelling + TOWN_WELL_FIRST_UPGRADE_BUILDING)))) {
            dwellingTypes[dwelling] = static_cast<unsigned char>(
                dwelling + TOWN_WELL_FIRST_UPGRADE_OFFSET);
        } else {
            dwellingTypes[dwelling] = static_cast<unsigned char>(dwelling);
        }
    }

    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 9;
    sprintf(iconName, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message.payload.widget.data.text = iconName;
    for (dwelling = 0; dwelling < TOWN_WELL_DWELLING_COUNT; ++dwelling) {
        message.payload.widget.id = dwelling + 1;
        window->BroadcastMessage(message);
    }

    for (dwelling = 0; dwelling < TOWN_WELL_DWELLING_COUNT; ++dwelling) {
        message.payload.widget.command = 4;
        message.payload.widget.id = dwelling + 1;
        message.payload.widget.data.value = dwellingTypes[dwelling] +
                          TOWN_COMMAND_FIRST_DWELLING;
        window->BroadcastMessage(message);
        sprintf(gText, "monh%04d.icn",
                gDwellingType[m_town->m_type][dwellingTypes[dwelling]]);
        message.payload.widget.command = 9;
        message.payload.widget.id = dwelling + TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }

    message.type = TOWN_MESSAGE_SELECT;
    message.payload.widget.command = 3;
    for (dwelling = 0; dwelling < TOWN_WELL_DWELLING_COUNT; ++dwelling) {
        sprintf(gText, GetBuildingName(
            m_town->m_type,
            dwellingTypes[dwelling] + TOWN_COMMAND_FIRST_DWELLING));
        message.payload.widget.id = dwelling + TOWN_WELL_FIRST_NAME_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);

        if (m_town->m_buildings &
            (1L << (dwellingTypes[dwelling] +
                    TOWN_COMMAND_FIRST_DWELLING))) {
            available = m_town->m_garrison[dwellingTypes[dwelling]];
            sprintf(gText, "Available:");
            message.payload.widget.id = dwelling + TOWN_WELL_FIRST_AVAILABLE_CONTROL;
            message.payload.widget.data.text = gText;
            window->BroadcastMessage(message);
            sprintf(gText, "%d", available);
            message.payload.widget.id = dwelling +
                             TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
            message.payload.widget.data.text = gText;
            window->BroadcastMessage(message);
        }

        message.payload.widget.id = dwelling + TOWN_WELL_FIRST_CREATURE_CONTROL;
        strcpy(gText, gArmyNamesPlural[
            gDwellingType[m_town->m_type][dwellingTypes[dwelling]]]);
        gText[0] -= ' ';
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }

    for (dwelling = 0; dwelling < TOWN_WELL_DWELLING_COUNT; ++dwelling) {
        monsterInfo = gMonsterDatabase[
            gDwellingType[m_town->m_type][dwellingTypes[dwelling]]];
        strcpy(gText, "");
        sprintf(detailText, "%s%d", cWellDetail[0], monsterInfo.attack);
        strcat(gText, detailText);
        sprintf(detailText, "\n%s%d", cWellDetail[1], monsterInfo.defense);
        strcat(gText, detailText);
        sprintf(detailText, "\n%s%d", cWellDetail[3], monsterInfo.damageMin);
        strcat(gText, detailText);
        if (monsterInfo.damageMin != monsterInfo.damageMax) {
            sprintf(detailText, "-%d", monsterInfo.damageMax);
            strcat(gText, detailText);
        }
        sprintf(detailText, "\n%s%d", cWellDetail[4], monsterInfo.hitPoints);
        strcat(gText, detailText);
        sprintf(detailText, cWellDetail[7], speedText[monsterInfo.speed]);
        strcat(gText, detailText);
        if (m_town->m_buildings &
            (1L << (dwellingTypes[dwelling] +
                    TOWN_COMMAND_FIRST_DWELLING))) {
            growth = gMonsterDatabase[
                gDwellingType[m_town->m_type][dwellingTypes[dwelling]]].growth;
            growth += TOWN_WELL_BASE_GROWTH_BONUS;
            if (dwelling == 0 &&
                (m_town->m_buildings &
                 (1L << TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING)))
                growth += TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS;
            sprintf(detailText, cWellDetail[8], growth);
            strcat(gText, detailText);
        }
        message.payload.widget.command = 3;
        message.payload.widget.id = dwelling + TOWN_WELL_FIRST_DETAIL_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);
    }
}

VA(0x0041a783, 0xf0f)
void townManager::SetupThievesGuild(heroWindow *window, int informationLevel)
{
    short unusedRankX = 0x102;
    short unusedRankWidth = 0x44;
    short unusedRankY = 0x1b;
    short unusedRankHeight = 0x18;
    short unusedIconWidth = 0x12;
    short unusedPlayerWidth = 0x48;
    short unusedFirstRankControl = TOWN_THIEVES_FIRST_RANK_CONTROL;
    short unusedFirstPlayerControl = TOWN_THIEVES_FIRST_PLAYER_CONTROL;
    short unusedCreatureY = 0x1a2;
    int maxCategories;
    int category;
    long categoryStats[TOWN_THIEVES_PLAYER_COUNT];
    signed char categoryOrder[TOWN_THIEVES_ORDER_BUFFER_SIZE];
    int rank;
    int firstAtRank;
    int lastAtRank;
    int tiedCount;
    int rankX;
    int position;
    widget *iconControl;
    tag_message message;
    int playerIndex;
    int displayPlayer;
    int heroPosition;
    int strongestHeroPosition;
    int strongestHeroValue;
    int heroValue;
    hero *strongestHero;
    char *widgetText;
    widget *textControl;
    char statText[200];
    int stat;
    int townPosition;
    int armySlot;
    town *playerTown;
    int strongestCreature;
    int strongestCreatureValue;

    if (informationLevel == -1)
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);

    if (informationLevel >= TOWN_THIEVES_INFO_ALL_CATEGORIES) {
        maxCategories = TOWN_THIEVES_MAX_CATEGORIES;
    } else {
        if (informationLevel == TOWN_THIEVES_INFO_STRONGEST_CREATURE)
            maxCategories = TOWN_THIEVES_CREATURE_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PERSONALITY)
            maxCategories = TOWN_THIEVES_PERSONALITY_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PRIMARY_STATS)
            maxCategories = TOWN_THIEVES_PRIMARY_CATEGORY_COUNT;
        else
            maxCategories = TOWN_THIEVES_BASIC_CATEGORY_COUNT;
    }

    for (position = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position < TOWN_THIEVES_PLAYER_COUNT; ++position) {
        message.type = TOWN_MESSAGE_SELECT;
        message.payload.widget.command = 6;
        message.payload.widget.id = position + TOWN_THIEVES_FIRST_RANK_CONTROL;
        message.payload.widget.data.value = 4;
        window->BroadcastMessage(message);
        message.payload.widget.id = position + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        window->BroadcastMessage(message);
    }
    for (position = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position < TOWN_THIEVES_PLAYER_COUNT; ++position) {
        message.type = TOWN_MESSAGE_SELECT;
        message.payload.widget.command = 6;
        message.payload.widget.id = position + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message.payload.widget.data.value = 4;
        window->BroadcastMessage(message);
    }

    for (category = 0; category < maxCategories; ++category) {
        GetCategoryStats(category, categoryStats, categoryOrder);
        SortStats(categoryStats, categoryOrder);
        firstAtRank = 0;
        for (rank = 0;
             rank < TOWN_THIEVES_PLAYER_COUNT &&
             gpGame->m_playerCount - gpGame->m_deadPlayerCount !=
                 firstAtRank;
             ++rank) {
            tiedCount = 1;
            for (lastAtRank = firstAtRank;
                 lastAtRank + 1 < gpGame->m_playerCount &&
                 categoryStats[lastAtRank] == categoryStats[lastAtRank + 1];
                 ++lastAtRank)
                ++tiedCount;
            rankX = rank * 0x44 + 0x102 - (tiedCount * 9 - 9);
            for (position = firstAtRank; position <= lastAtRank; ++position) {
                iconControl = new iconWidget(
                    static_cast<short>(
                        (position - firstAtRank) * 0x12 + rankX),
                    static_cast<short>(category * 0x18 + 0x1b),
                    0x12, 0x16, "townwind.icn",
                    static_cast<short>(
                        gpGame->m_players[categoryOrder[position]].color +
                        TOWN_THIEVES_RANK_ICON_FRAME_BASE),
                    0, -1, 0x10, 1);
                if (iconControl == 0)
                    MemError();
                window->AddWidget(iconControl, -1);
            }
            firstAtRank = lastAtRank + 1;
        }
    }

    playerIndex = 0;
    for (displayPlayer = 0;
         displayPlayer <
             gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         ++displayPlayer) {
        while (gpGame->m_playerDead[playerIndex] != 0)
            ++playerIndex;
        sprintf(gText, gColors[gpGame->m_players[playerIndex].color]);
        gText[0] -= ' ';
        message.type = TOWN_MESSAGE_SELECT;
        message.payload.widget.command = 3;
        message.payload.widget.id = displayPlayer + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message.payload.widget.data.text = gText;
        window->BroadcastMessage(message);

        if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_HERO) {
        } else {
            strongestHeroPosition = -1;
            strongestHeroValue = 0;
            for (heroPosition = 0;
                 heroPosition < gpGame->m_players[playerIndex].heroCount;
                 ++heroPosition) {
                strongestHero = &gpGame->m_heroRecs[
                    gpGame->m_players[playerIndex].heroes[heroPosition]];
                heroValue = gpPhilAI->FightValueOfStack(
                    &strongestHero->m_army, strongestHero, 0, 0, 0, 0);
                if (strongestHeroValue < heroValue) {
                    strongestHeroPosition = heroPosition;
                    strongestHeroValue = heroValue;
                }
            }

            if (strongestHeroPosition != -1) {
                strongestHero = &gpGame->m_heroRecs[
                    gpGame->m_players[playerIndex]
                        .heroes[strongestHeroPosition]];
                iconControl = new iconWidget(
                    static_cast<short>(displayPlayer * 0x44 + 0xf6),
                    0x12d, 0, 0, "locators.icn", 0x16, 0, -1, 0x10, 1);
                if (iconControl == 0)
                    MemError();
                window->AddWidget(iconControl, -1);
                iconControl = new iconWidget(
                    static_cast<short>(displayPlayer * 0x44 + 0xed),
                    300, 0, 0, "miniport.icn",
                    strongestHero->m_portrait, 0, -1, 0x10, 1);
                if (iconControl == 0)
                    MemError();
                window->AddWidget(iconControl, -1);
            }

            if (informationLevel < TOWN_THIEVES_INFO_PRIMARY_STATS) {
            } else {
                if (strongestHeroPosition != -1) {
                    strongestHero = &gpGame->m_heroRecs[
                        gpGame->m_players[playerIndex]
                            .heroes[strongestHeroPosition]];
                    sprintf(gText, "Att.\nDef.\nPower\nKnowl.");
                    widgetText = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        *reinterpret_cast<short *>("\x0e\x0e") +
                            TOWN_THIEVES_SOURCE_LINE_HERO_LABELS));
                    strcpy(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<short>(displayPlayer * 0x44 + 0xef),
                        0x153, 0x28, 0x30, widgetText, "smalfont.fnt",
                        1, -1, 0x200, 0);
                    window->AddWidget(textControl, -1);

                    sprintf(gText, "");
                    for (stat = 0; stat < TOWN_THIEVES_PRIMARY_STAT_COUNT;
                         ++stat) {
                        sprintf(statText, "%d\n", strongestHero->Stats(stat));
                        strcat(gText, statText);
                    }
                    widgetText = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        *reinterpret_cast<short *>("\x0e\x0e") +
                            TOWN_THIEVES_SOURCE_LINE_HERO_STATS));
                    strcpy(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<short>(displayPlayer * 0x44 + 0x11c),
                        0x153, 0xf, 0x30, widgetText, "smalfont.fnt",
                        1, -1, 0x200, 0);
                    window->AddWidget(textControl, -1);
                }

                if (informationLevel < TOWN_THIEVES_INFO_PERSONALITY) {
                } else {
                    strcpy(gText, cPersonality[
                        gpGame->m_players[playerIndex].unknown0f]);
                    widgetText = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        *reinterpret_cast<short *>("\x0e\x0e") +
                            TOWN_THIEVES_SOURCE_LINE_PERSONALITY));
                    strcpy(widgetText, gText);
                    textControl = new textWidget(
                        static_cast<short>(displayPlayer * 0x44 + 0xe3),
                        0x18d, 0x4a, 0x10, widgetText, "smalfont.fnt",
                        1, -1, 8, 1);
                    window->AddWidget(textControl, -1);

                    if (informationLevel <
                        TOWN_THIEVES_INFO_STRONGEST_CREATURE) {
                    } else {
                        strongestCreature = -1;
                        strongestCreatureValue = 0;
                        for (townPosition = 0;
                             townPosition <
                             gpGame->m_players[playerIndex].townCount;
                             ++townPosition) {
                            playerTown = &gpGame->m_castleRecs[
                                gpGame->m_players[playerIndex]
                                    .towns[townPosition]];
                            for (armySlot = 0;
                                 armySlot < TOWN_ARMY_SLOT_COUNT; ++armySlot) {
                                if (playerTown->m_army
                                            .m_creatureTypes[armySlot] != -1 &&
                                    playerTown->m_army
                                            .m_creatureCounts[armySlot] > 0 &&
                                    strongestCreatureValue <
                                        gMonsterDatabase[
                                            playerTown->m_army
                                                .m_creatureTypes[armySlot]]
                                            .fightValue) {
                                    strongestCreature = playerTown->m_army
                                                            .m_creatureTypes[armySlot];
                                    strongestCreatureValue =
                                        gMonsterDatabase[
                                            playerTown->m_army
                                                .m_creatureTypes[armySlot]]
                                            .fightValue;
                                }
                            }
                        }
                        for (heroPosition = 0;
                             heroPosition <
                             gpGame->m_players[playerIndex].heroCount;
                             ++heroPosition) {
                            strongestHero = &gpGame->m_heroRecs[
                                gpGame->m_players[playerIndex]
                                    .heroes[heroPosition]];
                            for (armySlot = 0;
                                 armySlot < TOWN_ARMY_SLOT_COUNT; ++armySlot) {
                                if (strongestHero->m_army
                                            .m_creatureTypes[armySlot] != -1 &&
                                    strongestHero->m_army
                                            .m_creatureCounts[armySlot] > 0 &&
                                    strongestCreatureValue <
                                        gMonsterDatabase[
                                            strongestHero->m_army
                                                .m_creatureTypes[armySlot]]
                                            .fightValue) {
                                    strongestCreature = strongestHero->m_army
                                                            .m_creatureTypes[armySlot];
                                    strongestCreatureValue =
                                        gMonsterDatabase[
                                            strongestHero->m_army
                                                .m_creatureTypes[armySlot]]
                                            .fightValue;
                                }
                            }
                        }
                        if (strongestCreature != -1) {
                            iconControl = new iconWidget(
                                static_cast<short>(
                                    displayPlayer * 0x44 + 0xf4),
                                0x1a2, 0x28, 0x22, "mons32.icn",
                                static_cast<short>(strongestCreature),
                                0, -1, 0x11, 1);
                            if (iconControl == 0)
                                MemError();
                            window->AddWidget(iconControl, -1);
                        }
                    }
                }
            }
        }
        ++playerIndex;
    }
}

VA(0x0041b692, 0x56a)
void GetCategoryStats(int category, long int * const stats,
                      signed char * const order)
{
    int player;
    int townIndex_c;
    hero *playerHero_h;
    int heroIndex_n;
    int townCount_k;
    int castleCount_p;
    int armyStrength;
    town *playerTown;

    for (player = 0; player < gpGame->m_playerCount; ++player) {
        townCount_k = 0;
        castleCount_p = 0;
        order[player] = static_cast<signed char>(player);
        if (gpGame->m_playerDead[player]) {
            stats[player] = TOWN_THIEVES_DEAD_PLAYER_STAT;
        } else {
            switch (category) {
            case TOWN_THIEVES_CATEGORY_TOWNS:
                for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT;
                     ++townIndex_c) {
                    if (gpGame->m_castleRecs[townIndex_c].m_owner == player &&
                        (gpGame->m_castleRecs[townIndex_c].m_buildings &
                         TOWN_BUILDING_TENT)) {
                        ++townCount_k;
                    }
                }
                stats[player] = townCount_k;
                break;
            case TOWN_THIEVES_CATEGORY_CASTLES:
                for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT;
                     ++townIndex_c) {
                    if (gpGame->m_castleRecs[townIndex_c].m_owner == player &&
                        (gpGame->m_castleRecs[townIndex_c].m_buildings &
                         TOWN_BUILDING_CASTLE)) {
                        ++castleCount_p;
                    }
                }
                stats[player] = castleCount_p;
                break;
            case TOWN_THIEVES_CATEGORY_HEROES:
                stats[player] = gpGame->m_players[player].heroCount;
                break;
            case TOWN_THIEVES_CATEGORY_GOLD:
                stats[player] = gpGame->m_players[player].resources[RES_GOLD];
                break;
            case TOWN_THIEVES_CATEGORY_WOOD_AND_ORE:
                stats[player] =
                    gpGame->m_players[player].resources[RES_WOOD] +
                    gpGame->m_players[player].resources[RES_ORE];
                break;
            case TOWN_THIEVES_CATEGORY_RARE_RESOURCES:
                stats[player] =
                    gpGame->m_players[player].resources[RES_MERCURY] +
                    gpGame->m_players[player].resources[RES_SULFUR] +
                    gpGame->m_players[player].resources[RES_CRYSTAL] +
                    gpGame->m_players[player].resources[RES_GEMS];
                break;
            case TOWN_THIEVES_CATEGORY_OBELISKS:
                stats[player] = GetNumObelisks(player);
                break;
            case TOWN_THIEVES_CATEGORY_ARTIFACTS:
                stats[player] = 0;
                for (townIndex_c = 0;
                     townIndex_c < gpGame->m_players[player].heroCount;
                     ++townIndex_c) {
                    playerHero_h = gpGame->GetHero(
                        gpGame->m_players[player].heroes[townIndex_c]);
                    for (heroIndex_n = 0;
                         heroIndex_n < TOWN_MAX_ARTIFACTS; ++heroIndex_n) {
                        if (playerHero_h->m_artifacts[heroIndex_n] != -1 &&
                            playerHero_h->m_artifacts[heroIndex_n] !=
                                TOWN_SPELL_BOOK_ARTIFACT) {
                            ++stats[player];
                        }
                    }
                }
                break;
            case TOWN_THIEVES_CATEGORY_ARMY_STRENGTH:
                armyStrength = 0;
                for (heroIndex_n = 0;
                     heroIndex_n < gpGame->m_players[player].heroCount;
                     ++heroIndex_n) {
                    playerHero_h =
                        gpGame->GetPlayerHero(player, heroIndex_n);
                    armyStrength += gpPhilAI->FightValueOfStack(
                        &playerHero_h->m_army, playerHero_h, 0, 0, 0, 0);
                }
                for (heroIndex_n = 0;
                     heroIndex_n < gpGame->m_players[player].townCount;
                     ++heroIndex_n) {
                    playerTown =
                        gpGame->GetPlayerTown(player, heroIndex_n);
                    if (playerTown->HasGarrison()) {
                        armyStrength += gpPhilAI->FightValueOfStack(
                            &playerTown->m_army, 0, 0, 0, 0, 0);
                    }
                }
                stats[player] = armyStrength;
                break;
            case TOWN_THIEVES_CATEGORY_INCOME:
                stats[player] = gpGame->ComputeDailyGold(player);
                break;
            }
        }
    }
}

VA(0x0041bbfc, 0xd9)
void SortStats(long int * const stats, signed char * const order)
{
    int temporaryOrder;
    int secondPlayer;
    int firstPlayer;
    long tempStat;

    for (firstPlayer = 0; firstPlayer < gpGame->m_playerCount - 1;
         ++firstPlayer) {
        for (secondPlayer = firstPlayer + 1;
             secondPlayer < gpGame->m_playerCount; ++secondPlayer) {
            if (stats[firstPlayer] < stats[secondPlayer]) {
                tempStat = stats[firstPlayer];
                stats[firstPlayer] = stats[secondPlayer];
                stats[secondPlayer] = tempStat;
                temporaryOrder = order[firstPlayer];
                order[firstPlayer] = order[secondPlayer];
                order[secondPlayer] = static_cast<signed char>(temporaryOrder);
            }
        }
    }
}


// ===== vtable townManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00414109, 0x1ef)  int townManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00414e98, 0xca)  void townManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0041595d, 0x1830)  int townManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(townManager, 0x004eb140);
