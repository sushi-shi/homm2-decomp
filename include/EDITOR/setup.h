#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>


extern i32 gNewMapSize;

extern b32 gNewRandomMap;


b32 SetupNewMap(void);
b32 SetupMapSize(void);
MessageDispatchResult SetupNewMapHandler(struct tag_message& message);
MessageDispatchResult SetupMapSizeHandler(struct tag_message& message);
MessageDispatchResult BaseSetupHandler(struct tag_message& message);
MessageDispatchResult SetupMainHandler(struct tag_message& message);

#endif
