#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H

// The editor's start-up dialogs (src/EDITOR/setup.cpp): the main set-up
// screen (stpemain.bin), the new-map choice (stpenew.bin) and its map size
// (stpesize.bin). The unit began as a copy of the game's SETUP.cpp handlers.

#include <va.h>
#include <Ints.h>
#include <BASE/message.h>

// Every set-up dialog (stpemain.bin, stpenew.bin, stpesize.bin) opens here.
H2_ENUM_BEGIN(SetupWindowPlace)
    SETUP_WINDOW_X = 405,
    SETUP_WINDOW_Y = 8
H2_ENUM_END(SetupWindowPlace)

// A set-up dialog's choices: the main screen's new map and load map, the
// new-map choice's blank and random map, the four map sizes, and the main
// screen's quit button.
H2_ENUM_BEGIN(SetupDialogChoice)
    SETUP_CHOICE_ONE   = 1,
    SETUP_CHOICE_TWO   = 2,
    SETUP_CHOICE_THREE = 3,
    SETUP_CHOICE_FOUR  = 4,
    SETUP_CHOICE_QUIT  = 0x69
H2_ENUM_END(SetupDialogChoice)

// The side of the square map a new map starts with.
extern i32 gNewMapSize;
// Set when the new map is to come from the random-map generator.
extern b32 gNewRandomMap;

// The new-map choice and its size; false when cancelled.
b32 SetupNewMap(void);
b32 SetupMapSize(void);
MessageDispatchResult SetupNewMapHandler(struct tag_message& message);
MessageDispatchResult SetupMapSizeHandler(struct tag_message& message);
MessageDispatchResult BaseSetupHandler(struct tag_message& message);
MessageDispatchResult SetupMainHandler(struct tag_message& message);

#endif
