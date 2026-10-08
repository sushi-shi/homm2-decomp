#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H

// The editor's start-up dialogs (src/EDITOR/setup.cpp): the main set-up
// screen (stpemain.bin), the new-map choice (stpenew.bin) and its map size
// (stpesize.bin). The unit began as a copy of the game's SETUP.cpp handlers.

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/SETUP.h>

// The set-up dialogs open at SETUP.h's SETUP_WINDOW_X/Y and return its
// CHOICE_* numbers: the main screen's new map and load map, the new-map
// choice's blank and random map, the four map sizes. The main screen's quit
// button is the editor's own.
H2_ENUM_BEGIN(EditorSetupChoice)
    SETUP_CHOICE_QUIT = 0x69
H2_ENUM_END(EditorSetupChoice)

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
