// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWNMGR.OBJ   from: (directly linked into exe)
// functions: 33   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
#include <_types.h>
#include <BASE/Misc.h>
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
#include <SOURCE/EVENTS.h>
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
#include <SOURCE/recruitUnit.h>
#include <SOURCE/strip.h>
#include <SOURCE/town.h>
#include <SOURCE/townObject.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/tradpost.h>
#include <stdio.h>
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
    int quickView_k = (message.fieldC & TOWN_QUICK_VIEW_MODIFIER) != 0;
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
        switch (message.field4) {
        case TOWN_INPUT_SELECT:
        case TOWN_INPUT_ALTERNATE_SELECT:
        switch (message.field8) {
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
                    m_town, message.field8 - TOWN_COMMAND_FIRST_DWELLING);
            } else {
                DrawTown(1, 1);
                dialogManager_d = new recruitUnit(
                    m_town,
                    message.field8 - TOWN_COMMAND_FIRST_DWELLING, 1);
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
                                            message.field8, 1));
                    NormalDialog(description_b, 1, -1, -1,
                                 m_town->m_type + 19, message.field8,
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
                        message.field4 = 5;
                        message.field8 = TOWN_DIALOG_BUILD_BOAT;
                        message.field18 = 0x1000;
                        m_heroWindow0->BroadcastMessage(message);
                        message.field4 = 6;
                        message.field18 = 2;
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
                                        message.field8, 1));
                NormalDialog(description_b, 4, -1, -1,
                             m_town->m_type + 19, message.field8,
                             -1, 0, -1, 0);
            } else {
                sprintf(description_b,
                        GetBuildingInfo(m_town->m_type, message.field8, 1));
                NormalDialog(description_b, 1, -1, -1,
                             m_town->m_type + 19, message.field8,
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
                if (message.field8 >= TOWN_GARRISON_SLOT_FIRST &&
                    message.field8 <= TOWN_GARRISON_SLOT_LAST) {
                    m_selectedStrip = m_garrisonStrip;
                    m_selectedArmySlot =
                        message.field8 - TOWN_GARRISON_SLOT_FIRST;
                    armySelected = 1;
                }
                if (message.field8 >= TOWN_HERO_SLOT_FIRST &&
                    message.field8 <= TOWN_HERO_SLOT_LAST) {
                    m_selectedStrip = m_heroStrip;
                    m_selectedArmySlot =
                        message.field8 - TOWN_HERO_SLOT_FIRST;
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
            switch (message.field8) {
            case TOWN_CONTROL_PREVIOUS_TOWN:
            case TOWN_CONTROL_NEXT_TOWN:
                if (m_town->m_owner == giCurPlayer &&
                    gpCurPlayer->m_townCount > 1) {
                    int townPosition =
                        gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
                    townPosition = (townPosition +
                                    (message.field8 == TOWN_CONTROL_PREVIOUS_TOWN
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
        switch (message.field4) {
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
        switch (message.field4) {
        case TOWN_KEY_SHIFT_LEFT:
        case TOWN_KEY_SHIFT_RIGHT:
            ShiftQualChange();
            break;
        }
        break;

    case TOWN_MESSAGE_HOVER:
        gpWindowManager->ConvertToHover(message);
        if (message.field8 == m_lastHoverId &&
            message.fieldC == m_lastHoverSubId)
            return 1;
        m_lastHoverId = message.field8;
        m_lastHoverSubId = message.fieldC;
        SetCommandAndText(message);
        return 1;
    }

    if (exitTown_i == 1) {
        message.type = TOWN_MESSAGE_EXIT;
        message.field4 = 1;
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
    message.field4 = 3;
    message.field8 = TOWN_CONTROL_STATUS_TEXT;
    message.text = m_statusText;
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
    message.field4 = 3;
    message.field8 = 1;
    message.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", m_splitAmount);
    message.field8 = 4;
    message.text = gText;
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
    message.field4 = 9;
    message.field8 = 2;
    sprintf(iconName, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message.text = iconName;
    window->BroadcastMessage(message);
    message.field4 = 4;
    message.field8 = 2;
    message.field18 = building;
    window->BroadcastMessage(message);

    if (building == TOWN_COMMAND_MAGE_GUILD) {
        sprintf(gText, "Mage Guild, Level %d",
                mageLevel + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                    ? mageLevel + 1
                    : TOWN_MAGE_GUILD_MAX_LEVEL);
    } else {
        strcpy(gText, GetBuildingName(m_town->m_type, building));
    }
    message.field4 = 3;
    message.field8 = 3;
    message.text = gText;
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
        message.field4 = 6;
        message.field18 = 6;
        message.field8 = TOWN_DIALOG_CONFIRM;
        window->BroadcastMessage(message);
        message.field4 = 6;
        message.field18 = 6;
        message.field8 = 0x7801;
        window->BroadcastMessage(message);
        message.field4 = 6;
        message.field18 = 6;
        message.field8 = 0;
        window->BroadcastMessage(message);
        gpWindowManager->AddWindow(window, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(window);
    } else {
        if (cannotBuy != 0) {
            message.field4 = 6;
            message.field8 = TOWN_DIALOG_CONFIRM;
            message.field18 = 2;
            window->BroadcastMessage(message);
            message.field4 = 5;
            message.field8 = TOWN_DIALOG_CONFIRM;
            message.field18 = 0x1000;
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
void townManager::BuildObj(int) {}

VA(0x00418fbb, 0x3d8)
void townManager::SetupMage(class heroWindow *) {}

VA(0x00419393, 0x190)
int MageGuildHandler(struct tag_message &) { return 0; }

VA(0x00419523, 0x706)
int townManager::RecruitHero(int, int) { return 0; }

VA(0x00419c29, 0x153)
int TavernHandler(struct tag_message &) { return 0; }

VA(0x00419d7c, 0x110)
void townManager::DoTavern(void) {}

VA(0x00419e8c, 0x328)
int SplitArmyHandler(struct tag_message &) { return 0; }

VA(0x0041a1b4, 0x5cf)
void townManager::SetupWell(class heroWindow *) {}

VA(0x0041a783, 0xf0f)
void townManager::SetupThievesGuild(class heroWindow *, int) {}

VA(0x0041b692, 0x56a)
void GetCategoryStats(int, long int * const, signed char * const) {}

VA(0x0041bbfc, 0xd9)
void SortStats(long int * const, signed char * const) {}


// ===== vtable townManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00414109, 0x1ef)  int townManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00414e98, 0xca)  void townManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0041595d, 0x1830)  int townManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(townManager, 0x004eb140);
