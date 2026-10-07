#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>


typedef enum SetupWindowPlace {
    SETUP_WINDOW_X = 405,
    SETUP_WINDOW_Y = 8
} SetupWindowPlace;


typedef enum SetupDialogChoice {
    SETUP_CHOICE_ONE   = 1,
    SETUP_CHOICE_TWO   = 2,
    SETUP_CHOICE_THREE = 3,
    SETUP_CHOICE_FOUR  = 4,
    SETUP_CHOICE_QUIT  = 0x69
} SetupDialogChoice;


extern i32 gNewMapSize;

extern b32 gNewRandomMap;


b32 SetupNewMap(void);
b32 SetupMapSize(void);
MessageDispatchResult SetupNewMapHandler(struct tag_message& message);
MessageDispatchResult SetupMapSizeHandler(struct tag_message& message);
MessageDispatchResult BaseSetupHandler(struct tag_message& message);
MessageDispatchResult SetupMainHandler(struct tag_message& message);

#endif
