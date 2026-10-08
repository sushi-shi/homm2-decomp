#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H


#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/SETUP.h>


typedef enum EditorSetupChoice {
    SETUP_CHOICE_QUIT = 0x69
} EditorSetupChoice;


extern i32 gNewMapSize;

extern b32 gNewRandomMap;


b32 SetupNewMap(void);
b32 SetupMapSize(void);
MessageDispatchResult SetupNewMapHandler(struct tag_message& message);
MessageDispatchResult SetupMapSizeHandler(struct tag_message& message);
MessageDispatchResult BaseSetupHandler(struct tag_message& message);
MessageDispatchResult SetupMainHandler(struct tag_message& message);

#endif
