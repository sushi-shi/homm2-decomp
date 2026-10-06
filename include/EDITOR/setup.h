#ifndef HOMM2_EDITOR_SETUP_H
#define HOMM2_EDITOR_SETUP_H

// The editor's start-up dialogs (src/EDITOR/setup.cpp): the main set-up
// screen (stpemain.bin), the new-map choice (stpenew.bin) and its map size
// (stpesize.bin). The unit began as a copy of the game's SETUP.cpp handlers.

#include <va.h>
#include <BASE/message.h>

MessageDispatchResult SetupMainHandler(struct tag_message& message);
i32 SetupNewMap(void);

#endif
