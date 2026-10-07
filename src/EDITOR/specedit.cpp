// The map specification dialog: the map's name, description and difficulty,
// which colours humans and the computer may play, the victory and loss
// conditions with their towns, heroes, artifact, side, gold or time, and the
// lists of time events and rumours. The unit name comes from its dialog
// resource (specedit.bin); every name in it is descriptive.

#include <va.h>
#include <EDITOR/specedit.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/heroedit.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/townedit.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/listBoxWidget.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(SpecDialogWidget)
    // A colour's player button: its frame shows who may play it.
    SPEC_PLAYER_FIRST          = 0x6e,
    SPEC_PLAYER_LAST           = SPEC_PLAYER_FIRST + EDITOR_PLAYER_COLOR_COUNT - 1,
    SPEC_VICTORY_LIST          = 0xd2,
    // The victory options: computer also wins and the normal victory, each
    // with two labels and a check box.
    SPEC_COMPUTER_WINS_LABEL   = 0xdc,
    SPEC_NORMAL_VICTORY_LABEL  = 0xdd,
    SPEC_COMPUTER_WINS_TEXT    = 0xe6,
    SPEC_NORMAL_VICTORY_TEXT   = 0xe7,
    SPEC_COMPUTER_WINS_BOX     = 0xf0,
    SPEC_NORMAL_VICTORY_BOX    = 0xf1,
    // The victory condition's town, hero, artifact, side or gold.
    SPEC_VICTORY_VALUE_LABEL   = 0xfa,
    SPEC_VICTORY_VALUE_LIST    = 0xfb,
    SPEC_LOSS_LIST             = 0x136,
    SPEC_LOSS_VALUE_LABEL      = 0x140,
    SPEC_LOSS_VALUE_LIST       = 0x141,
    SPEC_NAME                  = 0x19a,
    SPEC_FILE_NAME             = 0x19b,
    SPEC_DESCRIPTION           = 0x1f5,
    SPEC_DIFFICULTY_FIRST      = 0x26c,
    SPEC_DIFFICULTY_LAST       = SPEC_DIFFICULTY_FIRST + IDX(DIFFICULTY_COUNT) - 2,
    // "Start with hero in each player's main castle": checked while the
    // header's noStartingHero is clear.
    SPEC_STARTING_HERO_BOX     = 0x2bd,
    SPEC_RUMOUR_LIST           = 0x321,
    SPEC_RUMOUR_ADD            = 0x322,
    SPEC_RUMOUR_EDIT           = 0x323,
    SPEC_RUMOUR_DELETE         = 0x324,
    SPEC_EVENT_LIST            = 0x385,
    SPEC_EVENT_ADD             = 0x386,
    SPEC_EVENT_EDIT            = 0x387,
    SPEC_EVENT_DELETE          = 0x388
H2_ENUM_END(SpecDialogWidget)

H2_ENUM_BEGIN(SpecDialogConstant)
    SPEC_PERCENT               = 100,
    // The player buttons' frames: four per colour, by who may play it.
    SPEC_PLAYER_FRAME_FIRST    = 0x13,
    SPEC_PLAYER_FRAMES         = 4,
    SPEC_PLAYER_HUMAN          = 1,
    SPEC_PLAYER_COMPUTER       = 2,
    SPEC_PLAYER_STATES         = 4,
    // The towns and heroes a map holds at most.
    SPEC_MAX_TOWNS             = 72,
    SPEC_MAX_HEROES            = 54,
    // Accumulate gold: twenty choices in steps of 50,000; the header keeps
    // thousands.
    SPEC_GOLD_CHOICES          = 20,
    SPEC_GOLD_STEP             = 50,
    SPEC_GOLD_UNIT             = 1000,
    // Lose by time: 2-7 days, 2-8 weeks, 3-12 months.
    SPEC_DAYS_FIRST            = 2,
    SPEC_DAYS_LAST             = 7,
    SPEC_WEEKS_FIRST           = 2,
    SPEC_WEEKS_LAST            = 8,
    SPEC_MONTHS_FIRST          = 3,
    SPEC_MONTHS_LAST           = 12,
    SPEC_DAYS_PER_WEEK         = 7,
    SPEC_DAYS_PER_MONTH        = 28,
    // The time list's choices: days, then weeks, then months. Choosing
    // reads one week past the list and maps the first month a choice late.
    SPEC_LAST_DAYS_CHOICE      = SPEC_DAYS_LAST - SPEC_DAYS_FIRST,
    SPEC_FIRST_WEEKS_CHOICE    = SPEC_LAST_DAYS_CHOICE + 1,
    SPEC_LAST_WEEKS_CHOICE     = SPEC_FIRST_WEEKS_CHOICE + SPEC_WEEKS_LAST - SPEC_WEEKS_FIRST + 1,
    SPEC_FIRST_MONTHS_CHOICE   = SPEC_LAST_WEEKS_CHOICE + 1,
    // A list entry's text and the time event's message excerpt.
    SPEC_LIST_TEXT_SIZE        = 200,
    SPEC_EXCERPT_LENGTH        = 100,
    SPEC_EXCERPT_SIZE          = SPEC_EXCERPT_LENGTH + 1
H2_ENUM_END(SpecDialogConstant)

DATA(0x004a583c) i32 gLandPercent;
DATA(0x004a5840) heroWindow* gSpecWindow;

VA(0x00426640, 0x528)
b32 EditMapSpecifications(b32 randomMap) {
    char buffer[SPEC_EXCERPT_SIZE];
    tag_message message;
    i32 y;
    i32 i;
    i32 x;
    SMapHeader savedHeader;
    i32 count;

    savedHeader = gEditMapHeader;
    gSpecWindow = new heroWindow(0, 0, "specedit.bin");
    if (gSpecWindow == NULL)
        MemError();
    SetWinText(gSpecWindow, EVENTS_WINDOW_TEXT_SPECIFICATIONS);
    ResetPlayerAvailability();
    count = 0;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (CELL_TERRAIN(gMap.GetCell(x, y)) != TERRAIN_WATER)
                count++;
        }
    }
    gLandPercent = count * SPEC_PERCENT / (MAP_WIDTH * MAP_HEIGHT);
    for (i = 0; i < SPEC_VICTORY_CONDITION_COUNT; i++) {
        sprintf(gText, "%s", gVictoryConditionNames[i]);
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
        message.payload.widget.data.text = gText;
        message.payload.widget.id = SPEC_VICTORY_LIST;
        gSpecWindow->BroadcastMessage(message);
    }
    for (i = 0; i < SPEC_LOSS_CONDITION_COUNT; i++) {
        sprintf(gText, "%s", gLossConditionNames[i]);
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
        message.payload.widget.data.text = gText;
        message.payload.widget.id = SPEC_LOSS_LIST;
        gSpecWindow->BroadcastMessage(message);
    }
    FillLossConditionList();
    FillVictoryConditionList();
    for (i = 0; i < gEditMapHeader.timeEventCount; i++) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
        strncpy(buffer,
                static_cast<EventExtra*>(gEditManager->m_extras[gTimeEventExtras[i]])->message,
                SPEC_EXCERPT_LENGTH);
        buffer[SPEC_EXCERPT_LENGTH] = '\0';
        sprintf(gText, localization::Tr("editor.spec.event_entry"),
                static_cast<EventExtra*>(gEditManager->m_extras[gTimeEventExtras[i]])->firstDay,
                buffer);
        message.payload.widget.data.text = gText;
        message.payload.widget.id = SPEC_EVENT_LIST;
        gSpecWindow->BroadcastMessage(message);
    }
    for (i = 0; i < gEditMapHeader.rumourCount; i++) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
        strncpy(buffer,
                static_cast<rumourEventExtra*>(gEditManager->m_extras[gRumourExtras[i]])->text,
                SPEC_EXCERPT_LENGTH);
        buffer[SPEC_EXCERPT_LENGTH] = '\0';
        sprintf(gText, "%s", buffer);
        message.payload.widget.data.text = gText;
        message.payload.widget.id = SPEC_RUMOUR_LIST;
        gSpecWindow->BroadcastMessage(message);
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.id = SPEC_VICTORY_LIST;
    message.payload.widget.data.value = gEditMapHeader.victoryCondition;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.id = SPEC_LOSS_LIST;
    message.payload.widget.data.value = gEditMapHeader.lossCondition;
    gSpecWindow->BroadcastMessage(message);
    UpdateSpecificationsWindow();
    gpWindowManager->DoDialog(gSpecWindow, SpecificationsHandler, 0);
    delete gSpecWindow;
    gSpecWindow = NULL;
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    if (gpWindowManager->m_dialogResult == EVENTS_DIALOG_CANCEL)
        gEditMapHeader = savedHeader;
    return gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL;
}

VA(0x00426b68, 0x676)
void FillVictoryConditionList(void) {
    char initial;
    tag_message listMessage;
    char itemText[SPEC_LIST_TEXT_SIZE];
    i32 listSelection;
    i32 nextColor;
    TownExtra* townExtra;
    i32 j;
    i32 i;
    i32 y;
    i32 x;
    HeroExtra* heroExtra;

    listSelection = 0;
    listMessage.type = MESSAGE_WIDGET;
    listMessage.payload.widget.command = WIDGET_COMMAND_CLEAR_ITEMS;
    listMessage.payload.widget.id = SPEC_VICTORY_VALUE_LIST;
    gSpecWindow->BroadcastMessage(listMessage);
    listMessage.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    listMessage.payload.widget.data.text = itemText;
    switch (gEditMapHeader.victoryCondition) {
        case MAP_VICTORY_DEFEAT_ALL:
            return;
        case MAP_VICTORY_CAPTURE_TOWN:
            if (!gEditManager->FindTown(0, &x, &y)) {
                sprintf(itemText, localization::Tr("editor.spec.no_towns"));
                gSpecWindow->BroadcastMessage(listMessage);
            } else {
                for (i = 0; i < SPEC_MAX_TOWNS; i++) {
                    if (!gEditManager->FindTown(i, &x, &y))
                        break;
                    townExtra = static_cast<TownExtra*>(
                        gEditManager->m_extras[gMap.GetCell(x, y)->m_objectMetadata]);
                    sprintf(itemText, "(%d, %d) %s", x, y, townExtra->hasCustomName ? townExtra->name : "");
                    gSpecWindow->BroadcastMessage(listMessage);
                    if (x == gEditMapHeader.victoryConditionValue && y == gEditMapHeader.victoryTownY)
                        listSelection = i;
                }
            }
            break;
        case MAP_VICTORY_DEFEAT_HERO:
            if (!gEditManager->FindHero(0, &x, &y)) {
                sprintf(itemText, localization::Tr("editor.spec.no_heroes"));
                gSpecWindow->BroadcastMessage(listMessage);
            } else {
                for (i = 0; i < SPEC_MAX_HEROES; i++) {
                    if (!gEditManager->FindHero(i, &x, &y))
                        break;
                    heroExtra = static_cast<HeroExtra*>(
                        gEditManager->m_extras[gMap.GetCell(x, y)->m_objectMetadata]);
                    sprintf(itemText, "(%d, %d) %s", x, y, heroExtra->hasCustomName ? heroExtra->name : "");
                    gSpecWindow->BroadcastMessage(listMessage);
                    if (x == gEditMapHeader.victoryConditionValue && y == gEditMapHeader.victoryTownY)
                        listSelection = i;
                }
            }
            break;
        case MAP_VICTORY_FIND_ARTIFACT:
            if (gEditMapHeader.victoryConditionValue >= 0
                && gEditMapHeader.victoryConditionValue <= IDX(ARTIFACT_SPADE_NECROMANCY)) {
                listSelection = gEditMapHeader.victoryConditionValue;
                if (listSelection - 1 >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
                    listSelection -= EVENTS_HIDDEN_ARTIFACT_COUNT;
            } else {
                listSelection = 0;
            }
            sprintf(itemText, localization::Tr("artifact.ultimate_generic"));
            gSpecWindow->BroadcastMessage(listMessage);
            for (i = 0; i < IDX(ARTIFACT_SPADE_NECROMANCY); i++) {
                if (i < IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) || i > IDX(ARTIFACT_SPELL_SCROLL)) {
                    sprintf(itemText, gArtifactNames[i]);
                    gSpecWindow->BroadcastMessage(listMessage);
                }
            }
            break;
        case MAP_VICTORY_ACCUMULATE_GOLD:
            listSelection = 0;
            for (i = 0; i < SPEC_GOLD_CHOICES; i++) {
                if (gEditMapHeader.victoryConditionValue == (i + 1) * SPEC_GOLD_STEP)
                    listSelection = i;
                sprintf(itemText, localization::Tr("editor.spec.gold"),
                        (i + 1) * (SPEC_GOLD_STEP * SPEC_GOLD_UNIT));
                gSpecWindow->BroadcastMessage(listMessage);
            }
            break;
        case MAP_VICTORY_DEFEAT_SIDE:
            CalculatePlayerNumbers();
            listSelection = gEditMapHeader.victoryConditionValue - 1 < gEditMapHeader.playerCount - 2
                            ? gEditMapHeader.victoryConditionValue - 1
                            : gEditMapHeader.playerCount - 2;
            if (listSelection < 0)
                listSelection = 0;
            gEditMapHeader.victoryConditionValue = listSelection + 1;
            for (i = 0; i < gEditMapHeader.playerCount - 1; i++) {
                strcpy(itemText, "");
                nextColor = 0;
                for (j = 0; j < i + 1; j++) {
                    while (!gEditMapHeader.playerEnabled[nextColor])
                        nextColor++;
                    sprintf(gText, gColorAbbreviations[nextColor]);
                    if (static_cast<u8>(gText[0]) >= 'a' && static_cast<u8>(gText[0]) <= 'z')
                        initial = static_cast<u8>(gText[0]) - ('a' - 'A');
                    else if (static_cast<u8>(gText[0]) >= CYRILLIC_SMALL_A
                             && static_cast<u8>(gText[0]) <= CYRILLIC_SMALL_YA)
                        initial = static_cast<u8>(gText[0]) - (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
                    else if (static_cast<u8>(gText[0]) == CYRILLIC_SMALL_YO)
                        initial = static_cast<char>(CYRILLIC_CAPITAL_YO);
                    else
                        initial = gText[0];
                    gText[0] = initial;
                    strcat(itemText, gText);
                    if (j < i)
                        strcat(itemText, ", ");
                    else
                        strcat(itemText, localization::Tr("editor.spec.versus_others"));
                    nextColor++;
                }
                gSpecWindow->BroadcastMessage(listMessage);
            }
            break;
    }
    SetVictoryConditionChoice(listSelection);
    listMessage.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    listMessage.payload.widget.data.value = listSelection;
    gSpecWindow->BroadcastMessage(listMessage);
}

VA(0x004271de, 0x10f)
void SetVictoryConditionChoice(i32 choice) {
    i32 x;
    i32 y;

    switch (gEditMapHeader.victoryCondition) {
        case MAP_VICTORY_CAPTURE_TOWN:
            if (!gEditManager->FindTown(choice, &x, &y)) {
                x = 1;
            } else {
                gEditMapHeader.victoryConditionValue = x;
                gEditMapHeader.victoryTownY = y;
            }
            break;
        case MAP_VICTORY_DEFEAT_HERO:
            if (!gEditManager->FindHero(choice, &x, &y)) {
                x = 1;
            } else {
                gEditMapHeader.victoryConditionValue = x;
                gEditMapHeader.victoryTownY = y;
            }
            break;
        case MAP_VICTORY_ACCUMULATE_GOLD:
            gEditMapHeader.victoryConditionValue = (choice + 1) * SPEC_GOLD_STEP;
            break;
        case MAP_VICTORY_FIND_ARTIFACT:
            gEditMapHeader.victoryConditionValue = choice;
            if (gEditMapHeader.victoryConditionValue - 1 >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
                gEditMapHeader.victoryConditionValue += EVENTS_HIDDEN_ARTIFACT_COUNT;
            break;
        case MAP_VICTORY_DEFEAT_SIDE:
            gEditMapHeader.victoryConditionValue = choice + 1;
            break;
    }
}

VA(0x004272ed, 0x482)
void FillLossConditionList(void) {
    tag_message listMessage;
    char itemText[SPEC_LIST_TEXT_SIZE];
    i32 listSelection;
    TownExtra* townExtra;
    i32 i;
    i32 y;
    i32 x;
    HeroExtra* heroExtra;

    listSelection = 0;
    listMessage.type = MESSAGE_WIDGET;
    listMessage.payload.widget.command = WIDGET_COMMAND_CLEAR_ITEMS;
    listMessage.payload.widget.id = SPEC_LOSS_VALUE_LIST;
    gSpecWindow->BroadcastMessage(listMessage);
    listMessage.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    listMessage.payload.widget.data.text = itemText;
    switch (gEditMapHeader.lossCondition) {
        case MAP_LOSS_STANDARD:
            return;
        case MAP_LOSS_TOWN:
            if (!gEditManager->FindTown(0, &x, &y)) {
                sprintf(itemText, localization::Tr("editor.spec.no_towns"));
                gSpecWindow->BroadcastMessage(listMessage);
            } else {
                for (i = 0; i < SPEC_MAX_TOWNS; i++) {
                    if (!gEditManager->FindTown(i, &x, &y))
                        break;
                    townExtra = static_cast<TownExtra*>(
                        gEditManager->m_extras[gMap.GetCell(x, y)->m_objectMetadata]);
                    sprintf(itemText, "(%d, %d) %s", x, y, townExtra->hasCustomName ? townExtra->name : "");
                    gSpecWindow->BroadcastMessage(listMessage);
                    if (x == gEditMapHeader.lossConditionValue && y == gEditMapHeader.lossTownY)
                        listSelection = i;
                }
            }
            break;
        case MAP_LOSS_HERO:
            if (!gEditManager->FindHero(0, &x, &y)) {
                sprintf(itemText, localization::Tr("editor.spec.no_heroes"));
                gSpecWindow->BroadcastMessage(listMessage);
            } else {
                for (i = 0; i < SPEC_MAX_HEROES; i++) {
                    if (!gEditManager->FindHero(i, &x, &y))
                        break;
                    heroExtra = static_cast<HeroExtra*>(
                        gEditManager->m_extras[gMap.GetCell(x, y)->m_objectMetadata]);
                    sprintf(itemText, "(%d, %d) %s", x, y, heroExtra->hasCustomName ? heroExtra->name : "");
                    gSpecWindow->BroadcastMessage(listMessage);
                    if (x == gEditMapHeader.lossConditionValue && y == gEditMapHeader.lossTownY)
                        listSelection = i;
                }
            }
            break;
        case MAP_LOSS_TIME:
            for (i = SPEC_DAYS_FIRST; i <= SPEC_DAYS_LAST; i++) {
                sprintf(itemText, localization::Tr("editor.spec.days"), i);
                gSpecWindow->BroadcastMessage(listMessage);
            }
            for (i = SPEC_WEEKS_FIRST; i <= SPEC_WEEKS_LAST; i++) {
                sprintf(itemText, localization::Tr("editor.spec.weeks"), i);
                gSpecWindow->BroadcastMessage(listMessage);
            }
            for (i = SPEC_MONTHS_FIRST; i <= SPEC_MONTHS_LAST; i++) {
                sprintf(itemText, localization::Tr("editor.spec.months"), i);
                gSpecWindow->BroadcastMessage(listMessage);
            }
            if (gEditMapHeader.lossConditionValue == 0)
                listSelection = 0;
            else if (gEditMapHeader.lossConditionValue <= SPEC_DAYS_LAST)
                listSelection = gEditMapHeader.lossConditionValue - SPEC_DAYS_FIRST;
            else if (gEditMapHeader.lossConditionValue <= SPEC_WEEKS_LAST * SPEC_DAYS_PER_WEEK)
                listSelection = gEditMapHeader.lossConditionValue / SPEC_DAYS_PER_WEEK
                            + SPEC_FIRST_WEEKS_CHOICE - SPEC_WEEKS_FIRST;
            else
                listSelection = gEditMapHeader.lossConditionValue / SPEC_DAYS_PER_MONTH
                            + SPEC_FIRST_MONTHS_CHOICE - SPEC_MONTHS_FIRST;
            break;
    }
    SetLossConditionChoice(listSelection);
    listMessage.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    listMessage.payload.widget.data.value = listSelection;
    gSpecWindow->BroadcastMessage(listMessage);
}

VA(0x0042776f, 0xe5)
void SetLossConditionChoice(i32 choice) {
    i32 x;
    i32 y;

    switch (gEditMapHeader.lossCondition) {
        case MAP_LOSS_TOWN:
            if (!gEditManager->FindTown(choice, &x, &y)) {
                x = 1;
            } else {
                gEditMapHeader.lossConditionValue = x;
                gEditMapHeader.lossTownY = y;
            }
            break;
        case MAP_LOSS_HERO:
            if (!gEditManager->FindHero(choice, &x, &y)) {
                x = 1;
            } else {
                gEditMapHeader.lossConditionValue = x;
                gEditMapHeader.lossTownY = y;
            }
            break;
        case MAP_LOSS_TIME:
            if (choice <= SPEC_LAST_DAYS_CHOICE)
                gEditMapHeader.lossConditionValue = choice + SPEC_DAYS_FIRST;
            else if (choice <= SPEC_LAST_WEEKS_CHOICE)
                gEditMapHeader.lossConditionValue
                    = (choice - SPEC_FIRST_WEEKS_CHOICE) * SPEC_DAYS_PER_WEEK
                      + SPEC_WEEKS_FIRST * SPEC_DAYS_PER_WEEK;
            else
                gEditMapHeader.lossConditionValue
                    = (choice - SPEC_FIRST_MONTHS_CHOICE) * SPEC_DAYS_PER_MONTH
                      + SPEC_MONTHS_FIRST * SPEC_DAYS_PER_MONTH;
            break;
    }
}

VA(0x00427854, 0x3f4)
void UpdateSpecificationsWindow(void) {
    tag_message message;
    i32 i;
    b32 dimmed;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SPEC_NAME;
    message.payload.widget.data.text = gEditMapHeader.name;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_DESCRIPTION;
    message.payload.widget.data.text = gEditMapHeader.description;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_FILE_NAME;
    sprintf(gText, gMapFileName);
    message.payload.widget.data.text = gText;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    message.payload.widget.command
        = gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_ALL
                  || gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
                  || gEditMapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT
                  || gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
              ? WIDGET_COMMAND_CLEAR_FLAGS
              : WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.id = SPEC_COMPUTER_WINS_LABEL;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_COMPUTER_WINS_TEXT;
    gSpecWindow->BroadcastMessage(message);
    if (!gEditMapHeader.computerAlsoWins)
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_COMPUTER_WINS_BOX;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command
        = gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_ALL
                  || gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
                  || gEditMapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
              ? WIDGET_COMMAND_CLEAR_FLAGS
              : WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.id = SPEC_NORMAL_VICTORY_LABEL;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_NORMAL_VICTORY_TEXT;
    gSpecWindow->BroadcastMessage(message);
    if (!gEditMapHeader.allowNormalVictory)
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_NORMAL_VICTORY_BOX;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command
        = gEditMapHeader.victoryCondition ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_VICTORY_VALUE_LABEL;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command
        = gEditMapHeader.victoryCondition ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_VICTORY_VALUE_LIST;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command
        = gEditMapHeader.lossCondition ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_LOSS_VALUE_LABEL;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command
        = gEditMapHeader.lossCondition ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SPEC_LOSS_VALUE_LIST;
    gSpecWindow->BroadcastMessage(message);
    for (i = 0; i < IDX(DIFFICULTY_COUNT) - 1; i++) {
        message.payload.widget.command
            = i == gEditMapHeader.difficulty ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = SPEC_DIFFICULTY_FIRST + i;
        gSpecWindow->BroadcastMessage(message);
    }
    message.payload.widget.command
        = gEditMapHeader.noStartingHero ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.id = SPEC_STARTING_HERO_BOX;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_EVENT_LIST;
    gSpecWindow->BroadcastMessage(message);
    dimmed = message.payload.widget.data.value == LIST_BOX_NO_SELECTION;
    message.payload.widget.command = dimmed ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = WIDGET_FLAGS_ARGUMENT_DIMMED;
    message.payload.widget.id = SPEC_EVENT_EDIT;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_EVENT_DELETE;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_RUMOUR_LIST;
    gSpecWindow->BroadcastMessage(message);
    dimmed = message.payload.widget.data.value == LIST_BOX_NO_SELECTION;
    message.payload.widget.command = dimmed ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = WIDGET_FLAGS_ARGUMENT_DIMMED;
    message.payload.widget.id = SPEC_RUMOUR_EDIT;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.id = SPEC_RUMOUR_DELETE;
    gSpecWindow->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    for (i = 0; i < EDITOR_PLAYER_COLOR_COUNT; i++) {
        message.payload.widget.id = SPEC_PLAYER_FIRST + i;
        message.payload.widget.data.value = gEditMapHeader.playerCanHuman[i]
                                            + SPEC_PLAYER_FRAME_FIRST + i * SPEC_PLAYER_FRAMES
                                            + gEditMapHeader.playerCanComputer[i] * SPEC_PLAYER_COMPUTER;
        gSpecWindow->BroadcastMessage(message);
    }
}

VA(0x00427c48, 0x256)
void AddMapEvent(void) {
    char itemText[SPEC_EXCERPT_SIZE];
    EventExtra* event;
    i32 choice;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    if (gEditMapHeader.timeEventCount >= EDITOR_TIME_EVENT_CAPACITY) {
        sprintf(gText, localization::Tr("editor.spec.events_full"));
        NormalDialog(gText, NORMAL_DIALOG_INFO);
    } else {
        event = new EventExtra;
        memset(event, 0, sizeof(EventExtra));
        event->isMapEvent = false;
        event->artifact = IDX(ARTIFACT_NONE);
        event->firstDay = 1;
        event->appliesToHuman = 1;
        memset(event->players, 1, sizeof(event->players));
        gEditManager->m_extras[gEditManager->m_extraCount] = event;
        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(EventExtra);
        choice = static_cast<eventsManager*>(gEditManager->m_toolManager)
                     ->EditEvent(gEditManager->m_extraCount);
        if (choice != EVENTS_DIALOG_CANCEL) {
            gTimeEventExtras[gEditMapHeader.timeEventCount] = gEditManager->m_extraCount;
            message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
            message.payload.widget.id = SPEC_EVENT_LIST;
            strncpy(itemText,
                    static_cast<EventExtra*>(
                        gEditManager->m_extras[gTimeEventExtras[gEditMapHeader.timeEventCount]])
                        ->message,
                    SPEC_EXCERPT_LENGTH);
            itemText[SPEC_EXCERPT_LENGTH] = '\0';
            sprintf(gText, localization::Tr("editor.spec.event_entry"),
                    static_cast<EventExtra*>(
                        gEditManager->m_extras[gTimeEventExtras[gEditMapHeader.timeEventCount]])
                        ->firstDay,
                    itemText);
            message.payload.widget.data.text = gText;
            gSpecWindow->BroadcastMessage(message);
            gEditMapHeader.timeEventCount++;
            gEditManager->m_extraCount++;
        } else {
            delete[] static_cast<char*>(gEditManager->m_extras[gEditManager->m_extraCount]);
            gEditManager->m_extras[gEditManager->m_extraCount] = NULL;
            gEditManager->m_extraSizes[gEditManager->m_extraCount] = 0;
        }
    }
}

VA(0x00427e9e, 0x107)
void EditMapEvent(void) {
    char itemText[SPEC_EXCERPT_SIZE];
    i32 dialogResult;
    i32 index;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_EVENT_LIST;
    gSpecWindow->BroadcastMessage(message);
    index = message.payload.widget.data.value;
    if (index != LIST_BOX_NO_SELECTION) {
        dialogResult = static_cast<eventsManager*>(gEditManager->m_toolManager)
                     ->EditEvent(gTimeEventExtras[index]);
        if (dialogResult != EVENTS_DIALOG_CANCEL) {
            message.payload.widget.command = WIDGET_COMMAND_REPLACE_ITEM;
            message.payload.widget.id = SPEC_EVENT_LIST;
            strncpy(itemText,
                    static_cast<EventExtra*>(gEditManager->m_extras[gTimeEventExtras[index]])
                        ->message,
                    SPEC_EXCERPT_LENGTH);
            itemText[SPEC_EXCERPT_LENGTH] = '\0';
            sprintf(gText, localization::Tr("editor.spec.event_entry"),
                    static_cast<EventExtra*>(gEditManager->m_extras[gTimeEventExtras[index]])
                        ->firstDay,
                    itemText);
            message.payload.widget.parameter = index;
            message.payload.widget.data.text = gText;
            gSpecWindow->BroadcastMessage(message);
        }
    }
}

VA(0x00427fa5, 0xa6)
void DeleteMapEvent(void) {
    i32 index;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_EVENT_LIST;
    gSpecWindow->BroadcastMessage(message);
    index = message.payload.widget.data.value;
    if (index != LIST_BOX_NO_SELECTION) {
        if (index != gEditMapHeader.timeEventCount - 1)
            memmove(&gTimeEventExtras[index], &gTimeEventExtras[index + 1],
                    (gEditMapHeader.timeEventCount - index) * sizeof(gTimeEventExtras[0])
                        - sizeof(gTimeEventExtras[0]));
        gEditMapHeader.timeEventCount--;
        message.payload.widget.command = WIDGET_COMMAND_DELETE_ITEM;
        message.payload.widget.id = SPEC_EVENT_LIST;
        message.payload.widget.data.value = index;
        gSpecWindow->BroadcastMessage(message);
    }
}

VA(0x0042804b, 0x204)
void AddMapRumour(void) {
    char itemText[SPEC_EXCERPT_SIZE];
    i32 choice;
    tag_message message;
    rumourEventExtra* newRumour;

    message.type = MESSAGE_WIDGET;
    if (gEditMapHeader.rumourCount >= EDITOR_RUMOUR_CAPACITY) {
        sprintf(gText, localization::Tr("editor.spec.rumours_full"));
        NormalDialog(gText, NORMAL_DIALOG_INFO);
    } else {
        newRumour = new rumourEventExtra;
        memset(newRumour, 0, sizeof(rumourEventExtra));
        gEditManager->m_extras[gEditManager->m_extraCount] = newRumour;
        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(rumourEventExtra);
        choice = static_cast<eventsManager*>(gEditManager->m_toolManager)
                     ->EditRumour(gEditManager->m_extraCount);
        if (choice != EVENTS_DIALOG_CANCEL) {
            gRumourExtras[gEditMapHeader.rumourCount] = gEditManager->m_extraCount;
            message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
            message.payload.widget.id = SPEC_RUMOUR_LIST;
            strncpy(itemText,
                    static_cast<rumourEventExtra*>(
                        gEditManager->m_extras[gRumourExtras[gEditMapHeader.rumourCount]])
                        ->text,
                    SPEC_EXCERPT_LENGTH);
            itemText[SPEC_EXCERPT_LENGTH] = '\0';
            sprintf(gText, "%s", itemText);
            message.payload.widget.data.text = gText;
            gSpecWindow->BroadcastMessage(message);
            gEditMapHeader.rumourCount++;
            gEditManager->m_extraCount++;
        } else {
            delete[] static_cast<char*>(gEditManager->m_extras[gEditManager->m_extraCount]);
            gEditManager->m_extras[gEditManager->m_extraCount] = NULL;
            gEditManager->m_extraSizes[gEditManager->m_extraCount] = 0;
        }
    }
}

VA(0x0042824f, 0xe2)
void EditMapRumour(void) {
    char itemText[SPEC_EXCERPT_SIZE];
    i32 dialogResult;
    i32 index;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_RUMOUR_LIST;
    gSpecWindow->BroadcastMessage(message);
    index = message.payload.widget.data.value;
    if (index != LIST_BOX_NO_SELECTION) {
        dialogResult = static_cast<eventsManager*>(gEditManager->m_toolManager)
                     ->EditRumour(gRumourExtras[index]);
        if (dialogResult != EVENTS_DIALOG_CANCEL) {
            message.payload.widget.command = WIDGET_COMMAND_REPLACE_ITEM;
            message.payload.widget.id = SPEC_RUMOUR_LIST;
            strncpy(itemText,
                    static_cast<rumourEventExtra*>(gEditManager->m_extras[gRumourExtras[index]])
                        ->text,
                    SPEC_EXCERPT_LENGTH);
            itemText[SPEC_EXCERPT_LENGTH] = '\0';
            sprintf(gText, "%s", itemText);
            message.payload.widget.parameter = index;
            message.payload.widget.data.text = gText;
            gSpecWindow->BroadcastMessage(message);
        }
    }
}

VA(0x00428331, 0xa6)
void DeleteMapRumour(void) {
    i32 index;
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPEC_RUMOUR_LIST;
    gSpecWindow->BroadcastMessage(message);
    index = message.payload.widget.data.value;
    if (index != LIST_BOX_NO_SELECTION) {
        if (index != gEditMapHeader.rumourCount - 1)
            memmove(&gRumourExtras[index], &gRumourExtras[index + 1],
                    (gEditMapHeader.rumourCount - index) * sizeof(gRumourExtras[0])
                        - sizeof(gRumourExtras[0]));
        gEditMapHeader.rumourCount--;
        message.payload.widget.command = WIDGET_COMMAND_DELETE_ITEM;
        message.payload.widget.id = SPEC_RUMOUR_LIST;
        message.payload.widget.data.value = index;
        gSpecWindow->BroadcastMessage(message);
    }
}

VA(0x004283d7, 0x70e)
MessageDispatchResult SpecificationsHandler(struct tag_message& message) {
    char spareText[SPEC_EXCERPT_LENGTH];
    i32 unusedValues[2];
    i32 state;
    i32 color;
    b32 update;
    tag_message request;

    update = false;
    SET_WIDGET_MESSAGE(request, WIDGET_COMMAND_GET_TEXT, message.payload.widget.id);
    if (message.type != MESSAGE_WIDGET)
        return MESSAGE_DISPATCH_CONTINUE;
    switch (message.payload.widget.command) {
        case WIDGET_NOTIFY_DESELECT:
            switch (message.payload.widget.id) {
                case EVENTS_DIALOG_CANCEL:
                case EVENTS_DIALOG_OK:
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    FINISH_EDIT_DIALOG(message);
                    return MESSAGE_DISPATCH_FORWARD;
                case SPEC_EVENT_ADD:
                    AddMapEvent();
                    update = true;
                    break;
                case SPEC_EVENT_EDIT:
                    EditMapEvent();
                    update = true;
                    break;
                case SPEC_EVENT_DELETE:
                    DeleteMapEvent();
                    update = true;
                    break;
                case SPEC_RUMOUR_ADD:
                    AddMapRumour();
                    update = true;
                    break;
                case SPEC_RUMOUR_EDIT:
                    EditMapRumour();
                    update = true;
                    break;
                case SPEC_RUMOUR_DELETE:
                    DeleteMapRumour();
                    update = true;
                    break;
            }
            break;
        case WIDGET_NOTIFY_SELECT:
            update = true;
            switch (message.payload.widget.id) {
                case SPEC_EVENT_LIST:
                    if (message.payload.widget.parameter == SELECTION_DOUBLE_CLICK)
                        EditMapEvent();
                    break;
                case SPEC_RUMOUR_LIST:
                    if (message.payload.widget.parameter == SELECTION_DOUBLE_CLICK)
                        EditMapRumour();
                    break;
                case SPEC_DIFFICULTY_FIRST:
                case SPEC_DIFFICULTY_FIRST + 1:
                case SPEC_DIFFICULTY_FIRST + 2:
                case SPEC_DIFFICULTY_LAST:
                    gEditMapHeader.difficulty = message.payload.widget.id - SPEC_DIFFICULTY_FIRST;
                    break;
                case SPEC_STARTING_HERO_BOX:
                    gEditMapHeader.noStartingHero = 1 - gEditMapHeader.noStartingHero;
                    break;
                case SPEC_DESCRIPTION:
                    gSpecWindow->BroadcastMessage(request);
                    strcpy(gEditMapHeader.description, request.payload.widget.data.text);
                    break;
                case SPEC_NAME:
                    gSpecWindow->BroadcastMessage(request);
                    strcpy(gEditMapHeader.name, request.payload.widget.data.text);
                    break;
                case SPEC_PLAYER_FIRST:
                case SPEC_PLAYER_FIRST + 1:
                case SPEC_PLAYER_FIRST + 2:
                case SPEC_PLAYER_FIRST + 3:
                case SPEC_PLAYER_FIRST + 4:
                case SPEC_PLAYER_LAST:
                    color = message.payload.widget.id - SPEC_PLAYER_FIRST;
                    if (!gEditMapHeader.playerEnabled[color])
                        break;
                    state = gEditMapHeader.playerCanHuman[color]
                            + gEditMapHeader.playerCanComputer[color] * SPEC_PLAYER_COMPUTER;
                    state++;
                    if (state == SPEC_PLAYER_STATES)
                        state = SPEC_PLAYER_HUMAN;
                    gEditMapHeader.playerCanHuman[color] = (state & SPEC_PLAYER_HUMAN) != 0;
                    gEditMapHeader.playerCanComputer[color] = (state & SPEC_PLAYER_COMPUTER) != 0;
                    CalculatePlayerNumbers();
                    break;
                case SPEC_VICTORY_LIST:
                    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                    gSpecWindow->BroadcastMessage(message);
                    if (gEditMapHeader.victoryCondition != message.payload.widget.data.value) {
                        gEditMapHeader.victoryCondition = message.payload.widget.data.value;
                        gEditMapHeader.victoryConditionValue = gEditMapHeader.victoryTownY = 0;
                        FillVictoryConditionList();
                        update = true;
                    }
                    break;
                case SPEC_VICTORY_VALUE_LIST:
                    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                    gSpecWindow->BroadcastMessage(message);
                    SetVictoryConditionChoice(message.payload.widget.data.value);
                    break;
                case SPEC_COMPUTER_WINS_BOX:
                    gEditMapHeader.computerAlsoWins = 1 - gEditMapHeader.computerAlsoWins;
                    break;
                case SPEC_NORMAL_VICTORY_BOX:
                    gEditMapHeader.allowNormalVictory = 1 - gEditMapHeader.allowNormalVictory;
                    break;
                case SPEC_LOSS_LIST:
                    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                    gSpecWindow->BroadcastMessage(message);
                    if (gEditMapHeader.lossCondition != message.payload.widget.data.value) {
                        gEditMapHeader.lossCondition = message.payload.widget.data.value;
                        gEditMapHeader.lossConditionValue = gEditMapHeader.lossTownY = 0;
                        FillLossConditionList();
                        update = true;
                    }
                    break;
                case SPEC_LOSS_VALUE_LIST:
                    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                    gSpecWindow->BroadcastMessage(message);
                    SetLossConditionChoice(message.payload.widget.data.value);
                    break;
                default:
                    update = false;
                    break;
            }
            break;
    }
    if (update) {
        UpdateSpecificationsWindow();
        gSpecWindow->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
